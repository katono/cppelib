#ifndef OS_WRAPPER_EVENT_FLAG_H_INCLUDED
#define OS_WRAPPER_EVENT_FLAG_H_INCLUDED

#include <cstddef>
#include "Container/BitPattern.h"
#include "Timeout.h"
#include "OSWrapperError.h"

namespace OSWrapper {

class EventFlagFactory;

void registerEventFlagFactory(EventFlagFactory* factory);

class EventFlag {
protected:
	virtual ~EventFlag() {}

public:
	enum Mode {
		OR,
		AND
	};

	typedef Container::BitPattern<unsigned int> Pattern;

	static EventFlag* create(bool autoReset);
	static void destroy(EventFlag* e);

	virtual Error waitAny() = 0;
	virtual Error waitOne(std::size_t pos) = 0;
	virtual Error wait(Pattern bitPattern, Mode waitMode, Pattern* releasedPattern) = 0;

	virtual Error tryWaitAny() = 0;
	virtual Error tryWaitOne(std::size_t pos) = 0;
	virtual Error tryWait(Pattern bitPattern, Mode waitMode, Pattern* releasedPattern) = 0;

	virtual Error timedWaitAny(Timeout tmout) = 0;
	virtual Error timedWaitOne(std::size_t pos, Timeout tmout) = 0;
	virtual Error timedWait(Pattern bitPattern, Mode waitMode, Pattern* releasedPattern, Timeout tmout) = 0;

	virtual Error setAll() = 0;
	virtual Error setOne(std::size_t pos) = 0;
	virtual Error set(Pattern bitPattern) = 0;

	virtual Error resetAll() = 0;
	virtual Error resetOne(std::size_t pos) = 0;
	virtual Error reset(Pattern bitPattern) = 0;

	virtual Pattern getCurrentPattern() const = 0;

};

}

#endif // OS_WRAPPER_EVENT_FLAG_H_INCLUDED
