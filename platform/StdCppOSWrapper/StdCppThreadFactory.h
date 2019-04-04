#ifndef STDCPP_OS_WRAPPER_STDCPP_THREAD_FACTORY_H_INCLUDED
#define STDCPP_OS_WRAPPER_STDCPP_THREAD_FACTORY_H_INCLUDED

#include "OSWrapper/ThreadFactory.h"
#include "OSWrapper/Thread.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unordered_map>

namespace StdCppOSWrapper {

class StdCppThreadFactory : public OSWrapper::ThreadFactory {
protected:
	class StdCppThread : public OSWrapper::Thread {
	protected:
		int m_priority;
		int m_initialPriority;
		std::size_t m_stackSize;
		const char* m_name;

		std::thread m_thread;
		mutable std::mutex m_mutex;
	private:
		std::condition_variable m_condStarted;
		std::condition_variable m_condFinished;
		bool m_isActive;
		bool m_endThreadRequested;
		std::thread::id m_threadId;

		static void threadEntry(StdCppThread* t);
		void threadLoop();

	public:
		StdCppThread(OSWrapper::Runnable* r, int priority, std::size_t stackSize, const char* name);
		virtual ~StdCppThread() {}

		void beginThread();
		void endThread();

		virtual void start();
		virtual OSWrapper::Error wait();
		virtual OSWrapper::Error tryWait();
		virtual OSWrapper::Error timedWait(OSWrapper::Timeout tmout);
		virtual bool isFinished() const;
		virtual void setName(const char* name);
		virtual const char* getName() const;
		virtual void setPriority(int priority);
		virtual int getPriority() const;
		virtual int getInitialPriority() const;
		virtual std::size_t getStackSize() const;
		virtual void* getNativeHandle();

		std::thread::id getId() const { return m_threadId; }
	};

public:
	StdCppThreadFactory();
	virtual ~StdCppThreadFactory() {}

private:
	virtual OSWrapper::Thread* create(OSWrapper::Runnable* r, int priority, std::size_t stackSize, void* stackAddress, const char* name);
	virtual void destroy(OSWrapper::Thread* t);
	virtual void sleep(unsigned long millis);
	virtual void yield();
	virtual OSWrapper::Thread* getCurrentThread();
	virtual int getMaxPriority() const;
	virtual int getMinPriority() const;
	virtual int getHighestPriority() const;
	virtual int getLowestPriority() const;

	StdCppThreadFactory(const StdCppThreadFactory&);
	StdCppThreadFactory& operator=(const StdCppThreadFactory&);

	virtual StdCppThread* createStdCppThread(OSWrapper::Runnable* r, int priority, std::size_t stackSize, const char* name);

	std::unordered_map<std::thread::id, OSWrapper::Thread*> m_threadIdMap;
	std::recursive_mutex m_mutex;
};

}

#endif // STDCPP_OS_WRAPPER_STDCPP_THREAD_FACTORY_H_INCLUDED
