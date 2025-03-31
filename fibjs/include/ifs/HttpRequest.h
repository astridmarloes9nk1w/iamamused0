/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _HttpRequest_base_H_
#define _HttpRequest_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "HttpMessage.h"

namespace fibjs
{

class HttpMessage_base;
class HttpResponse_base;
class HttpCollection_base;

class HttpRequest_base : public HttpMessage_base
{
public:
	// HttpRequest_base
	static result_t _new(obj_ptr<HttpRequest_base>& retVal);
	virtual result_t get_method(std::string& retVal) = 0;
	virtual result_t set_method(const char* newVal) = 0;
	virtual result_t get_address(std::string& retVal) = 0;
	virtual result_t set_address(const char* newVal) = 0;
	virtual result_t get_queryString(std::string& retVal) = 0;
	virtual result_t set_queryString(const char* newVal) = 0;
	virtual result_t get_response(obj_ptr<HttpResponse_base>& retVal) = 0;
	virtual result_t get_cookie(obj_ptr<HttpCollection_base>& retVal) = 0;
	virtual result_t get_form(obj_ptr<HttpCollection_base>& retVal) = 0;
	virtual result_t get_query(obj_ptr<HttpCollection_base>& retVal) = 0;

	DECLARE_CLASSINFO(HttpRequest_base);

public:
	static v8::Handle<v8::Value> s__new(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_get_method(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_method(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_address(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_address(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_queryString(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_queryString(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_response(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_cookie(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_form(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_query(v8::Local<v8::String> property, const v8::AccessorInfo &info);
};

}

#include "HttpResponse.h"
#include "HttpCollection.h"

namespace fibjs
{
	inline ClassInfo& HttpRequest_base::class_info()
	{
		static ClassData::ClassProperty s_property[] = 
		{
			{"method", s_get_method, s_set_method},
			{"address", s_get_address, s_set_address},
			{"queryString", s_get_queryString, s_set_queryString},
			{"response", s_get_response},
			{"cookie", s_get_cookie},
			{"form", s_get_form},
			{"query", s_get_query}
		};

		static ClassData s_cd = 
		{ 
			"HttpRequest", s__new, 
			0, NULL, 0, NULL, 7, s_property, NULL, NULL,
			&HttpMessage_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> HttpRequest_base::s_get_method(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpRequest_base);

		hr = pInst->get_method(vr);

		METHOD_RETURN();
	}

	inline void HttpRequest_base::s_set_method(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpRequest_base);

		PROPERTY_VAL_String();
		hr = pInst->set_method(v0);

		PROPERTY_SET_LEAVE();
	}

	inline v8::Handle<v8::Value> HttpRequest_base::s_get_address(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpRequest_base);

		hr = pInst->get_address(vr);

		METHOD_RETURN();
	}

	inline void HttpRequest_base::s_set_address(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpRequest_base);

		PROPERTY_VAL_String();
		hr = pInst->set_address(v0);

		PROPERTY_SET_LEAVE();
	}

	inline v8::Handle<v8::Value> HttpRequest_base::s_get_queryString(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpRequest_base);

		hr = pInst->get_queryString(vr);

		METHOD_RETURN();
	}

	inline void HttpRequest_base::s_set_queryString(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpRequest_base);

		PROPERTY_VAL_String();
		hr = pInst->set_queryString(v0);

		PROPERTY_SET_LEAVE();
	}

	inline v8::Handle<v8::Value> HttpRequest_base::s_get_response(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<HttpResponse_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpRequest_base);

		hr = pInst->get_response(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> HttpRequest_base::s_get_cookie(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<HttpCollection_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpRequest_base);

		hr = pInst->get_cookie(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> HttpRequest_base::s_get_form(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<HttpCollection_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpRequest_base);

		hr = pInst->get_form(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> HttpRequest_base::s_get_query(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<HttpCollection_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpRequest_base);

		hr = pInst->get_query(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> HttpRequest_base::s__new(const v8::Arguments& args)
	{
		obj_ptr<HttpRequest_base> vr;

		CONSTRUCT_ENTER(0, 0);

		hr = _new(vr);

		CONSTRUCT_RETURN();
	}

}

#endif

