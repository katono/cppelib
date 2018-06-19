#ifndef ASSERTION_ASSERTION_H_INCLUDED
#define ASSERTION_ASSERTION_H_INCLUDED

#define ASSERTION_ASSERT_STRINGIFY(n) #n
#define ASSERTION_ASSERT_TOSTRING(n) ASSERTION_ASSERT_STRINGIFY(n)

#define CHECK_ASSERT(x)\
	do {\
		if (!(x)) {\
			throw Assertion::Failure(__FILE__ "(" ASSERTION_ASSERT_TOSTRING(__LINE__) "): Assertion failed (" #x ")");\
		}\
	} while (false)


#ifdef NDEBUG
#define DEBUG_ASSERT(x)
#else
#define DEBUG_ASSERT(x)  CHECK_ASSERT(x)
#endif


namespace Assertion {

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
