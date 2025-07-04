/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _os_base_H_
#define _os_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;

class os_base : public module_base
{
public:
	// os_base
	static result_t get_hostname(std::string& retVal);
	static result_t get_type(std::string& retVal);
	static result_t get_version(std::string& retVal);
	static result_t get_arch(std::string& retVal);
	static result_t get_timezone(int32_t& retVal);
	static result_t uptime(double& retVal);
	static result_t loadavg(v8::Handle<v8::Array>& retVal);
	static result_t totalmem(int64_t& retVal);
	static result_t freemem(int64_t& retVal);
	static result_t CPUInfo(v8::Handle<v8::Array>& retVal);
	static result_t CPUs(int32_t& retVal);
	static result_t networkInfo(v8::Handle<v8::Object>& retVal);
	static result_t time(const char* tmString, date_t& retVal);
	static result_t get_execPath(std::string& retVal);
	static result_t memoryUsage(v8::Handle<v8::Object>& retVal);

	DECLARE_CLASSINFO(os_base);

public:
	static v8::Handle<v8::Value> s_get_hostname(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_type(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_version(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_arch(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_timezone(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_uptime(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_loadavg(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_totalmem(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_freemem(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_CPUInfo(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_CPUs(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_networkInfo(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_time(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_get_execPath(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_memoryUsage(const v8::Arguments& args);
};

}


namespace fibjs
{
	inline ClassInfo& os_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"uptime", s_uptime, true},
			{"loadavg", s_loadavg, true},
			{"totalmem", s_totalmem, true},
			{"freemem", s_freemem, true},
			{"CPUInfo", s_CPUInfo, true},
			{"CPUs", s_CPUs, true},
			{"networkInfo", s_networkInfo, true},
			{"time", s_time, true},
			{"memoryUsage", s_memoryUsage, true}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"hostname", s_get_hostname, NULL, true},
			{"type", s_get_type, NULL, true},
			{"version", s_get_version, NULL, true},
			{"arch", s_get_arch, NULL, true},
			{"timezone", s_get_timezone, NULL, true},
			{"execPath", s_get_execPath, NULL, true}
		};

		static ClassData s_cd = 
		{ 
			"os", NULL, 
			9, s_method, 0, NULL, 6, s_property, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> os_base::s_get_hostname(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();

		hr = get_hostname(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_get_type(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();

		hr = get_type(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_get_version(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();

		hr = get_version(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_get_arch(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();

		hr = get_arch(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_get_timezone(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();

		hr = get_timezone(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_get_execPath(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();

		hr = get_execPath(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_uptime(const v8::Arguments& args)
	{
		double vr;

		METHOD_ENTER(0, 0);

		hr = uptime(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_loadavg(const v8::Arguments& args)
	{
		v8::Handle<v8::Array> vr;

		METHOD_ENTER(0, 0);

		hr = loadavg(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_totalmem(const v8::Arguments& args)
	{
		int64_t vr;

		METHOD_ENTER(0, 0);

		hr = totalmem(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_freemem(const v8::Arguments& args)
	{
		int64_t vr;

		METHOD_ENTER(0, 0);

		hr = freemem(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_CPUInfo(const v8::Arguments& args)
	{
		v8::Handle<v8::Array> vr;

		METHOD_ENTER(0, 0);

		hr = CPUInfo(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_CPUs(const v8::Arguments& args)
	{
		int32_t vr;

		METHOD_ENTER(0, 0);

		hr = CPUs(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_networkInfo(const v8::Arguments& args)
	{
		v8::Handle<v8::Object> vr;

		METHOD_ENTER(0, 0);

		hr = networkInfo(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_time(const v8::Arguments& args)
	{
		date_t vr;

		METHOD_ENTER(1, 0);

		OPT_ARG_String(0, "");

		hr = time(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_memoryUsage(const v8::Arguments& args)
	{
		v8::Handle<v8::Object> vr;

		METHOD_ENTER(0, 0);

		hr = memoryUsage(vr);

		METHOD_RETURN();
	}

}

#endif

