/*
 * options.cpp
 *
 *  Created on: Jul 23, 2015
 *      Author: lion
 */

#include "v8/src/flags.h"

#include "version.h"
#include "console.h"
#include "path.h"
#include "Fiber.h"

namespace fibjs {

#ifdef amd64
int32_t stack_size = 512;
#else
int32_t stack_size = 256;
#endif

#ifdef DEBUG
#define GUARD_SIZE 32
#else
#define GUARD_SIZE 16
#endif

static void printHelp()
{
    printf("Usage: fibjs [options] [script.js] [arguments] \n"
           "\n"
           "Options:\n"
           "  -h, --help           print fibjs command line options\n"
           "  -v, --version        print fibjs version\n"
           "  --v8-options         print v8 command line options\n"
           "\n"
           "Documentation can be found at http://fibjs.org/\n");
}

void options(int32_t argc, char* argv[])
{
    int32_t df = 0;

    for (int32_t i = 0; i < argc; i++) {
        char* arg = argv[i];

        if (df)
            argv[i - df] = arg;

        if (!qstrcmp(arg, "--help") || !qstrcmp(arg, "-h")) {
            printHelp();
            _exit(0);
        } else if (!qstrcmp(arg, "--version") || !qstrcmp(arg, "-v")) {
            printf("v%s\n", fibjs_version);
            _exit(0);
        } else if (!qstrcmp(arg, "--v8-options")) {
            v8::internal::FlagList::PrintHelp();
            _exit(0);
        }
    }

    v8::internal::FLAG_lazy = false;
    v8::internal::FLAG_stack_size = stack_size - GUARD_SIZE;

    v8::internal::FLAG_turbo_inlining = false;

    v8::internal::FLAG_wasm_async_compilation = false;
    v8::internal::FLAG_parallel_scavenge = false;

    if (df)
        argc -= df;

    v8::V8::SetFlagsFromCommandLine(&argc, argv, true);
}
}
