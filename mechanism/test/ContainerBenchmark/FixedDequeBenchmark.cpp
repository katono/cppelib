#define DESIGN_BY_CONTRACT
#include "Container/FixedDeque.h"
#include <deque>
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

using Container::FixedDeque;

TEST_GROUP(FixedDequeBenchmark) {
	static const std::size_t SIZE = 1000000;
	static const std::size_t SORT_SIZE = 100000;
	FixedDeque<int, SIZE> x;
	std::deque<int> y;
	void setup()
	{
		static bool first = true;
		if (first) {
			std::srand((unsigned int) time(0));
			first = false;
		}
		x.clear();
		y.clear();
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

	SimpleString StringFrom(FixedDeque<int, SIZE>::iterator value)
	{
		return StringFromFormat("%p", (void *)&*value);
	}
};

TEST(FixedDequeBenchmark, push_back_pop_back)
{
	unsigned long t;
	t = get_msec();
	for (std::size_t i = 0; i < SIZE; ++i) {
		x.push_back(i);
	}
	std::printf("FixedDeque::push_back, %ld, %ld ms\n", x.size(), get_msec() - t);

	t = get_msec();
	for (std::size_t i = 0; i < SIZE; ++i) {
		y.push_back(i);
	}
	std::printf("std::deque::push_back, %ld, %ld ms\n", y.size(), get_msec() - t);

	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(x[i], y[i]);
	}

	t = get_msec();
	while (!x.empty()) {
		x.pop_back();
	}
	std::printf("FixedDeque::pop_back, %ld, %ld ms\n", x.size(), get_msec() - t);

	t = get_msec();
	while (!y.empty()) {
		y.pop_back();
	}
	std::printf("std::deque::pop_back, %ld, %ld ms\n", y.size(), get_msec() - t);

}

TEST(FixedDequeBenchmark, push_front_pop_front)
{
	unsigned long t;
	t = get_msec();
	for (std::size_t i = 0; i < SIZE; ++i) {
		x.push_front(i);
	}
	std::printf("FixedDeque::push_front, %ld, %ld ms\n", x.size(), get_msec() - t);

	t = get_msec();
	for (std::size_t i = 0; i < SIZE; ++i) {
		y.push_front(i);
	}
	std::printf("std::deque::push_front, %ld, %ld ms\n", y.size(), get_msec() - t);

	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(x[i], y[i]);
	}

	t = get_msec();
	while (!x.empty()) {
		x.pop_front();
	}
	std::printf("FixedDeque::pop_front, %ld, %ld ms\n", x.size(), get_msec() - t);

	t = get_msec();
	while (!y.empty()) {
		y.pop_front();
	}
	std::printf("std::deque::pop_front, %ld, %ld ms\n", y.size(), get_msec() - t);

}

TEST(FixedDequeBenchmark, sort)
{
	for (std::size_t i = 0; i < SORT_SIZE; ++i) {
		const int tmp = std::rand();
		x.push_back(tmp);
		y.push_back(tmp);
	}

	unsigned long t;
	t = get_msec();
	std::sort(x.begin(), x.end());
	std::printf("sort FixedDeque, %ld, %ld ms\n", x.size(), get_msec() - t);

	t = get_msec();
	std::sort(y.begin(), y.end());
	std::printf("sort std::deque, %ld, %ld ms\n", y.size(), get_msec() - t);

	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(x[i], y[i]);
	}

	t = get_msec();
	std::sort(x.begin(), x.end());
	std::printf("sort 2 FixedDeque, %ld, %ld ms\n", x.size(), get_msec() - t);

	t = get_msec();
	std::sort(y.begin(), y.end());
	std::printf("sort 2 std::deque, %ld, %ld ms\n", y.size(), get_msec() - t);

	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(x[i], y[i]);
	}

	t = get_msec();
	std::sort(x.begin(), x.end(), std::greater<int>());
	std::printf("sort greater FixedDeque, %ld, %ld ms\n", x.size(), get_msec() - t);

	t = get_msec();
	std::sort(y.begin(), y.end(), std::greater<int>());
	std::printf("sort greater std::deque, %ld, %ld ms\n", y.size(), get_msec() - t);

	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(x[i], y[i]);
	}
}

