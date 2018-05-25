#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "OSWrapper/Runnable.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/EventFlag.h"
#include "WindowsOSWrapper/WindowsThreadFactory.h"
#include "WindowsOSWrapper/WindowsEventFlagFactory.h"
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
		OSWrapper::registerThreadFactory(&testThreadFactory);
		OSWrapper::registerEventFlagFactory(&testEventFlagFactory);
	}
	void teardown()
	{
		mock().checkExpectations();
		mock().clear();
	}

	template<class Run1, class Run2>
	void testTwoThreadsSharingOneEventFlag(bool autoReset)
	{
		EventFlag* ef = EventFlag::create(autoReset);
		Run1 r1(ef);
		Thread* thread1 = Thread::create(&r1, Thread::getNormalPriority());
		Run2 r2(ef);
		Thread* thread2 = Thread::create(&r2, Thread::getNormalPriority());

		thread1->start();
		thread2->start();

		thread1->wait();
		thread2->wait();

		Thread::destroy(thread1);
		Thread::destroy(thread2);

		EventFlag::destroy(ef);
	}

	class BaseRunnable : public Runnable {
	protected:
		EventFlag* m_ef;
	public:
		BaseRunnable(EventFlag* e) : m_ef(e) {}
		virtual void run() = 0;
	};

};

TEST(WindowsEventFlagTest, create_destroy)
{
	EventFlag* ef = EventFlag::create(true);
	CHECK(ef);
	EventFlag::destroy(ef);
}

TEST(WindowsEventFlagTest, waitAny_setAll_autoReset)
{
	class WaitAnyAutoReset : public BaseRunnable {
	public:
		WaitAnyAutoReset(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			OSWrapper::Error err = m_ef->waitAny();
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(OSWrapper::OK, err);
			LONGS_EQUAL(0, m_ef->getCurrentPattern());
		}
	};
	class SetAll : public BaseRunnable {
	public:
		SetAll(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			Thread::sleep(10);
			OSWrapper::Error err = m_ef->setAll();
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(OSWrapper::OK, err);
		}
	};
	testTwoThreadsSharingOneEventFlag<WaitAnyAutoReset, SetAll>(true);
}

TEST(WindowsEventFlagTest, tryWaitAny_setAll_autoReset)
{
	class WaitAnyAutoReset : public BaseRunnable {
	public:
		WaitAnyAutoReset(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			Thread::sleep(10);
			OSWrapper::Error err = m_ef->tryWaitAny();
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(OSWrapper::OK, err);
			LONGS_EQUAL(0, m_ef->getCurrentPattern());
		}
	};
	class SetAll : public BaseRunnable {
	public:
		SetAll(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			OSWrapper::Error err = m_ef->setAll();
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(OSWrapper::OK, err);
		}
	};
	testTwoThreadsSharingOneEventFlag<WaitAnyAutoReset, SetAll>(true);
}

TEST(WindowsEventFlagTest, waitAny_setAll_resetAll)
{
	class WaitAnyManualReset : public BaseRunnable {
	public:
		WaitAnyManualReset(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			OSWrapper::Error err = m_ef->waitAny();
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(OSWrapper::OK, err);
			LONGS_EQUAL(EventFlag::Pattern().set(), m_ef->getCurrentPattern());
			err = m_ef->resetAll();
			LONGS_EQUAL(OSWrapper::OK, err);
			LONGS_EQUAL(0, m_ef->getCurrentPattern());
		}
	};
	class SetAll : public BaseRunnable {
	public:
		SetAll(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			Thread::sleep(10);
			OSWrapper::Error err = m_ef->setAll();
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(OSWrapper::OK, err);
		}
	};
	testTwoThreadsSharingOneEventFlag<WaitAnyManualReset, SetAll>(false);
}

TEST(WindowsEventFlagTest, waitOne_setOne_resetOne)
{
	class WaitOne : public BaseRunnable {
	public:
		WaitOne(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			OSWrapper::Error err = m_ef->waitOne(2);
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(OSWrapper::OK, err);
			LONGS_EQUAL(1 << 2, m_ef->getCurrentPattern());

			err = m_ef->resetOne(2);
			LONGS_EQUAL(OSWrapper::OK, err);
			LONGS_EQUAL(0, m_ef->getCurrentPattern());
		}
	};

	class SetOne : public BaseRunnable {
	public:
		SetOne(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			Thread::sleep(10);
			OSWrapper::Error err = m_ef->setOne(2);
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(OSWrapper::OK, err);
		}
	};
	testTwoThreadsSharingOneEventFlag<WaitOne, SetOne>(false);
}

TEST(WindowsEventFlagTest, tryWaitOne_setOne_resetOne)
{
	class WaitOne : public BaseRunnable {
	public:
		WaitOne(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			Thread::sleep(10);
			OSWrapper::Error err = m_ef->tryWaitOne(2);
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(OSWrapper::OK, err);
			LONGS_EQUAL(1 << 2, m_ef->getCurrentPattern());

			err = m_ef->resetOne(2);
			LONGS_EQUAL(OSWrapper::OK, err);
			LONGS_EQUAL(0, m_ef->getCurrentPattern());
		}
	};

	class SetOne : public BaseRunnable {
	public:
		SetOne(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			OSWrapper::Error err = m_ef->setOne(2);
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(OSWrapper::OK, err);
		}
	};
	testTwoThreadsSharingOneEventFlag<WaitOne, SetOne>(false);
}

TEST(WindowsEventFlagTest, waitOne_setOne_resetOne_InvalidParameter)
{
	class WaitOne : public BaseRunnable {
	public:
		WaitOne(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			OSWrapper::Error err = m_ef->waitOne(EventFlag::Pattern().size());
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(OSWrapper::InvalidParameter, err);

			err = m_ef->resetOne(EventFlag::Pattern().size());
			LONGS_EQUAL(OSWrapper::InvalidParameter, err);
		}
	};

	class SetOne : public BaseRunnable {
	public:
		SetOne(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			OSWrapper::Error err = m_ef->setOne(EventFlag::Pattern().size());
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(OSWrapper::InvalidParameter, err);
		}
	};
	testTwoThreadsSharingOneEventFlag<WaitOne, SetOne>(false);
}

TEST(WindowsEventFlagTest, wait_set_reset_AND)
{
	class WaitPattern : public BaseRunnable {
	public:
		WaitPattern(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			EventFlag::Pattern ptn;
			OSWrapper::Error err = m_ef->wait(EventFlag::Pattern(0x0F), EventFlag::AND, &ptn);
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(OSWrapper::OK, err);
			LONGS_EQUAL(0x0F, ptn);
			LONGS_EQUAL(0x0F, m_ef->getCurrentPattern());

			err = m_ef->reset(EventFlag::Pattern(0x01));
			LONGS_EQUAL(OSWrapper::OK, err);
			LONGS_EQUAL(0x0E, m_ef->getCurrentPattern());
			err = m_ef->reset(EventFlag::Pattern(0x02));
			LONGS_EQUAL(OSWrapper::OK, err);
			LONGS_EQUAL(0x0C, m_ef->getCurrentPattern());
			err = m_ef->reset(EventFlag::Pattern(0x0C));
			LONGS_EQUAL(OSWrapper::OK, err);
			LONGS_EQUAL(0x00, m_ef->getCurrentPattern());
		}
	};

	class SetPattern : public BaseRunnable {
	public:
		SetPattern(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			Thread::sleep(10);
			OSWrapper::Error err = m_ef->set(EventFlag::Pattern(0x01));
			Thread::sleep(10);
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(OSWrapper::OK, err);
			LONGS_EQUAL(0x01, m_ef->getCurrentPattern());

			err = m_ef->set(EventFlag::Pattern(0x0E));
			LONGS_EQUAL(OSWrapper::OK, err);
		}
	};
	testTwoThreadsSharingOneEventFlag<WaitPattern, SetPattern>(false);
}

TEST(WindowsEventFlagTest, timedWait_set_reset_AND_Timeout100)
{
	class WaitPattern : public BaseRunnable {
	public:
		WaitPattern(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			EventFlag::Pattern ptn;
			OSWrapper::Error err = m_ef->timedWait(EventFlag::Pattern(0x0F), EventFlag::AND, &ptn, Timeout(100));
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(OSWrapper::OK, err);
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
			OSWrapper::Error err = m_ef->set(EventFlag::Pattern(0x01));
			Thread::sleep(10);
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(OSWrapper::OK, err);
			LONGS_EQUAL(0x01, m_ef->getCurrentPattern());

			err = m_ef->set(EventFlag::Pattern(0x0E));
			LONGS_EQUAL(OSWrapper::OK, err);
		}
	};
	testTwoThreadsSharingOneEventFlag<WaitPattern, SetPattern>(true);
}

TEST(WindowsEventFlagTest, tryWait_set_reset_AND_TimedOut)
{
	class WaitPattern : public BaseRunnable {
	public:
		WaitPattern(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			EventFlag::Pattern ptn;
			OSWrapper::Error err = m_ef->tryWait(EventFlag::Pattern(0x0F), EventFlag::AND, &ptn);
			{
				std::lock_guard<std::mutex> lock(s_mutex);
				LONGS_EQUAL(OSWrapper::TimedOut, err);
			}
			Thread::sleep(50);
			err = m_ef->tryWait(EventFlag::Pattern(0x0F), EventFlag::AND, &ptn);
			{
				std::lock_guard<std::mutex> lock(s_mutex);
				LONGS_EQUAL(OSWrapper::OK, err);
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
			OSWrapper::Error err = m_ef->set(EventFlag::Pattern(0x01));
			Thread::sleep(10);
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(OSWrapper::OK, err);
			LONGS_EQUAL(0x01, m_ef->getCurrentPattern());

			err = m_ef->set(EventFlag::Pattern(0x0E));
			LONGS_EQUAL(OSWrapper::OK, err);
		}
	};
	testTwoThreadsSharingOneEventFlag<WaitPattern, SetPattern>(true);
}

TEST(WindowsEventFlagTest, wait_set_reset_OR)
{
	class WaitPattern : public BaseRunnable {
	public:
		WaitPattern(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			EventFlag::Pattern ptn;
			OSWrapper::Error err = m_ef->wait(EventFlag::Pattern(0x0F), EventFlag::OR, &ptn);
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(OSWrapper::OK, err);
			LONGS_EQUAL(0x01, ptn);
			LONGS_EQUAL(0x01, m_ef->getCurrentPattern());

			err = m_ef->reset(EventFlag::Pattern(0x01));
			LONGS_EQUAL(OSWrapper::OK, err);
			LONGS_EQUAL(0x00, m_ef->getCurrentPattern());
		}
	};

	class SetPattern : public BaseRunnable {
	public:
		SetPattern(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			Thread::sleep(10);
			OSWrapper::Error err = m_ef->set(EventFlag::Pattern(0x01));
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(OSWrapper::OK, err);
		}
	};
	testTwoThreadsSharingOneEventFlag<WaitPattern, SetPattern>(false);
}

TEST(WindowsEventFlagTest, timedWait_set_reset_OR_Timeout100)
{
	class WaitPattern : public BaseRunnable {
	public:
		WaitPattern(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			EventFlag::Pattern ptn;
			OSWrapper::Error err = m_ef->timedWait(EventFlag::Pattern(0x0F), EventFlag::OR, &ptn, Timeout(100));
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(OSWrapper::OK, err);
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
			OSWrapper::Error err = m_ef->set(EventFlag::Pattern(0x01));
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(OSWrapper::OK, err);
		}
	};
	testTwoThreadsSharingOneEventFlag<WaitPattern, SetPattern>(true);
}

TEST(WindowsEventFlagTest, tryWait_set_reset_OR_TimedOut)
{
	class WaitPattern : public BaseRunnable {
	public:
		WaitPattern(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			EventFlag::Pattern ptn;
			OSWrapper::Error err = m_ef->tryWait(EventFlag::Pattern(0x0F), EventFlag::OR, &ptn);
			{
				std::lock_guard<std::mutex> lock(s_mutex);
				LONGS_EQUAL(OSWrapper::TimedOut, err);
			}
			Thread::sleep(50);
			err = m_ef->tryWait(EventFlag::Pattern(0x0F), EventFlag::OR, &ptn);
			{
				std::lock_guard<std::mutex> lock(s_mutex);
				LONGS_EQUAL(OSWrapper::OK, err);
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
			OSWrapper::Error err = m_ef->set(EventFlag::Pattern(0x01));
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(OSWrapper::OK, err);
		}
	};
	testTwoThreadsSharingOneEventFlag<WaitPattern, SetPattern>(true);
}

IGNORE_TEST(WindowsEventFlagTest, wait_OtherThreadWaiting)
{
	class WaitOK : public BaseRunnable {
	public:
		WaitOK(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			OSWrapper::Error err = m_ef->wait(EventFlag::Pattern(0x0F), EventFlag::OR, 0);
			{
				std::lock_guard<std::mutex> lock(s_mutex);
				LONGS_EQUAL(OSWrapper::OK, err);
				LONGS_EQUAL(0x01, m_ef->getCurrentPattern());
			}

			err = m_ef->wait(EventFlag::Pattern(0x0F), EventFlag::OR, 0);
			{
				std::lock_guard<std::mutex> lock(s_mutex);
				LONGS_EQUAL(OSWrapper::OK, err);
				LONGS_EQUAL(0x01, m_ef->getCurrentPattern());

				err = m_ef->reset(EventFlag::Pattern(0x01));
				LONGS_EQUAL(OSWrapper::OK, err);
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
			OSWrapper::Error err = m_ef->wait(EventFlag::Pattern(0x0F), EventFlag::OR, 0);
			LONGS_EQUAL(OSWrapper::OtherThreadWaiting, err);

			err = m_ef->set(EventFlag::Pattern(0x01));
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(OSWrapper::OK, err);
		}
	};
	testTwoThreadsSharingOneEventFlag<WaitOK, WaitFailed>(false);
}

TEST(WindowsEventFlagTest, wait_TwoThreadsWaiting)
{
	class Set2 : public BaseRunnable {
	public:
		Set2(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			Thread::sleep(10);
			OSWrapper::Error err = m_ef->set(EventFlag::Pattern(0x02));
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(OSWrapper::OK, err);
		}
	};

	class Wait1 : public BaseRunnable {
	public:
		Wait1(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			Thread::sleep(10);
			Set2 s(m_ef);
			Thread* t = Thread::create(&s);
			t->start();

			OSWrapper::Error err = m_ef->wait(EventFlag::Pattern(0x01), EventFlag::OR, 0);
			{
				std::lock_guard<std::mutex> lock(s_mutex);
				LONGS_EQUAL(OSWrapper::OK, err);
			}
			err = m_ef->reset(EventFlag::Pattern(0x01));
			{
				std::lock_guard<std::mutex> lock(s_mutex);
				LONGS_EQUAL(OSWrapper::OK, err);
			}

			Thread::destroy(t);
		}
	};

	class Wait2 : public BaseRunnable {
	public:
		Wait2(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			OSWrapper::Error err = m_ef->wait(EventFlag::Pattern(0x02), EventFlag::OR, 0);
			{
				std::lock_guard<std::mutex> lock(s_mutex);
				LONGS_EQUAL(OSWrapper::OK, err);
			}

			err = m_ef->set(EventFlag::Pattern(0x01));
			{
				std::lock_guard<std::mutex> lock(s_mutex);
				LONGS_EQUAL(OSWrapper::OK, err);
			}
		}
	};
	testTwoThreadsSharingOneEventFlag<Wait1, Wait2>(false);
}

TEST(WindowsEventFlagTest, waitPattern_InvalidParameter)
{
	class WaitFailedInvalidMode : public BaseRunnable {
	public:
		WaitFailedInvalidMode(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			OSWrapper::Error err = m_ef->wait(EventFlag::Pattern(0x0F), (EventFlag::Mode)-1, 0);
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(OSWrapper::InvalidParameter, err);
		}
	};

	class WaitFailedInvalidPattern : public BaseRunnable {
	public:
		WaitFailedInvalidPattern(EventFlag* e) : BaseRunnable(e) {}
		virtual void run()
		{
			Thread::sleep(10);
			OSWrapper::Error err = m_ef->wait(EventFlag::Pattern(0x00), EventFlag::OR, 0);
			std::lock_guard<std::mutex> lock(s_mutex);
			LONGS_EQUAL(OSWrapper::InvalidParameter, err);
		}
	};
	testTwoThreadsSharingOneEventFlag<WaitFailedInvalidMode, WaitFailedInvalidPattern>(true);
}

