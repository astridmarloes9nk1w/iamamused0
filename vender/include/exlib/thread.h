/*
 *  thread.h
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#endif

#ifndef _ex_thread_h__
#define _ex_thread_h__

namespace exlib
{

#ifdef _WIN32

class OSMutex
{
public:
    OSMutex()
    {
        InitializeCriticalSection(&cs_);
    }

    ~OSMutex()
    {
        DeleteCriticalSection(&cs_);
    }

    void Lock()
    {
        EnterCriticalSection(&cs_);
    }

    void Unlock()
    {
        LeaveCriticalSection(&cs_);
    }

    bool TryLock()
    {
        return !!TryEnterCriticalSection(&cs_);
    }

private:
    CRITICAL_SECTION cs_;
};

class OSSemaphore
{
public:
    OSSemaphore(int start_val = 0);

    ~OSSemaphore()
    {
        CloseHandle(m_handle);
    }

    void Post()
    {
        ReleaseSemaphore(m_handle, 1, NULL);
    }

    void Wait()
    {
        WaitForSingleObject(m_handle, INFINITE);
    }

    bool TryWait()
    {
        return WaitForSingleObject(m_handle, 0) == WAIT_OBJECT_0;
    }

private:
    HANDLE m_handle;
};

#else

class OSMutex
{
public:
    OSMutex()
    {
        pthread_mutexattr_t attrs;
        pthread_mutexattr_init(&attrs);
        pthread_mutexattr_settype(&attrs, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&mutex_, &attrs);
    }

    ~OSMutex()
    {
        pthread_mutex_destroy(&mutex_);
    }

    void Lock()
    {
        pthread_mutex_lock(&mutex_);
    }

    void Unlock()
    {
        pthread_mutex_unlock(&mutex_);
    }

    bool TryLock()
    {
        return !pthread_mutex_trylock(&mutex_);
    }

private:
    pthread_mutex_t mutex_;
};

class OSSemaphore
{
public:
    OSSemaphore(int start_val = 0)
    {
        sem_init(&m_sem, 0, start_val);
    }

    ~OSSemaphore()
    {
        sem_destroy(&m_sem);
    }

    void Post()
    {
        sem_post(&m_sem);
    }

    void Wait()
    {
        sem_wait(&m_sem);
    }

    bool TryWait()
    {
        return sem_trywait(&m_sem) == 0;
    }

private:
    sem_t m_sem;
};

#endif

class OSThread
{
public:
    OSThread();
    virtual ~OSThread();

    void start();
    void detach();
    void join();
    virtual void Run() = 0;

    static void Sleep(int ms)
    {
#ifdef _WIN32
        ::Sleep(ms);
#else
        ::usleep(1000 * ms);
#endif
    }

public:
#ifdef _WIN32
    HANDLE thread_;
    DWORD threadid;
#else
    pthread_t thread_;
#endif
};

}

#endif
