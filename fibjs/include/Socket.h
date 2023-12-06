/*
 * Socket.h
 *
 *  Created on: Apr 22, 2012
 *      Author: lion
 */

#include "ifs/Socket.h"
#include "Stream.h"
#include "inetAddr.h"

#ifndef SOCKET_H_
#define SOCKET_H_

namespace fibjs
{

#define KEEPALIVE_TIMEOUT	(2 * 60)

class Socket: public Socket_base
{
EVENT_SUPPORT()
	;

public:
	Socket() :
			m_sock(INVALID_SOCKET), m_family(net_base::_AF_INET), m_type(
					net_base::_SOCK_STREAM)
#ifdef _WIN32
	, m_bBind(FALSE)
#endif
	{
	}

	Socket(SOCKET s, int32_t family, int32_t type) :
			m_sock(s), m_family(family), m_type(type)
#ifdef _WIN32
	, m_bBind(FALSE)
#endif
	{
	}

	virtual ~Socket();

public:
	// Stream_base
	virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
			exlib::AsyncEvent* ac);
	virtual result_t asyncRead(int32_t bytes);
	virtual result_t onread(v8::Handle<v8::Function> func);
	virtual result_t write(obj_ptr<Buffer_base>& data, exlib::AsyncEvent* ac);
	virtual result_t asyncWrite(obj_ptr<Buffer_base>& data);
	virtual result_t onwrite(v8::Handle<v8::Function> func);
	virtual result_t copyTo(obj_ptr<Stream_base>& stm, int64_t bytes,
			int64_t& retVal, exlib::AsyncEvent* ac);
	virtual result_t asyncCopyTo(obj_ptr<Stream_base>& stm, int64_t bytes);
	virtual result_t oncopyto(v8::Handle<v8::Function> func);
	virtual result_t stat(obj_ptr<Stat_base>& retVal, exlib::AsyncEvent* ac);
	virtual result_t asyncStat();
	virtual result_t onstat(v8::Handle<v8::Function> func);
	virtual result_t onerror(v8::Handle<v8::Function> func);

public:
	// Socket_base
	virtual result_t get_family(int32_t& retVal);
	virtual result_t get_type(int32_t& retVal);
	virtual result_t get_remoteAddress(std::string& retVal);
	virtual result_t get_remotePort(int32_t& retVal);
	virtual result_t get_localAddress(std::string& retVal);
	virtual result_t get_localPort(int32_t& retVal);
	virtual result_t connect(const char* host, int32_t port,
			exlib::AsyncEvent* ac);
	virtual result_t asyncConnect(const char* host, int32_t port);
	virtual result_t onconnect(v8::Handle<v8::Function> func);
	virtual result_t bind(const char* addr, int32_t port, bool allowIPv4);
	virtual result_t bind(int32_t port, bool allowIPv4);
	virtual result_t listen(int32_t backlog);
	virtual result_t accept(obj_ptr<Socket_base>& retVal,
			exlib::AsyncEvent* ac);
	virtual result_t asyncAccept();
	virtual result_t onaccept(v8::Handle<v8::Function> func);
	virtual result_t close(exlib::AsyncEvent* ac);
	virtual result_t asyncClose();
	virtual result_t onclose(v8::Handle<v8::Function> func);
	virtual result_t recv(int32_t bytes, obj_ptr<Buffer_base>& retVal,
			exlib::AsyncEvent* ac);
	virtual result_t asyncRecv(int32_t bytes);
	virtual result_t onrecv(v8::Handle<v8::Function> func);
	virtual result_t recvFrom(int32_t bytes, obj_ptr<Buffer_base>& retVal);
	virtual result_t send(obj_ptr<Buffer_base>& data, exlib::AsyncEvent* ac);
	virtual result_t asyncSend(obj_ptr<Buffer_base>& data);
	virtual result_t onsend(v8::Handle<v8::Function> func);
	virtual result_t sendto(obj_ptr<Buffer_base>& data, const char* host,
			int32_t port);

public:
	result_t create(int32_t family, int32_t type);
	result_t recv(int32_t bytes, obj_ptr<Buffer_base>& retVal,
			exlib::AsyncEvent* ac, bool bRead);

private:
	SOCKET m_sock;
	int32_t m_family;
	int32_t m_type;

#ifdef _WIN32
	BOOL m_bBind;
#endif
};

}

#endif /* SOCKET_H_ */
