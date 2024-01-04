#include "Container/Array.h"
#include <algorithm>
#include <functional>
#include <cstdio>
#include <ctime>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include "CppUTest/TestHarness.h"

using Container::Array;

TEST_GROUP(ArrayBenchmark) {
	static const std::size_t SIZE = 1000000;
	Array<int, SIZE> x;
	int y[SIZE];
	void setup()
	{
	}
	void teardown()
	{
		std::printf("\n\n");
	}
	unsigned long get_msec(void)
	{
#ifdef _WIN32
		return GetTickCount();
#else
		struct timeval tv;
		gettimeofday(&tv, NULL);
		return tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
	}
};

TEST(ArrayBenchmark, benchmark)
{
	unsigned long t;
	t = get_msec();
	for (std::size_t i = 0; i < x.size(); ++i) {
		x[i] = i;
	}
	std::printf("Array::operator[], %ld, %ld ms\n", x.size(), get_msec() - t);

	t = get_msec();
	for (std::size_t i = 0; i < sizeof y / sizeof y[0]; ++i) {
		y[i] = i;
	}
	std::printf("C array [], %ld, %ld ms\n", SIZE, get_msec() - t);
}

