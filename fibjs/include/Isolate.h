/*
 * Isolate.h
 *
 *  Created on: May 2, 2015
 *      Author: lion
 */

#ifndef ISOLATE_H_
#define ISOLATE_H_

#include <exlib/include/list.h>
#include <exlib/include/service.h>
#include "DateCache.h"
#include "QuickArray.h"

namespace fibjs
{

class SandBox;
class JSFiber;
class Isolate : public exlib::linkitem
{
public:
	class rt
	{
	public:
		rt();
		~rt();

	public:
		static bool g_trace;

	private:
		JSFiber *m_fiber;
		v8::Unlocker unlocker;
	};

public:
	Isolate();

public:
	static Isolate* now();
	static bool check();
	static void reg(void *rt);

public:
	exlib::Service *m_service;
	v8::Isolate *m_isolate;
	DateCache m_dc;
	QuickArray<void*> m_ci;
	v8::Persistent<v8::Context> m_context;
	v8::Persistent<v8::Object> m_global;
	obj_ptr<SandBox> m_topSandbox;
	exlib::List<exlib::linkitem> m_fibers;
	bool m_test_setup_bbd, m_test_setup_tdd;

	exlib::Queue<exlib::linkitem> m_jobs;
	exlib::IDLE_PROC m_oldIdle;
	int32_t m_currentFibers;
	int32_t m_idleFibers;
};

} /* namespace fibjs */
#endif /* ISOLATE_H_ */
