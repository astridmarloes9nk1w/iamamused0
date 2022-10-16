/*
 * Socket_ev.cpp
 *
 *  Created on: Apr 26, 2012
 *      Author: lion
 */

#include "Socket.h"
#include "ifs/net.h"
#include "Buffer.h"
#include "ev.h"
#include  <fcntl.h>

namespace fibjs
{

static struct ev_loop *s_loop;

result_t net_base::backend(std::string& retVal)
{
	switch (ev_backend(s_loop))
	{
	case EVBACKEND_SELECT:
		retVal = "Select";
		break;
	case EVBACKEND_POLL:
		retVal = "Poll";
		break;
	case EVBACKEND_EPOLL:
		retVal = "EPoll";
		break;
	case EVBACKEND_KQUEUE:
		retVal = "KQueue";
		break;
	case EVBACKEND_DEVPOLL:
		retVal = "DevPoll";
		break;
	case EVBACKEND_PORT:
		retVal = "Port";
		break;
	}

	return 0;
}

class asyncProc;

static ev_async s_asEvent;
static exlib::lockfree<asyncProc> s_evWait;

class asyncProc: public ev_io
{
public:
	asyncProc(SOCKET s, int op) :
		m_s(s), m_op(op), m_next(NULL)

	{
	}

	virtual ~asyncProc()
	{
	}

	void call()
	{
		s_evWait.put(this);
		ev_async_send(s_loop, &s_asEvent);
	}

	inline void start()
	{
		ev_io_init(((ev_io*)this), io_cb, m_s, m_op);
		ev_io_start(s_loop, this);
	}

	virtual void proc()
	{
	}

public:
	SOCKET m_s;
	int m_op;
	asyncProc* m_next;

private:
	static void io_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
	{
		ev_io_stop(s_loop, watcher);
		((asyncProc*) watcher)->proc();
	}
};

static class _acSocket: public exlib::Thread
{
public:
	_acSocket()
	{
		s_loop = EV_DEFAULT;
		start();
	}

	virtual void Run()
	{
		ev_async_init(&s_asEvent, as_cb);
		ev_async_start(s_loop, &s_asEvent);

		ev_timer tm;
		tm_cb(s_loop, &tm, 0);

		ev_run(s_loop, 0);
	}

private:
	static void doAsync()
	{
		asyncProc *p = s_evWait.getList(), *p1;

		while (p)
		{
			p1 = (asyncProc*) p->m_next;
			p->m_next = NULL;
			p->start();

			p = p1;
		}
	}

	static void tm_cb(struct ev_loop *loop, struct ev_timer *watcher,
			int revents)
	{
		ev_timer_init(watcher, tm_cb, 1, 0);
		ev_timer_start(s_loop, watcher);

		doAsync();
	}

	static void as_cb(struct ev_loop *loop, struct ev_async *watcher,
			int revents)
	{
		doAsync();
	}
} s_acSock;

class asyncWait: public asyncProc
{
public:
	asyncWait(SOCKET s, int op) :
		asyncProc(s, op)
	{
		call();

		v8::Unlocker unlocker(isolate);
		m_aEvent.wait();
	}

	virtual void proc()
	{
		m_aEvent.post();
	}

public:
	exlib::AsyncEvent m_aEvent;
};

inline bool wouldBlock()
{
#ifdef _WIN32
	return WSAGetLastError() == WSAEWOULDBLOCK;
#else
	return errno == EWOULDBLOCK;
#endif
}

int a_connect(SOCKET s, sockaddr* ai, int sz)
{
	int n = ::connect(s, ai, sz);
	if (n == SOCKET_ERROR && (errno == EINPROGRESS))
	{
		asyncWait ac(s, EV_WRITE);
		_sockaddr addr_info;
		socklen_t sz1 = sizeof(addr_info);

		return ::getpeername(s, (sockaddr*) &addr_info, &sz1);
	}

	return 0;
}

SOCKET a_accept(SOCKET s, sockaddr* ai, socklen_t* sz)
{
	SOCKET c = ::accept(s, ai, sz);
	if (c == INVALID_SOCKET && wouldBlock())
	{
		asyncWait ac(s, EV_READ);
		c = ::accept(s, ai, sz);
	}

	return c;
}

int a_recv(SOCKET s, char *p, size_t sz, int f)
{
	int n = (int) ::recv(s, p, sz, f);
	if (n == SOCKET_ERROR && wouldBlock())
	{
		asyncWait ac(s, EV_READ);
		n = (int) ::recv(s, p, sz, f);
	}

	return n;
}

int a_send(SOCKET s, const char *p, size_t sz, int f)
{
	int n = (int) ::send(s, p, sz, f);
	if (n == SOCKET_ERROR && wouldBlock())
	{
		asyncWait ac(s, EV_WRITE);
		n = (int) ::send(s, p, sz, f);
	}

	return n;
}

class asyncConnect: public asyncProc
{
public:
	asyncConnect(SOCKET s, _sockaddr& ai, AsyncCall* ac) :
		asyncProc(s, EV_WRITE), m_ai(ai), m_ac(ac)
	{
	}

	result_t connect()
	{
		int n = ::connect(
				m_s,
				(struct sockaddr*) &m_ai,
				m_ai.addr4.sin_family == PF_INET ? sizeof(m_ai.addr4)
						: sizeof(m_ai.addr6));
		if (n == SOCKET_ERROR)
		{
			if (errno == EINPROGRESS)
			{
				call();
				return CALL_E_PENDDING;
			}
			return SocketError();
		}

		return 0;
	}

	virtual void proc()
	{
		_sockaddr addr_info;
		socklen_t sz1 = sizeof(addr_info);

		if (::getpeername(m_s, (sockaddr*) &addr_info, &sz1) == SOCKET_ERROR)
			m_ac->hr = -ETIMEDOUT;
		else
			m_ac->hr = 0;

		m_ac->post();
		delete this;
	}

public:
	_sockaddr m_ai;
	AsyncCall* m_ac;
};

result_t Socket::connect(const char* addr, int32_t port, AsyncCall* ac)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	_sockaddr addr_info;
	result_t hr = getAddrInfo(addr, port, addr_info);
	if (hr < 0)
		return hr;

	return (new asyncConnect(m_sock, addr_info, ac))->connect();
}

}
