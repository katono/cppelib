#ifndef OS_WRAPPER_MESSAGE_QUEUE_H_INCLUDED
#define OS_WRAPPER_MESSAGE_QUEUE_H_INCLUDED

#include <cstddef>
#include "Timeout.h"
#include "OSWrapperError.h"
#include "Mutex.h"
#include "EventFlag.h"

namespace OSWrapper {

template <typename T>
class MessageQueue {
public:
	static MessageQueue* create(std::size_t maxSize)
	{
		MessageQueue* m = 0;
		try {
			m = new MessageQueue(maxSize);
			return m;
		}
		catch (...) {
			delete m;
			return 0;
		}
	}

	static void destroy(MessageQueue* m)
	{
		delete m;
	}

	Error send(const T& msg)
	{
		return timedSend(msg, Timeout::FOREVER);
	}

	Error trySend(const T& msg)
	{
		return timedSend(msg, Timeout::POLLING);
	}

	Error timedSend(const T& msg, Timeout tmout)
	{
		Error err = m_mtxSend->timedLock(tmout);
		if (err != OK) {
			return err;
		}

		if (isFull()) {
			err = m_evNotFull->timedWaitAny(tmout);
			if (err != OK) {
				m_mtxSend->unlock();
				return err;
			}
		}

		push(msg);
		m_mtxSend->unlock();
		return OK;
	}

	Error receive(T* msg)
	{
		return timedReceive(msg, Timeout::FOREVER);
	}

	Error tryReceive(T* msg)
	{
		return timedReceive(msg, Timeout::POLLING);
	}

	Error timedReceive(T* msg, Timeout tmout)
	{
		Error err = m_mtxRecv->timedLock(tmout);
		if (err != OK) {
			return err;
		}

		if (isEmpty()) {
			err = m_evNotEmpty->timedWaitAny(tmout);
			if (err != OK) {
				m_mtxRecv->unlock();
				return err;
			}
		}

		pop(msg);
		m_mtxRecv->unlock();
		return OK;
	}

	std::size_t getSize() const
	{
		LockGuard lock(m_mtxRB);
		return m_rb->getSize();
	}

	std::size_t getMaxSize() const
	{
		LockGuard lock(m_mtxRB);
		return m_rb->getMaxSize();
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

	public:
		RingBuf(std::size_t bufSize) : m_begin(0U), m_end(0U), m_bufSize(bufSize), m_buf(0)
		{
			m_buf = new T[m_bufSize];
		}

		~RingBuf()
		{
			delete[] m_buf;
		}

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

		RingBuf(const RingBuf&);
		RingBuf& operator=(const RingBuf&);
	};
	RingBuf* m_rb;

	Mutex* m_mtxRB;
	Mutex* m_mtxSend;
	Mutex* m_mtxRecv;
	EventFlag* m_evNotEmpty;
	EventFlag* m_evNotFull;

	MessageQueue(std::size_t maxSize)
	: m_rb(0), m_mtxRB(0), m_mtxSend(0), m_mtxRecv(0), m_evNotEmpty(0), m_evNotFull(0)
	{
		m_rb = new RingBuf(maxSize + 1U);
		m_mtxRB = Mutex::create();
		if (m_mtxRB == 0) {
			throw 0;
		}
		m_mtxSend = Mutex::create();
		if (m_mtxSend == 0) {
			throw 0;
		}
		m_mtxRecv = Mutex::create();
		if (m_mtxRecv == 0) {
			throw 0;
		}
		m_evNotEmpty = EventFlag::create(true);
		if (m_evNotEmpty == 0) {
			throw 0;
		}
		m_evNotFull = EventFlag::create(true);
		if (m_evNotFull == 0) {
			throw 0;
		}
	}

	~MessageQueue()
	{
		EventFlag::destroy(m_evNotFull);
		EventFlag::destroy(m_evNotEmpty);
		Mutex::destroy(m_mtxRecv);
		Mutex::destroy(m_mtxSend);
		Mutex::destroy(m_mtxRB);
		delete m_rb;
	}

	bool isEmpty() const
	{
		LockGuard lock(m_mtxRB);
		const bool is_empty = m_rb->isEmpty();
		if (is_empty) {
			m_evNotEmpty->resetAll();
		}
		return is_empty;
	}

	bool isFull() const
	{
		LockGuard lock(m_mtxRB);
		const bool is_full = m_rb->isFull();
		if (is_full) {
			m_evNotFull->resetAll();
		}
		return is_full;
	}

	void push(const T& msg)
	{
		LockGuard lock(m_mtxRB);
		m_rb->push(msg);
		m_evNotEmpty->setAll();
	}

	void pop(T* msg)
	{
		LockGuard lock(m_mtxRB);
		m_rb->pop(msg);
		m_evNotFull->setAll();
	}

	MessageQueue(const MessageQueue&);
	MessageQueue& operator=(const MessageQueue&);
};

}

#endif // OS_WRAPPER_MESSAGE_QUEUE_H_INCLUDED
