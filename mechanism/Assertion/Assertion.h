#ifndef ASSERTION_ASSERTION_H_INCLUDED
#define ASSERTION_ASSERTION_H_INCLUDED

//! @cond
#define ASSERTION_ASSERT_STRINGIFY(n) #n
#define ASSERTION_ASSERT_TOSTRING(n) ASSERTION_ASSERT_STRINGIFY(n)
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
			throw Assertion::Failure(__FILE__ "(" ASSERTION_ASSERT_TOSTRING(__LINE__) "): Assertion failed (" #x ")");\
		}\
	} while (false)


#ifdef NDEBUG
#define DEBUG_ASSERT(x)
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
	const char* message() const
	{
		return m_msg;
	}
private:
	const char* m_msg;
};

}

#endif // ASSERTION_ASSERTION_H_INCLUDED
