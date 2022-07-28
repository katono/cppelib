#ifndef ASSERTION_ASSERTION_H_INCLUDED
#define ASSERTION_ASSERTION_H_INCLUDED

//! @cond
#define ASSERTION_ASSERT_STRINGIFY(n) #n
#define ASSERTION_ASSERT_TOSTRING(n) ASSERTION_ASSERT_STRINGIFY(n)

#ifdef NO_EXCEPTIONS
#include <cstdlib>
#define ASSERTION_ASSERTION_FAILURE(x) Assertion::UserSpecificFunc::assertFail(x)
#else
#define ASSERTION_ASSERTION_FAILURE(x) throw Assertion::Failure(x)
#endif
//! @endcond

/*!
 * @brief Used instead of standard assert() macro for implementing Design by Contract
 * @note This macro is always enabled.
 *
 * @attention If an assertion fails, it throws the exception Assertion::Failure.
 *            If this exception is thrown, you must do shutdown your application safely.
 */
#define CHECK_ASSERT(x)\
	do {\
		if (!(x)) {\
			ASSERTION_ASSERTION_FAILURE(__FILE__ "(" ASSERTION_ASSERT_TOSTRING(__LINE__) "): Assertion failed (" #x ")");\
		}\
	} while (false)


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
 * @brief Thrown class when CHECK_ASSERT() macro fails
 */
class Failure {
public:
	explicit Failure(const char* msg) : m_msg(msg) {}
	/*!
	 * @brief Get the message when assertion fails
	 * @return message when assertion fails
	 */
	const char* message() const
	{
		return m_msg;
	}
private:
	const char* m_msg;
};

/*!
 * @brief User specific function class
 *
 * If NO_EXCEPTIONS macro is defined, the specified functions are called when CHECK_ASSERT() macro fails.
 * If NO_EXCEPTIONS macro is not defined, the specified functions are not called.
 */
class UserSpecificFunc {
public:
	/*!
	 * @brief The same function type as std::puts
	 */
	typedef int (*PutsType)(const char*);

	/*!
	 * @brief The same function type as std::abort
	 */
	typedef void (*AbortType)();

	/*!
	 * @brief Set user specific function like puts
	 * @param func user specific function like puts
	 *
	 * When assertion fails, the message is output by func.
	 * If not called setPuts(), no output.
	 */
	static void setPuts(PutsType func)
	{
		getPuts() = func;
	}

	/*!
	 * @brief Set user specific function like abort
	 * @param func user specific function like abort
	 *
	 * When assertion fails, the application must be aborted by func.
	 * If not called setAbort() function, aborted by std::abort.
	 */
	static void setAbort(AbortType func)
	{
		getAbort() = func;
	}

//! @cond
private:
	UserSpecificFunc();

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

#ifdef NO_EXCEPTIONS
public:
	static void assertFail(const char* msg)
	{
		PutsType& putsFunc = getPuts();
		if (putsFunc != reinterpret_cast<PutsType>(0)) {
			putsFunc(msg);
		}
		AbortType& abortFunc = getAbort();
		if (abortFunc != reinterpret_cast<AbortType>(0)) {
			abortFunc();
		} else {
			std::abort();
		}
	}
#endif
//! @endcond
};

}

#endif // ASSERTION_ASSERTION_H_INCLUDED
