
#include <locale.h>

#include <string.h>
#include "ifs/global.h"
#include "ifs/process.h"
#include "ifs/global.h"
#include "console.h"
#include "SandBox.h"
#include "Fiber.h"
#include "utf8.h"
#include "include/libplatform/libplatform.h"

namespace fibjs
{

void init_argv(int argc, char **argv);
void init_prof();
void options(int* argc, char *argv[]);

class ShellArrayBufferAllocator : public v8::ArrayBuffer::Allocator
{
public:
    virtual void* Allocate(size_t length)
    {
        void* data = AllocateUninitialized(length);
        return data == NULL ? data : memset(data, 0, length);
    }

    virtual void* AllocateUninitialized(size_t length)
    {
        return malloc(length);
    }

    virtual void Free(void* data, size_t)
    {
        free(data);
    }
};

exlib::LockedList<Isolate> s_isolates;

void _main(const char *fname)
{
    v8::Platform *platform = v8::platform::CreateDefaultPlatform();
    v8::V8::InitializePlatform(platform);

    v8::V8::Initialize();

    Isolate is;
    Isolate::reg(&is);
    s_isolates.putTail(&is);

    Isolate& isolate = Isolate::now();

    v8::Isolate::CreateParams create_params;
    ShellArrayBufferAllocator array_buffer_allocator;
    create_params.array_buffer_allocator = &array_buffer_allocator;

    isolate.service = exlib::Service::current();
    isolate.isolate = v8::Isolate::New(create_params);
    v8::Locker locker(isolate.isolate);
    v8::Isolate::Scope isolate_scope(isolate.isolate);

    v8::HandleScope handle_scope(isolate.isolate);

    v8::Local<v8::Context> _context = v8::Context::New(isolate.isolate);
    v8::Context::Scope context_scope(_context);

    v8::Local<v8::Object> glob = _context->Global();
    global_base::class_info().Attach(glob, NULL);

    obj_ptr<console_base> s_console = new console_base();
    glob->ForceSet(_context, v8::String::NewFromUtf8(isolate.isolate, "console"), s_console->wrap());

    isolate.s_context.Reset(isolate.isolate, _context);
    isolate.s_global.Reset(isolate.isolate, glob);

    exlib::mem_check();

    JSFiber *fb = new JSFiber();
    {
        JSFiber::scope s(fb);
        isolate.s_topSandbox = new SandBox();

        isolate.s_topSandbox->initRoot();
        if (fname)
            s.m_hr = isolate.s_topSandbox->run(fname);
        else
            s.m_hr = isolate.s_topSandbox->repl();
    }

#ifdef DEBUG
    global_base::GC();
#endif

    process_base::exit(0);

    isolate.isolate->Dispose();

    v8::V8::ShutdownPlatform();
    delete platform;

    isolate.s_context.Reset();
}

}

int main(int argc, char *argv[])
{
    ::setlocale(LC_ALL, "");

    fibjs::options(&argc, argv);
    fibjs::init_prof();

    fibjs::init_argv(argc, argv);

    int i;

    for (i = 1; (i < argc) && (argv[i][0] == '-'); i ++);

    if (i < argc)
        fibjs::_main(argv[i]);
    else
        fibjs::_main(NULL);

    return 0;
}
