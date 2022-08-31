#ifndef ASSERTION_ASSERTION_H_INCLUDED
#define ASSERTION_ASSERTION_H_INCLUDED

#ifndef CPPELIB_ASSERTION_FAILURE_BUFSIZE
/*!
 * @brief Buffer size of Assertion::Failure object
 *
 * This buffer is used as assertion failure message when an assertion fails.
 * That time, Assertion::Failure object is allocated on the stack.
 * If the stack size of the thread is small, you can define this macro by preprocessor according as the stack size.
 * If you define this buffer size smaller than the default value, assertion failure message might be cut off.
 */
#define CPPELIB_ASSERTION_FAILURE_BUFSIZE (512)
#endif

/*!
 * @brief Used instead of standard assert() macro for implementing Design by Contract
 * @note This macro is always enabled.
 *
 * If an assertion fails, you must handle to do shutdown your application safely.
 * In the assertion failure function, calls the Assertion::AssertHandler that is set by Assertion::setHandler().
 * If Assertion::AssertHandler is not set, it throws the exception Assertion::Failure.
 *
 * @attention If CPPELIB_NO_EXCEPTIONS macro is defined, the exception is not thrown, therefore you must surely set Assertion::AssertHandler.
 * .
 */
#define CHECK_ASSERT(x)\
	((x) ? (void)0 : Assertion::Failure::assertFail(__FILE__, __LINE__, #x))


#ifdef NDEBUG
#define DEBUG_ASSERT(x)  ((void)0)
#else
/*!
 * @brief The same as CHECK_ASSERT() macro
 * @note When NDEBUG macro is defined, calls of this macro is disabled by preprocessor.
 */
#define DEBUG_ASSERT(x)  CHECK_ASSERT(x)
#endif


namespace Assertion {

/*!
 * @brief Interface for handling assertion failure by CHECK_ASSERT() macro
 *
 * When CHECK_ASSERT() macro fails, the class that implements this interface handles the failure.
 */
class AssertHandler {
public:
	virtual ~AssertHandler() {}
	/*!
	 * @brief Handle an assertion failure
	 * @param msg Message when assertion fails
	 *
	 * Implement this method that never returns like this:
	 * @code
	 * void handle(const char* msg)
	 * {
	 *     std::puts(msg);
	 *     std::abort();
	 * }
	 * @endcode
	 */
	virtual void handle(const char* msg) = 0;
};

/*!
 * @brief Class used when CHECK_ASSERT() macro fails
 *
 * This object is thrown when CHECK_ASSERT() macro fails if AssertHandler is not set.
 */
class Failure {
public:
//! @cond
	Failure(const char* file, unsigned int line, const char* expr)
	: m_buf()
	{
		unsigned int remain = sizeof m_buf;
		char* pBuf = m_buf;
		pBuf = concatCString(pBuf, remain, file);
		pBuf = concatCString(pBuf, remain, "(");
		char lineBuf[16];
		toCString(lineBuf, sizeof lineBuf, line);
		pBuf = concatCString(pBuf, remain, lineBuf);
		pBuf = concatCString(pBuf, remain, "): Assertion failed (");
		pBuf = concatCString(pBuf, remain, expr);
		concatCString(pBuf, remain, ")");
	}
//! @endcond

	/*!
	 * @brief Get the message when assertion fails
	 * @return Message when assertion fails
	 */
	const char* message() const
	{
		return m_buf;
	}

//! @cond
	static void assertFail(const char* file, unsigned int line, const char* expr)
	{
		Failure failure(file, line, expr);
		AssertHandler* handler = getHandler();
		if (handler != 0) {
			handler->handle(failure.message());
		}
#ifndef CPPELIB_NO_EXCEPTIONS
		throw failure;
#endif
	}

	static void setHandler(AssertHandler* handler)
	{
		getHandler() = handler;
	}

private:
	char m_buf[CPPELIB_ASSERTION_FAILURE_BUFSIZE];

	static AssertHandler*& getHandler()
	{
		static AssertHandler* handler;
		return handler;
	}

	static void toCString(char* buf, unsigned int size, unsigned int val)
	{
		if (size == 0U) {
			return;
		}
		const unsigned int TMP_SIZE = 16U;
		char tmp[TMP_SIZE];
		const char* const numStr = "0123456789";
		unsigned int i;
		for (i = 0U; val > 0U && i < TMP_SIZE - 1U; i++) {
			tmp[i] = numStr[val % 10U];
			val /= 10U;
		}
		const unsigned int tmpLen = i;
		for (i = 0U; i < tmpLen; i++) {
			if (i < size) {
				buf[i] = tmp[tmpLen - i - 1U];
			} else {
				buf[size - 1U] = '\0';
				return;
			}
		}
		buf[tmpLen] = '\0';
	}

	static char* concatCString(char* buf, unsigned int& remain, const char* str)
	{
		if (remain <= 1U) {
			return buf;
		}
		unsigned int len = copyCString(buf, remain, str);
		remain -= len;
		return buf + len;
	}

	static unsigned int copyCString(char* to, unsigned int size, const char* from)
	{
		if (size == 0U) {
			return 0U;
		}
		const char* p = from;
		char* q = to;
		for (unsigned int i = 0U; i < size; i++) {
			if (*p == '\0') {
				*q = '\0';
				return i;
			}
			*q = *p;
			p++;
			q++;
		}
		to[size - 1U] = '\0';
		return size - 1U;
	}
//! @endcond
};

/*!
 * @brief Set AssertHandler
 * @param handler Pointer of the object that implements AssertHandler
 */
inline void setHandler(AssertHandler* handler)
{
	Failure::setHandler(handler);
}

}

#endif // ASSERTION_ASSERTION_H_INCLUDED
