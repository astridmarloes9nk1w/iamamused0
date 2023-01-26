/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _coroutine_base_H_
#define _coroutine_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class Lock_base;
class Semaphore_base;
class Condition_base;
class Event_base;
class Trigger_base;
class Fiber_base;

class coroutine_base : public module_base
{
public:
	// coroutine_base
	static result_t start(v8::Handle<v8::Function> func, const v8::Arguments& args, obj_ptr<Fiber_base>& retVal);
	static result_t current(obj_ptr<Fiber_base>& retVal);
	static result_t sleep(int32_t ms);

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

protected:
	static v8::Handle<v8::Value> s_start(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_current(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_sleep(const v8::Arguments& args);
};

}

#include "Lock.h"
#include "Semaphore.h"
#include "Condition.h"
#include "Event.h"
#include "Trigger.h"
#include "Fiber.h"

namespace fibjs
{
	inline ClassInfo& coroutine_base::class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"start", s_start, true},
			{"current", s_current, true},
			{"sleep", s_sleep, true}
		};

		static ClassObject s_object[] = 
		{
			{"Lock", Lock_base::class_info},
			{"Semaphore", Semaphore_base::class_info},
			{"Condition", Condition_base::class_info},
			{"Event", Event_base::class_info},
			{"Trigger", Trigger_base::class_info}
		};

		static ClassData s_cd = 
		{ 
			"coroutine", NULL, 
			3, s_method, 5, s_object, 0, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline v8::Handle<v8::Value> coroutine_base::s_start(const v8::Arguments& args)
	{
		obj_ptr<Fiber_base> vr;

		METHOD_ENTER(-1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = start(v0, args, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> coroutine_base::s_current(const v8::Arguments& args)
	{
		obj_ptr<Fiber_base> vr;

		METHOD_ENTER(0, 0);

		hr = current(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> coroutine_base::s_sleep(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 0);

		OPT_ARG(int32_t, 0, 0);

		hr = sleep(v0);

		METHOD_VOID();
	}

}

#endif

