/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _ssl_base_H_
#define _ssl_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class SslSocket_base;
class Stream_base;
class Buffer_base;

class ssl_base : public module_base
{
public:
	// ssl_base
	static result_t connect(Stream_base* s, obj_ptr<SslSocket_base>& retVal, exlib::AsyncEvent* ac);
	static result_t accept(Stream_base* s, obj_ptr<SslSocket_base>& retVal, exlib::AsyncEvent* ac);
	static result_t loadCert(Buffer_base* DerCert);
	static result_t loadCert(const char* txtCert);
	static result_t loadCertFile(const char* filename);
	static result_t exportCert(v8::Local<v8::Array>& retVal);
	static result_t clearCert();
	static result_t loadCrl(const char* pemCrl);
	static result_t loadCrlFile(const char* filename);
	static result_t exportCrl(v8::Local<v8::Array>& retVal);
	static result_t clearCrl();

	DECLARE_CLASSINFO(ssl_base);

public:
	static void s_connect(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_accept(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_loadCert(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_loadCertFile(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_exportCert(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_clearCert(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_loadCrl(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_loadCrlFile(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_exportCrl(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_clearCrl(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
	ASYNC_STATICVALUE2(ssl_base, connect, Stream_base*, obj_ptr<SslSocket_base>);
	ASYNC_STATICVALUE2(ssl_base, accept, Stream_base*, obj_ptr<SslSocket_base>);
};

}

#include "SslSocket.h"
#include "Stream.h"
#include "Buffer.h"

namespace fibjs
{
	inline ClassInfo& ssl_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"connect", s_connect, true},
			{"accept", s_accept, true},
			{"loadCert", s_loadCert, true},
			{"loadCertFile", s_loadCertFile, true},
			{"exportCert", s_exportCert, true},
			{"clearCert", s_clearCert, true},
			{"loadCrl", s_loadCrl, true},
			{"loadCrlFile", s_loadCrlFile, true},
			{"exportCrl", s_exportCrl, true},
			{"clearCrl", s_clearCrl, true}
		};

		static ClassData::ClassObject s_object[] = 
		{
			{"Socket", SslSocket_base::class_info}
		};

		static ClassData s_cd = 
		{ 
			"ssl", NULL, 
			10, s_method, 1, s_object, 0, NULL, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void ssl_base::s_connect(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<SslSocket_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Stream_base>, 0);

		hr = ac_connect(v0, vr);

		METHOD_RETURN();
	}

	inline void ssl_base::s_accept(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<SslSocket_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Stream_base>, 0);

		hr = ac_accept(v0, vr);

		METHOD_RETURN();
	}

	inline void ssl_base::s_loadCert(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = loadCert(v0);

		METHOD_OVER(1, 1);

		ARG_String(0);

		hr = loadCert(v0);

		METHOD_VOID();
	}

	inline void ssl_base::s_loadCertFile(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = loadCertFile(v0);

		METHOD_VOID();
	}

	inline void ssl_base::s_exportCert(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Array> vr;

		METHOD_ENTER(0, 0);

		hr = exportCert(vr);

		METHOD_RETURN();
	}

	inline void ssl_base::s_clearCert(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(0, 0);

		hr = clearCert();

		METHOD_VOID();
	}

	inline void ssl_base::s_loadCrl(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = loadCrl(v0);

		METHOD_VOID();
	}

	inline void ssl_base::s_loadCrlFile(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = loadCrlFile(v0);

		METHOD_VOID();
	}

	inline void ssl_base::s_exportCrl(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Array> vr;

		METHOD_ENTER(0, 0);

		hr = exportCrl(vr);

		METHOD_RETURN();
	}

	inline void ssl_base::s_clearCrl(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(0, 0);

		hr = clearCrl();

		METHOD_VOID();
	}

}

#endif

