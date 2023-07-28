/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _HttpMessage_base_H_
#define _HttpMessage_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Trigger.h"

namespace fibjs
{

class Trigger_base;
class HttpCollection_base;
class Stream_base;
class BufferedStream_base;

class HttpMessage_base : public Trigger_base
{
public:
	// HttpMessage_base
	virtual result_t get_protocol(int32_t& retVal) = 0;
	virtual result_t set_protocol(int32_t newVal) = 0;
	virtual result_t get_headers(obj_ptr<HttpCollection_base>& retVal) = 0;
	virtual result_t get_body(obj_ptr<Stream_base>& retVal) = 0;
	virtual result_t set_body(obj_ptr<Stream_base>& newVal) = 0;
	virtual result_t get_contentType(std::string& retVal) = 0;
	virtual result_t set_contentType(const char* newVal) = 0;
	virtual result_t get_contentLength(int32_t& retVal) = 0;
	virtual result_t set_contentLength(int32_t newVal) = 0;
	virtual result_t get_keepAlive(bool& retVal) = 0;
	virtual result_t set_keepAlive(bool newVal) = 0;
	virtual result_t clear() = 0;
	virtual result_t send(obj_ptr<Stream_base>& stm) = 0;
	virtual result_t asyncSend(obj_ptr<Stream_base>& stm) = 0;
	virtual result_t onsend(v8::Handle<v8::Function> func) = 0;
	virtual result_t read(obj_ptr<BufferedStream_base>& stm) = 0;
	virtual result_t asyncRead(obj_ptr<BufferedStream_base>& stm) = 0;
	virtual result_t onread(v8::Handle<v8::Function> func) = 0;

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		result_t hr = Trigger_base::toJSON(key, retVal);
		if(hr < 0)return hr;

		CLONE(protocol, int32_t);
		CLONE_CLASS(headers, HttpCollection_base);
		CLONE_CLASS(body, Stream_base);
		CLONE_String(contentType);
		CLONE(contentLength, int32_t);
		CLONE(keepAlive, bool);

		return 0;
	}

public:
	static v8::Handle<v8::Value> s_get_protocol(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_protocol(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_headers(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_body(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_body(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_contentType(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_contentType(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_contentLength(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_contentLength(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_keepAlive(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_keepAlive(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_clear(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_send(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_asyncSend(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_onsend(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_read(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_asyncRead(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_onread(const v8::Arguments& args);
};

}

#include "HttpCollection.h"
#include "Stream.h"
#include "BufferedStream.h"

namespace fibjs
{
	inline ClassInfo& HttpMessage_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"clear", s_clear},
			{"send", s_send},
			{"asyncSend", s_asyncSend},
			{"onsend", s_onsend},
			{"read", s_read},
			{"asyncRead", s_asyncRead},
			{"onread", s_onread}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"protocol", s_get_protocol, s_set_protocol},
			{"headers", s_get_headers},
			{"body", s_get_body, s_set_body},
			{"contentType", s_get_contentType, s_set_contentType},
			{"contentLength", s_get_contentLength, s_set_contentLength},
			{"keepAlive", s_get_keepAlive, s_set_keepAlive}
		};

		static ClassData s_cd = 
		{ 
			"HttpMessage", NULL, 
			7, s_method, 0, NULL, 6, s_property, NULL, NULL,
			&Trigger_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> HttpMessage_base::s_get_protocol(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpMessage_base);

		hr = pInst->get_protocol(vr);

		METHOD_RETURN();
	}

	inline void HttpMessage_base::s_set_protocol(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpMessage_base);

		PROPERTY_VAL(int32_t);
		hr = pInst->set_protocol(v0);

		PROPERTY_SET_LEAVE();
	}

	inline v8::Handle<v8::Value> HttpMessage_base::s_get_headers(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<HttpCollection_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpMessage_base);

		hr = pInst->get_headers(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> HttpMessage_base::s_get_body(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<Stream_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpMessage_base);

		hr = pInst->get_body(vr);

		METHOD_RETURN();
	}

	inline void HttpMessage_base::s_set_body(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpMessage_base);

		PROPERTY_VAL(obj_ptr<Stream_base>);
		hr = pInst->set_body(v0);

		PROPERTY_SET_LEAVE();
	}

	inline v8::Handle<v8::Value> HttpMessage_base::s_get_contentType(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpMessage_base);

		hr = pInst->get_contentType(vr);

		METHOD_RETURN();
	}

	inline void HttpMessage_base::s_set_contentType(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpMessage_base);

		PROPERTY_VAL_String();
		hr = pInst->set_contentType(v0);

		PROPERTY_SET_LEAVE();
	}

	inline v8::Handle<v8::Value> HttpMessage_base::s_get_contentLength(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpMessage_base);

		hr = pInst->get_contentLength(vr);

		METHOD_RETURN();
	}

	inline void HttpMessage_base::s_set_contentLength(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpMessage_base);

		PROPERTY_VAL(int32_t);
		hr = pInst->set_contentLength(v0);

		PROPERTY_SET_LEAVE();
	}

	inline v8::Handle<v8::Value> HttpMessage_base::s_get_keepAlive(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpMessage_base);

		hr = pInst->get_keepAlive(vr);

		METHOD_RETURN();
	}

	inline void HttpMessage_base::s_set_keepAlive(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpMessage_base);

		PROPERTY_VAL(bool);
		hr = pInst->set_keepAlive(v0);

		PROPERTY_SET_LEAVE();
	}

	inline v8::Handle<v8::Value> HttpMessage_base::s_clear(const v8::Arguments& args)
	{
		METHOD_INSTANCE(HttpMessage_base);
		METHOD_ENTER(0, 0);

		hr = pInst->clear();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> HttpMessage_base::s_send(const v8::Arguments& args)
	{
		METHOD_INSTANCE(HttpMessage_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Stream_base>, 0);

		hr = pInst->send(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> HttpMessage_base::s_asyncSend(const v8::Arguments& args)
	{
		METHOD_INSTANCE(HttpMessage_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Stream_base>, 0);

		hr = pInst->asyncSend(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> HttpMessage_base::s_onsend(const v8::Arguments& args)
	{
		METHOD_INSTANCE(HttpMessage_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = pInst->onsend(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> HttpMessage_base::s_read(const v8::Arguments& args)
	{
		METHOD_INSTANCE(HttpMessage_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<BufferedStream_base>, 0);

		hr = pInst->read(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> HttpMessage_base::s_asyncRead(const v8::Arguments& args)
	{
		METHOD_INSTANCE(HttpMessage_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<BufferedStream_base>, 0);

		hr = pInst->asyncRead(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> HttpMessage_base::s_onread(const v8::Arguments& args)
	{
		METHOD_INSTANCE(HttpMessage_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = pInst->onread(v0);

		METHOD_VOID();
	}

}

#endif

