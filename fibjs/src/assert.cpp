/*
 * assert.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: lion
 */

#include "ifs/assert.h"
#include "ifs/encoding.h"
#include <log4cpp/Category.hh>
#include <sstream>

namespace fibjs {

static bool s_bThrow;

result_t assert_base::throwAssert(bool bThrow) {
	s_bThrow = bThrow;
	return 0;
}

class _msg {
public:
	_msg(const char * s0, const char * s1, v8::Handle<v8::Value>& v1,
			const char * s2) {
		if (*s0) {
			strs[0] = s0;
			strs[1] = NULL;
		} else {
			strs[0] = s1;
			strs[1] = s2;
			strs[2] = NULL;

			vs[0] = &v1;
		}
	}

	_msg(const char * s0, const char * s1, v8::Handle<v8::Value>& v1,
			const char * s2, v8::Handle<v8::Value>& v2, const char * s3 = "") {
		if (*s0) {
			strs[0] = s0;
			strs[1] = NULL;
		} else {
			strs[0] = s1;
			strs[1] = s2;
			strs[2] = s3;
			strs[3] = NULL;

			vs[0] = &v1;
			vs[1] = &v2;
		}
	}

	_msg(const char * s0, const char * s1, v8::Handle<v8::Value>& v1,
			const char * s2, v8::Handle<v8::Value>& v2, const char * s3,
			v8::Handle<v8::Value>& v3, const char * s4 = "") {
		if (*s0) {
			strs[0] = s0;
			strs[1] = NULL;
		} else {
			strs[0] = s1;
			strs[1] = s2;
			strs[2] = s3;
			strs[3] = s4;

			vs[0] = &v1;
			vs[1] = &v2;
			vs[2] = &v3;
		}
	}

	std::string str() {
		std::string str;
		std::string s;

		str = strs[0];

		if (strs[1]) {
			encoding_base::jsonEncode(*vs[0], s);
			str.append(s);
			str.append(strs[1]);

			if (strs[2]) {
				encoding_base::jsonEncode(*vs[1], s);
				str.append(s);
				str.append(strs[2]);

				if (strs[3]) {
					encoding_base::jsonEncode(*vs[2], s);
					str.append(s);
					str.append(strs[3]);
				}
			}
		}

		return str;
	}

private:
	const char * strs[4];
	v8::Handle<v8::Value>* vs[3];
};

inline void _test(bool value, _msg msg) {
	if (!value) {
		std::string str("assert: ");

		str.append(msg.str());

		if (s_bThrow)
			ThrowError(str.c_str());
		else {
			str.append(traceInfo());
			asyncLog(log4cpp::Priority::WARN, str.c_str());
		}
	}
}

inline void _throw(const char* msg) {
	std::stringstream strBuffer;

	strBuffer << "assert: " << msg;

	if (s_bThrow)
		ThrowError(strBuffer.str().c_str());
	else {
		strBuffer << traceInfo();
		asyncLog(log4cpp::Priority::WARN, strBuffer.str());
	}
}

result_t assert_base::ok(v8::Handle<v8::Value> actual, const char* msg) {
	_test(actual->Equals(v8::True()),
			_msg(msg, "expected ", actual, " to be truthy"));
	return 0;
}

result_t assert_base::notOk(v8::Handle<v8::Value> actual, const char* msg) {
	_test(!actual->Equals(v8::True()),
			_msg(msg, "expected ", actual, " to be falsy"));
	return 0;
}

bool regexpEquals(v8::Handle<v8::Value> actual,
		v8::Handle<v8::Value> expected) {
	v8::Handle<v8::RegExp> re1 = v8::Handle<v8::RegExp>::Cast(actual);
	v8::Handle<v8::String> src1 = re1->GetSource();
	v8::RegExp::Flags flgs1 = re1->GetFlags();

	v8::Handle<v8::RegExp> re2 = v8::Handle<v8::RegExp>::Cast(expected);
	v8::Handle<v8::String> src2 = re2->GetSource();
	v8::RegExp::Flags flgs2 = re2->GetFlags();

	return src1->StrictEquals(src2) && flgs1 == flgs2;
}

bool deepEquals(v8::Handle<v8::Value> actual, v8::Handle<v8::Value> expected);

bool arrayEqual(v8::Handle<v8::Value> actual, v8::Handle<v8::Value> expected) {
	v8::Handle<v8::Array> act = v8::Handle<v8::Array>::Cast(actual);
	v8::Handle<v8::Array> exp = v8::Handle<v8::Array>::Cast(expected);
	int len = (int) act->Length();
	int i;

	if (len != (int) exp->Length())
		return false;

	for (i = 0; i < len; i++)
		if (!deepEquals(act->Get(i), exp->Get(i)))
			return false;

	return true;
}

bool objectEquals(v8::Handle<v8::Value> actual,
		v8::Handle<v8::Value> expected) {
	if (actual->IsArray() && expected->IsArray())
		return arrayEqual(actual, expected);

	v8::Handle<v8::Object> act = v8::Handle<v8::Object>::Cast(actual);
	v8::Handle<v8::Object> exp = v8::Handle<v8::Object>::Cast(expected);

	v8::Handle<v8::Array> keys = act->GetPropertyNames();
	int len = (int) keys->Length();
	int i;

	if (!arrayEqual(keys, exp->GetPropertyNames()))
		return false;

	for (i = 0; i < len; i++) {
		v8::Handle<v8::Value> ks = keys->Get(i);

		if (!ks->IsNumber()) {
			v8::Handle<v8::String> k = v8::Handle<v8::String>::Cast(ks);

			if (!deepEquals(act->Get(k), exp->Get(k)))
				return false;
		}
	}

	return true;
}

bool deepEquals(v8::Handle<v8::Value> actual, v8::Handle<v8::Value> expected) {
	if (!IsEmpty(actual) && !IsEmpty(expected) && !actual->IsFunction()
			&& !expected->IsFunction()) {
		if (actual->IsDate() && expected->IsDate())
			return actual->NumberValue() == expected->NumberValue();
		else if (actual->IsRegExp() && expected->IsRegExp())
			return regexpEquals(actual, expected);
		else if (actual->IsObject() && expected->IsObject())
			return objectEquals(actual, expected);
	}

	return actual->Equals(expected);
}

bool valueEquals(v8::Handle<v8::Value> actual, v8::Handle<v8::Value> expected,
		bool bStrict) {
	if (!IsEmpty(actual) && !IsEmpty(expected) && !actual->IsFunction()
			&& !expected->IsFunction()) {
		if (actual->IsDate() && expected->IsDate())
			return actual->NumberValue() == expected->NumberValue();
		else if (actual->IsRegExp() && expected->IsRegExp())
			return regexpEquals(actual, expected);
	}

	return bStrict ? actual->StrictEquals(expected) : actual->Equals(expected);
}

result_t assert_base::equal(v8::Handle<v8::Value> actual,
		v8::Handle<v8::Value> expected, const char* msg) {
	_test(valueEquals(actual, expected, false),
			_msg(msg, "expected ", actual, " to equal ", expected));
	return 0;
}

result_t assert_base::notEqual(v8::Handle<v8::Value> actual,
		v8::Handle<v8::Value> expected, const char* msg) {
	_test(!valueEquals(actual, expected, false),
			_msg(msg, "expected ", actual, " to not equal ", expected));
	return 0;
}

result_t assert_base::strictEqual(v8::Handle<v8::Value> actual,
		v8::Handle<v8::Value> expected, const char* msg) {
	_test(valueEquals(actual, expected, true),
			_msg(msg, "expected ", actual, " to strictly equal ", expected));
	return 0;
}

result_t assert_base::notStrictEqual(v8::Handle<v8::Value> actual,
		v8::Handle<v8::Value> expected, const char* msg) {
	_test(!valueEquals(actual, expected, true),
			_msg(msg, "expected ", actual, " to not strictly equal ",
					expected));
	return 0;
}

result_t assert_base::deepEqual(v8::Handle<v8::Value> actual,
		v8::Handle<v8::Value> expected, const char* msg) {
	_test(deepEquals(actual, expected),
			_msg(msg, "expected ", actual, " to deeply equal ", expected));
	return 0;
}

result_t assert_base::notDeepEqual(v8::Handle<v8::Value> actual,
		v8::Handle<v8::Value> expected, const char* msg) {
	_test(!deepEquals(actual, expected),
			_msg(msg, "expected ", actual, " to not deeply equal ", expected));
	return 0;
}

result_t assert_base::closeTo(v8::Handle<v8::Value> actual,
		v8::Handle<v8::Value> expected, v8::Handle<v8::Value> delta,
		const char* msg) {
	double n, n1;

	n = actual->NumberValue();
	if (isnan(n))
		return CALL_E_INVALIDARG;

	n1 = expected->NumberValue();
	if (isnan(n1))
		return CALL_E_INVALIDARG;

	n -= n1;

	n1 = delta->NumberValue();
	if (isnan(n1))
		return CALL_E_INVALIDARG;

	if (n < 0)
		n = -n;
	_test(n <= n1,
			_msg(msg, "expected ", actual, " to be close to ", expected,
					" +/- ", delta));
	return 0;
}

double valcmp(v8::Handle<v8::Value>& val1, v8::Handle<v8::Value>& val2) {
	bool n1 = val1->IsNumber();
	bool n2 = val2->IsNumber();
	double v1;

	if (n1 && n2)
		return val1->NumberValue() - val2->NumberValue();

	if (n1) {
		v1 = val2->NumberValue();
		if (!isnan(v1))
			return val1->NumberValue() - v1;
	} else if (n2) {
		v1 = val1->NumberValue();
		if (!isnan(v1))
			return v1 - val2->NumberValue();
	}

	v8::String::Utf8Value s1(val1);
	v8::String::Utf8Value s2(val2);

	if (!*s1 || !*s2)
		return NAN;

	return qstrcmp(*s1, *s2);
}

result_t assert_base::lessThan(v8::Handle<v8::Value> actual,
		v8::Handle<v8::Value> expected, const char* msg) {
	double r = valcmp(actual, expected);

	if (isnan(r))
		return CALL_E_INVALIDARG;

	_test(r < 0, _msg(msg, "expected ", actual, " to be below ", expected));
	return 0;
}

result_t assert_base::greaterThan(v8::Handle<v8::Value> actual,
		v8::Handle<v8::Value> expected, const char* msg) {
	double r = valcmp(actual, expected);

	if (isnan(r))
		return CALL_E_INVALIDARG;

	_test(r > 0, _msg(msg, "expected ", actual, " to be above ", expected));
	return 0;
}

result_t assert_base::isTrue(v8::Handle<v8::Value> actual, const char* msg) {
	_test(actual->IsTrue(), _msg(msg, "expected ", actual, " to be true"));
	return 0;
}

result_t assert_base::isFalse(v8::Handle<v8::Value> actual, const char* msg) {
	_test(actual->IsFalse(), _msg(msg, "expected ", actual, " to be false"));
	return 0;
}

result_t assert_base::isNull(v8::Handle<v8::Value> actual, const char* msg) {
	_test(actual->IsNull(), _msg(msg, "expected ", actual, " to be null"));
	return 0;
}

result_t assert_base::isNotNull(v8::Handle<v8::Value> actual, const char* msg) {
	_test(!actual->IsNull(), _msg(msg, "expected ", actual, " not to be null"));
	return 0;
}

result_t assert_base::isUndefined(v8::Handle<v8::Value> actual,
		const char* msg) {
	_test(actual->IsUndefined(),
			_msg(msg, "expected ", actual, " to be undefined"));
	return 0;
}

result_t assert_base::isDefined(v8::Handle<v8::Value> actual, const char* msg) {
	_test(!actual->IsUndefined(),
			_msg(msg, "expected ", actual, " not to be undefined"));
	return 0;
}

result_t assert_base::isFunction(v8::Handle<v8::Value> actual,
		const char* msg) {
	_test(actual->IsFunction(),
			_msg(msg, "expected ", actual, " to be function"));
	return 0;
}

result_t assert_base::isNotFunction(v8::Handle<v8::Value> actual,
		const char* msg) {
	_test(!actual->IsFunction(),
			_msg(msg, "expected ", actual, " not to be function"));
	return 0;
}

result_t assert_base::isObject(v8::Handle<v8::Value> actual, const char* msg) {
	_test(actual->IsObject(), _msg(msg, "expected ", actual, " to be object"));
	return 0;
}

result_t assert_base::isNotObject(v8::Handle<v8::Value> actual,
		const char* msg) {
	_test(!actual->IsObject(),
			_msg(msg, "expected ", actual, " not to be object"));
	return 0;
}

result_t assert_base::isArray(v8::Handle<v8::Value> actual, const char* msg) {
	_test(actual->IsArray(), _msg(msg, "expected ", actual, " to be array"));
	return 0;
}

result_t assert_base::isNotArray(v8::Handle<v8::Value> actual,
		const char* msg) {
	_test(!actual->IsArray(),
			_msg(msg, "expected ", actual, " not to be array"));
	return 0;
}

result_t assert_base::isString(v8::Handle<v8::Value> actual, const char* msg) {
	_test(actual->IsString(), _msg(msg, "expected ", actual, " to be string"));
	return 0;
}

result_t assert_base::isNotString(v8::Handle<v8::Value> actual,
		const char* msg) {
	_test(!actual->IsString(),
			_msg(msg, "expected ", actual, " not to be string"));
	return 0;
}

result_t assert_base::isNumber(v8::Handle<v8::Value> actual, const char* msg) {
	_test(actual->IsNumber(), _msg(msg, "expected ", actual, " to be number"));
	return 0;
}

result_t assert_base::isNotNumber(v8::Handle<v8::Value> actual,
		const char* msg) {
	_test(!actual->IsNumber(),
			_msg(msg, "expected ", actual, " not to be number"));
	return 0;
}

result_t assert_base::isBoolean(v8::Handle<v8::Value> actual, const char* msg) {
	_test(actual->IsBoolean(),
			_msg(msg, "expected ", actual, " to be boolean"));
	return 0;
}

result_t assert_base::isNotBoolean(v8::Handle<v8::Value> actual,
		const char* msg) {
	_test(!actual->IsBoolean(),
			_msg(msg, "expected ", actual, " not to be boolean"));
	return 0;
}

result_t assert_base::property(v8::Handle<v8::Value> object,
		v8::Handle<v8::Value> prop, const char* msg) {
	if (!object->IsObject() || !prop->IsString())
		return CALL_E_INVALIDARG;

	v8::Handle<v8::Object> v = v8::Handle<v8::Object>::Cast(object);
	_test(v->Has(prop), _msg(msg, "expected ", object, " to have a ", prop));
	return 0;
}

result_t assert_base::notProperty(v8::Handle<v8::Value> object,
		v8::Handle<v8::Value> prop, const char* msg) {
	if (!object->IsObject() || !prop->IsString())
		return CALL_E_INVALIDARG;

	v8::Handle<v8::Object> v = v8::Handle<v8::Object>::Cast(object);
	_test(!v->Has(prop),
			_msg(msg, "expected ", object, " to not have a ", prop));
	return 0;
}

result_t assert_base::throws(v8::Handle<v8::Function> block, const char* msg) {
	v8::TryCatch try_catch;
	block->Call(block, 0, NULL);
	if (!try_catch.HasCaught()) {
		std::string str = "Missing expected exception. ";
		str += msg;
		_throw(str.c_str());
	}

	return 0;
}

result_t assert_base::doesNotThrow(v8::Handle<v8::Function> block,
		const char* msg) {
	v8::TryCatch try_catch;
	block->Call(block, 0, NULL);
	if (try_catch.HasCaught()) {
		std::string str = "Got unwanted exception. ";
		str += msg;
		_throw(str.c_str());
	}

	return 0;
}

}
