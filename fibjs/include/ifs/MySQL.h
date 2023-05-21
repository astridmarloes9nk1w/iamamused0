/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _MySQL_base_H_
#define _MySQL_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "DbConnection.h"

namespace fibjs
{

class DbConnection_base;

class MySQL_base : public DbConnection_base
{
public:
	// MySQL_base
	virtual result_t get_rxBufferSize(int32_t& retVal) = 0;
	virtual result_t get_txBufferSize(int32_t& retVal) = 0;

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		result_t hr = DbConnection_base::toJSON(key, retVal);
		if(hr < 0)return hr;

		CLONE(rxBufferSize, int32_t);
		CLONE(txBufferSize, int32_t);

		return 0;
	}

public:
	static v8::Handle<v8::Value> s_get_rxBufferSize(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_txBufferSize(v8::Local<v8::String> property, const v8::AccessorInfo &info);
};

}


namespace fibjs
{
	inline ClassInfo& MySQL_base::class_info()
	{
		static ClassData::ClassProperty s_property[] = 
		{
			{"rxBufferSize", s_get_rxBufferSize},
			{"txBufferSize", s_get_txBufferSize}
		};

		static ClassData s_cd = 
		{ 
			"MySQL", NULL, 
			0, NULL, 0, NULL, 2, s_property, NULL, NULL,
			&DbConnection_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> MySQL_base::s_get_rxBufferSize(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(MySQL_base);

		hr = pInst->get_rxBufferSize(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> MySQL_base::s_get_txBufferSize(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(MySQL_base);

		hr = pInst->get_txBufferSize(vr);

		METHOD_RETURN();
	}

}

#endif

