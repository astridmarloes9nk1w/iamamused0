/*
 *  fiber.h
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include <string.h>

#ifndef fiber_h__
#define fiber_h__

namespace fiber
{

#pragma pack (1)
#if defined(__x86_64__)

typedef struct
{
    unsigned long long Part[2];
} SETJMP_FLOAT128;

typedef struct
{
    unsigned long long Rbp;
    unsigned long long Rbx;
    unsigned long long Rcx;
    unsigned long long Rdx;
    unsigned long long Rsi;
    unsigned long long Rdi;
    unsigned long long R12;
    unsigned long long R13;
    unsigned long long R14;
    unsigned long long R15;
    unsigned long long Rsp;
    unsigned long long Rip;
    SETJMP_FLOAT128 Xmm6;
    SETJMP_FLOAT128 Xmm7;
    SETJMP_FLOAT128 Xmm8;
    SETJMP_FLOAT128 Xmm9;
    SETJMP_FLOAT128 Xmm10;
    SETJMP_FLOAT128 Xmm11;
    SETJMP_FLOAT128 Xmm12;
    SETJMP_FLOAT128 Xmm13;
    SETJMP_FLOAT128 Xmm14;
    SETJMP_FLOAT128 Xmm15;
} context;

#else

typedef struct __JUMP_BUFFER
{
    unsigned long Ebp;
    unsigned long Ebx;
    unsigned long Ecx;
    unsigned long Edx;
    unsigned long Esi;
    unsigned long Edi;
    unsigned long Esp;
    unsigned long Eip;
} context;

#endif
#pragma pack ()

#define TLS_SIZE    8

#define FB_RESUME 1
#define FB_SUSPEND 2
#define FB_TERMINATE 3

class Fiber
{
public:
    void join();

    static void yield();
    static Fiber* Current();

public:
    context m_cntxt;
    Fiber* m_next;
    Fiber* m_join;
    void* m_tls[TLS_SIZE];
};

class FiberList
{
public:
    FiberList() : m_first(NULL), m_last(NULL)
    {
    }

    void put(Fiber* pNew)
    {
        if(m_last)
            m_last->m_next = pNew;
        else
            m_first = pNew;

        m_last = pNew;
    }

    Fiber* get()
    {
        Fiber* pNow = m_first;

        if(pNow)
        {
            m_first = pNow->m_next;
            if(m_first == NULL)
                m_last = NULL;

            pNow->m_next = NULL;
        }

        return pNow;
    }

    bool empty()
    {
        return m_first == NULL;
    }

private:
    Fiber* m_first;
    Fiber* m_last;
};

#define FIBER_STACK_SIZE    (65536 - sizeof(Fiber))

class Locker
{
public:
    Locker() :
        m_locked(false), m_count(0), m_locker(0)
    {}

public:
    void lock();
    void unlock();
    bool trylock();

private:
    bool m_locked;
    int m_count;
    Fiber* m_locker;
    FiberList m_blocks;
};

class CondVar
{
public:
    void wait(Locker& l);
    void notify_one();
    void notify_all();

private:
    FiberList m_blocks;
};

class Semaphore
{
public:
    Semaphore(int count = 0) : m_count(count)
    {
    }

public:
    void wait();
    void post();
    bool trywait();

private:
    int m_count;
    FiberList m_blocks;
};

class Service
{
public:
    Service();

public:
    static Fiber* CreateFiber(void* (*func)(void *), void *data = NULL, int stacksize = FIBER_STACK_SIZE);

    static int tlsAlloc();
    static void* tlsGet(int idx);
    static void tlsPut(int idx, void* v);
    static void tlsFree(int idx);

public:
    void switchtonext();
    static Service* getTLSService();

public:
    Fiber m_main;
    Fiber* m_running;
    Fiber* m_recycle;
    char m_tls[TLS_SIZE];
    FiberList m_resume;

    friend class Locker;
    friend class CondVar;
    friend class Semaphore;
};

}

#endif

