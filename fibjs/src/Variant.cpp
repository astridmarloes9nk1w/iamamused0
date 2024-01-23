/*
 * Variant.cpp
 *
 *  Created on: Jul 31, 2012
 *      Author: lion
 */

#include <math.h>
#include "object.h"

namespace fibjs
{

bool Variant::assign(v8::Handle<v8::Value> v)
{
	clear();

	if (!IsEmpty(v))
	{
		if (v->IsDate())
		{
			m_type = VT_Date;
			*(date_t*) m_Val.dateVal = v;
		}
		else if (v->IsBoolean())
		{
			m_type = VT_Boolean;
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
					m_type = VT_Integer;
					m_Val.intVal = (int32_t) num;
				}
				else
				{
					m_type = VT_Long;
					m_Val.longVal = num;
				}
			}
			else
			{
				m_type = VT_Number;
				m_Val.dblVal = n;
			}
		}
		else
		{
			if (v->IsObject())
			{
				object_base* obj = object_base::getInstance(v);

				if (obj)
					operator=(obj);
				else
					return false;
			}
			else
			{
				v8::String::Utf8Value s(v);
				std::string str(*s, s.length());
				operator=(str);
			}
		}
	}

	return true;
}

Variant::operator v8::Handle<v8::Value>() const
{
	switch (m_type)
	{
	case VT_Null:
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

	if (bNeg = (pick(str, len, pos) == '-'))
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

		m_type = VT_Number;
		m_Val.dblVal = v;

		return;
	}

	if (bNeg)
		digit = -digit;

	if (digit >= -2147483648ll && digit <= 2147483647ll)
	{
		m_type = VT_Integer;
		m_Val.intVal = (int32_t) digit;
	}
	else
	{
		m_type = VT_Long;
		m_Val.longVal = digit;
	}
}

}
