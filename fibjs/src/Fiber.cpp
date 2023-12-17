/*
 * Fiber.cpp
 *
 *  Created on: Jul 23, 2012
 *      Author: lion
 */

#include "Fiber.h"
#include "ifs/os.h"

namespace fibjs
{

#define FIBER_PER_CPU	3000
extern v8::Persistent<v8::Context> s_context;

static exlib::Queue<asyncEvent> g_jobs;
static exlib::IDLE_PROC s_oldIdle;
static int32_t s_cpus;
static int32_t s_fibers;

int g_tlsCurrent;

static class _fiber_init
{
public:
	_fiber_init()
	{
		if (os_base::CPUs(s_cpus) < 0)
			s_cpus = 4;
		s_fibers = 0;

		exlib::Service* pService = exlib::Service::getFiberService();

		g_tlsCurrent = pService->tlsAlloc();
		s_oldIdle = pService->onIdle(onIdle);
	}

	static void onIdle()
	{
		if (!g_jobs.empty() && (s_fibers < s_cpus * FIBER_PER_CPU))
		{
			s_fibers++;
			exlib::Service::CreateFiber(FiberBase::fiber_proc)->Unref();
		}

		if (s_oldIdle)
			s_oldIdle();
	}
} s_fiber_init;

static class null_fiber_data: public Fiber_base
{
EVENT_SUPPORT()
	;FIBER_FREE()
	;

public:
	null_fiber_data()
	{
		Ref();
	}

	result_t get_func(v8::Handle<v8::Function>& retVal)
	{
		return CALL_E_INVALID_CALL;
	}

	result_t join()
	{
		return 0;
	}

	result_t get_caller(obj_ptr<Fiber_base>& retVal)
	{
		return CALL_E_INVALID_CALL;
	}

	result_t onerror(v8::Handle<v8::Function> trigger)
	{
		return 0;
	}

	result_t onexit(v8::Handle<v8::Function> trigger)
	{
		return 0;
	}

} s_null;

void* FiberBase::fiber_proc(void* p)
{
	v8::Locker locker(isolate);
	v8::Isolate::Scope isolate_scope(isolate);

	v8::HandleScope handle_scope;
	v8::Context::Scope context_scope(s_context);

	while (1)
	{
		asyncEvent* ae;

		if ((ae = g_jobs.tryget()) == NULL)
		{
			v8::Unlocker unlocker(isolate);
			ae = g_jobs.get();
		}

		if (ae == NULL)
			break;

		{
			v8::HandleScope handle_scope;
			ae->js_callback();
		}
	}

	return NULL;
}

void FiberBase::start()
{
	m_caller = (Fiber_base*) g_pService->tlsGet(g_tlsCurrent);

	g_jobs.put(this);
	Ref();
}

void JSFiber::callFunction(v8::Handle<v8::Value>& retVal)
{
	size_t i;

	std::vector<v8::Handle<v8::Value> > argv;

	argv.resize(m_argv.size());
	for (i = 0; i < m_argv.size(); i++)
		argv[i] = m_argv[i];

	v8::Handle<v8::Object> o = wrap();

	v8::TryCatch try_catch;
	retVal = m_func->Call(o, (int) argv.size(), argv.data());
	if (try_catch.HasCaught())
	{
		v8::Handle<v8::Value> err = try_catch.Exception();
		m_error = true;

		_trigger("error", &err, 1);
		ReportException(&try_catch, true);
	}

	if (!IsEmpty(retVal))
	{
		m_result = v8::Persistent<v8::Value>::New(retVal);
		_trigger("exit", &retVal, 1);
	}
	else
	{
		v8::Handle<v8::Value> v = v8::Null();
		_trigger("exit", &v, 1);
	}

	exit();

	s_null.Ref();
	o->SetPointerInInternalField(0, &s_null);
}

void JSFiber::js_callback()
{
	v8::Handle<v8::Value> retVal;

	g_pService->tlsPut(g_tlsCurrent, this);
	callFunction(retVal);
	g_pService->tlsPut(g_tlsCurrent, NULL);
}

void JSFiber::callFunction(v8::Handle<v8::Function> func,
		v8::Handle<v8::Value>* args, int argCount,
		v8::Handle<v8::Value>& retVal)
{
	obj_ptr<JSFiber> fb = new JSFiber();
	int i;

	fb->m_argv.resize(argCount);
	for (i = 0; i < argCount; i++)
		fb->m_argv[i] = v8::Persistent<v8::Value>::New(args[i]);
	fb->m_func = v8::Persistent<v8::Function>::New(func);

	fb->Ref();

	g_pService->tlsPut(g_tlsCurrent, fb);
	fb->callFunction(retVal);
	g_pService->tlsPut(g_tlsCurrent, NULL);
}

void AsyncCallBack::callback()
{
	g_jobs.put(this);
}

void asyncCallBack::callback()
{
	g_jobs.put(this);
}

} /* namespace fibjs */
