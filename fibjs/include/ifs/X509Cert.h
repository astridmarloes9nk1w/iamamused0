/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _X509Cert_base_H_
#define _X509Cert_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Buffer_base;

class X509Cert_base : public object_base
{
public:
	// X509Cert_base
	static result_t _new(obj_ptr<X509Cert_base>& retVal);
	virtual result_t load(Buffer_base* DerCrt) = 0;
	virtual result_t load(const char* pemCrt) = 0;

	DECLARE_CLASSINFO(X509Cert_base);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_load(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "Buffer.h"

namespace fibjs
{
	inline ClassInfo& X509Cert_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"load", s_load}
		};

		static ClassData s_cd = 
		{ 
			"X509Cert", s__new, 
			1, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void X509Cert_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<X509Cert_base> vr;

		CONSTRUCT_ENTER(0, 0);

		hr = _new(vr);

		CONSTRUCT_RETURN();
	}

	inline void X509Cert_base::s_load(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(X509Cert_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->load(v0);

		METHOD_OVER(1, 1);

		ARG_String(0);

		hr = pInst->load(v0);

		METHOD_VOID();
	}

}

#endif

