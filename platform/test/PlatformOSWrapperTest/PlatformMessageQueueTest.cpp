#include "OSWrapper/Runnable.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Mutex.h"
#include "OSWrapper/EventFlag.h"
#include "OSWrapper/MessageQueue.h"
#include "OSWrapper/FixedMemoryPool.h"
#include <exception>

#ifdef PLATFORM_OS_WINDOWS
#include "WindowsOSWrapper/WindowsThreadFactory.h"
#include "WindowsOSWrapper/WindowsMutexFactory.h"
#include "WindowsOSWrapper/WindowsEventFlagFactory.h"
#include "WindowsOSWrapper/WindowsFixedMemoryPoolFactory.h"
typedef WindowsOSWrapper::WindowsThreadFactory PlatformThreadFactory;
typedef WindowsOSWrapper::WindowsMutexFactory PlatformMutexFactory;
typedef WindowsOSWrapper::WindowsEventFlagFactory PlatformEventFlagFactory;
typedef WindowsOSWrapper::WindowsFixedMemoryPoolFactory PlatformFixedMemoryPoolFactory;
#elif PLATFORM_OS_ITRON
#include "ItronOSWrapper/ItronThreadFactory.h"
#include "ItronOSWrapper/ItronMutexFactory.h"
#include "ItronOSWrapper/ItronEventFlagFactory.h"
#include "ItronOSWrapper/ItronFixedMemoryPoolFactory.h"
typedef ItronOSWrapper::ItronThreadFactory PlatformThreadFactory;
typedef ItronOSWrapper::ItronMutexFactory PlatformMutexFactory;
typedef ItronOSWrapper::ItronEventFlagFactory PlatformEventFlagFactory;
typedef ItronOSWrapper::ItronFixedMemoryPoolFactory PlatformFixedMemoryPoolFactory;
#endif

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

namespace PlatformMessageQueueTest {

using OSWrapper::Runnable;
using OSWrapper::Thread;
using OSWrapper::Mutex;
using OSWrapper::EventFlag;
using OSWrapper::FixedMemoryPool;
using OSWrapper::MessageQueue;
using OSWrapper::Timeout;
using OSWrapper::LockGuard;

static Mutex* s_mutex;

TEST_GROUP(PlatformMessageQueueTest) {
	PlatformThreadFactory testThreadFactory;
	PlatformMutexFactory testMutexFactory;
	PlatformEventFlagFactory testEventFlagFactory;
	PlatformFixedMemoryPoolFactory testFixedMemoryPoolFactory;

	static const std::size_t SIZE = 10;
	typedef MessageQueue<int> IntMQ;

	class BaseRunnable : public Runnable {
	protected:
		IntMQ* m_mq;
	public:
		BaseRunnable(IntMQ* mq) : m_mq(mq) {}
		virtual void run() = 0;
	};

	void setup()
	{
		OSWrapper::registerThreadFactory(&testThreadFactory);
		OSWrapper::registerMutexFactory(&testMutexFactory);
		OSWrapper::registerEventFlagFactory(&testEventFlagFactory);
		OSWrapper::registerFixedMemoryPoolFactory(&testFixedMemoryPoolFactory);

		s_mutex = Mutex::create();
	}
	void teardown()
	{
		Mutex::destroy(s_mutex);

		mock().checkExpectations();
		mock().clear();
	}

	template<class Run1, class Run2, class MQ>
	void testTwoThreadsSharingOneMQ()
	{
		MQ* mq = MQ::create(SIZE);
		CHECK(mq);
		Run1 r1(mq);
		Thread* thread1 = Thread::create(&r1, Thread::getNormalPriority());
		Run2 r2(mq);
		Thread* thread2 = Thread::create(&r2, Thread::getNormalPriority());

		thread1->start();
		thread2->start();

		thread1->wait();
		thread2->wait();

		Thread::destroy(thread1);
		Thread::destroy(thread2);

		MQ::destroy(mq);
	}
};

TEST(PlatformMessageQueueTest, create_destroy)
{
	MessageQueue<int>* mq = MessageQueue<int>::create(SIZE);
	CHECK(mq);
	MessageQueue<int>::destroy(mq);
}

TEST(PlatformMessageQueueTest, destroy_nullptr)
{
	MessageQueue<int>::destroy(0);
}

TEST(PlatformMessageQueueTest, getMaxSize)
{
	MessageQueue<int>* mq = MessageQueue<int>::create(SIZE);
	LONGS_EQUAL(SIZE, mq->getMaxSize());
	MessageQueue<int>::destroy(mq);
}

TEST(PlatformMessageQueueTest, getSize_0)
{
	MessageQueue<int>* mq = MessageQueue<int>::create(SIZE);
	LONGS_EQUAL(0, mq->getSize());
	MessageQueue<int>::destroy(mq);
}

TEST(PlatformMessageQueueTest, tryReceive_TimedOut)
{
	MessageQueue<int>* mq = MessageQueue<int>::create(SIZE);
	int data = 0;
	OSWrapper::Error err = mq->tryReceive(&data);
	LONGS_EQUAL(OSWrapper::TimedOut, err);
	MessageQueue<int>::destroy(mq);
}

TEST(PlatformMessageQueueTest, timedReceive_Timeout10)
{
	MessageQueue<int>* mq = MessageQueue<int>::create(SIZE);
	int data = 0;
	OSWrapper::Error err = mq->timedReceive(&data, Timeout(10));
	LONGS_EQUAL(OSWrapper::TimedOut, err);
	MessageQueue<int>::destroy(mq);
}

TEST(PlatformMessageQueueTest, send_receive)
{
	class Sender : public BaseRunnable {
	public:
		Sender(IntMQ* mq) : BaseRunnable(mq) {}
		virtual void run()
		{
			Thread::sleep(10);
			OSWrapper::Error err = m_mq->send(100);
			LockGuard lock(s_mutex);
			LONGS_EQUAL(OSWrapper::OK, err);
		}
	};

	class Receiver : public BaseRunnable {
	public:
		Receiver(IntMQ* mq) : BaseRunnable(mq) {}
		virtual void run()
		{
			int data = 0;
			OSWrapper::Error err = m_mq->receive(&data);
			LockGuard lock(s_mutex);
			LONGS_EQUAL(OSWrapper::OK, err);
			LONGS_EQUAL(100, data);
		}
	};
	testTwoThreadsSharingOneMQ<Sender, Receiver, IntMQ>();
}

TEST(PlatformMessageQueueTest, send_receive_nullptr)
{
	class Sender : public BaseRunnable {
	public:
		Sender(IntMQ* mq) : BaseRunnable(mq) {}
		virtual void run()
		{
			Thread::sleep(10);
			OSWrapper::Error err = m_mq->send(100);
			LockGuard lock(s_mutex);
			LONGS_EQUAL(OSWrapper::OK, err);
		}
	};

	class Receiver : public BaseRunnable {
	public:
		Receiver(IntMQ* mq) : BaseRunnable(mq) {}
		virtual void run()
		{
			OSWrapper::Error err = m_mq->receive(0);
			LockGuard lock(s_mutex);
			LONGS_EQUAL(OSWrapper::OK, err);
		}
	};
	testTwoThreadsSharingOneMQ<Sender, Receiver, IntMQ>();
}

TEST(PlatformMessageQueueTest, trySend_TimedOut_receive)
{
	class Sender : public BaseRunnable {
	public:
		Sender(IntMQ* mq) : BaseRunnable(mq) {}
		virtual void run()
		{
			for (int i = 0; i < SIZE; i++) {
				OSWrapper::Error err = m_mq->trySend(i);
				LockGuard lock(s_mutex);
				LONGS_EQUAL(OSWrapper::OK, err);
			}
			{
				std::size_t size = m_mq->getSize();
				LockGuard lock(s_mutex);
				LONGS_EQUAL(SIZE, size);
			}
			{
				OSWrapper::Error err = m_mq->trySend(SIZE);
				LockGuard lock(s_mutex);
				LONGS_EQUAL(OSWrapper::TimedOut, err);
			}
			{
				OSWrapper::Error err = m_mq->send(SIZE);
				LockGuard lock(s_mutex);
				LONGS_EQUAL(OSWrapper::OK, err);
			}
		}
	};

	class Receiver : public BaseRunnable {
	public:
		Receiver(IntMQ* mq) : BaseRunnable(mq) {}
		virtual void run()
		{
			Thread::sleep(100);
			for (int i = 0; i < SIZE + 1; i++) {
				int data = 0;
				OSWrapper::Error err = m_mq->receive(&data);
				LockGuard lock(s_mutex);
				LONGS_EQUAL(OSWrapper::OK, err);
				LONGS_EQUAL(i, data);
			}
		}
	};
	testTwoThreadsSharingOneMQ<Sender, Receiver, IntMQ>();
}

TEST(PlatformMessageQueueTest, send_receive_many_msgs)
{
	static const int num = SIZE + 1000;
	class Sender : public BaseRunnable {
	public:
		Sender(IntMQ* mq) : BaseRunnable(mq) {}
		virtual void run()
		{
			for (int i = 0; i < num; i++) {
				OSWrapper::Error err = m_mq->send(i);
				LockGuard lock(s_mutex);
				LONGS_EQUAL(OSWrapper::OK, err);
			}
		}
	};

	class Receiver : public BaseRunnable {
	public:
		Receiver(IntMQ* mq) : BaseRunnable(mq) {}
		virtual void run()
		{
			for (int i = 0; i < num; i++) {
				int data = 0;
				OSWrapper::Error err = m_mq->receive(&data);
				LockGuard lock(s_mutex);
				LONGS_EQUAL(OSWrapper::OK, err);
				LONGS_EQUAL(i, data);
			}
		}
	};
	testTwoThreadsSharingOneMQ<Sender, Receiver, IntMQ>();
}

TEST(PlatformMessageQueueTest, send_receive_many_threads)
{
	static int send_count = 0;
	static int recv_count = 0;

	class Sender : public BaseRunnable {
	public:
		Sender(IntMQ* mq) : BaseRunnable(mq) {}
		virtual void run()
		{
			for (int i = 0; i < SIZE; i++) {
				OSWrapper::Error err = m_mq->send(i);
				LockGuard lock(s_mutex);
				LONGS_EQUAL(OSWrapper::OK, err);
				send_count++;
			}
		}
	};

	class Receiver : public BaseRunnable {
	public:
		Receiver(IntMQ* mq) : BaseRunnable(mq) {}
		virtual void run()
		{
			for (int i = 0; i < SIZE; i++) {
				int data = 0;
				OSWrapper::Error err = m_mq->receive(&data);
				LockGuard lock(s_mutex);
				LONGS_EQUAL(OSWrapper::OK, err);
				recv_count++;
			}
		}
	};

	const int num = 100;
	IntMQ* mq = IntMQ::create(SIZE);
	CHECK(mq);
	Sender r1(mq);
	Receiver r2(mq);
	Thread* thread1[num];
	Thread* thread2[num];

	for (int i = 0; i < num; i++) {
		thread1[i] = Thread::create(&r1, Thread::getNormalPriority());
		thread2[i] = Thread::create(&r2, Thread::getNormalPriority());
	}

	for (int i = 0; i < num; i++) {
		thread1[i]->start();
		thread2[i]->start();
	}

	for (int i = 0; i < num; i++) {
		thread1[i]->wait();
		thread2[i]->wait();
	}

	for (int i = 0; i < num; i++) {
		Thread::destroy(thread1[i]);
		Thread::destroy(thread2[i]);
	}

	IntMQ::destroy(mq);

	LONGS_EQUAL(send_count, recv_count);
	LONGS_EQUAL(num * SIZE, recv_count);
}

struct Elem {
	unsigned int data;
	static const unsigned int EXCEPTION_DATA = 0;
	class Exception : public std::exception {};

	Elem() : data(100) {}
	Elem(const Elem& x) : data(x.data)
	{
		if (x.data == EXCEPTION_DATA) {
			throw Exception();
		}
	}
	Elem& operator=(const Elem&)
	{
		if (data == EXCEPTION_DATA) {
			throw Exception();
		}
		return *this;
	}
};

TEST(PlatformMessageQueueTest, send_receive_exception)
{
	MessageQueue<Elem>* mq = MessageQueue<Elem>::create(SIZE);
	CHECK(mq);
	Elem a;

	OSWrapper::Error err;
	err = mq->send(a);
	LONGS_EQUAL(OSWrapper::OK, err);
	LONGS_EQUAL(1, mq->getSize());

	a.data = Elem::EXCEPTION_DATA;
	err = mq->send(a);
	LONGS_EQUAL(OSWrapper::OtherError, err);
	LONGS_EQUAL(1, mq->getSize());

	Elem b;
	b.data = Elem::EXCEPTION_DATA;
	err = mq->receive(&b);
	LONGS_EQUAL(OSWrapper::OtherError, err);
	LONGS_EQUAL(1, mq->getSize());

	MessageQueue<Elem>::destroy(mq);
}

} // namespace PlatformMessageQueueTest
