#ifndef ASSERTION_ASSERTION_H_INCLUDED
#define ASSERTION_ASSERTION_H_INCLUDED

#ifndef CPPELIB_ASSERTION_FAILURE_BUFSIZE
/*!
 * @brief Buffer size of Assertion::Failure object
 *
 * This buffer is used as assertion failure message when an assertion fails.
 * That time, Assertion::Failure object is allocated on the stack.
 * If the stack size of the thread is small, you can define this macro according as the stack size.
 * If you define this buffer size smaller than the default value, assertion failure message might be cut off.
 */
#define CPPELIB_ASSERTION_FAILURE_BUFSIZE (512)
#endif

/*!
 * @brief Used instead of standard assert() macro for implementing Design by Contract
 * @note This macro is always enabled.
 *
 * @attention If an assertion fails, it throws the exception Assertion::Failure.
 *            If this exception is thrown, you must do shutdown your application safely.
 * @attention If CPPELIB_NO_EXCEPTIONS macro is defined, user specific functions are called instead of the exception.
 *            See Assertion::UserSpecificFunction.
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
 * @brief User specific function class
 *
 * If CPPELIB_NO_EXCEPTIONS macro is defined, the specified functions are called when CHECK_ASSERT() macro fails.
 * If CPPELIB_NO_EXCEPTIONS macro is not defined, the specified functions are not called.
 */
class UserSpecificFunction {
public:
	/*!
	 * @brief The same function type as std::puts()
	 */
	typedef int (*PutsType)(const char*);

	/*!
	 * @brief The same function type as std::abort()
	 */
	typedef void (*AbortType)();

	/*!
	 * @brief Set user specific function like std::puts()
	 * @param func user specific function like std::puts()
	 *
	 * When assertion fails, the message is output by \p func.
	 * If not called setPuts(), no output.
	 */
	static void setPuts(PutsType func)
	{
		getPuts() = func;
	}

	/*!
	 * @brief Set user specific function like std::abort()
	 * @param func user specific function like std::abort()
	 *
	 * When assertion fails, the application must be aborted by \p func.
	 * If CPPELIB_NO_EXCEPTIONS macro is defined, you must surely set the abort function by setAbort().
	 */
	static void setAbort(AbortType func)
	{
		getAbort() = func;
	}

//! @cond
	static PutsType& getPuts()
	{
		static PutsType func;
		return func;
	}

	static AbortType& getAbort()
	{
		static AbortType func;
		return func;
	}

private:
	UserSpecificFunction();
//! @endcond
};

/*!
 * @brief Used class when CHECK_ASSERT() macro fails
 */
class Failure {
public:
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
		pBuf = concatCString(pBuf, remain, ")");
	}

	/*!
	 * @brief Get the message when assertion fails
	 * @return message when assertion fails
	 */
	const char* message() const
	{
		return m_buf;
	}

//! @cond
	static void assertFail(const char* file, unsigned int line, const char* expr)
	{
#ifdef CPPELIB_NO_EXCEPTIONS
		UserSpecificFunction::PutsType& putsFunc = UserSpecificFunction::getPuts();
		if (putsFunc != reinterpret_cast<UserSpecificFunction::PutsType>(0)) {
			Failure failure(file, line, expr);
			putsFunc(failure.message());
		}
		UserSpecificFunction::AbortType& abortFunc = UserSpecificFunction::getAbort();
		if (abortFunc != reinterpret_cast<UserSpecificFunction::AbortType>(0)) {
			abortFunc();
		}
#else
		throw Failure(file, line, expr);
#endif
	}
private:
	char m_buf[CPPELIB_ASSERTION_FAILURE_BUFSIZE];

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

}

#endif // ASSERTION_ASSERTION_H_INCLUDED
