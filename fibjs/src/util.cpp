#include "utils.h"
#include "acPool.h"
#include <string.h>
#include <sstream>
#include <log4cpp/Category.hh>
#include "utf8.h"

namespace fibjs
{

std::string getResultMessage(result_t hr)
{
	static const char* s_errors[] =
	{ "",
	// CALL_E_BADPARAMCOUNT
			"Invalid number of parameters.",
			// CALL_E_PARAMNOTOPTIONAL
			"Parameter not optional.",
			// CALL_E_CONSTRUCTOR
			"Constructor cannot be called as a function.",
			// CALL_E_NOTINSTANCE
			"Object is not an instance of declaring class.",
			// CALL_E_BADVARTYPE
			"The input parameter is not a valid type.",
			// CALL_E_INVALIDARG
			"Invalid argument.",
			// CALL_E_INVALID_CALL
			"Invalid procedure call.",
			// CALL_E_TYPEMISMATCH
			"The argument could not be coerced to the specified type.",
			// CALL_E_OUTRANGE
			"Value is out of range.",
			// CALL_E_BADINDEX
			"Index was out of range.",
			// CALL_E_OVERFLOW
			"Memory overflow error."
	};

	if (hr < 0 && hr > CALL_E_MAX)
		return s_errors[-hr];

#ifdef _WIN32
	LPWSTR pMsgBuf = NULL;

	if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL, CALL_E_MAX - hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR) &pMsgBuf, 0, NULL ) && pMsgBuf)
	{
		std::string str = utf16to8String(pMsgBuf);
		LocalFree(pMsgBuf);
		return str;
	}

	return "Unknown error.";
#else
	return strerror(CALL_E_MAX - hr);
#endif
}

v8::Handle<v8::Value> ThrowResult(result_t hr)
{
	return ThrowError(getResultMessage(hr).c_str());
}

inline const char* ToCString(const v8::String::Utf8Value& value)
{
	return *value ? *value : "<string conversion failed>";
}

void ReportException(v8::TryCatch* try_catch, bool rt)
{
	v8::String::Utf8Value exception(try_catch->Exception());

	v8::Handle<v8::Message> message = try_catch->Message();
	if (message.IsEmpty())
		asyncLog(log4cpp::Priority::ERROR, ToCString(exception));
	else
	{
		if (rt)
		{
			v8::String::Utf8Value stack_trace(try_catch->StackTrace());
			if (stack_trace.length() > 0)
			{
				asyncLog(log4cpp::Priority::ERROR, ToCString(stack_trace));
				return;
			}
		}

		std::stringstream strError;

		v8::String::Utf8Value filename(message->GetScriptResourceName());
		strError << ToCString(exception) << "\n    at ";
		strError << ToCString(filename);
		int lineNumber = message->GetLineNumber();
		strError << ':' << lineNumber << ':' << (message->GetStartColumn() + 1);

		asyncLog(log4cpp::Priority::ERROR, strError.str());
	}
}

std::string traceInfo()
{
	v8::Handle<v8::StackTrace> stackTrace = v8::StackTrace::CurrentStackTrace(
			10, v8::StackTrace::kOverview);
	int count = stackTrace->GetFrameCount();
	int i;
	std::stringstream strBuffer;

	for (i = 0; i < count; i++)
	{
		v8::Local<v8::StackFrame> f = stackTrace->GetFrame(i);

		v8::String::Utf8Value funname(f->GetFunctionName());
		v8::String::Utf8Value filename(f->GetScriptName());

		strBuffer << "\n    at ";

		if (**funname)
			strBuffer << *funname << " (";

		strBuffer << *filename << ':' << f->GetLineNumber() << ':'
				<< f->GetColumn();

		if (**funname)
			strBuffer << ')';
	}

	return strBuffer.str();
}

std::string JSON_stringify(v8::Handle<v8::Value> v)
{
	std::string str;

	v8::Handle<v8::Context> context = v8::Context::GetCurrent();
	v8::Handle<v8::Object> global = context->Global();

	v8::Handle<v8::Object> JSON =
			global->Get(v8::String::New("JSON"))->ToObject();
	v8::Handle<v8::Function> JSON_stringify = v8::Handle<v8::Function>::Cast(
			JSON->Get(v8::String::New("stringify")));

	v8::Handle<v8::Value> myargs[] =
	{ v };

	v8::Handle<v8::Value> sJSON = JSON_stringify->Call(JSON, 1, myargs);

	if (sJSON.IsEmpty())
		return str;

	str = *v8::String::Utf8Value(sJSON);

	return str;
}

}
