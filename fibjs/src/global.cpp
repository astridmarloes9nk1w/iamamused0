#include "ifs/global.h"
#include "ifs/coroutine.h"

namespace fibjs
{

result_t global_base::get_console(obj_ptr<console_base>& retVal)
{
	static obj_ptr<console_base> s_console;

	if (s_console == NULL)
		s_console = new console_base();

	retVal = s_console;

	return 0;
}

result_t global_base::print(const char* fmt, const v8::Arguments& args)
{
	return console_base::log(fmt, args);
}

result_t global_base::sleep(int32_t ms)
{
	return coroutine_base::sleep(ms);
}

result_t global_base::GC()
{
	v8::V8::LowMemoryNotification();
	return 0;
}

}
