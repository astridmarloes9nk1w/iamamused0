/*
 * options.cpp
 *
 *  Created on: Jul 23, 2015
 *      Author: lion
 */

#include "console.h"
#include "Fiber.h"

namespace v8 {
namespace internal {
class FlagList {
public:
	static void PrintHelp();
};
}
}

namespace fibjs
{

bool g_perf;

#ifdef amd64
int32_t stack_size = 512;
#else
int32_t stack_size = 256;
#endif

#ifdef DEBUG
#define GUARD_SIZE	32
#else
#define GUARD_SIZE	16
#endif

static void printHelp() {
	printf("Usage: fibjs [options] [script.js] [arguments] \n"
	       "\n"
	       "Options:\n"
	       "  --trace_fiber        allow user to query the non-current\n"
	       "                       fiber's stack infomation\n"
	       "  --help               print fibjs command line options\n"
	       "  --v8-options         print v8 command line options\n"
	       "\n"
	       "Documentation can be found at http://fibjs.org/\n");
}

bool options(int32_t* argc, char *argv[])
{
	char s_opts[64];
	char s_sharmony[] = " --harmony --harmony_async_await "
	                    "--use_strict --always-opt --nolazy";

	v8::V8::SetFlagsFromString(s_sharmony, sizeof(s_sharmony) - 1);
	v8::V8::SetFlagsFromString(s_opts,
	                           sprintf(s_opts, "--stack_size=%d", stack_size - GUARD_SIZE));

	v8::V8::SetFlagsFromCommandLine(argc, argv, true);

	int32_t df = 0;

	for (int32_t i = 0; i < *argc; i ++)
	{
		char* arg = argv[i];

		if (df)
			argv[i - df] = arg;

		if (!qstrcmp(arg, "--trace_fiber")) {
			df ++;
			Isolate::rt::g_trace = true;
		} else if (!qstrcmp(arg, "--help")) {
			printHelp();
			return true;
		} else if (!qstrcmp(arg, "--v8-options")) {
			v8::internal::FlagList::PrintHelp();
			return true;
		}
	}

	if (df)
		*argc -= df;

#ifdef DEBUG
	Isolate::rt::g_trace = true;
#endif

	return false;
}

}
