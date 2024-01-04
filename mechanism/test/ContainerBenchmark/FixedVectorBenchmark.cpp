#include "Container/FixedVector.h"
#include <vector>
#include <algorithm>
#include <functional>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include "CppUTest/TestHarness.h"

using Container::FixedVector;

TEST_GROUP(FixedVectorBenchmark) {
	static const std::size_t SIZE = 1000000;
	static const std::size_t SORT_SIZE = 100000;
	FixedVector<int, SIZE> x;
	std::vector<int> y;
	void setup()
	{
		static bool first = true;
		if (first) {
			std::srand((unsigned int) time(0));
			first = false;
		}
		x.clear();
		y.clear();
		y.reserve(SIZE);
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

TEST(FixedVectorBenchmark, push_back_pop_back)
{
	unsigned long t;
	t = get_msec();
	for (std::size_t i = 0; i < SIZE; ++i) {
		x.push_back(i);
	}
	std::printf("FixedVector::push_back, %ld, %ld ms\n", x.size(), get_msec() - t);

	t = get_msec();
	for (std::size_t i = 0; i < SIZE; ++i) {
		y.push_back(i);
	}
	std::printf("std::vector::push_back, %ld, %ld ms\n", y.size(), get_msec() - t);

	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(x[i], y[i]);
	}

	t = get_msec();
	while (!x.empty()) {
		x.pop_back();
	}
	std::printf("FixedVector::pop_back, %ld, %ld ms\n", x.size(), get_msec() - t);

	t = get_msec();
	while (!y.empty()) {
		y.pop_back();
	}
	std::printf("std::vector::pop_back, %ld, %ld ms\n", y.size(), get_msec() - t);

}

TEST(FixedVectorBenchmark, sort)
{
	for (std::size_t i = 0; i < SORT_SIZE; ++i) {
		const int tmp = std::rand();
		x.push_back(tmp);
		y.push_back(tmp);
	}

	unsigned long t;
	t = get_msec();
	std::sort(x.begin(), x.end());
	std::printf("sort FixedVector, %ld, %ld ms\n", x.size(), get_msec() - t);

	t = get_msec();
	std::sort(y.begin(), y.end());
	std::printf("sort std::vector, %ld, %ld ms\n", y.size(), get_msec() - t);

	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(x[i], y[i]);
	}

	t = get_msec();
	std::sort(x.begin(), x.end());
	std::printf("sort 2 FixedVector, %ld, %ld ms\n", x.size(), get_msec() - t);

	t = get_msec();
	std::sort(y.begin(), y.end());
	std::printf("sort 2 std::vector, %ld, %ld ms\n", y.size(), get_msec() - t);

	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(x[i], y[i]);
	}

	t = get_msec();
	std::sort(x.begin(), x.end(), std::greater<int>());
	std::printf("sort greater FixedVector, %ld, %ld ms\n", x.size(), get_msec() - t);

	t = get_msec();
	std::sort(y.begin(), y.end(), std::greater<int>());
	std::printf("sort greater std::vector, %ld, %ld ms\n", y.size(), get_msec() - t);

	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(x[i], y[i]);
	}
}

