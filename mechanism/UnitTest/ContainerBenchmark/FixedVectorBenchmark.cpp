#include "CppUTest/TestHarness.h"
#define DESIGN_BY_CONTRACT
#include "Container/FixedVector.h"
#include <vector>
#include <algorithm>
#include <functional>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

using namespace Container;

TEST_GROUP(FixedVectorBenchmark) {
	static const size_t SIZE = 1000000;
	static const size_t SORT_SIZE = 100000;
	FixedVector<int, SIZE> x;
	std::vector<int> y;
	void setup()
	{
		static bool first = true;
		if (first) {
			srand((unsigned int) time(0));
			first = false;
		}
		x.clear();
		y.clear();
		y.reserve(SIZE);
	}
	void teardown()
	{
		printf("\n\n");
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
	for (size_t i = 0; i < SIZE; ++i) {
		x.push_back(i);
	}
	printf("FixedVector::push_back, %ld, %ld ms\n", x.size(), get_msec() - t);

	t = get_msec();
	for (size_t i = 0; i < SIZE; ++i) {
		y.push_back(i);
	}
	printf("std::vector::push_back, %ld, %ld ms\n", y.size(), get_msec() - t);

	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(x[i], y[i]);
	}

	t = get_msec();
	while (!x.empty()) {
		x.pop_back();
	}
	printf("FixedVector::pop_back, %ld, %ld ms\n", x.size(), get_msec() - t);

	t = get_msec();
	while (!y.empty()) {
		y.pop_back();
	}
	printf("std::vector::pop_back, %ld, %ld ms\n", y.size(), get_msec() - t);

}

TEST(FixedVectorBenchmark, sort)
{
	for (size_t i = 0; i < SORT_SIZE; ++i) {
		const int tmp = rand();
		x.push_back(tmp);
		y.push_back(tmp);
	}

	unsigned long t;
	t = get_msec();
	std::sort(x.begin(), x.end());
	printf("sort FixedVector, %ld, %ld ms\n", x.size(), get_msec() - t);

	t = get_msec();
	std::sort(y.begin(), y.end());
	printf("sort std::vector, %ld, %ld ms\n", y.size(), get_msec() - t);

	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(x[i], y[i]);
	}

	t = get_msec();
	std::sort(x.begin(), x.end());
	printf("sort 2 FixedVector, %ld, %ld ms\n", x.size(), get_msec() - t);

	t = get_msec();
	std::sort(y.begin(), y.end());
	printf("sort 2 std::vector, %ld, %ld ms\n", y.size(), get_msec() - t);

	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(x[i], y[i]);
	}

	t = get_msec();
	std::sort(x.begin(), x.end(), std::greater<int>());
	printf("sort greater FixedVector, %ld, %ld ms\n", x.size(), get_msec() - t);

	t = get_msec();
	std::sort(y.begin(), y.end(), std::greater<int>());
	printf("sort greater std::vector, %ld, %ld ms\n", y.size(), get_msec() - t);

	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(x[i], y[i]);
	}
}

