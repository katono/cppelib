#ifndef OS_WRAPPER_TIMEOUT_H_INCLUDED
#define OS_WRAPPER_TIMEOUT_H_INCLUDED

namespace OSWrapper {

class Timeout {
private:
	long m_milliseconds;
public:
	explicit Timeout(long milliseconds) : m_milliseconds(milliseconds) {}
	operator long() const { return m_milliseconds; }

	static const Timeout POLLING;
	static const Timeout FOREVER;
};

}

#endif // OS_WRAPPER_TIMEOUT_H_INCLUDED
