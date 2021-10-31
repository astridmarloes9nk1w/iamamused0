/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _stream_base_H_
#define _stream_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "if.h"

namespace fibjs
{

class Buffer_base;

class stream_base : public object_base
{
public:
	static const int32_t FSEEK_SET = 0;
	static const int32_t FSEEK_CUR = 1;
	static const int32_t FSEEK_END = 2;

public:
	// stream_base
	virtual result_t get_readable(bool& retVal) = 0;
	virtual result_t get_writable(bool& retVal) = 0;
	virtual result_t read(int32_t size, obj_ptr<Buffer_base>& retVal) = 0;
	virtual result_t write(obj_ptr<Buffer_base> data) = 0;

public:
	static ClassInfo& info()
	{
		static ClassMethod s_method[] = 
		{
			{"read", s_read},
			{"write", s_write}
		};

		static ClassProperty s_property[] = 
		{
			{"FSEEK_SET", s_get_FSEEK_SET},
			{"FSEEK_CUR", s_get_FSEEK_CUR},
			{"FSEEK_END", s_get_FSEEK_END},
			{"readable", s_get_readable},
			{"writable", s_get_writable}
		};

		static ClassData s_cd = 
		{ 
			"stream", NULL, 
			2, s_method, 5, s_property, NULL,
			&object_base::info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	virtual v8::Handle<v8::Value> JSObject()
	{
		return wrap(info());
	}

private:
	static v8::Handle<v8::Value> s_get_FSEEK_SET(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_FSEEK_CUR(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_FSEEK_END(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_readable(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_writable(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_read(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_write(const v8::Arguments& args);
};

}

#include "Buffer.h"

namespace fibjs
{
	inline v8::Handle<v8::Value> stream_base::s_get_FSEEK_SET(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();

		int32_t vr = FSEEK_SET;

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> stream_base::s_get_FSEEK_CUR(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();

		int32_t vr = FSEEK_CUR;

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> stream_base::s_get_FSEEK_END(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();

		int32_t vr = FSEEK_END;

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> stream_base::s_get_readable(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(stream_base);

		bool vr;
		hr = pInst->get_readable(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> stream_base::s_get_writable(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(stream_base);

		bool vr;
		hr = pInst->get_writable(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> stream_base::s_read(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 0);
		METHOD_INSTANCE(stream_base);

		OPT_ARG(int32_t, 0, -1);

		obj_ptr<Buffer_base> vr;
		hr = pInst->read(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> stream_base::s_write(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);
		METHOD_INSTANCE(stream_base);

		ARG_CLASS(Buffer_base, 0);

		hr = pInst->write(v0);

		METHOD_VOID();
	}

}

#endif

