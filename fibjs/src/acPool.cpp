#include <exlib/thread.h>
#include <exlib/lockfree.h>
#include "AsyncCall.h"
#include <stdio.h>

#include <map>

#ifdef _WIN32
#include <windows.h>

static LARGE_INTEGER systemFrequency;

inline int64_t Ticks()
{
	LARGE_INTEGER t;

	if(systemFrequency.QuadPart == 0)
	QueryPerformanceFrequency(&systemFrequency);

	QueryPerformanceCounter(&t);

	return t.QuadPart * 1000000 / systemFrequency.QuadPart;
}

#else
#include <sys/time.h>

inline int64_t Ticks()
{
	struct timeval tv;
	if (gettimeofday(&tv, NULL) < 0)
		return 0;
	return (tv.tv_sec * 1000000ll) + tv.tv_usec;
}

#endif

namespace fibjs
{

#define MAX_WORKER 128
#define MIN_WORKER 6

AsyncQueue s_acPool;
AsyncQueue s_acSleep;

std::multimap<int64_t, _AsyncCall*> s_tms;
static int64_t s_time;

static class _acThread: public exlib::Thread
{
public:
	_acThread()
	{
		for (int i = 0; i < MIN_WORKER; i++)
		{
			start();
			detach();
		}
	}

	virtual void Run()
	{
		_AsyncCall *p;

		while (1)
		{
			p = s_acPool.wait();
			p->func(p);
		}
	}
} s_ac;

static class _timerThread: public exlib::Thread
{
public:
	_timerThread()
	{
		s_time = Ticks();
		start();
	}

	virtual void Run()
	{
		_AsyncCall *p;
		int64_t tm;
		std::multimap<int64_t, _AsyncCall*>::iterator e;

		while (1)
		{
			Sleep(1);

			while (1)
			{
				p = s_acSleep.get();
				if (p == NULL)
					break;

				tm = s_time + *(int*) p->args[0] * 1000;
				s_tms.insert(std::make_pair(tm, p));
			}

			s_time = Ticks();

			while (1)
			{
				e = s_tms.begin();
				if (e == s_tms.end())
					break;
				if (e->first > s_time)
					break;

				e->second->post();
				s_tms.erase(e);
			}
		}
	}
} s_timer;

}
