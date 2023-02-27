/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _MemoryStream_base_H_
#define _MemoryStream_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Stream.h"

namespace fibjs
{

class Stream_base;
class io_base;

class MemoryStream_base : public Stream_base
{
public:
	// MemoryStream_base
	static result_t _new(obj_ptr<MemoryStream_base>& retVal);
	virtual result_t seek(double offset, int32_t whence) = 0;
	virtual result_t tell(double& retVal) = 0;
	virtual result_t rewind() = 0;
	virtual result_t size(double& retVal) = 0;
	virtual result_t clear() = 0;

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

protected:
	static v8::Handle<v8::Value> s__new(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_seek(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_tell(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_rewind(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_size(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_clear(const v8::Arguments& args);
};

}

#include "io.h"

namespace fibjs
{
	inline ClassInfo& MemoryStream_base::class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"seek", s_seek},
			{"tell", s_tell},
			{"rewind", s_rewind},
			{"size", s_size},
			{"clear", s_clear}
		};

		static ClassData s_cd = 
		{ 
			"MemoryStream", s__new, 
			5, s_method, 0, NULL, 0, NULL, NULL,
			&Stream_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline v8::Handle<v8::Value> MemoryStream_base::s__new(const v8::Arguments& args)
	{
		obj_ptr<MemoryStream_base> vr;

		CONSTRUCT_ENTER(0, 0);

		hr = _new(vr);

		CONSTRUCT_RETURN();
	}

	inline v8::Handle<v8::Value> MemoryStream_base::s_seek(const v8::Arguments& args)
	{
		METHOD_INSTANCE(MemoryStream_base);
		METHOD_ENTER(2, 1);

		ARG(double, 0);
		OPT_ARG(int32_t, 1, io_base::_SEEK_SET);

		hr = pInst->seek(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> MemoryStream_base::s_tell(const v8::Arguments& args)
	{
		double vr;

		METHOD_INSTANCE(MemoryStream_base);
		METHOD_ENTER(0, 0);

		hr = pInst->tell(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> MemoryStream_base::s_rewind(const v8::Arguments& args)
	{
		METHOD_INSTANCE(MemoryStream_base);
		METHOD_ENTER(0, 0);

		hr = pInst->rewind();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> MemoryStream_base::s_size(const v8::Arguments& args)
	{
		double vr;

		METHOD_INSTANCE(MemoryStream_base);
		METHOD_ENTER(0, 0);

		hr = pInst->size(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> MemoryStream_base::s_clear(const v8::Arguments& args)
	{
		METHOD_INSTANCE(MemoryStream_base);
		METHOD_ENTER(0, 0);

		hr = pInst->clear();

		METHOD_VOID();
	}

}

#endif

