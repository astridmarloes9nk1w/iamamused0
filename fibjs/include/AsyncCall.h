#ifndef _fj_ASYNCCALL_H
#define _fj_ASYNCCALL_H

#include <string>

namespace fibjs
{

class AsyncCall;
typedef exlib::lockfree<AsyncCall> AsyncQueue;

class AsyncCall: public exlib::AsyncEvent
{
public:
	AsyncCall(void ** a, void (*f)(AsyncCall*) = NULL) :
			func(f), args(a)
	{
	}

	int wait()
	{
		v8::Unlocker unlocker(isolate);
		return exlib::AsyncEvent::wait();
	}

public:
	void (*func)(AsyncCall*);
	void ** args;
};

class AsyncCallBack: public AsyncCall
{
public:
	AsyncCallBack(object_base* pThis, void ** a, void (*f)(AsyncCall*) = NULL) :
			AsyncCall(a, f), m_pThis(pThis)
	{
	}

	virtual void invoke();
	virtual void callback() = 0;

protected:
	static const char* m_v(const char*& s);
	static const char* m_v(std::string& s)
	{
		return s.c_str();
	}

	template<typename T>
	static T& m_v(T& v)
	{
		return v;
	}

	static bool c_v(std::string& s)
	{
		s.clear();
		return true;
	}

	template<typename T>
	static bool c_v(T& v)
	{
		return true;
	}

	template<typename T>
	static bool c_v(obj_ptr<T>& v)
	{
		if(v == NULL)
			return true;

		if (v->isJSObject())
			return false;

		v.Release();
		return true;
	}

	template<typename T>
	static bool c_v(T*& v)
	{
		if(v == NULL)
			return true;

		if (v->isJSObject())
			return false;

		v->Unref();
		v = NULL;
		return true;
	}

	template<typename T>
	void _trigger(const char* strEvent, T& pv)
	{
		if (m_pThis)
		{
			if (result() >= 0)
			{
				v8::Handle<v8::Value> v = ReturnValue(pv);
				m_pThis->_trigger(strEvent, &v, 1);
			}
			else
			{
				puts("error");
				m_pThis->_trigger("error", NULL, 0);
			}

			m_pThis->Unref();
		}

		delete this;
	}

	template<typename T>
	void _trigger(const char* strEvent, T* pv)
	{
		if (m_pThis)
		{
			if (result() >= 0)
				m_pThis->_trigger(strEvent, NULL, 0);
			else
			{
				puts("error");
				m_pThis->_trigger("error", NULL, 0);
			}

			m_pThis->Unref();
		}

		delete this;
	}

public:
	object_base* m_pThis;
};

class AsyncLog: public exlib::AsyncEvent
{
public:
	AsyncLog(int priority, std::string msg) :
			m_priority(priority), m_msg(msg)
	{
	}

public:
	int m_priority;
	std::string m_msg;
};

typedef exlib::lockfree<AsyncLog> AsyncLogQueue;

}

#endif
