/*
 * HttpCollection.h
 *
 *  Created on: Aug 9, 2012
 *      Author: lion
 */

#include "ifs/HttpCollection.h"
#include "QuickArray.h"

#ifndef HTTPCOLLECTION_H_
#define HTTPCOLLECTION_H_

namespace fibjs
{

class HttpCollection: public HttpCollection_base
{
public:
	HttpCollection() :
			m_count(0)
	{
	}

public:
	// HttpCollection_base
	virtual result_t clear();
	virtual result_t has(const char* name, bool& retVal);
	virtual result_t first(const char* name, std::string& retVal);
	virtual result_t all(const char* name, v8::Handle<v8::Array>& retVal);
	virtual result_t add(const char* name, const char* value);
	virtual result_t add(v8::Handle<v8::Object> map);
	virtual result_t set(const char* name, const char* value);
	virtual result_t set(v8::Handle<v8::Object> map);
	virtual result_t remove(const char* name);
	virtual result_t _named_getter(const char* property, std::string& retVal);
	virtual result_t _named_setter(const char* property, const char* newVal);

private:
	QuickArray<std::string> m_array;
	int32_t m_count;
};

} /* namespace fibjs */
#endif /* HTTPCOLLECTION_H_ */
