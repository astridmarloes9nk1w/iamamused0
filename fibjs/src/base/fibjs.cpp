
#include <locale.h>

#include <string.h>
#include <time.h>
#include "object.h"
#include "ifs/os.h"
#include "ifs/global.h"
#include "ifs/zip.h"
#include "SandBox.h"
#include "Fiber.h"
#include "include/libplatform/libplatform.h"
#include "Stat.h"
#include "utf8.h"
#include "path.h"
#include "unzip/include/unzip.h"

namespace fibjs {

void init_date();
void init_argv(int32_t argc, char** argv);
void init_start_argv(int32_t argc, char** argv);
void init_acThread();
void init_aio();
void init_process();
void options(int32_t& pos, char* argv[]);
result_t ifZipFile(exlib::string filename, bool& retVal);

void run_gui();

exlib::string s_root;

void init()
{
    ::setlocale(LC_ALL, "");

    int32_t cpus = 0;

    process_base::cwd(s_root);

    os_base::cpuNumbers(cpus);
    if (cpus < 2)
        cpus = 2;

    exlib::Service::init(cpus + 1);

    init_date();
    init_acThread();
    init_aio();
    init_process();

    srand((unsigned int)time(0));

    v8::Platform* platform = v8::platform::CreateDefaultPlatform();
    v8::V8::InitializePlatform(platform);

    v8::V8::Initialize();
}

static result_t main_fiber(Isolate* isolate)
{
    JSFiber::scope s;

    isolate->start_profiler();

    if (!isolate->m_fname.empty()) {
        v8::Local<v8::Array> argv;

        global_base::get_argv(argv);
        s.m_hr = isolate->m_topSandbox->run_main(isolate->m_fname, argv);
    } else {
        v8::Local<v8::Array> cmds = v8::Array::New(isolate->m_isolate);
        RootModule* pModule = RootModule::g_root;
        v8::Local<v8::Context> _context = isolate->context();
        v8::Local<v8::Object> glob = _context->Global();

        while (pModule) {
            glob->DefineOwnProperty(_context,
                    isolate->NewString(pModule->name()),
                    pModule->getModule(isolate),
                    (v8::PropertyAttribute)(v8::DontEnum))
                .IsJust();
            pModule = pModule->m_next;
        }

        s.m_hr = isolate->m_topSandbox->repl(cmds);
    }

    return s.m_hr;
}

static exlib::string s_start;
static void start_fiber(void* p)
{
    Isolate* isolate = new Isolate(s_start);
    syncCall(isolate, main_fiber, isolate);
}

void main(int32_t argc, char** argv)
{
    class MainThread : public exlib::OSThread {
    public:
        MainThread(int32_t argc, char** argv)
            : m_argc(argc)
            , m_argv(argv)
        {
        }

    public:
        virtual void Run()
        {
            int32_t argc = m_argc;
            char** argv = m_argv;

            exlib::string exePath;
            std::vector<char*> ptrArg;

            process_base::get_execPath(exePath);

            bool bZip;
            ifZipFile(exePath, bZip);
            if (bZip) {

                exePath.append(1, '$');
                ptrArg.resize(argc + 1);

                ptrArg[0] = argv[0];
                ptrArg[1] = exePath.c_buffer();

                int32_t i;
                for (i = 1; i < argc; i++)
                    ptrArg[i + 1] = argv[i];

                argv = &ptrArg[0];
                argc++;
            }

            init_start_argv(argc, argv);

            int32_t pos = argc;
            options(pos, argv);

            init();

            if (pos < argc) {
                if (argv[pos][0] == '-')
                    s_start = argv[pos];
                else {
                    s_start = s_root;
                    resolvePath(s_start, argv[pos]);
                }

                if (pos != 1) {
                    int32_t p = 1;
                    for (; pos < argc; pos++)
                        argv[p++] = argv[pos];
                    argc = p;
                }
            }

            init_argv(argc, argv);
            exlib::Service::Create(start_fiber, NULL, 256 * 1024, "start");
            exlib::Service::dispatch();
        }

    private:
        int32_t m_argc;
        char** m_argv;
    };

    MainThread* main_thread = new MainThread(argc, argv);
    main_thread->start();

    run_gui();
}
}

#ifdef _WIN32

#ifdef _CONSOLE
int32_t main()
#else
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
    int32_t argc;
    char** argv;

    LPWSTR* szArglist = CommandLineToArgvW(GetCommandLineW(), &argc);
    std::vector<exlib::string> strArgList;
    std::vector<char*> ptrArgList;
    int32_t i;

    strArgList.resize(argc);
    ptrArgList.resize(argc);
    for (i = 0; i < argc; i++) {
        strArgList[i] = fibjs::utf16to8String(szArglist[i]);
        ptrArgList[i] = strArgList[i].c_buffer();
    }

    argv = ptrArgList.data();

    fibjs::main(argc, argv);
    return 0;
}

#else

int32_t main(int32_t argc, char* argv[])
{
    fibjs::main(argc, argv);
    return 0;
}
#endif
