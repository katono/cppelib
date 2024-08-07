#include "OSWrapper/Runnable.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Mutex.h"
#include "OSWrapper/EventFlag.h"
#include "OSWrapper/MessageQueue.h"
#include "OSWrapper/FixedMemoryPool.h"
#include "Assertion/Assertion.h"
#include <exception>

#include "PlatformOSWrapperTestHelper.h"

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
		PlatformOSWrapperTestHelper::createAndRegisterOSWrapperFactories();
		s_mutex = Mutex::create();
		CHECK(s_mutex);
	}
	void teardown()
	{
		Mutex::destroy(s_mutex);
		PlatformOSWrapperTestHelper::destroyOSWrapperFactories();

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
		CHECK(thread1);
		Run2 r2(mq);
		Thread* thread2 = Thread::create(&r2, Thread::getNormalPriority());
		CHECK(thread2);

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
	CHECK(mq);
	LONGS_EQUAL(SIZE, mq->getMaxSize());
	MessageQueue<int>::destroy(mq);
}

TEST(PlatformMessageQueueTest, getSize_0)
{
	MessageQueue<int>* mq = MessageQueue<int>::create(SIZE);
	CHECK(mq);
	LONGS_EQUAL(0, mq->getSize());
	MessageQueue<int>::destroy(mq);
}

TEST(PlatformMessageQueueTest, tryReceive_TimedOut)
{
	MessageQueue<int>* mq = MessageQueue<int>::create(SIZE);
	CHECK(mq);
	int data = 0;
	OSWrapper::Error err = mq->tryReceive(&data);
	LONGS_EQUAL(OSWrapper::TimedOut, err);
	MessageQueue<int>::destroy(mq);
}

TEST(PlatformMessageQueueTest, timedReceive_Timeout10)
{
	MessageQueue<int>* mq = MessageQueue<int>::create(SIZE);
	CHECK(mq);
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
			for (std::size_t i = 0; i < SIZE; i++) {
				OSWrapper::Error err = m_mq->trySend(int(i));
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
			for (std::size_t i = 0; i < SIZE + 1; i++) {
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
	send_count = 0;
	recv_count = 0;

	class Sender : public BaseRunnable {
	public:
		Sender(IntMQ* mq) : BaseRunnable(mq) {}
		virtual void run()
		{
			for (std::size_t i = 0; i < SIZE; i++) {
				OSWrapper::Error err = m_mq->send(int(i));
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
			for (std::size_t i = 0; i < SIZE; i++) {
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
		CHECK(thread1[i]);
		thread2[i] = Thread::create(&r2, Thread::getNormalPriority());
		CHECK(thread2[i]);
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

#ifndef CPPELIB_NO_EXCEPTIONS
struct Elem {
	unsigned int data;

	static const unsigned int STD_EXCEPTION_DATA = 0;
	class StdException : public std::exception {};

	static const unsigned int ASSERTION_FAIL_DATA = 1;

	Elem() : data(100) {}
	Elem(const Elem& x) : data(x.data)
	{
		if (x.data == STD_EXCEPTION_DATA) {
			throw StdException();
		} else if (x.data == ASSERTION_FAIL_DATA) {
			CHECK_ASSERT(false);
		}
	}
	Elem& operator=(const Elem&)
	{
		if (data == STD_EXCEPTION_DATA) {
			throw StdException();
		} else if (data == ASSERTION_FAIL_DATA) {
			CHECK_ASSERT(false);
		}
		return *this;
	}
};

TEST(PlatformMessageQueueTest, send_receive_std_exception)
{
	MessageQueue<Elem>* mq = MessageQueue<Elem>::create(SIZE);
	CHECK(mq);
	Elem a;

	OSWrapper::Error err;
	err = mq->send(a);
	LONGS_EQUAL(OSWrapper::OK, err);
	LONGS_EQUAL(1, mq->getSize());

	a.data = Elem::STD_EXCEPTION_DATA;
	err = mq->send(a);
	LONGS_EQUAL(OSWrapper::OtherError, err);
	LONGS_EQUAL(1, mq->getSize());

	Elem b;
	b.data = Elem::STD_EXCEPTION_DATA;
	err = mq->receive(&b);
	LONGS_EQUAL(OSWrapper::OtherError, err);
	LONGS_EQUAL(1, mq->getSize());

	MessageQueue<Elem>::destroy(mq);
}

TEST(PlatformMessageQueueTest, send_receive_assertion_failure)
{
	MessageQueue<Elem>* mq = MessageQueue<Elem>::create(SIZE);
	CHECK(mq);
	Elem a;

	OSWrapper::Error err;
	err = mq->send(a);
	LONGS_EQUAL(OSWrapper::OK, err);
	LONGS_EQUAL(1, mq->getSize());

	bool exception_occurred = false;
	a.data = Elem::ASSERTION_FAIL_DATA;
	try {
		err = mq->send(a);
	}
	catch (const Assertion::Failure&) {
		exception_occurred = true;
	}
	CHECK(exception_occurred);
	LONGS_EQUAL(1, mq->getSize());

	exception_occurred = false;
	Elem b;
	b.data = Elem::ASSERTION_FAIL_DATA;
	try {
		err = mq->receive(&b);
	}
	catch (const Assertion::Failure&) {
		exception_occurred = true;
	}
	CHECK(exception_occurred);
	LONGS_EQUAL(1, mq->getSize());

	MessageQueue<Elem>::destroy(mq);
}
#endif

} // namespace PlatformMessageQueueTest
