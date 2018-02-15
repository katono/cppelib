#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "CppUTest/CommandLineTestRunner.h"
#include "OSWrapper/Runnable.h"
#include "OSWrapper/Thread.h"
#include "WindowsOSWrapper/WindowsThreadFactory.h"
#include "OSWrapper/EventFlag.h"
#include "WindowsOSWrapper/WindowsEventFlagFactory.h"
#include "DesignByContract/Assertion.h"
#include <mutex>

using OSWrapper::Runnable;
using OSWrapper::Thread;
using OSWrapper::EventFlag;
using OSWrapper::Timeout;
using WindowsOSWrapper::WindowsThreadFactory;
using WindowsOSWrapper::WindowsEventFlagFactory;

static std::mutex s_mutex;

TEST_GROUP(WindowsEventFlagTest) {
	WindowsThreadFactory testThreadFactory;
	WindowsEventFlagFactory testEventFlagFactory;

	void setup()
	{
		Thread::setFactory(&testThreadFactory);
		EventFlag::setFactory(&testEventFlagFactory);
	}
	void teardown()
	{
		mock().checkExpectations();
		mock().clear();
	}

	template<class Run1, class Run2>
	void testWaitSet(bool autoReset)
	{
		EventFlag* ef = EventFlag::create(autoReset);
		Run1 r1(ef);
		Thread* thread1 = Thread::create(&r1);
		Run2 r2(ef);
		Thread* thread2 = Thread::create(&r2);

		thread1->start();
		thread2->start();

		thread1->join();
		thread2->join();

		Thread::destroy(thread1);
		Thread::destroy(thread2);

		EventFlag::destroy(ef);
	}
};

TEST(WindowsEventFlagTest, create_destroy)
{
	EventFlag* ef = EventFlag::create(true);
	CHECK(ef);
	EventFlag::destroy(ef);
}

class BaseRunnable : public Runnable {
protected:
	EventFlag* m_ef;
public:
	BaseRunnable(EventFlag* e) : m_ef(e) {}
	virtual void run() = 0;
};

class SetAll : public BaseRunnable {
public:
	SetAll(EventFlag* e) : BaseRunnable(e) {}
	virtual void run()
	{
		Thread::sleep(10);
		EventFlag::Error err = m_ef->setAll();
		std::lock_guard<std::mutex> lock(s_mutex);
		LONGS_EQUAL(EventFlag::OK, err);
	}
};

TEST(WindowsEventFlagTest, waitAny_setAll_autoReset)
{
	class WaitAnyAutoReset : public BaseRunnable {
	public:
		WaitAnyAutoReset(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			EventFlag::Error err = m_ef->waitAny();
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(EventFlag::OK, err);
			LONGS_EQUAL(0, m_ef->getCurrentPattern());
		}
	};
	testWaitSet<WaitAnyAutoReset, SetAll>(true);
}

TEST(WindowsEventFlagTest, waitAny_setAll_resetAll)
{
	class WaitAnyManualReset : public BaseRunnable {
	public:
		WaitAnyManualReset(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			EventFlag::Error err = m_ef->waitAny();
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(EventFlag::OK, err);
			LONGS_EQUAL(EventFlag::Pattern().set(), m_ef->getCurrentPattern());
			err = m_ef->resetAll();
			LONGS_EQUAL(EventFlag::OK, err);
			LONGS_EQUAL(0, m_ef->getCurrentPattern());
		}
	};
	testWaitSet<WaitAnyManualReset, SetAll>(false);
}

TEST(WindowsEventFlagTest, waitOne_setOne_resetOne)
{
	class WaitOne : public BaseRunnable {
	public:
		WaitOne(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			EventFlag::Error err = m_ef->waitOne(2);
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(EventFlag::OK, err);
			LONGS_EQUAL(1 << 2, m_ef->getCurrentPattern());

			err = m_ef->resetOne(2);
			LONGS_EQUAL(EventFlag::OK, err);
			LONGS_EQUAL(0, m_ef->getCurrentPattern());
		}
	};

	class SetOne : public BaseRunnable {
	public:
		SetOne(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			Thread::sleep(10);
			EventFlag::Error err = m_ef->setOne(2);
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(EventFlag::OK, err);
		}
	};
	testWaitSet<WaitOne, SetOne>(false);
}

TEST(WindowsEventFlagTest, waitOne_setOne_resetOne_InvalidParameter)
{
	class WaitOne : public BaseRunnable {
	public:
		WaitOne(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			EventFlag::Error err = m_ef->waitOne(EventFlag::Pattern().size());
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(EventFlag::InvalidParameter, err);

			err = m_ef->resetOne(EventFlag::Pattern().size());
			LONGS_EQUAL(EventFlag::InvalidParameter, err);
		}
	};

	class SetOne : public BaseRunnable {
	public:
		SetOne(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			EventFlag::Error err = m_ef->setOne(EventFlag::Pattern().size());
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(EventFlag::InvalidParameter, err);
		}
	};
	testWaitSet<WaitOne, SetOne>(false);
}

TEST(WindowsEventFlagTest, wait_set_reset_AND)
{
	class WaitPattern : public BaseRunnable {
	public:
		WaitPattern(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			EventFlag::Pattern ptn;
			EventFlag::Error err = m_ef->wait(EventFlag::Pattern(0x0F), EventFlag::AND, &ptn);
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(EventFlag::OK, err);
			LONGS_EQUAL(0x0F, ptn);
			LONGS_EQUAL(0x0F, m_ef->getCurrentPattern());

			err = m_ef->reset(EventFlag::Pattern(0x01));
			LONGS_EQUAL(EventFlag::OK, err);
			LONGS_EQUAL(0x0E, m_ef->getCurrentPattern());
			err = m_ef->reset(EventFlag::Pattern(0x02));
			LONGS_EQUAL(EventFlag::OK, err);
			LONGS_EQUAL(0x0C, m_ef->getCurrentPattern());
			err = m_ef->reset(EventFlag::Pattern(0x0C));
			LONGS_EQUAL(EventFlag::OK, err);
			LONGS_EQUAL(0x00, m_ef->getCurrentPattern());
		}
	};

	class SetPattern : public BaseRunnable {
	public:
		SetPattern(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			Thread::sleep(10);
			EventFlag::Error err = m_ef->set(EventFlag::Pattern(0x01));
			Thread::sleep(10);
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(EventFlag::OK, err);
			LONGS_EQUAL(0x01, m_ef->getCurrentPattern());

			err = m_ef->set(EventFlag::Pattern(0x0E));
			LONGS_EQUAL(EventFlag::OK, err);
		}
	};
	testWaitSet<WaitPattern, SetPattern>(false);
}

TEST(WindowsEventFlagTest, wait_set_reset_AND_Timeout100)
{
	class WaitPattern : public BaseRunnable {
	public:
		WaitPattern(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			EventFlag::Pattern ptn;
			EventFlag::Error err = m_ef->wait(EventFlag::Pattern(0x0F), EventFlag::AND, &ptn, Timeout(100));
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(EventFlag::OK, err);
			LONGS_EQUAL(0x0F, ptn);
			LONGS_EQUAL(0x00, m_ef->getCurrentPattern());
		}
	};

	class SetPattern : public BaseRunnable {
	public:
		SetPattern(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			Thread::sleep(10);
			EventFlag::Error err = m_ef->set(EventFlag::Pattern(0x01));
			Thread::sleep(10);
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(EventFlag::OK, err);
			LONGS_EQUAL(0x01, m_ef->getCurrentPattern());

			err = m_ef->set(EventFlag::Pattern(0x0E));
			LONGS_EQUAL(EventFlag::OK, err);
		}
	};
	testWaitSet<WaitPattern, SetPattern>(true);
}

TEST(WindowsEventFlagTest, wait_set_reset_AND_POLLING_TimedOut)
{
	class WaitPattern : public BaseRunnable {
	public:
		WaitPattern(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			EventFlag::Pattern ptn;
			EventFlag::Error err = m_ef->wait(EventFlag::Pattern(0x0F), EventFlag::AND, &ptn, Timeout::POLLING);
			{
				std::lock_guard<std::mutex> lock(s_mutex);
				LONGS_EQUAL(EventFlag::TimedOut, err);
			}
			Thread::sleep(30);
			err = m_ef->wait(EventFlag::Pattern(0x0F), EventFlag::AND, &ptn, Timeout::POLLING);
			{
				std::lock_guard<std::mutex> lock(s_mutex);
				LONGS_EQUAL(EventFlag::OK, err);
				LONGS_EQUAL(0x0F, ptn);
				LONGS_EQUAL(0x00, m_ef->getCurrentPattern());
			}
		}
	};

	class SetPattern : public BaseRunnable {
	public:
		SetPattern(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			Thread::sleep(10);
			EventFlag::Error err = m_ef->set(EventFlag::Pattern(0x01));
			Thread::sleep(10);
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(EventFlag::OK, err);
			LONGS_EQUAL(0x01, m_ef->getCurrentPattern());

			err = m_ef->set(EventFlag::Pattern(0x0E));
			LONGS_EQUAL(EventFlag::OK, err);
		}
	};
	testWaitSet<WaitPattern, SetPattern>(true);
}

TEST(WindowsEventFlagTest, wait_set_reset_OR)
{
	class WaitPattern : public BaseRunnable {
	public:
		WaitPattern(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			EventFlag::Pattern ptn;
			EventFlag::Error err = m_ef->wait(EventFlag::Pattern(0x0F), EventFlag::OR, &ptn);
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(EventFlag::OK, err);
			LONGS_EQUAL(0x01, ptn);
			LONGS_EQUAL(0x01, m_ef->getCurrentPattern());

			err = m_ef->reset(EventFlag::Pattern(0x01));
			LONGS_EQUAL(EventFlag::OK, err);
			LONGS_EQUAL(0x00, m_ef->getCurrentPattern());
		}
	};

	class SetPattern : public BaseRunnable {
	public:
		SetPattern(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			Thread::sleep(10);
			EventFlag::Error err = m_ef->set(EventFlag::Pattern(0x01));
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(EventFlag::OK, err);
		}
	};
	testWaitSet<WaitPattern, SetPattern>(false);
}

TEST(WindowsEventFlagTest, wait_set_reset_OR_Timeout100)
{
	class WaitPattern : public BaseRunnable {
	public:
		WaitPattern(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			EventFlag::Pattern ptn;
			EventFlag::Error err = m_ef->wait(EventFlag::Pattern(0x0F), EventFlag::OR, &ptn, Timeout(100));
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(EventFlag::OK, err);
			LONGS_EQUAL(0x01, ptn);
			LONGS_EQUAL(0x00, m_ef->getCurrentPattern());
		}
	};

	class SetPattern : public BaseRunnable {
	public:
		SetPattern(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			Thread::sleep(10);
			EventFlag::Error err = m_ef->set(EventFlag::Pattern(0x01));
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(EventFlag::OK, err);
		}
	};
	testWaitSet<WaitPattern, SetPattern>(true);
}

TEST(WindowsEventFlagTest, wait_set_reset_OR_POLLING_TimedOut)
{
	class WaitPattern : public BaseRunnable {
	public:
		WaitPattern(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			EventFlag::Pattern ptn;
			EventFlag::Error err = m_ef->wait(EventFlag::Pattern(0x0F), EventFlag::OR, &ptn, Timeout::POLLING);
			{
				std::lock_guard<std::mutex> lock(s_mutex);
				LONGS_EQUAL(EventFlag::TimedOut, err);
			}
			Thread::sleep(20);
			err = m_ef->wait(EventFlag::Pattern(0x0F), EventFlag::OR, &ptn, Timeout::POLLING);
			{
				std::lock_guard<std::mutex> lock(s_mutex);
				LONGS_EQUAL(EventFlag::OK, err);
				LONGS_EQUAL(0x01, ptn);
				LONGS_EQUAL(0x00, m_ef->getCurrentPattern());
			}
		}
	};

	class SetPattern : public BaseRunnable {
	public:
		SetPattern(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			Thread::sleep(10);
			EventFlag::Error err = m_ef->set(EventFlag::Pattern(0x01));
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(EventFlag::OK, err);
		}
	};
	testWaitSet<WaitPattern, SetPattern>(true);
}

TEST(WindowsEventFlagTest, wait_OtherThreadWaiting)
{
	class WaitOK : public BaseRunnable {
	public:
		WaitOK(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			EventFlag::Error err = m_ef->wait(EventFlag::Pattern(0x0F), EventFlag::OR, 0);
			{
				std::lock_guard<std::mutex> lock(s_mutex);
				LONGS_EQUAL(EventFlag::OK, err);
				LONGS_EQUAL(0x01, m_ef->getCurrentPattern());
			}

			err = m_ef->wait(EventFlag::Pattern(0x0F), EventFlag::OR, 0);
			{
				std::lock_guard<std::mutex> lock(s_mutex);
				LONGS_EQUAL(EventFlag::OK, err);
				LONGS_EQUAL(0x01, m_ef->getCurrentPattern());

				err = m_ef->reset(EventFlag::Pattern(0x01));
				LONGS_EQUAL(EventFlag::OK, err);
				LONGS_EQUAL(0x00, m_ef->getCurrentPattern());
			}
		}
	};

	class WaitFailed : public BaseRunnable {
	public:
		WaitFailed(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			Thread::sleep(10);
			EventFlag::Error err = m_ef->wait(EventFlag::Pattern(0x0F), EventFlag::OR, 0);
			LONGS_EQUAL(EventFlag::OtherThreadWaiting, err);

			err = m_ef->set(EventFlag::Pattern(0x01));
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(EventFlag::OK, err);
		}
	};
	testWaitSet<WaitOK, WaitFailed>(false);
}

TEST(WindowsEventFlagTest, waitPattern_InvalidParameter)
{
	class WaitFailedInvalidMode : public BaseRunnable {
	public:
		WaitFailedInvalidMode(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			EventFlag::Error err = m_ef->wait(EventFlag::Pattern(0x0F), (EventFlag::Mode)-1, 0);
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(EventFlag::InvalidParameter, err);
		}
	};

	class WaitFailedInvalidPattern : public BaseRunnable {
	public:
		WaitFailedInvalidPattern(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			Thread::sleep(10);
			EventFlag::Error err = m_ef->wait(EventFlag::Pattern(0x00), EventFlag::OR, 0);
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(EventFlag::InvalidParameter, err);
		}
	};
	testWaitSet<WaitFailedInvalidMode, WaitFailedInvalidPattern>(true);
}

