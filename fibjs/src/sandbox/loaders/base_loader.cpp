/*
 * base_loader.cpp
 *
 *  Created on: Jun 4, 2017
 *      Author: lion
 */

#include "object.h"
#include "SandBox.h"
#include "ifs/global.h"

namespace fibjs {

const char* SandBox::script_args = "(async function(__filename,__dirname,require,run,exports,module,argv,repl){";
static const int32_t script_args_count = 8;

const char* SandBox::worker_args = "(async function(__filename,__dirname,require,run,exports,module,Master){";
static const int32_t worker_args_count = 7;

const char* SandBox::module_args = "(async function(__filename,__dirname,require,run,exports,module){";
static const int32_t module_args_count = 6;

result_t SandBox::ExtLoader::run_script(Context* ctx, Buffer_base* src, exlib::string name,
    v8::Local<v8::Array> argv)
{
    Isolate* isolate = ctx->m_sb->holder();

    v8::Local<v8::String> strRequire = isolate->NewString("require");
    v8::Local<v8::String> strExports = isolate->NewString("exports");

    v8::Local<v8::Object> module = v8::Object::New(isolate->m_isolate);
    v8::Local<v8::Object> exports = v8::Object::New(isolate->m_isolate);

    module->Set(strExports, exports);
    module->Set(strRequire, ctx->m_fnRequest);

    v8::Local<v8::Value> args[10] = {
        v8::Local<v8::Value>(), v8::Local<v8::Value>(),
        ctx->m_fnRequest,
        ctx->m_fnRun,
        exports,
        module,
        argv,
        v8::Undefined(ctx->m_sb->holder()->m_isolate)
    };

    return run(ctx, src, name, script_args, args, script_args_count);
}

result_t SandBox::ExtLoader::run_main(Context* ctx, Buffer_base* src, exlib::string name,
    v8::Local<v8::Array> argv)
{
    Isolate* isolate = ctx->m_sb->holder();

    v8::Local<v8::String> strRequire = isolate->NewString("require");
    v8::Local<v8::String> strExports = isolate->NewString("exports");

    v8::Local<v8::Object> module = v8::Object::New(isolate->m_isolate);
    v8::Local<v8::Object> exports = v8::Object::New(isolate->m_isolate);

    module->Set(strExports, exports);
    module->Set(strRequire, ctx->m_fnRequest);

    ctx->m_fnRequest->Set(isolate->NewString("main"), module);

    v8::Local<v8::Value> replFunc = global_base::class_info().getModule(isolate)->Get(
        isolate->NewString("repl"));

    v8::Local<v8::Value> args[10] = {
        v8::Local<v8::Value>(), v8::Local<v8::Value>(),
        ctx->m_fnRequest,
        ctx->m_fnRun,
        exports,
        module,
        argv,
        replFunc
    };
    return run(ctx, src, name, script_args, args, script_args_count);
}

result_t SandBox::ExtLoader::run_worker(Context* ctx, Buffer_base* src, exlib::string name,
    Worker_base* master)
{
    Isolate* isolate = ctx->m_sb->holder();

    v8::Local<v8::String> strRequire = isolate->NewString("require");
    v8::Local<v8::String> strExports = isolate->NewString("exports");

    v8::Local<v8::Object> module = v8::Object::New(isolate->m_isolate);
    v8::Local<v8::Object> exports = v8::Object::New(isolate->m_isolate);

    module->Set(strExports, exports);
    module->Set(strRequire, ctx->m_fnRequest);

    v8::Local<v8::Value> args[10] = {
        v8::Local<v8::Value>(), v8::Local<v8::Value>(),
        ctx->m_fnRequest,
        ctx->m_fnRun,
        exports,
        module,
        master->wrap()
    };
    return run(ctx, src, name, worker_args, args, worker_args_count);
}

result_t SandBox::ExtLoader::run_module(Context* ctx, Buffer_base* src, exlib::string name,
    v8::Local<v8::Object> module, v8::Local<v8::Object> exports)
{
    v8::Local<v8::Value> args[10] = {
        v8::Local<v8::Value>(), v8::Local<v8::Value>(),
        ctx->m_fnRequest,
        ctx->m_fnRun,
        exports,
        module
    };

    return run(ctx, src, name, module_args, args, module_args_count);
}
}
