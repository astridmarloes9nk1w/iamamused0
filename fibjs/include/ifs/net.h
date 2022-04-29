/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _net_base_H_
#define _net_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;

class net_base : public module_base
{
public:
	static const int32_t AF_INET = 2;
	static const int32_t AF_INET6 = 10;
	static const int32_t SOCK_STREAM = 1;
	static const int32_t SOCK_DGRAM = 2;

public:
	static ClassInfo& class_info()
	{
		static ClassProperty s_property[] = 
		{
			{"AF_INET", s_get_AF_INET},
			{"AF_INET6", s_get_AF_INET6},
			{"SOCK_STREAM", s_get_SOCK_STREAM},
			{"SOCK_DGRAM", s_get_SOCK_DGRAM}
		};

		static ClassData s_cd = 
		{ 
			"net", NULL, 
			0, NULL, 4, s_property, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

private:
	static v8::Handle<v8::Value> s_get_AF_INET(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_AF_INET6(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_SOCK_STREAM(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_SOCK_DGRAM(v8::Local<v8::String> property, const v8::AccessorInfo &info);
};

}


namespace fibjs
{
	inline v8::Handle<v8::Value> net_base::s_get_AF_INET(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = AF_INET;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> net_base::s_get_AF_INET6(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = AF_INET6;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> net_base::s_get_SOCK_STREAM(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = SOCK_STREAM;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> net_base::s_get_SOCK_DGRAM(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = SOCK_DGRAM;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

}

#endif

