#ifndef OS_WRAPPER_EVENT_FLAG_H_INCLUDED
#define OS_WRAPPER_EVENT_FLAG_H_INCLUDED

#include <cstddef>
#include "Container/BitPattern.h"
#include "Timeout.h"

namespace OSWrapper {

class EventFlagFactory;

class EventFlag {
public:
	enum Error {
		OK,
		TimedOut,
		CalledByNonThread,
		InvalidParameter,
		OtherThreadWaiting,
		OtherError
	};

	enum Mode {
		OR,
		AND
	};

	typedef Container::BitPattern<unsigned int> Pattern;

	virtual ~EventFlag() {}

	static void setFactory(EventFlagFactory* factory);

	static EventFlag* create(bool autoReset);
	static void destroy(EventFlag* e);

	virtual Error waitAny(Timeout tmout = Timeout::FOREVER) = 0;
	virtual Error waitOne(std::size_t pos, Timeout tmout = Timeout::FOREVER) = 0;
	virtual Error wait(Pattern bitPattern, Mode waitMode, Pattern* releasedPattern, Timeout tmout = Timeout::FOREVER) = 0;

	virtual Error setAll() = 0;
	virtual Error setOne(std::size_t pos) = 0;
	virtual Error set(Pattern bitPattern) = 0;

	virtual Error resetAll() = 0;
	virtual Error resetOne(std::size_t pos) = 0;
	virtual Error reset(Pattern bitPattern) = 0;

	virtual Pattern getCurrentPattern() const = 0;

private:
	static EventFlagFactory* m_factory;
};

}

#endif // OS_WRAPPER_EVENT_FLAG_H_INCLUDED
