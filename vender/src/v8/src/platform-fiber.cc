#include "v8.h"

#include <exlib/fiber.h>

#include "platform.h"
#include "v8threads.h"
#include "vm-state-inl.h"

#ifdef _WIN32

#ifdef CreateMutex
#undef CreateMutex
#endif

#ifdef CreateSemaphore
#undef CreateSemaphore
#endif

#endif

#if 1

inline int64_t getTime();

namespace v8
{
namespace internal
{

class Thread::PlatformData: public Malloced
{
public:
	PlatformData() :
			thread_(NULL)
	{
	}

	exlib::Fiber* thread_; // Thread handle for pthread.
};

Thread::Thread(const Options& options) :
		data_(new PlatformData()), stack_size_(options.stack_size())
{
	set_name(options.name());
}

Thread::~Thread()
{
	data_->thread_->Unref();
}

static void* ThreadEntry(void* arg)
{
	Thread* thread = reinterpret_cast<Thread*>(arg);

	thread->data()->thread_ = exlib::Fiber::Current();
	ASSERT(thread->data()->thread_ != NULL);
	thread->Run();
	return NULL;
}

void Thread::set_name(const char* name)
{
	strncpy(name_, name, sizeof(name_));
	name_[sizeof(name_) - 1] = '\0';
}

void Thread::Start()
{
	data_->thread_ = exlib::Service::CreateFiber(ThreadEntry, this);
}

void Thread::Join()
{
	data_->thread_->join();
}

Thread::LocalStorageKey Thread::CreateThreadLocalKey()
{
	return static_cast<LocalStorageKey>(exlib::Service::tlsAlloc());
}

void Thread::DeleteThreadLocalKey(LocalStorageKey key)
{
	exlib::Service::tlsFree(static_cast<int>(key));
}

void* Thread::GetThreadLocal(LocalStorageKey key)
{
	return exlib::Service::tlsGet(static_cast<int>(key));
}

void Thread::SetThreadLocal(LocalStorageKey key, void* value)
{
	exlib::Service::tlsPut(static_cast<int>(key), value);
}

void Thread::YieldCPU()
{
	exlib::Fiber::yield();
}

class FiberMutex: public Mutex
{
public:

	virtual int Lock()
	{
		_mutex.lock();
		return 0;
	}

	virtual int Unlock()
	{
		_mutex.unlock();
		return 0;
	}

	virtual bool TryLock()
	{
		return _mutex.trylock();
	}

private:
	exlib::Locker _mutex;
};

Mutex* OS::CreateMutex()
{
	return new FiberMutex();
}

class FiberSemaphore: public Semaphore
{
public:
	FiberSemaphore(int count) :
			sem_(count)
	{
	}
	virtual void Wait();
	virtual bool Wait(int timeout);
	virtual void Signal()
	{
		sem_.post();
	}
private:
	exlib::Semaphore sem_;
};

void FiberSemaphore::Wait()
{
	sem_.wait();
}

bool FiberSemaphore::Wait(int timeout)
{
	sem_.wait();
	return true;
}

Semaphore* OS::CreateSemaphore(int count)
{
	return new FiberSemaphore(count);
}

/*
class Sampler::PlatformData: public Malloced
{
public:
	PlatformData() :
			m_pFiber(exlib::Fiber::Current())
	{
		m_pFiber->Ref();
	}

	~PlatformData()
	{
		m_pFiber->Unref();
	}

	exlib::Fiber* profiled_thread()
	{
		return m_pFiber;
	}

private:
	// Note: for profiled_thread_ Mach primitives are used instead of PThread's
	// because the latter doesn't provide thread manipulation primitives required.
	// For details, consult "Mac OS X Internals" book, Section 7.3.
	exlib::Fiber* m_pFiber;
};
*/
#undef REGISTER_FIELD

extern Mutex* limit_mutex;

void OS::SetUp()
{
#ifdef _WIN32
	uint64_t seed = static_cast<uint64_t>(TimeCurrentMillis());
	srand(static_cast<unsigned int>(seed));
#else
	uint64_t seed = Ticks() ^ (getpid() << 16);
	srandom(static_cast<unsigned int>(seed));
#endif

	limit_mutex = CreateMutex();
}

void OS::TearDown()
{
	delete limit_mutex;
}

void OS::Sleep(int milliseconds)
{
	exlib::Fiber::sleep(milliseconds);
}

}

}

#endif
