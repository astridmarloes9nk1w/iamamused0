/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _test_base_H_
#define _test_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class console_base;
class assert_base;
class Expect_base;

class test_base : public module_base
{
public:
	enum{
		_BDD = 0,
		_TDD = 1
	};

public:
	// test_base
	static result_t describe(const char* name, v8::Handle<v8::Function> block);
	static result_t xdescribe(const char* name, v8::Handle<v8::Function> block);
	static result_t it(const char* name, v8::Handle<v8::Function> block);
	static result_t xit(const char* name, v8::Handle<v8::Function> block);
	static result_t before(v8::Handle<v8::Function> func);
	static result_t after(v8::Handle<v8::Function> func);
	static result_t beforeEach(v8::Handle<v8::Function> func);
	static result_t afterEach(v8::Handle<v8::Function> func);
	static result_t run(int32_t loglevel);
	static result_t get_assert(obj_ptr<assert_base>& retVal);
	static result_t expect(v8::Handle<v8::Value> actual, const char* msg, obj_ptr<Expect_base>& retVal);
	static result_t setup(int32_t mode);
	static result_t get_slow(int32_t& retVal);
	static result_t set_slow(int32_t newVal);

	DECLARE_CLASSINFO(test_base);

public:
	static v8::Handle<v8::Value> s_get_BDD(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_TDD(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_describe(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_xdescribe(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_it(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_xit(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_before(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_after(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_beforeEach(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_afterEach(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_run(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_get_assert(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_expect(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_setup(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_get_slow(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_slow(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
};

}

#include "console.h"
#include "assert.h"
#include "Expect.h"

namespace fibjs
{
	inline ClassInfo& test_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"describe", s_describe, true},
			{"xdescribe", s_xdescribe, true},
			{"it", s_it, true},
			{"xit", s_xit, true},
			{"before", s_before, true},
			{"after", s_after, true},
			{"beforeEach", s_beforeEach, true},
			{"afterEach", s_afterEach, true},
			{"run", s_run, true},
			{"expect", s_expect, true},
			{"setup", s_setup, true}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"BDD", s_get_BDD, NULL, true},
			{"TDD", s_get_TDD, NULL, true},
			{"assert", s_get_assert, NULL, true},
			{"slow", s_get_slow, s_set_slow}
		};

		static ClassData s_cd = 
		{ 
			"test", NULL, 
			11, s_method, 0, NULL, 4, s_property, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> test_base::s_get_BDD(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _BDD;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> test_base::s_get_TDD(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _TDD;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> test_base::s_get_assert(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<assert_base> vr;

		PROPERTY_ENTER();

		hr = get_assert(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> test_base::s_get_slow(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();

		hr = get_slow(vr);

		METHOD_RETURN();
	}

	inline void test_base::s_set_slow(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_VAL(int32_t);

		hr = set_slow(v0);

		PROPERTY_SET_LEAVE();
	}

	inline v8::Handle<v8::Value> test_base::s_describe(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG(v8::Handle<v8::Function>, 1);

		hr = describe(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> test_base::s_xdescribe(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG(v8::Handle<v8::Function>, 1);

		hr = xdescribe(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> test_base::s_it(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG(v8::Handle<v8::Function>, 1);

		hr = it(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> test_base::s_xit(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG(v8::Handle<v8::Function>, 1);

		hr = xit(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> test_base::s_before(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = before(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> test_base::s_after(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = after(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> test_base::s_beforeEach(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = beforeEach(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> test_base::s_afterEach(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = afterEach(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> test_base::s_run(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 0);

		OPT_ARG(int32_t, 0, console_base::_ERROR);

		hr = run(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> test_base::s_expect(const v8::Arguments& args)
	{
		obj_ptr<Expect_base> vr;

		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Value>, 0);
		OPT_ARG_String(1, "");

		hr = expect(v0, v1, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> test_base::s_setup(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 0);

		OPT_ARG(int32_t, 0, _BDD);

		hr = setup(v0);

		METHOD_VOID();
	}

}

#endif

