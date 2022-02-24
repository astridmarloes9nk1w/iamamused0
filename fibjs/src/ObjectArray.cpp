/*
 * ObjectArray.cpp
 *
 *  Created on: Apr 11, 2012
 *      Author: lion
 */

#include "ObjectArray.h"

namespace fibjs
{

result_t ObjectArray::_indexed_getter(uint32_t index, obj_ptr<object_base>& retVal)
{
	if(index >= m_array.size())
		return CALL_E_BADINDEX;

	retVal = m_array[index];

	return 0;
}

result_t ObjectArray::_indexed_setter(uint32_t index, obj_ptr<object_base> newVal)
{
	if(index >= m_array.size())
		return CALL_E_BADINDEX;

	m_array[index] = newVal;

	return 0;
}

result_t ObjectArray::get_length(int32_t& retVal)
{
	retVal = m_array.size();
	return 0;
}

result_t ObjectArray::resize(int32_t sz)
{
	m_array.reserve(sz);
	return 0;
}

result_t ObjectArray::slice(int32_t start, int32_t end, obj_ptr<ObjectArray_base>& retVal)
{
	return 0;
}

result_t ObjectArray::toJSON(const char* key, v8::Handle<v8::Object>& retVal)
{
	v8::Handle<v8::Array> a = v8::Array::New(m_array.size());
	int i;

	for(i = 0; i < (int)m_array.size(); i ++)
		if(m_array[i])
			a->Set(i, m_array[i]->wrap());

	retVal = a;

	return 0;
}

} /* namespace fibjs */
