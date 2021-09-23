
#include <fiber.h>
#include "v8.h"

#include "platform.h"
#include "v8threads.h"
#include "vm-state-inl.h"

#if 1
namespace v8 {
namespace internal {


class Thread::PlatformData : public Malloced {
 public:
  PlatformData() : thread_(NULL) {}

  fiber::Fiber* thread_;  // Thread handle for pthread.
};

Thread::Thread(const Options& options)
    : data_(new PlatformData()),
      stack_size_(options.stack_size()) {
  set_name(options.name());
}


Thread::~Thread() {
}


static void* ThreadEntry(void* arg) {
  Thread* thread = reinterpret_cast<Thread*>(arg);

  thread->data()->thread_ = fiber::Fiber::Current();
  ASSERT(thread->data()->thread_ != NULL);
  thread->Run();
  return NULL;
}


void Thread::set_name(const char* name) {
  strncpy(name_, name, sizeof(name_));
  name_[sizeof(name_) - 1] = '\0';
}


void Thread::Start() {
  data_->thread_ = fiber::Service::CreateFiber(ThreadEntry, this);
}


void Thread::Join() {
  data_->thread_->join();
}



Thread::LocalStorageKey Thread::CreateThreadLocalKey() {
  return static_cast<LocalStorageKey>(fiber::Service::tlsAlloc());
}

void Thread::DeleteThreadLocalKey(LocalStorageKey key) {
  fiber::Service::tlsFree(static_cast<int>(key));
}


void* Thread::GetThreadLocal(LocalStorageKey key) {
  return fiber::Service::tlsGet(static_cast<int>(key));
}


void Thread::SetThreadLocal(LocalStorageKey key, void* value) {
    fiber::Service::tlsPut(static_cast<int>(key), value);
}



void Thread::YieldCPU() {
  fiber::Fiber::yield();
}


class FiberMutex : public Mutex {
 public:

  virtual int Lock() {
    _mutex.lock();
    return 0;
  }

  virtual int Unlock() {
    _mutex.unlock();
    return 0;
  }

  virtual bool TryLock() {
    return _mutex.trylock();
  }

 private:
  fiber::Locker _mutex;
};


Mutex* OS::CreateMutex() {
  return new FiberMutex();
}


class FiberSemaphore : public Semaphore {
 public:
  FiberSemaphore(int count) : sem_(count){}
  virtual void Wait();
  virtual bool Wait(int timeout);
  virtual void Signal() { sem_.post(); }
 private:
   fiber::Semaphore sem_;
};


void FiberSemaphore::Wait() {
    sem_.wait();
}


bool FiberSemaphore::Wait(int timeout) {
    sem_.wait();
    return true;
}


Semaphore* OS::CreateSemaphore(int count) {
  return new FiberSemaphore(count);
}

}
}

#endif
