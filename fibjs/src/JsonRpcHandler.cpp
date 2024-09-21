/*
 * JsonRpcHandler.cpp
 *
 *  Created on: Sep 3, 2012
 *      Author: lion
 */

#include "JsonRpcHandler.h"
#include "ifs/mq.h"
#include "ifs/encoding.h"
#include "ifs/HttpRequest.h"
#include "Buffer.h"
#include "MemoryStream.h"
#include "JSHandler.h"

namespace fibjs
{

result_t JsonRpcHandler::invoke(object_base* v, obj_ptr<Handler_base>& retVal,
		exlib::AsyncEvent* ac)
{
	if (ac)
		return CALL_E_NOASYNC;

	obj_ptr<Message_base> msg = Message_base::getInstance(v);
	if (msg == NULL)
		return CALL_E_BADVARTYPE;

	obj_ptr<HttpRequest_base> htreq = HttpRequest_base::getInstance(v);
	obj_ptr<SeekableStream_base> body;
	obj_ptr<Buffer_base> buf;
	v8::Handle<v8::Value> jsval;
	v8::Handle<v8::Object> o;
	Variant result;
	std::string str;
	int64_t len;
	int32_t sz, i;
	result_t hr;
	bool bFormReq = false;
	obj_ptr<List_base> params;

	if (htreq != NULL)
	{
		if (htreq->firstHeader("Content-Type", result) == CALL_RETURN_NULL)
			return CALL_E_INVALID_CALL;

		str = result.string();
		if (!qstricmp(str.c_str(), "application/x-www-form-urlencoded", 33))
		{
			obj_ptr<HttpCollection_base> form;
			htreq->get_form(form);
			if (form->first("jsonrpc", result) == CALL_RETURN_NULL)
				return CALL_E_INVALID_CALL;
			str = result.string();
			bFormReq = true;
		}
		else if (qstricmp(str.c_str(), "application/json", 16))
			return CALL_E_INVALID_CALL;
	}

	if (!bFormReq)
	{
		msg->get_body(body);

		body->size(len);
		sz = (int32_t) len;

		body->rewind();
		hr = body->ac_read(sz, buf);
		if (hr < 0)
			return hr;
		if (hr == CALL_RETURN_NULL)
			return CALL_E_INVALID_CALL;
		body.Release();

		buf->toString(str);
		buf.Release();
	}

	hr = encoding_base::jsonDecode(str.c_str(), jsval);
	if (hr < 0)
		return hr;

	if (!jsval->IsObject())
		return CALL_E_INVALID_CALL;

	o = v8::Handle<v8::Object>::Cast(jsval);

	jsval = o->Get(v8::String::NewSymbol("method", 6));
	if (IsEmpty(jsval))
		return CALL_E_INVALID_CALL;

	msg->get_value(str);
	str += '/';
	str.append(*v8::String::Utf8Value(jsval));
	msg->set_value(str.c_str());

	jsval = o->Get(v8::String::NewSymbol("params", 6));
	if (!jsval.IsEmpty() && jsval->IsArray())
	{
		v8::Handle<v8::Array> jsparams = v8::Handle<v8::Array>::Cast(jsval);

		sz = jsparams->Length();
		msg->get_params(params);
		params->resize(sz);

		for (i = 0; i < sz; i++)
			params->_indexed_setter(i, jsparams->Get(i));
	}

	obj_ptr<Handler_base> hdlr1;

	hr = JSHandler::js_invoke(m_hdlr, v, hdlr1, NULL);
	if (hr >= 0 && hr != CALL_RETURN_NULL)
		hr = mq_base::ac_invoke(m_hdlr, v);

	v8::Handle<v8::String> strId = v8::String::NewSymbol("id", 2);
	jsval = o->Get(strId);

	o = v8::Object::New();
	o->Set(strId, jsval);

	if (hr < 0)
	{
		result_t hr1 = encoding_base::jsonEncode(o, str);
		if (hr1 < 0)
			return hr1;

		if (str.length() <= 2)
			str.assign("{", 1);
		else
		{
			str.resize(str.length() - 1);
			str += ',';
		}

		if (hr == CALL_E_INVALID_CALL)
			str.append(
					"\"error\": {\"code\": -32601, \"message\": \"Method not found.\"}}");
		else
			str.append(
					"\"error\": {\"code\": -32603, \"message\": \"Internal error.\"}}");
	}
	else
	{
		msg->get_result(result);
		o->Set(v8::String::NewSymbol("result", 6), result);

		hr = encoding_base::jsonEncode(o, str);

		if (hr < 0)
			return hr;
	}

	body = new MemoryStream();

	if (bFormReq)
	{
		std::string strTemp;

		strTemp.assign(
				"<html><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"><script>window.name=\"",
				94);

		encoding_base::jsstr(str.c_str(), str);
		strTemp.append(str);

		strTemp.append("\";</script></html>", 18);

		str = strTemp;
	}

	buf = new Buffer(str);
	hr = body->ac_write(buf);
	if (hr < 0)
		return hr;

	if (htreq)
	{
		obj_ptr<HttpResponse_base> htrep;

		htreq->get_response(htrep);
		htrep->set_body(body);
		htrep->setHeader("Content-Type",
				bFormReq ? "text/html" : "application/json");
	}
	else
		msg->set_body(body);

	return CALL_RETURN_NULL;
}

} /* namespace fibjs */
