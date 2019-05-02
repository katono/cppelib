#ifndef OS_WRAPPER_TIMEOUT_H_INCLUDED
#define OS_WRAPPER_TIMEOUT_H_INCLUDED

namespace OSWrapper {

/*!
 * @brief Value object for the timeout
 *
 * @note Used for the parameter of methods that wait for.
 */
class Timeout {
private:
	long m_milliseconds;

	Timeout& operator=(const Timeout&);
public:
	/*!
	 * @brief Constructor of Timeout
	 * @param milliseconds Set timeout in milliseconds
	 */
	explicit Timeout(long milliseconds) : m_milliseconds(milliseconds) {}

	/*!
	 * @brief Implicit conversion from the Timeout object to integer value
	 */
	operator long() const { return m_milliseconds; }

	/*!
	 * @brief Constant value object for non-blocking
	 */
	static const Timeout POLLING;

	/*!
	 * @brief Constant value object for waiting forever until condition satisfied
	 */
	static const Timeout FOREVER;
};

}

#endif // OS_WRAPPER_TIMEOUT_H_INCLUDED
