/*
 * Variant.cpp
 *
 *  Created on: Jul 31, 2012
 *      Author: lion
 */

#include <math.h>
#include "object.h"
#include <stdio.h>
#include <stdlib.h>

namespace fibjs
{

Variant& Variant::operator=(v8::Handle<v8::Object> v)
{
	clear();

	if (!v.IsEmpty())
	{
		set_type(VT_JSObject);

		if (isPersistent())
		{
			new (((v8::Persistent<v8::Object>*) m_Val.jsobjVal)) v8::Persistent<
					v8::Object>();
			((v8::Persistent<v8::Object>*) m_Val.jsobjVal)->Reset(isolate, v);
		}
		else
		{
			new (((v8::Handle<v8::Object>*) m_Val.jsobjVal)) v8::Handle<
					v8::Object>();
			*(v8::Handle<v8::Object>*) m_Val.jsobjVal = v;
		}
	}

	return *this;
}

Variant& Variant::operator=(v8::Handle<v8::Value> v)
{
	clear();

	if (!IsEmpty(v))
	{
		if (v->IsDate())
		{
			set_type(VT_Date);
			*(date_t*) m_Val.dateVal = v;
		}
		else if (v->IsBoolean())
		{
			set_type(VT_Boolean);
			m_Val.boolVal = v->BooleanValue();
		}
		else if (v->IsNumber())
		{

			double n = v->NumberValue();
			int64_t num = (int64_t) n;

			if (n == (double) num)
			{
				if (num >= -2147483648ll && num <= 2147483647ll)
				{
					set_type(VT_Integer);
					m_Val.intVal = (int32_t) num;
				}
				else
				{
					set_type(VT_Long);
					m_Val.longVal = num;
				}
			}
			else
			{
				set_type(VT_Number);
				m_Val.dblVal = n;
			}
		}
		else
		{
			if (v->IsObject())
			{
				object_base* obj = object_base::getInstance(v);

				if (obj)
					return operator=(obj);
				else
					return operator=(v8::Handle<v8::Object>::Cast(v));
			}
			else
			{
				v8::String::Utf8Value s(v);
				std::string str(*s, s.length());
				return operator=(str);
			}
		}
	}

	return *this;
}

Variant::operator v8::Handle<v8::Value>() const
{
	switch (type())
	{
	case VT_Undefined:
		return v8::Undefined();
	case VT_Null:
	case VT_Type:
	case VT_Persistent:
		return v8::Null();
	case VT_Boolean:
		return m_Val.boolVal ? v8::True() : v8::False();
	case VT_Integer:
		return v8::Int32::New(m_Val.intVal);
	case VT_Long:
		return v8::Number::New((double) m_Val.longVal);
	case VT_Number:
		return v8::Number::New(m_Val.dblVal);
	case VT_Date:
		return *(date_t*) m_Val.dateVal;
	case VT_Object:
	{
		object_base* obj = (object_base*) m_Val.objVal;

		if (obj == NULL)
			break;

		return obj->wrap();
	}
	case VT_JSObject:
	{
		v8::Handle<v8::Value> v;

		if (isPersistent())
			v = v8::Handle<v8::Object>::New(isolate,
					*(v8::Persistent<v8::Object>*) m_Val.jsobjVal);
		else
			v = *(v8::Handle<v8::Object>*) m_Val.jsobjVal;

		return v;
	}
	case VT_String:
	{
		std::string& str = *(std::string*) m_Val.strVal;
		return v8::String::New(str.c_str(), (int) str.length());
	}
	}

	return v8::Null();
}

inline void next(int &len, int& pos)
{
	pos++;
	if (len > 0)
		len--;
}

inline int64_t getInt(const char* s, int &len, int& pos)
{
	char ch;
	int64_t n = 0;

	while (len && (ch = s[pos]) && ch >= '0' && ch <= '9')
	{
		n = n * 10 + ch - '0';
		next(len, pos);
	}

	return n;
}

inline char pick(const char* s, int &len, int& pos)
{
	return len == 0 ? 0 : s[pos];
}

void Variant::parseNumber(const char* str, int len)
{
	int64_t digit, frac = 0, exp = 0;
	double v, div = 1.0;
	bool bNeg, bExpNeg;
	int pos = 0;
	char ch;

	bNeg = (pick(str, len, pos) == '-');
	if (bNeg)
		next(len, pos);

	digit = getInt(str, len, pos);

	ch = pick(str, len, pos);
	if (ch == '.')
	{
		next(len, pos);

		while ((ch = pick(str, len, pos)) && ch >= '0' && ch <= '9')
		{
			frac = frac * 10 + ch - '0';
			div *= 10;
			next(len, pos);
		}
	}

	if (ch == 'e' || ch == 'E')
	{
		next(len, pos);

		bExpNeg = false;
		ch = pick(str, len, pos);
		if (ch == '+')
			next(len, pos);
		else if (ch == '-')
		{
			bExpNeg = true;
			next(len, pos);
		}

		exp = getInt(str, len, pos);
		if (bExpNeg)
			exp = -exp;
	}

	if (div > 1 || exp != 0)
	{
		v = digit + (frac / div);
		if (bNeg)
			v = -v;

		if (exp != 0)
			v *= pow((double) 10, (int) exp);

		set_type(VT_Number);
		m_Val.dblVal = v;

		return;
	}

	if (bNeg)
		digit = -digit;

	if (digit >= -2147483648ll && digit <= 2147483647ll)
	{
		set_type(VT_Integer);
		m_Val.intVal = (int32_t) digit;
	}
	else
	{
		set_type(VT_Long);
		m_Val.longVal = digit;
	}
}

#define STRING_BUF_SIZE	1024

bool Variant::toString(std::string& retVal)
{
	switch (type())
	{
	case VT_Undefined:
		retVal = "undefined";
		return false;
	case VT_Null:
	case VT_Type:
	case VT_Persistent:
		retVal = "null";
		return false;
	case VT_Boolean:
		retVal = m_Val.boolVal ? "true" : "false";
		return true;
	case VT_Integer:
	{
		char str[STRING_BUF_SIZE];

		sprintf(str, "%d", m_Val.intVal);
		retVal = str;

		return true;
	}
	case VT_Long:
	{
		char str[STRING_BUF_SIZE];

		sprintf(str, "%lld", m_Val.longVal);
		retVal = str;

		return true;
	}
	case VT_Number:
	{
		char str[STRING_BUF_SIZE];

		sprintf(str, "%.16g", m_Val.dblVal);
		retVal = str;

		return true;
	}
	case VT_Date:
		((date_t*) m_Val.dateVal)->toString(retVal);
		return true;
	case VT_Object:
		return false;
	case VT_String:
		retVal = *(std::string*) m_Val.strVal;
		return true;
	case VT_JSObject:
		return false;
	}

	return false;
}

}
