#ifndef OS_WRAPPER_RUNNABLE_H_INCLUDED
#define OS_WRAPPER_RUNNABLE_H_INCLUDED

namespace OSWrapper {

/*!
 * @brief Interface for implementing an active class
 */
class Runnable {
public:
	virtual ~Runnable() {}
	/*!
	 * @brief Run the active class
	 * @note This is the entry point of the active class.
	 */
	virtual void run() = 0;
};

}

#endif // OS_WRAPPER_RUNNABLE_H_INCLUDED
