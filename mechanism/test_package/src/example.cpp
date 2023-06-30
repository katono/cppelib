#include "Assertion/Assertion.h"
#include "Container/Array.h"
#include "OSWrapper/Thread.h"

int main()
{
	CHECK_ASSERT(true);

	Container::Array<int, 10> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	a[0] = 1;

	OSWrapper::registerThreadFactory(0);
	return 0;
}
