#include "OSWrapper/Runnable.h"
#include "OSWrapper/Thread.h"
#include "WindowsOSWrapper/WindowsThreadFactory.h"
#include "OSWrapper/Mutex.h"
#include "WindowsOSWrapper/WindowsMutexFactory.h"
#include "OSWrapper/EventFlag.h"
#include "WindowsOSWrapper/WindowsEventFlagFactory.h"
#include "OSWrapper/MessageQueue.h"
#include "DesignByContract/Assertion.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "CppUTest/CommandLineTestRunner.h"

using OSWrapper::Runnable;
using OSWrapper::Thread;
using OSWrapper::Mutex;
using OSWrapper::EventFlag;
using OSWrapper::MessageQueue;
using OSWrapper::Timeout;
using OSWrapper::LockGuard;
using WindowsOSWrapper::WindowsThreadFactory;
using WindowsOSWrapper::WindowsMutexFactory;
using WindowsOSWrapper::WindowsEventFlagFactory;

static Mutex* s_mutex;

TEST_GROUP(WindowsMessageQueueTest) {
	WindowsThreadFactory testThreadFactory;
	WindowsMutexFactory testMutexFactory;
	WindowsEventFlagFactory testEventFlagFactory;

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
		Thread::setFactory(&testThreadFactory);
		Mutex::setFactory(&testMutexFactory);
		EventFlag::setFactory(&testEventFlagFactory);
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
		Run1 r1(mq);
		Thread* thread1 = Thread::create(&r1);
		Run2 r2(mq);
		Thread* thread2 = Thread::create(&r2);

		thread1->start();
		thread2->start();

		thread1->join();
		thread2->join();

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

TEST(WindowsMessageQueueTest, receive_nullptr)
{
	MessageQueue<int>* mq = MessageQueue<int>::create(SIZE);
	OSWrapper::Error err = mq->receive(0);
	LONGS_EQUAL(OSWrapper::InvalidParameter, err);
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
		thread1[i]->join();
		thread2[i]->join();
	}

	for (int i = 0; i < num; i++) {
		Thread::destroy(thread1[i]);
		Thread::destroy(thread2[i]);
	}

	IntMQ::destroy(mq);

	LONGS_EQUAL(send_count, recv_count);
	LONGS_EQUAL(num * SIZE, recv_count);
}

