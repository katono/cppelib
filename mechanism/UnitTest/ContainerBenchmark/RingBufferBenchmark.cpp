#include "CppUTest/TestHarness.h"
#define DESIGN_BY_CONTRACT
#include "Container/RingBuffer.h"
#include <deque>
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

TEST_GROUP(RingBufferBenchmark) {
	static const size_t SIZE = 1000000;
	static const size_t SORT_SIZE = 100000;
	RingBuffer<int, SIZE> x;
	std::deque<int> y;
	void setup()
	{
		static bool first = true;
		if (first) {
			srand((unsigned int) time(0));
			first = false;
		}
		x.clear();
		y.clear();
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

	SimpleString StringFrom(RingBuffer<int, SIZE>::iterator value)
	{
		return StringFromFormat("%p", (void *)&*value);
	}
};

TEST(RingBufferBenchmark, push_back_pop_back)
{
	unsigned long t;
	t = get_msec();
	for (size_t i = 0; i < SIZE; ++i) {
		x.push_back(i);
	}
	printf("RingBuffer::push_back, %ld, %ld ms\n", x.size(), get_msec() - t);

	t = get_msec();
	for (size_t i = 0; i < SIZE; ++i) {
		y.push_back(i);
	}
	printf("std::deque::push_back, %ld, %ld ms\n", y.size(), get_msec() - t);

	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(x[i], y[i]);
	}

	t = get_msec();
	while (!x.empty()) {
		x.pop_back();
	}
	printf("RingBuffer::pop_back, %ld, %ld ms\n", x.size(), get_msec() - t);

	t = get_msec();
	while (!y.empty()) {
		y.pop_back();
	}
	printf("std::deque::pop_back, %ld, %ld ms\n", y.size(), get_msec() - t);

}

TEST(RingBufferBenchmark, push_front_pop_front)
{
	unsigned long t;
	t = get_msec();
	for (size_t i = 0; i < SIZE; ++i) {
		x.push_front(i);
	}
	printf("RingBuffer::push_front, %ld, %ld ms\n", x.size(), get_msec() - t);

	t = get_msec();
	for (size_t i = 0; i < SIZE; ++i) {
		y.push_front(i);
	}
	printf("std::deque::push_front, %ld, %ld ms\n", y.size(), get_msec() - t);

	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(x[i], y[i]);
	}

	t = get_msec();
	while (!x.empty()) {
		x.pop_front();
	}
	printf("RingBuffer::pop_front, %ld, %ld ms\n", x.size(), get_msec() - t);

	t = get_msec();
	while (!y.empty()) {
		y.pop_front();
	}
	printf("std::deque::pop_front, %ld, %ld ms\n", y.size(), get_msec() - t);

}

TEST(RingBufferBenchmark, sort)
{
	for (size_t i = 0; i < SORT_SIZE; ++i) {
		const int tmp = rand();
		x.push_back(tmp);
		y.push_back(tmp);
	}

	unsigned long t;
	t = get_msec();
	std::sort(x.begin(), x.end());
	printf("sort RingBuffer, %ld, %ld ms\n", x.size(), get_msec() - t);

	t = get_msec();
	std::sort(y.begin(), y.end());
	printf("sort std::deque, %ld, %ld ms\n", y.size(), get_msec() - t);

	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(x[i], y[i]);
	}

	t = get_msec();
	std::sort(x.begin(), x.end());
	printf("sort 2 RingBuffer, %ld, %ld ms\n", x.size(), get_msec() - t);

	t = get_msec();
	std::sort(y.begin(), y.end());
	printf("sort 2 std::deque, %ld, %ld ms\n", y.size(), get_msec() - t);

	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(x[i], y[i]);
	}

	t = get_msec();
	std::sort(x.begin(), x.end(), std::greater<int>());
	printf("sort greater RingBuffer, %ld, %ld ms\n", x.size(), get_msec() - t);

	t = get_msec();
	std::sort(y.begin(), y.end(), std::greater<int>());
	printf("sort greater std::deque, %ld, %ld ms\n", y.size(), get_msec() - t);

	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(x[i], y[i]);
	}
}

