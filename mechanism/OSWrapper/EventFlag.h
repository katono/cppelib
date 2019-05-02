#ifndef OS_WRAPPER_EVENT_FLAG_H_INCLUDED
#define OS_WRAPPER_EVENT_FLAG_H_INCLUDED

#include <cstddef>
#include "Container/BitPattern.h"
#include "Timeout.h"
#include "OSWrapperError.h"

namespace OSWrapper {

class EventFlagFactory;

/*!
 * @brief Register the EventFlagFactory
 * @param factory Pointer of the object of the concrete class derived from EventFlagFactory
 *
 * @note You need to call this function only one time when the application is initialized.
 */
void registerEventFlagFactory(EventFlagFactory* factory);

/*!
 * @brief Abstract class that has functions of common RTOS's event flag
 */
class EventFlag {
protected:
	virtual ~EventFlag() {}

public:
	/*!
	 * @brief Used as argument of the wait methods
	 */
	enum Mode {
		OR,
		AND
	};

	/*!
	 * @brief Type for bit pattern of EventFlag
	 */
	typedef Container::BitPattern<unsigned int> Pattern;

	/*!
	 * @brief Create an EventFlag object
	 * @param autoReset If it is true then all the bits are automatically reset OFF when the wait methods succeeds
	 * @return If this method succeeds then returns a pointer of EventFlag object, else returns null pointer
	 */
	static EventFlag* create(bool autoReset);

	/*!
	 * @brief Destroy an EventFlag object
	 * @param e Pointer of EventFlag object created by EventFlag::create()
	 *
	 * @note If e is null pointer, do nothing.
	 */
	static void destroy(EventFlag* e);

	/*!
	 * @brief Block the current thread until any bit of this EventFlag is set ON
	 *
	 * If this EventFlag object has created with auto reset flag, when this method succeeds, the bit pattern of this EventFlag object is all reset OFF.
	 *
	 * @retval OK Success. Any bit is set ON
	 * @retval CalledByNonThread Called from non thread context (interrupt handler, timer, etc)
	 *
	 * @note Same as timedWaitAny(Timeout::FOREVER)
	 */
	virtual Error waitAny() = 0;

	/*!
	 * @brief Block the current thread until the bit at position pos of this EventFlag is set ON
	 *
	 * If this EventFlag object has created with auto reset flag, when this method succeeds, the bit pattern of this EventFlag object is all reset OFF.
	 *
	 * @param pos Position of the bit
	 * @retval OK Success. The bit at position pos is set ON
	 * @retval CalledByNonThread Called from non thread context (interrupt handler, timer, etc)
	 * @retval InvalidParameter pos >= EventFlag::Pattern().size()
	 *
	 * @note Same as timedWaitOne(pos, Timeout::FOREVER)
	 */
	virtual Error waitOne(std::size_t pos) = 0;

	/*!
	 * @brief Block the current thread until the condition is satisfied
	 *
	 * The condition is the same as timedWait().
	 *
	 * If this EventFlag object has created with auto reset flag, when this method succeeds, the bit pattern of this EventFlag object is all reset OFF.
	 *
	 * @param bitPattern Requested bit pattern
	 * @param waitMode Specify EventFlag::OR or EventFlag::AND
	 * @param releasedPattern Pointer of variable that stores the bit pattern of this EventFlag when the condition is satisfied. If null pointer, not accessed
	 * @retval OK Success. The condition is satisfied
	 * @retval CalledByNonThread Called from non thread context (interrupt handler, timer, etc)
	 * @retval InvalidParameter All the bits of bitPattern are OFF, or waitMode is invalid value
	 *
	 * @note Same as timedWait(bitPattern, waitMode, releasedPattern, Timeout::FOREVER)
	 */
	virtual Error wait(Pattern bitPattern, Mode waitMode, Pattern* releasedPattern) = 0;

	/*!
	 * @brief Query without blocking whether any bit of this EventFlag is set ON
	 *
	 * If this EventFlag object has created with auto reset flag, when this method succeeds, the bit pattern of this EventFlag object is all reset OFF.
	 *
	 * @retval OK Success. Any bit is set ON
	 * @retval TimedOut All the bits of this EventFlag object are OFF
	 *
	 * @note Same as timedWaitAny(Timeout::POLLING)
	 */
	virtual Error tryWaitAny() = 0;

	/*!
	 * @brief Query without blocking whether the bit at position pos of this EventFlag is set ON
	 *
	 * If this EventFlag object has created with auto reset flag, when this method succeeds, the bit pattern of this EventFlag object is all reset OFF.
	 *
	 * @param pos Position of the bit
	 * @retval OK Success. The bit at position pos is set ON
	 * @retval TimedOut The bit at position pos of this EventFlag object is OFF
	 * @retval InvalidParameter pos >= EventFlag::Pattern().size()
	 *
	 * @note Same as timedWaitOne(pos, Timeout::POLLING)
	 */
	virtual Error tryWaitOne(std::size_t pos) = 0;

	/*!
	 * @brief Query without blocking whether the condition is satisfied
	 *
	 * The condition is the same as timedWait().
	 *
	 * If this EventFlag object has created with auto reset flag, when this method succeeds, the bit pattern of this EventFlag object is all reset OFF.
	 *
	 * @param bitPattern Requested bit pattern
	 * @param waitMode Specify EventFlag::OR or EventFlag::AND
	 * @param releasedPattern Pointer of variable that stores the bit pattern of this EventFlag when the condition is satisfied. If null pointer, not accessed
	 * @retval OK Success. The condition is satisfied
	 * @retval TimedOut The bit pattern of this EventFlag object is not satisfied the condition
	 * @retval InvalidParameter All the bits of bitPattern are OFF, or waitMode is invalid value
	 *
	 * @note Same as timedWait(bitPattern, waitMode, releasedPattern, Timeout::POLLING)
	 */
	virtual Error tryWait(Pattern bitPattern, Mode waitMode, Pattern* releasedPattern) = 0;

	/*!
	 * @brief Block the current thread until any bit of this EventFlag is set ON but only within the limited time
	 *
	 * If this EventFlag object has created with auto reset flag, when this method succeeds, the bit pattern of this EventFlag object is all reset OFF.
	 *
	 * @param tmout The limited time
	 * @retval OK Success. Any bit is set ON
	 * @retval TimedOut The limited time was elapsed
	 * @retval CalledByNonThread Called from non thread context (interrupt handler, timer, etc)
	 *
	 * @note If tmout is Timeout::POLLING then this method queries whether the condition has been satisfied without blocking.
	 * @note If tmout is Timeout::FOREVER then this method waits forever until the condition is satisfied.
	 */
	virtual Error timedWaitAny(Timeout tmout) = 0;

	/*!
	 * @brief Block the current thread until the bit at position pos of this EventFlag is set ON but only within the limited time
	 *
	 * If this EventFlag object has created with auto reset flag, when this method succeeds, the bit pattern of this EventFlag object is all reset OFF.
	 *
	 * @param pos Position of the bit
	 * @param tmout The limited time
	 * @retval OK Success. The bit at position pos is set ON
	 * @retval TimedOut The limited time was elapsed
	 * @retval CalledByNonThread Called from non thread context (interrupt handler, timer, etc)
	 * @retval InvalidParameter pos >= EventFlag::Pattern().size()
	 *
	 * @note If tmout is Timeout::POLLING then this method queries whether the condition has been satisfied without blocking.
	 * @note If tmout is Timeout::FOREVER then this method waits forever until the condition is satisfied.
	 */
	virtual Error timedWaitOne(std::size_t pos, Timeout tmout) = 0;

	/*!
	 * @brief Block the current thread until the condition is satisfied but only within the limited time
	 *
	 * Let selfPattern is the bit pattern of this EventFlag object:
	 * - If waitMode is EventFlag::OR, the condition is (selfPattern & bitPattern) != 0
	 * - If waitMode is EventFlag::AND, the condition is (selfPattern & bitPattern) == bitPattern
	 *
	 * If this EventFlag object has created with auto reset flag, when this method succeeds, the bit pattern of this EventFlag object is all reset OFF.
	 *
	 * @param bitPattern Requested bit pattern
	 * @param waitMode Specify EventFlag::OR or EventFlag::AND
	 * @param releasedPattern Pointer of variable that stores the bit pattern of this EventFlag when the condition is satisfied. If null pointer, not accessed
	 * @param tmout The limited time
	 * @retval OK Success. The condition is satisfied
	 * @retval TimedOut The limited time was elapsed
	 * @retval CalledByNonThread Called from non thread context (interrupt handler, timer, etc)
	 * @retval InvalidParameter All the bits of bitPattern are OFF, or waitMode is invalid value
	 *
	 * @note If tmout is Timeout::POLLING then this method queries whether the condition has been satisfied without blocking.
	 * @note If tmout is Timeout::FOREVER then this method waits forever until the condition is satisfied.
	 */
	virtual Error timedWait(Pattern bitPattern, Mode waitMode, Pattern* releasedPattern, Timeout tmout) = 0;

	/*!
	 * @brief Set all the bits of this EventFlag object ON
	 * @retval OK Always success
	 */
	virtual Error setAll() = 0;

	/*!
	 * @brief Set the bit at position pos of this EventFlag object ON
	 * @param pos Position of the bit
	 * @retval OK Success
	 * @retval InvalidParameter pos >= EventFlag::Pattern().size()
	 */
	virtual Error setOne(std::size_t pos) = 0;

	/*!
	 * @brief Set the bits of bitPattern of this EventFlag object ON
	 *
	 * Let selfPattern is the bit pattern of this EventFlag object:
	 * Bitwise operation is (selfPattern |= bitPattern)
	 *
	 * @param bitPattern Requested bit pattern
	 * @retval OK Always success
	 */
	virtual Error set(Pattern bitPattern) = 0;

	/*!
	 * @brief Reset all the bits of this EventFlag object OFF
	 * @retval OK Always success
	 */
	virtual Error resetAll() = 0;

	/*!
	 * @brief Reset the bit at position pos of this EventFlag object OFF
	 * @param pos Position of the bit
	 * @retval OK Success
	 * @retval InvalidParameter pos >= EventFlag::Pattern().size()
	 */
	virtual Error resetOne(std::size_t pos) = 0;

	/*!
	 * @brief Reset the bits of bitPattern of this EventFlag object OFF
	 *
	 * Let selfPattern is the bit pattern of this EventFlag object:
	 * Bitwise operation is (selfPattern &= ~bitPattern)
	 *
	 * @param bitPattern Requested bit pattern
	 * @retval OK Always success
	 */
	virtual Error reset(Pattern bitPattern) = 0;

	/*!
	 * @brief Get current bit pattern of this EventFlag object
	 * @return Current bit pattern
	 */
	virtual Pattern getCurrentPattern() const = 0;

};

}

#endif // OS_WRAPPER_EVENT_FLAG_H_INCLUDED
