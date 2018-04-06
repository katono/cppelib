#include "OSWrapper/Runnable.h"
#include "OSWrapper/Thread.h"
#include "WindowsOSWrapper/WindowsThreadFactory.h"
#include "OSWrapper/Mutex.h"
#include "WindowsOSWrapper/WindowsMutexFactory.h"
#include "OSWrapper/EventFlag.h"
#include "WindowsOSWrapper/WindowsEventFlagFactory.h"
#include "WindowsOSWrapper/WindowsVariableAllocatorFactory.h"
#include "OSWrapper/MessageQueue.h"
#include "Assertion/Assertion.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "CppUTest/CommandLineTestRunner.h"

using OSWrapper::Runnable;
using OSWrapper::Thread;
using OSWrapper::Mutex;
using OSWrapper::EventFlag;
using OSWrapper::VariableAllocator;
using OSWrapper::MessageQueue;
using OSWrapper::Timeout;
using OSWrapper::LockGuard;
using WindowsOSWrapper::WindowsThreadFactory;
using WindowsOSWrapper::WindowsMutexFactory;
using WindowsOSWrapper::WindowsEventFlagFactory;
using WindowsOSWrapper::WindowsVariableAllocatorFactory;

static Mutex* s_mutex;

TEST_GROUP(WindowsMessageQueueTest) {
	WindowsThreadFactory testThreadFactory;
	WindowsMutexFactory testMutexFactory;
	WindowsEventFlagFactory testEventFlagFactory;
	WindowsVariableAllocatorFactory testVariableAllocatorFactory;
	VariableAllocator* allocator;

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
		OSWrapper::registerVariableAllocatorFactory(&testVariableAllocatorFactory);

		const std::size_t dummy_size = 1000;
		allocator = VariableAllocator::create(dummy_size);
		CHECK(allocator);
		OSWrapper::registerMessageQueueAllocator(allocator);

		s_mutex = Mutex::create();
	}
	void teardown()
	{
		Mutex::destroy(s_mutex);
		VariableAllocator::destroy(allocator);
		mock().checkExpectations();
		mock().clear();
	}

	template<class Run1, class Run2, class MQ>
	void testTwoThreadsSharingOneMQ()
	{
		MQ* mq = MQ::create(SIZE);
		CHECK(mq);
		Run1 r1(mq);
		Thread* thread1 = Thread::create(&r1);
		Run2 r2(mq);
		Thread* thread2 = Thread::create(&r2);

		thread1->start();
		thread2->start();

		thread1->wait();
		thread2->wait();

		Thread::destroy(thread1);
		Thread::destroy(thread2);

		MQ::destroy(mq);
	}
};

TEST(WindowsMessageQueueTest, create_destroy)
{
	MessageQueue<int>* mq = MessageQueue<int>::create(SIZE);
	CHECK(mq);
	MessageQueue<int>::destroy(mq);
}

TEST(WindowsMessageQueueTest, destroy_nullptr)
{
	MessageQueue<int>::destroy(0);
}

TEST(WindowsMessageQueueTest, create_failed)
{
	OSWrapper::registerMessageQueueAllocator(0);
	MessageQueue<int>* mq = MessageQueue<int>::create(SIZE);
	CHECK_FALSE(mq);
}

TEST(WindowsMessageQueueTest, getMaxSize)
{
	MessageQueue<int>* mq = MessageQueue<int>::create(SIZE);
	LONGS_EQUAL(SIZE, mq->getMaxSize());
	MessageQueue<int>::destroy(mq);
}

TEST(WindowsMessageQueueTest, getSize_0)
{
	MessageQueue<int>* mq = MessageQueue<int>::create(SIZE);
	LONGS_EQUAL(0, mq->getSize());
	MessageQueue<int>::destroy(mq);
}

TEST(WindowsMessageQueueTest, tryReceive_TimedOut)
{
	MessageQueue<int>* mq = MessageQueue<int>::create(SIZE);
	int data = 0;
	OSWrapper::Error err = mq->tryReceive(&data);
	LONGS_EQUAL(OSWrapper::TimedOut, err);
	MessageQueue<int>::destroy(mq);
}

TEST(WindowsMessageQueueTest, timedReceive_Timeout10)
{
	MessageQueue<int>* mq = MessageQueue<int>::create(SIZE);
	int data = 0;
	OSWrapper::Error err = mq->timedReceive(&data, Timeout(10));
	LONGS_EQUAL(OSWrapper::TimedOut, err);
	MessageQueue<int>::destroy(mq);
}

TEST(WindowsMessageQueueTest, send_receive)
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

TEST(WindowsMessageQueueTest, send_receive_nullptr)
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

TEST(WindowsMessageQueueTest, trySend_TimedOut_receive)
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

TEST(WindowsMessageQueueTest, send_receive_many_msgs)
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

TEST(WindowsMessageQueueTest, send_receive_many_threads)
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
		thread1[i] = Thread::create(&r1);
		thread2[i] = Thread::create(&r2);
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

