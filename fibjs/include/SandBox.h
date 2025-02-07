/*
 * SandBox.h
 *
 *  Created on: Oct 22, 2012
 *      Author: lion
 */

#include "ifs/SandBox.h"
#include <map>

#ifndef SANDBOX_H_
#define SANDBOX_H_

namespace fibjs
{

class SandBox: public fibjs::SandBox_base
{
public:
	class mod: public obj_base
	{
	public:
		~mod()
		{
			m_mod.Dispose();
		}

	public:
		v8::Persistent<v8::Value> m_mod;
		date_t m_mtime;
		date_t m_check;
	};

public:
	// SandBox_base
	virtual result_t add(const char* id, v8::Handle<v8::Value> mod);
	virtual result_t add(v8::Handle<v8::Object> mods);
	virtual result_t remove(const char* id);
	virtual result_t run(const char* fname);
	virtual result_t require(const char* id, v8::Handle<v8::Value>& retVal);

public:
	void initRoot();

private:
	void InstallModule(std::string fname, v8::Handle<v8::Value> o, date_t d,
			date_t now);
	result_t runScript(const char* id, v8::Handle<v8::Value>& retVal,
			bool bMod);

	inline void InstallNativeModule(const char* fname, ClassInfo& ci,
			date_t now)
	{
		add(fname, ci.CreateInstance());
	}

private:
	std::map<std::string, obj_ptr<mod> > m_mods;
};

} /* namespace fibjs */
#endif /* SANDBOX_H_ */
