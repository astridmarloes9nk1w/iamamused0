/*
 * SubProcess_posix.cpp
 *
 *  Created on: Feb 11, 2016
 *      Author: lion
 */

#ifndef _WIN32

#include "config.h"
#include "object.h"
#include "ifs/process.h"
#include "SubProcess.h"
#include <spawn.h>
#include <vector>
#include <sys/wait.h>
#include <signal.h>

namespace fibjs {

class PSTimer : public Timer {
public:
    PSTimer(int32_t timeout, intptr_t pid)
        : Timer(timeout)
        , m_pid(pid)
    {
    }

public:
    virtual void on_timer()
    {
        ::kill(m_pid, SIGINT);
    }

private:
    intptr_t m_pid;
};

#ifdef HAVE_GLIB_C_225_H
__asm__(".symver posix_spawnp,posix_spawnp@GLIBC_2.2.5");
#endif

#ifdef HAVE_GLIB_C_22_H
__asm__(".symver posix_spawnp,posix_spawnp@GLIBC_2.2");
#endif

result_t SubProcess::create(exlib::string command, v8::Local<v8::Array> args, v8::Local<v8::Object> opts,
    bool redirect, obj_ptr<SubProcess_base>& retVal)
{
    int32_t err = 0;
    pid_t pid = 0;
    result_t hr;
    posix_spawn_file_actions_t fops;
    std::vector<exlib::string> argStr;
    std::vector<char*> _args;
    int32_t len = args->Length();
    int32_t i;
    int32_t cin_pipe[2];
    int32_t cout_pipe[2];
    int32_t timeout;

    Isolate* isolate = Isolate::current();

    hr = GetConfigValue(isolate->m_isolate, opts, "timeout", timeout);
    if (hr == CALL_E_PARAMNOTOPTIONAL)
        timeout = 0;
    else if (hr < 0)
        return CHECK_ERROR(hr);

    argStr.resize(len);
    _args.resize(len + 2);

    _args[0] = (char*)command.c_str();
    for (i = 0; i < len; i++) {
        hr = GetArgumentValue(isolate->m_isolate, args->Get(i), argStr[i]);
        if (hr < 0)
            return hr;

        _args[i + 1] = &argStr[i][0];
    }
    _args[i + 1] = NULL;

    if (redirect) {
        if (pipe(cout_pipe))
            return CHECK_ERROR(LastError());

        if (pipe(cin_pipe)) {
            ::close(cout_pipe[0]);
            ::close(cout_pipe[1]);
            return CHECK_ERROR(LastError());
        }

        posix_spawn_file_actions_init(&fops);

        posix_spawn_file_actions_adddup2(&fops, cin_pipe[0], 0);
        posix_spawn_file_actions_adddup2(&fops, cout_pipe[1], 1);

        posix_spawn_file_actions_addclose(&fops, cin_pipe[0]);
        posix_spawn_file_actions_addclose(&fops, cin_pipe[1]);

        posix_spawn_file_actions_addclose(&fops, cout_pipe[0]);
        posix_spawn_file_actions_addclose(&fops, cout_pipe[1]);
    }

    std::vector<exlib::string> envstr;
    std::vector<char*> envp;

    v8::Local<v8::Object> envs;

    hr = GetConfigValue(isolate->m_isolate, opts, "env", envs, true);
    if (hr == CALL_E_PARAMNOTOPTIONAL)
        hr = process_base::get_env(envs);
    if (hr < 0)
        return hr;

    v8::Local<v8::Array> keys = envs->GetPropertyNames();
    len = (int32_t)keys->Length();

    for (i = 0; i < len; i++) {
        v8::Local<v8::Value> k = keys->Get(i);
        v8::Local<v8::Value> v = envs->Get(k);
        exlib::string ks, vs;

        hr = GetArgumentValue(k, ks);
        if (hr < 0)
            return hr;

        hr = GetArgumentValue(v, vs);
        if (hr < 0)
            return hr;

        ks.append(1, '=');
        ks.append(vs);

        envstr.push_back(ks);
    }

    for (i = 0; i < len; i++)
        envp.push_back(&envstr[i][0]);
    envp.push_back(NULL);

    errno = 0;
    err = posix_spawnp(&pid, command.c_str(), redirect ? &fops : NULL,
        NULL, _args.data(), &envp[0]);

    if (redirect) {
        ::close(cin_pipe[0]);
        ::close(cout_pipe[1]);

        posix_spawn_file_actions_destroy(&fops);
    }

#ifdef Linux
    if (err == 0) {
        int32_t status = 0;
        pid_t pid1 = waitpid(pid, &status, WNOHANG);
        if (WIFEXITED(status))
            status = WEXITSTATUS(status);

        if (pid1 == -1 || status == 127)
            err = 2;
    }
#endif

    if (err != 0) {
        if (redirect) {
            ::close(cin_pipe[1]);
            ::close(cout_pipe[0]);
        }

        return CHECK_ERROR(-err);
    }

    obj_ptr<SubProcess> sub = new SubProcess(pid);
    if (redirect) {
        wrap_pipe(cin_pipe[1], sub->m_stdin);
        wrap_pipe(cout_pipe[0], sub->m_stdout);
    }

    if (timeout > 0) {
        sub->m_timer = new PSTimer(timeout, pid);
        sub->m_timer->sleep();
    }

    retVal = sub;

    return 0;
}

result_t SubProcess::get_pid(int32_t& retVal)
{
    retVal = m_pid;
    return 0;
}

result_t SubProcess::kill(int32_t signal)
{
    if (m_timer)
        m_timer->clear();

    if (::kill(m_pid, signal))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t SubProcess::wait(int32_t& retVal, AsyncEvent* ac)
{
    class asyncWaitPid : public AsyncState {
    public:
        asyncWaitPid(intptr_t pid, Timer* timer, int32_t& retVal, AsyncEvent* ac)
            : AsyncState(ac)
            , m_pid(pid)
            , m_timer(timer)
            , m_retVal(retVal)
        {
            set(wait);
        }

    public:
        static int32_t wait(AsyncState* pState, int32_t n)
        {
            asyncWaitPid* pThis = (asyncWaitPid*)pState;

            pThis->set(result);
            return AsyncIO::waitpid(pThis->m_pid, pThis->m_retVal, pThis);
        }

        static int32_t result(AsyncState* pState, int32_t n)
        {
            asyncWaitPid* pThis = (asyncWaitPid*)pState;

            if (WIFEXITED(pThis->m_retVal))
                pThis->m_retVal = WEXITSTATUS(pThis->m_retVal);

            if (pThis->m_timer)
                pThis->m_timer->clear();

            return pThis->done();
        }

    private:
        intptr_t m_pid;
        obj_ptr<Timer> m_timer;
        int32_t& m_retVal;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncWaitPid(m_pid, m_timer, retVal, ac))->post(0);
}

result_t SubProcess::findWindow(exlib::string name, v8::Local<v8::Value>& retVal)
{
    return CALL_E_INVALID_CALL;
}
}

#endif
