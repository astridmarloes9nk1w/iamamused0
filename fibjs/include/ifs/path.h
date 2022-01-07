/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _path_base_H_
#define _path_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class path_base : public object_base
{
public:
	// path_base
	static result_t normalize(const char* path, std::string& retVal);
	static result_t basename(const char* path, const char* ext, std::string& retVal);
	static result_t extname(const char* path, std::string& retVal);
	static result_t dirname(const char* path, std::string& retVal);
	static result_t join(const v8::Arguments& args, std::string& retVal);

public:
	static ClassInfo& class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"normalize", s_normalize},
			{"basename", s_basename},
			{"extname", s_extname},
			{"dirname", s_dirname},
			{"join", s_join}
		};

		static ClassData s_cd = 
		{ 
			"path", NULL, 
			5, s_method, 0, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

private:
	static v8::Handle<v8::Value> s_normalize(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_basename(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_extname(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_dirname(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_join(const v8::Arguments& args);
};

}

namespace fibjs
{
	inline v8::Handle<v8::Value> path_base::s_normalize(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG_String(0);

		std::string vr;
		hr = normalize(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> path_base::s_basename(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG_String(0);
		OPT_ARG_String(1, "");

		std::string vr;
		hr = basename(v0, v1, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> path_base::s_extname(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG_String(0);

		std::string vr;
		hr = extname(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> path_base::s_dirname(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG_String(0);

		std::string vr;
		hr = dirname(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> path_base::s_join(const v8::Arguments& args)
	{
		METHOD_ENTER(-1, 0);

		std::string vr;
		hr = join(args, vr);

		METHOD_RETURN();
	}

}

#endif

