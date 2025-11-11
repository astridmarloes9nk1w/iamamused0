/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _DBRow_base_H_
#define _DBRow_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class DBRow_base : public object_base
{
public:
	// DBRow_base
	virtual result_t _indexed_getter(uint32_t index, v8::Handle<v8::Value>& retVal) = 0;
	virtual result_t _named_getter(const char* property, v8::Handle<v8::Value>& retVal) = 0;
	virtual result_t _named_enumerator(v8::Handle<v8::Array>& retVal) = 0;

	DECLARE_CLASSINFO(DBRow_base);

public:
	static v8::Handle<v8::Value> i_IndexedGetter(uint32_t index, const v8::AccessorInfo& info);
	static v8::Handle<v8::Value> i_NamedGetter(v8::Local<v8::String> property, const v8::AccessorInfo& info);
	static v8::Handle<v8::Array> i_NamedEnumerator(const v8::AccessorInfo& info);
};

}

namespace fibjs
{
	inline ClassInfo& DBRow_base::class_info()
	{
		static ClassData::ClassIndexed s_indexed = 
		{
			i_IndexedGetter, i_IndexedSetter
		};

		static ClassData::ClassNamed s_named = 
		{
			i_NamedGetter, i_NamedSetter, i_NamedDeleter, i_NamedEnumerator
		};

		static ClassData s_cd = 
		{ 
			"DBRow", NULL, 
			0, NULL, 0, NULL, 0, NULL, &s_indexed, &s_named,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> DBRow_base::i_IndexedGetter(uint32_t index, const v8::AccessorInfo& info)
	{
		v8::Handle<v8::Value> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(DBRow_base);

		hr = pInst->_indexed_getter(index, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> DBRow_base::i_NamedGetter(v8::Local<v8::String> property, const v8::AccessorInfo& info)
	{
		v8::Handle<v8::Value> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(DBRow_base);

		v8::String::Utf8Value k(property);
		if(class_info().has(*k))return v8::Handle<v8::Value>();

		hr = pInst->_named_getter(*k, vr);
		if(hr == CALL_RETURN_NULL)return v8::Handle<v8::Value>();

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Array> DBRow_base::i_NamedEnumerator(const v8::AccessorInfo& info)
	{
		v8::Handle<v8::Array> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(DBRow_base);

		hr = pInst->_named_enumerator(vr);

		METHOD_RETURN1();
	}

}

#endif

