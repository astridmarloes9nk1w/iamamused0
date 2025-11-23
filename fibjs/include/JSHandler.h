/*
 * JSHandler.h
 *
 *  Created on: Aug 25, 2012
 *      Author: lion
 */

#include "ifs/Handler.h"
#include "ifs/vm.h"

#ifndef JSHANDLER_H_
#define JSHANDLER_H_

namespace fibjs
{

class JSHandler: public Handler_base
{
public:
	~JSHandler()
	{
		m_handler.Dispose(v8::Isolate::GetCurrent());
	}

public:
	// Handler_base
	virtual result_t invoke(object_base* v, obj_ptr<Handler_base>& retVal,
			exlib::AsyncEvent* ac);

public:
	result_t setHandler(v8::Handle<v8::Value> hdlr);

	static result_t New(v8::Handle<v8::Value> hdlr,
			obj_ptr<Handler_base>& retVal)
	{
		obj_ptr<JSHandler> r = new JSHandler();
		result_t hr = r->setHandler(hdlr);
		if (hr < 0)
			return hr;

		retVal = r;

		return 0;
	}

	static result_t New(const char* id, const char* method,
			obj_ptr<Handler_base>& retVal)
	{
		if (!*id)
			return CALL_E_INVALIDARG;

		obj_ptr<JSHandler> r = new JSHandler();

		result_t hr = vm_base::current(r->m_sbox);
		if (hr < 0)
			return hr;

		r->m_id = id;
		r->m_method = method;

		if (isPathSlash(r->m_id[r->m_id.length() - 1]) && *method)
			return CALL_E_INVALIDARG;

		retVal = r;

		return 0;
	}

public:
	static result_t js_invoke(Handler_base* hdlr, object_base* v,
			obj_ptr<Handler_base>& retVal, exlib::AsyncEvent* ac);

private:
	v8::Persistent<v8::Value> m_handler;
	std::string m_id;
	std::string m_method;
	obj_ptr<SandBox_base> m_sbox;
};

} /* namespace fibjs */
#endif /* JSHANDLER_H_ */
