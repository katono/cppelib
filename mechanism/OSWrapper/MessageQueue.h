#ifndef OS_WRAPPER_MESSAGE_QUEUE_H_INCLUDED
#define OS_WRAPPER_MESSAGE_QUEUE_H_INCLUDED

#include <cstddef>
#include <new>
#include "Timeout.h"
#include "OSWrapperError.h"
#include "Mutex.h"
#include "EventFlag.h"
#include "FixedMemoryPool.h"
#include "Assertion/Assertion.h"

namespace OSWrapper {

/*!
 * @brief Class template of message queue
 * @tparam T Type of element
 *
 * @note All the methods are thread-safe.
 */
template <typename T>
class MessageQueue {
public:
	/*!
	 * @brief Create a MessageQueue object
	 * @param maxSize Max queue size
	 * @return If this method succeeds then returns a pointer of MessageQueue object, else returns null pointer
	 */
	static MessageQueue* create(std::size_t maxSize)
	{
		const std::size_t alignedMQSize =
			(sizeof(MessageQueue) + (sizeof(double) - 1U)) & ~(sizeof(double) - 1U);
		const std::size_t rbBufSize = maxSize + 1U;
		const std::size_t poolBufSize = alignedMQSize + (sizeof(T) * rbBufSize);

		FixedMemoryPool* pool = FixedMemoryPool::create(poolBufSize,
				FixedMemoryPool::getRequiredMemorySize(poolBufSize, 1U));
		if (pool == 0) {
			return 0;
		}

		void* p = pool->allocate();
		if (p == 0) {
			FixedMemoryPool::destroy(pool);
			return 0;
		}

		MessageQueue* m = new(p) MessageQueue(pool, 
				reinterpret_cast<T*>(static_cast<unsigned char*>(p) + alignedMQSize), rbBufSize);
		if (!m->createOSObjects()) {
			destroy(m);
			return 0;
		}
		return m;
	}

	/*!
	 * @brief Destroy a MessageQueue object
	 * @param m Pointer of MessageQueue object created by MessageQueue<T>::create()
	 *
	 * @note If m is null pointer, do nothing.
	 */
	static void destroy(MessageQueue* m)
	{
		if (m == 0) {
			return;
		}
		FixedMemoryPool* pool = m->m_pool;
		m->~MessageQueue();
		pool->deallocate(m);
		FixedMemoryPool::destroy(pool);
	}

	/*!
	 * @brief Send the message
	 *
	 * This method enqueues the message into this queue.
	 * If this queue is full, block the current thread until to be dequeued by receive methods.
	 *
	 * @param msg Message to send
	 * @retval OK Success. The message was sent
	 * @retval CalledByNonThread Called from non thread context (interrupt handler, timer, etc)
	 *
	 * @note Same as timedSend(msg, Timeout::FOREVER)
	 */
	Error send(const T& msg)
	{
		return timedSend(msg, Timeout::FOREVER);
	}

	/*!
	 * @brief Send the message without blocking
	 *
	 * This method enqueues the message into this queue.
	 * If this queue is full, returns TimedOut immediately.
	 *
	 * @param msg Message to send
	 * @retval OK Success. The message was sent
	 * @retval TimedOut This queue is full
	 *
	 * @note Same as timedSend(msg, Timeout::POLLING)
	 */
	Error trySend(const T& msg)
	{
		return timedSend(msg, Timeout::POLLING);
	}

	/*!
	 * @brief Send the message within the limited time
	 *
	 * This method enqueues the message into this queue.
	 * If this queue is full, block the current thread until to be dequeued by receive methods but only within the limited time.
	 *
	 * @param msg Message to send
	 * @param tmout The limited time
	 * @retval OK Success. The message was sent
	 * @retval TimedOut The limited time was elapsed
	 * @retval CalledByNonThread Called from non thread context (interrupt handler, timer, etc)
	 *
	 * @note If tmout is Timeout::POLLING then this method tries to send the message without blocking.
	 * @note If tmout is Timeout::FOREVER then this method waits forever until has sent the message.
	 */
	Error timedSend(const T& msg, Timeout tmout)
	{
		Error err = m_mtxSend->timedLock(tmout);
		if (err != OK) {
			return err;
		}
		LockGuard lock(m_mtxSend, LockGuard::ADOPT_LOCK);

		if (isFull()) {
			err = m_event->timedWait(EV_NOT_FULL, EventFlag::OR, 0, tmout);
			if (err != OK) {
				return err;
			}
		}

		err = OK;
		try {
			push(msg);
		}
		catch (const Assertion::Failure&) {
			throw;
		}
		catch (...) {
			err = OtherError;
		}
		return err;
	}

	/*!
	 * @brief Receive the message
	 *
	 * This method dequeues the message from this queue.
	 * If this queue is empty, block the current thread until to be enqueued by send methods.
	 *
	 * @param msg Pointer of variable that stores the message received. If null pointer, not accessed
	 * @retval OK Success. The message was received
	 * @retval CalledByNonThread Called from non thread context (interrupt handler, timer, etc)
	 *
	 * @note Same as timedReceive(msg, Timeout::FOREVER)
	 */
	Error receive(T* msg)
	{
		return timedReceive(msg, Timeout::FOREVER);
	}

	/*!
	 * @brief Receive the message without blocking
	 *
	 * This method dequeues the message from this queue.
	 * If this queue is empty, returns TimedOut immediately.
	 *
	 * @param msg Pointer of variable that stores the message received. If null pointer, not accessed
	 * @retval OK Success. The message was received
	 * @retval TimedOut This queue is empty
	 *
	 * @note Same as timedReceive(msg, Timeout::POLLING)
	 */
	Error tryReceive(T* msg)
	{
		return timedReceive(msg, Timeout::POLLING);
	}

	/*!
	 * @brief Receive the message within the limited time
	 *
	 * This method dequeues the message from this queue.
	 * If this queue is empty, block the current thread until to be enqueued by send methods but only within the limited time.
	 *
	 * @param msg Pointer of variable that stores the message received. If null pointer, not accessed
	 * @param tmout The limited time
	 * @retval OK Success. The message was received
	 * @retval TimedOut The limited time was elapsed
	 * @retval CalledByNonThread Called from non thread context (interrupt handler, timer, etc)
	 *
	 * @note If tmout is Timeout::POLLING then this method tries to receive the message without blocking.
	 * @note If tmout is Timeout::FOREVER then this method waits forever until has received the message.
	 */
	Error timedReceive(T* msg, Timeout tmout)
	{
		Error err = m_mtxRecv->timedLock(tmout);
		if (err != OK) {
			return err;
		}
		LockGuard lock(m_mtxRecv, LockGuard::ADOPT_LOCK);

		if (isEmpty()) {
			err = m_event->timedWait(EV_NOT_EMPTY, EventFlag::OR, 0, tmout);
			if (err != OK) {
				return err;
			}
		}

		err = OK;
		try {
			pop(msg);
		}
		catch (const Assertion::Failure&) {
			throw;
		}
		catch (...) {
			err = OtherError;
		}
		return err;
	}

	/*!
	 * @brief Get the queue size
	 * @return Queue size
	 */
	std::size_t getSize() const
	{
		LockGuard lock(m_mtxRB);
		return m_rb.getSize();
	}

	/*!
	 * @brief Get the max queue size
	 * @return Max queue size
	 */
	std::size_t getMaxSize() const
	{
		LockGuard lock(m_mtxRB);
		return m_rb.getMaxSize();
	}

private:
	class RingBuf {
	private:
		std::size_t m_begin;
		std::size_t m_end;
		std::size_t m_bufSize;
		T* m_buf;

		std::size_t next_idx(std::size_t idx) const
		{
			if (idx + 1U < m_bufSize) {
				return idx + 1U;
			}
			return idx + 1U - m_bufSize;
		}

		void construct(T* p, const T& val) { new(p) T(val); }
		void destroy(T* p) { p->~T(); }

		RingBuf(const RingBuf&);
		RingBuf& operator=(const RingBuf&);
	public:
		RingBuf(T* buf, std::size_t bufSize) : m_begin(0U), m_end(0U), m_bufSize(bufSize), m_buf(buf) {}

		~RingBuf() {}

		std::size_t getSize() const
		{
			if (m_begin <= m_end) {
				return m_end - m_begin;
			}
			return m_bufSize - m_begin + m_end;
		}

		std::size_t getMaxSize() const
		{
			return m_bufSize - 1U;
		}

		bool isEmpty() const
		{
			return m_begin == m_end;
		}

		bool isFull() const
		{
			return getSize() == getMaxSize();
		}

		void push(const T& data)
		{
			construct(&m_buf[m_end], data);
			m_end = next_idx(m_end);
		}

		void pop(T* data)
		{
			if (data != 0) {
				*data = m_buf[m_begin];
			}
			destroy(&m_buf[m_begin]);
			m_begin = next_idx(m_begin);
		}

	};
	RingBuf m_rb;

	FixedMemoryPool* m_pool;
	Mutex* m_mtxRB;
	Mutex* m_mtxSend;
	Mutex* m_mtxRecv;
	EventFlag* m_event;

	static const EventFlag::Pattern EV_NOT_EMPTY;
	static const EventFlag::Pattern EV_NOT_FULL;

	MessageQueue(FixedMemoryPool* pool, T* rbBuffer, std::size_t rbBufSize)
	: m_rb(rbBuffer, rbBufSize), m_pool(pool), m_mtxRB(0), m_mtxSend(0), m_mtxRecv(0), m_event(0)
	{
	}

	~MessageQueue()
	{
		EventFlag::destroy(m_event);
		Mutex::destroy(m_mtxRecv);
		Mutex::destroy(m_mtxSend);
		Mutex::destroy(m_mtxRB);
	}

	bool createOSObjects()
	{
		m_mtxRB = Mutex::create();
		if (m_mtxRB == 0) {
			return false;
		}
		m_mtxSend = Mutex::create();
		if (m_mtxSend == 0) {
			return false;
		}
		m_mtxRecv = Mutex::create();
		if (m_mtxRecv == 0) {
			return false;
		}
		m_event = EventFlag::create(false);
		if (m_event == 0) {
			return false;
		}
		return true;
	}

	bool isEmpty() const
	{
		LockGuard lock(m_mtxRB);
		const bool is_empty = m_rb.isEmpty();
		if (is_empty) {
			m_event->reset(EV_NOT_EMPTY);
		}
		return is_empty;
	}

	bool isFull() const
	{
		LockGuard lock(m_mtxRB);
		const bool is_full = m_rb.isFull();
		if (is_full) {
			m_event->reset(EV_NOT_FULL);
		}
		return is_full;
	}

	void push(const T& msg)
	{
		LockGuard lock(m_mtxRB);
		m_rb.push(msg);
		m_event->set(EV_NOT_EMPTY);
	}

	void pop(T* msg)
	{
		LockGuard lock(m_mtxRB);
		m_rb.pop(msg);
		m_event->set(EV_NOT_FULL);
	}

	MessageQueue(const MessageQueue&);
	MessageQueue& operator=(const MessageQueue&);
};

template <typename T>
const EventFlag::Pattern MessageQueue<T>::EV_NOT_EMPTY = 0x0001U;

template <typename T>
const EventFlag::Pattern MessageQueue<T>::EV_NOT_FULL = 0x0002U;

}

#endif // OS_WRAPPER_MESSAGE_QUEUE_H_INCLUDED
