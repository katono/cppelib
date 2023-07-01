#include "Container/PreallocatedDeque.h"
#include "Container/Array.h"
#include "Container/FixedVector.h"
#include "Container/FixedDeque.h"
#ifndef CPPELIB_NO_STD_CONTAINER
#include <deque>
#include <list>
#endif
#ifndef CPPELIB_NO_STD_ALGORITHM
#include <algorithm>
#include <functional>
#endif
#include <cstdlib>
#include "CppUTest/TestHarness.h"

namespace PreallocatedDequeTest {

using Container::PreallocatedDeque;
using Container::Array;
using Container::FixedVector;
using Container::FixedDeque;


TEST_GROUP(PreallocatedDequeTest) {
	static const std::size_t SIZE = 10;
	static const std::size_t ALLOC_SIZE = 1024;
	void* alloc_buf;
	void setup()
	{
		alloc_buf = std::calloc(ALLOC_SIZE, 1);
	}
	void teardown()
	{
		std::free(alloc_buf);
	}

	SimpleString StringFrom(PreallocatedDeque<int>::iterator value)
	{
		return StringFromFormat("%p", (void *)&*value);
	}
	SimpleString StringFrom(PreallocatedDeque<int>::const_iterator value)
	{
		return StringFromFormat("%p", (void *)&*value);
	}
};

TEST(PreallocatedDequeTest, init_only_one_time)
{
	PreallocatedDeque<int> x;
	x.init(alloc_buf, ALLOC_SIZE);
	x.resize(1);
	LONGS_EQUAL(1, x.size());
	LONGS_EQUAL(0, x[0]);

	int b[100] = {1};
	x.init(b, sizeof b); // do nothing
	LONGS_EQUAL(1, x.size());
	LONGS_EQUAL(0, x[0]);
}

TEST(PreallocatedDequeTest, size_0)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	LONGS_EQUAL(0, x.size());
}

TEST(PreallocatedDequeTest, size)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(5);
	LONGS_EQUAL(5, x.size());
}

TEST(PreallocatedDequeTest, max_size)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	LONGS_EQUAL(ALLOC_SIZE / sizeof(int) - 1, x.max_size());
}

TEST(PreallocatedDequeTest, available_size)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(5);
	LONGS_EQUAL(x.max_size() - 5, x.available_size());
}

TEST(PreallocatedDequeTest, empty_true)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	CHECK(x.empty());
}

TEST(PreallocatedDequeTest, empty_false)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(1);
	CHECK_FALSE(x.empty());
}

TEST(PreallocatedDequeTest, full_true)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	CHECK(x.full());
}

TEST(PreallocatedDequeTest, full_false)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size() - 1);
	CHECK_FALSE(x.full());
}

TEST(PreallocatedDequeTest, clear)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	x.clear();
	CHECK(x.empty());
}

TEST(PreallocatedDequeTest, operator_bracket_read)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	LONGS_EQUAL(0, x[0]);
	LONGS_EQUAL(5, x[5]);
	LONGS_EQUAL(9, x[9]);
}

TEST(PreallocatedDequeTest, operator_bracket_read_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> z(alloc_buf, ALLOC_SIZE);
	z.assign(a.begin(), a.end());
	const PreallocatedDeque<int>& x = z;
	LONGS_EQUAL(0, x[0]);
	LONGS_EQUAL(5, x[5]);
	LONGS_EQUAL(9, x[9]);
}

TEST(PreallocatedDequeTest, operator_bracket_write)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(10);
	x[0] = 100;
	LONGS_EQUAL(100, x[0]);

	x[9] = 10;
	LONGS_EQUAL(10, x[9]);
}

TEST(PreallocatedDequeTest, at_read)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	LONGS_EQUAL(0, x.at(0));
	LONGS_EQUAL(5, x.at(5));
	LONGS_EQUAL(9, x.at(9));
}

TEST(PreallocatedDequeTest, at_read_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> z(alloc_buf, ALLOC_SIZE);
	z.assign(a.begin(), a.end());
	const PreallocatedDeque<int>& x = z;
	LONGS_EQUAL(0, x.at(0));
	LONGS_EQUAL(5, x.at(5));
	LONGS_EQUAL(9, x.at(9));
}

TEST(PreallocatedDequeTest, at_write)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(10);
	x.at(0) = 100;
	LONGS_EQUAL(100, x.at(0));

	x.at(9) = 10;
	LONGS_EQUAL(10, x.at(9));
}

TEST(PreallocatedDequeTest, at_exception)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	try {
		x.at(0);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("PreallocatedDeque::at", e.what());
		return;
	}
	FAIL("failed");
}

TEST(PreallocatedDequeTest, at_exception_const)
{
	const PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	try {
		x.at(0);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("PreallocatedDeque::at", e.what());
		return;
	}
	FAIL("failed");
}

TEST(PreallocatedDequeTest, begin_end)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	std::size_t i = 0;
	for (PreallocatedDeque<int>::iterator it = x.begin(); it != x.end(); ++it, ++i) {
		LONGS_EQUAL(a[i], *it);
	}
}

TEST(PreallocatedDequeTest, begin_end_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> z(alloc_buf, ALLOC_SIZE);
	z.assign(a.begin(), a.end());
	const PreallocatedDeque<int>& x = z;
	std::size_t i = 0;
	for (PreallocatedDeque<int>::const_iterator it = x.begin(); it != x.end(); ++it, ++i) {
		LONGS_EQUAL(a[i], *it);
	}
}

TEST(PreallocatedDequeTest, front_read)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(1);
	LONGS_EQUAL(0, x.front());
}

TEST(PreallocatedDequeTest, front_read_const)
{
	PreallocatedDeque<int> z(alloc_buf, ALLOC_SIZE);
	z.resize(1);
	const PreallocatedDeque<int>& x = z;
	LONGS_EQUAL(0, x.front());
}

TEST(PreallocatedDequeTest, front_write)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(1);
	x.front() = 100;
	LONGS_EQUAL(100, x.front());
}

TEST(PreallocatedDequeTest, back_read)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	LONGS_EQUAL(9, x.back());
}

TEST(PreallocatedDequeTest, back_read_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> z(alloc_buf, ALLOC_SIZE);
	z.assign(a.begin(), a.end());
	const PreallocatedDeque<int>& x = z;
	LONGS_EQUAL(9, x.back());
}

TEST(PreallocatedDequeTest, back_write)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	x.back() = 100;
	LONGS_EQUAL(100, x.back());
}

TEST(PreallocatedDequeTest, operator_assign)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	int buf[ALLOC_SIZE];
	PreallocatedDeque<int> b(buf, sizeof buf);
	b.assign(a.begin(), a.end());
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x = b;
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(b[i], x[i]);
	}
}

TEST(PreallocatedDequeTest, operator_assign_self)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	x = x;
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(a[i], x[i]);
	}
}

TEST(PreallocatedDequeTest, resize_shorten)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(5);
	x.resize(3);
	LONGS_EQUAL(3, x.size());
}

TEST(PreallocatedDequeTest, resize_make_longer)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(5);
	x.resize(8, 100);
	LONGS_EQUAL(8, x.size());
	std::size_t i;
	for (i = 0; i < 5; ++i) {
		LONGS_EQUAL(0, x[i]);
	}
	for (; i < x.size(); ++i) {
		LONGS_EQUAL(100, x[i]);
	}
}

TEST(PreallocatedDequeTest, resize_exception)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	try {
		x.resize(x.max_size() + 1);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("PreallocatedDeque::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(PreallocatedDequeTest, push_back)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.push_back(100);
	LONGS_EQUAL(1, x.size());
	LONGS_EQUAL(100, x.back());

	x.push_back(101);
	LONGS_EQUAL(2, x.size());
	LONGS_EQUAL(101, x.back());

	x.push_back(102);
	LONGS_EQUAL(3, x.size());
	LONGS_EQUAL(102, x.back());
}

TEST(PreallocatedDequeTest, push_back_pop_front)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	for (std::size_t i = 0; i < x.max_size() * 100; ++i) {
		x.push_back(int(i));
		LONGS_EQUAL(i, x.front());
		x.pop_front();
		LONGS_EQUAL(0, x.size());
	}
}

TEST(PreallocatedDequeTest, push_back_exception)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size() - 1);
	x.push_back(100);
	LONGS_EQUAL(x.max_size(), x.size());

	try {
		x.push_back(101);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("PreallocatedDeque::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(PreallocatedDequeTest, pop_back)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(3);
	x.pop_back();
	LONGS_EQUAL(2, x.size());
	x.pop_back();
	LONGS_EQUAL(1, x.size());
	x.pop_back();
	LONGS_EQUAL(0, x.size());
}

TEST(PreallocatedDequeTest, push_front)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.push_front(100);
	LONGS_EQUAL(1, x.size());
	LONGS_EQUAL(100, x.front());

	x.push_front(101);
	LONGS_EQUAL(2, x.size());
	LONGS_EQUAL(101, x.front());

	x.push_front(102);
	LONGS_EQUAL(3, x.size());
	LONGS_EQUAL(102, x.front());
}

TEST(PreallocatedDequeTest, push_front_pop_back)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	for (std::size_t i = 0; i < x.max_size() * 100; ++i) {
		x.push_front(int(i));
		LONGS_EQUAL(i, x.back());
		x.pop_back();
		LONGS_EQUAL(0, x.size());
	}
}

TEST(PreallocatedDequeTest, push_front_exception)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size() - 1);
	x.push_front(100);
	LONGS_EQUAL(x.max_size(), x.size());

	try {
		x.push_front(101);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("PreallocatedDeque::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(PreallocatedDequeTest, pop_front)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(3);
	x.pop_front();
	LONGS_EQUAL(2, x.size());
	x.pop_front();
	LONGS_EQUAL(1, x.size());
	x.pop_front();
	LONGS_EQUAL(0, x.size());
}

TEST(PreallocatedDequeTest, assign_n)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(x.max_size(), 100);
	LONGS_EQUAL(x.max_size(), x.size());
	for (PreallocatedDeque<int>::iterator it = x.begin(); it != x.end(); ++it) {
		LONGS_EQUAL(100, *it);
	}
}

TEST(PreallocatedDequeTest, assign_n_exception)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	try {
		x.assign(x.max_size() + 1, 100);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("PreallocatedDeque::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(PreallocatedDequeTest, assign_range)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(PreallocatedDequeTest, assign_range_c_array)
{
	const int a[SIZE] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(&a[0], &a[SIZE]);
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(PreallocatedDequeTest, assign_range_FixedVector_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedVector<int, SIZE> b(a.begin(), a.end());
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(PreallocatedDequeTest, assign_range_FixedDeque_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedDeque<int, SIZE> b(a.begin(), a.end());
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(PreallocatedDequeTest, assign_range_PreallocatedDeque_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	int buf[ALLOC_SIZE];
	PreallocatedDeque<int> b(buf, sizeof buf);
	b.assign(a.begin(), a.end());
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

#ifndef CPPELIB_NO_STD_CONTAINER
TEST(PreallocatedDequeTest, assign_range_deque_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const std::deque<int> b(a.begin(), a.end());
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(PreallocatedDequeTest, assign_range_list_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const std::list<int> b(a.begin(), a.end());
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}
#endif

TEST(PreallocatedDequeTest, assign_range_exception)
{
	Array<int, ALLOC_SIZE / sizeof(int)> a = {0};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	try {
		x.assign(a.begin(), a.end());
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("PreallocatedDeque::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(PreallocatedDequeTest, insert)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	PreallocatedDeque<int>::iterator it;
	it = x.insert(x.end(), 1);
	CHECK_EQUAL(x.begin(), it);
	LONGS_EQUAL(1, x.size());
	LONGS_EQUAL(1, x[0]);

	it = x.insert(x.begin(), 2);
	CHECK_EQUAL(x.begin(), it);
	LONGS_EQUAL(2, x.size());
	LONGS_EQUAL(2, x[0]);
	LONGS_EQUAL(1, x[1]);

	it = x.insert(x.begin() + 1, 3);
	CHECK_EQUAL(x.begin() + 1, it);
	LONGS_EQUAL(3, x.size());
	LONGS_EQUAL(2, x[0]);
	LONGS_EQUAL(3, x[1]);
	LONGS_EQUAL(1, x[2]);

	it = x.insert(x.begin() + 2, 4);
	CHECK_EQUAL(x.begin() + 2, it);
	LONGS_EQUAL(4, x.size());
	LONGS_EQUAL(2, x[0]);
	LONGS_EQUAL(3, x[1]);
	LONGS_EQUAL(4, x[2]);
	LONGS_EQUAL(1, x[3]);

}

TEST(PreallocatedDequeTest, insert2)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.push_front(1);
	x.pop_back();
	LONGS_EQUAL(0, x.size());

	PreallocatedDeque<int>::iterator it;
	it = x.insert(x.end(), 1);
	CHECK_EQUAL(x.begin(), it);
	LONGS_EQUAL(1, x.size());
	LONGS_EQUAL(1, x[0]);

	it = x.insert(x.begin(), 2);
	CHECK_EQUAL(x.begin(), it);
	LONGS_EQUAL(2, x.size());
	LONGS_EQUAL(2, x[0]);
	LONGS_EQUAL(1, x[1]);

	it = x.insert(x.begin() + 1, 3);
	CHECK_EQUAL(x.begin() + 1, it);
	LONGS_EQUAL(3, x.size());
	LONGS_EQUAL(2, x[0]);
	LONGS_EQUAL(3, x[1]);
	LONGS_EQUAL(1, x[2]);

	it = x.insert(x.begin() + 2, 4);
	CHECK_EQUAL(x.begin() + 2, it);
	LONGS_EQUAL(4, x.size());
	LONGS_EQUAL(2, x[0]);
	LONGS_EQUAL(3, x[1]);
	LONGS_EQUAL(4, x[2]);
	LONGS_EQUAL(1, x[3]);

}

TEST(PreallocatedDequeTest, insert_exception)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	try {
		x.insert(x.begin(), 100);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("PreallocatedDeque::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(PreallocatedDequeTest, insert_n)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.insert(x.end(), 2U, 100);
	LONGS_EQUAL(2, x.size());
	LONGS_EQUAL(100, x[0]);
	LONGS_EQUAL(100, x[1]);

	x.insert(x.begin(), 3U, 200);
	LONGS_EQUAL(5, x.size());
	LONGS_EQUAL(200, x[0]);
	LONGS_EQUAL(200, x[1]);
	LONGS_EQUAL(200, x[2]);
	LONGS_EQUAL(100, x[3]);
	LONGS_EQUAL(100, x[4]);

	x.insert(x.begin() + 3, 1U, 300);
	LONGS_EQUAL(6, x.size());
	LONGS_EQUAL(200, x[0]);
	LONGS_EQUAL(200, x[1]);
	LONGS_EQUAL(200, x[2]);
	LONGS_EQUAL(300, x[3]);
	LONGS_EQUAL(100, x[4]);
	LONGS_EQUAL(100, x[5]);

}

TEST(PreallocatedDequeTest, insert_n2)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.insert(x.end(), 2U, 100);
	LONGS_EQUAL(2, x.size());
	LONGS_EQUAL(100, x[0]);
	LONGS_EQUAL(100, x[1]);

	x.insert(x.begin() + 1, 3U, 200);
	LONGS_EQUAL(5, x.size());
	LONGS_EQUAL(100, x[0]);
	LONGS_EQUAL(200, x[1]);
	LONGS_EQUAL(200, x[2]);
	LONGS_EQUAL(200, x[3]);
	LONGS_EQUAL(100, x[4]);

	x.insert(x.end(), 1U, 300);
	LONGS_EQUAL(6, x.size());
	LONGS_EQUAL(100, x[0]);
	LONGS_EQUAL(200, x[1]);
	LONGS_EQUAL(200, x[2]);
	LONGS_EQUAL(200, x[3]);
	LONGS_EQUAL(100, x[4]);
	LONGS_EQUAL(300, x[5]);

	x.insert(x.begin() + 3, 1U, 400);
	LONGS_EQUAL(7, x.size());
	LONGS_EQUAL(100, x[0]);
	LONGS_EQUAL(200, x[1]);
	LONGS_EQUAL(200, x[2]);
	LONGS_EQUAL(400, x[3]);
	LONGS_EQUAL(200, x[4]);
	LONGS_EQUAL(100, x[5]);
	LONGS_EQUAL(300, x[6]);

}

TEST(PreallocatedDequeTest, insert_n_dispatch)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.insert(x.end(), 2, 100);
	LONGS_EQUAL(2, x.size());
	LONGS_EQUAL(100, x[0]);
	LONGS_EQUAL(100, x[1]);

	x.insert(x.begin(), 3, 200);
	LONGS_EQUAL(5, x.size());
	LONGS_EQUAL(200, x[0]);
	LONGS_EQUAL(200, x[1]);
	LONGS_EQUAL(200, x[2]);
	LONGS_EQUAL(100, x[3]);
	LONGS_EQUAL(100, x[4]);

	x.insert(x.begin() + 3, 1, 300);
	LONGS_EQUAL(6, x.size());
	LONGS_EQUAL(200, x[0]);
	LONGS_EQUAL(200, x[1]);
	LONGS_EQUAL(200, x[2]);
	LONGS_EQUAL(300, x[3]);
	LONGS_EQUAL(100, x[4]);
	LONGS_EQUAL(100, x[5]);

}

TEST(PreallocatedDequeTest, insert_n_dispatch2)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.push_front(1);
	x.pop_back();
	LONGS_EQUAL(0, x.size());

	x.insert(x.end(), 2, 100);
	LONGS_EQUAL(2, x.size());
	LONGS_EQUAL(100, x[0]);
	LONGS_EQUAL(100, x[1]);

	x.insert(x.begin(), 3, 200);
	LONGS_EQUAL(5, x.size());
	LONGS_EQUAL(200, x[0]);
	LONGS_EQUAL(200, x[1]);
	LONGS_EQUAL(200, x[2]);
	LONGS_EQUAL(100, x[3]);
	LONGS_EQUAL(100, x[4]);

	x.insert(x.begin() + 3, 1, 300);
	LONGS_EQUAL(6, x.size());
	LONGS_EQUAL(200, x[0]);
	LONGS_EQUAL(200, x[1]);
	LONGS_EQUAL(200, x[2]);
	LONGS_EQUAL(300, x[3]);
	LONGS_EQUAL(100, x[4]);
	LONGS_EQUAL(100, x[5]);

}

TEST(PreallocatedDequeTest, insert_n_exception)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	try {
		x.insert(x.begin(), x.max_size() + 1, 100);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("PreallocatedDeque::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(PreallocatedDequeTest, insert_range)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.insert(x.end(), a.begin(), a.begin() + 3);
	LONGS_EQUAL(3, x.size());
	LONGS_EQUAL(0, x[0]);
	LONGS_EQUAL(1, x[1]);
	LONGS_EQUAL(2, x[2]);

	x.insert(x.begin(), a.begin() + 3, a.begin() + 5);
	LONGS_EQUAL(5, x.size());
	LONGS_EQUAL(3, x[0]);
	LONGS_EQUAL(4, x[1]);
	LONGS_EQUAL(0, x[2]);
	LONGS_EQUAL(1, x[3]);
	LONGS_EQUAL(2, x[4]);

	x.insert(x.begin() + 3, a.end() - 1, a.end());
	LONGS_EQUAL(6, x.size());
	LONGS_EQUAL(3, x[0]);
	LONGS_EQUAL(4, x[1]);
	LONGS_EQUAL(0, x[2]);
	LONGS_EQUAL(9, x[3]);
	LONGS_EQUAL(1, x[4]);
	LONGS_EQUAL(2, x[5]);

	x.insert(x.end(), a.begin(), a.begin() + 1);
	LONGS_EQUAL(7, x.size());
	LONGS_EQUAL(3, x[0]);
	LONGS_EQUAL(4, x[1]);
	LONGS_EQUAL(0, x[2]);
	LONGS_EQUAL(9, x[3]);
	LONGS_EQUAL(1, x[4]);
	LONGS_EQUAL(2, x[5]);
	LONGS_EQUAL(0, x[6]);

	x.insert(x.begin() + 2, a.begin() + 5, a.begin() + 6);
	LONGS_EQUAL(8, x.size());
	LONGS_EQUAL(3, x[0]);
	LONGS_EQUAL(4, x[1]);
	LONGS_EQUAL(5, x[2]);
	LONGS_EQUAL(0, x[3]);
	LONGS_EQUAL(9, x[4]);
	LONGS_EQUAL(1, x[5]);
	LONGS_EQUAL(2, x[6]);
	LONGS_EQUAL(0, x[7]);
}

TEST(PreallocatedDequeTest, insert_range2)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.push_front(1);
	x.pop_back();
	LONGS_EQUAL(0, x.size());

	x.insert(x.end(), a.begin(), a.begin() + 3);
	LONGS_EQUAL(3, x.size());
	LONGS_EQUAL(0, x[0]);
	LONGS_EQUAL(1, x[1]);
	LONGS_EQUAL(2, x[2]);

	x.insert(x.begin(), a.begin() + 3, a.begin() + 5);
	LONGS_EQUAL(5, x.size());
	LONGS_EQUAL(3, x[0]);
	LONGS_EQUAL(4, x[1]);
	LONGS_EQUAL(0, x[2]);
	LONGS_EQUAL(1, x[3]);
	LONGS_EQUAL(2, x[4]);

	x.insert(x.begin() + 3, a.end() - 1, a.end());
	LONGS_EQUAL(6, x.size());
	LONGS_EQUAL(3, x[0]);
	LONGS_EQUAL(4, x[1]);
	LONGS_EQUAL(0, x[2]);
	LONGS_EQUAL(9, x[3]);
	LONGS_EQUAL(1, x[4]);
	LONGS_EQUAL(2, x[5]);

	x.insert(x.end(), a.begin(), a.begin() + 1);
	LONGS_EQUAL(7, x.size());
	LONGS_EQUAL(3, x[0]);
	LONGS_EQUAL(4, x[1]);
	LONGS_EQUAL(0, x[2]);
	LONGS_EQUAL(9, x[3]);
	LONGS_EQUAL(1, x[4]);
	LONGS_EQUAL(2, x[5]);
	LONGS_EQUAL(0, x[6]);

	x.insert(x.begin() + 2, a.begin() + 5, a.begin() + 6);
	LONGS_EQUAL(8, x.size());
	LONGS_EQUAL(3, x[0]);
	LONGS_EQUAL(4, x[1]);
	LONGS_EQUAL(5, x[2]);
	LONGS_EQUAL(0, x[3]);
	LONGS_EQUAL(9, x[4]);
	LONGS_EQUAL(1, x[5]);
	LONGS_EQUAL(2, x[6]);
	LONGS_EQUAL(0, x[7]);
}

TEST(PreallocatedDequeTest, insert_range_c_array)
{
	const int a[SIZE] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.insert(x.end(), &a[0], &a[SIZE]);
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(PreallocatedDequeTest, insert_range_FixedVector_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedVector<int, SIZE> b(a.begin(), a.end());
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.insert(x.end(), b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(PreallocatedDequeTest, insert_range_FixedDeque_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedDeque<int, SIZE> b(a.begin(), a.end());
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.insert(x.end(), b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(PreallocatedDequeTest, insert_range_PreallocatedDeque_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	int buf[ALLOC_SIZE];
	PreallocatedDeque<int> b(buf, sizeof buf);
	b.assign(a.begin(), a.end());
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.insert(x.end(), b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

#ifndef CPPELIB_NO_STD_CONTAINER
TEST(PreallocatedDequeTest, insert_range_deque_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const std::deque<int> b(a.begin(), a.end());
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.insert(x.end(), b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(PreallocatedDequeTest, insert_range_list_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const std::list<int> b(a.begin(), a.end());
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.insert(x.end(), b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}
#endif

TEST(PreallocatedDequeTest, insert_range_exception)
{
	Array<int, ALLOC_SIZE / sizeof(int)> a = {0};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	try {
		x.insert(x.end(), a.begin(), a.end());
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("PreallocatedDeque::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(PreallocatedDequeTest, erase)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedDeque<int>::iterator it;
	it = x.erase(x.begin());
	LONGS_EQUAL(SIZE - 1, x.size());
	LONGS_EQUAL(1, x[0]);
	LONGS_EQUAL(2, x[1]);
	CHECK_EQUAL(x.begin(), it);

	it = x.erase(x.end() - 1);
	LONGS_EQUAL(SIZE - 2, x.size());
	LONGS_EQUAL(8, x.back());
	CHECK_EQUAL(x.end(), it);

	it = x.erase(x.begin() + 1);
	LONGS_EQUAL(SIZE - 3, x.size());
	LONGS_EQUAL(1, x[0]);
	LONGS_EQUAL(3, x[1]);
	LONGS_EQUAL(4, x[2]);
	CHECK_EQUAL(x.begin() + 1, it);

	it = x.erase(x.end() - 2);
	LONGS_EQUAL(SIZE - 4, x.size());
	LONGS_EQUAL(5, x[3]);
	LONGS_EQUAL(6, x[4]);
	LONGS_EQUAL(8, x[5]);
	CHECK_EQUAL(x.end() - 1, it);
}

TEST(PreallocatedDequeTest, erase_range)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedDeque<int>::iterator it;
	it = x.erase(x.begin(), x.begin() + 2);
	LONGS_EQUAL(SIZE - 2, x.size());
	LONGS_EQUAL(2, x[0]);
	LONGS_EQUAL(3, x[1]);
	CHECK_EQUAL(x.begin(), it);

	it = x.erase(x.end() - 3, x.end());
	LONGS_EQUAL(SIZE - 5, x.size());
	LONGS_EQUAL(6, x.back());
	CHECK_EQUAL(x.end(), it);

	it = x.erase(x.begin() + 1, x.begin() + 3);
	LONGS_EQUAL(SIZE - 7, x.size());
	LONGS_EQUAL(2, x[0]);
	LONGS_EQUAL(5, x[1]);
	CHECK_EQUAL(x.begin() + 1, it);
}

TEST(PreallocatedDequeTest, erase_range2)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.push_front(1);
	x.pop_back();
	LONGS_EQUAL(0, x.size());
	x.assign(a.begin(), a.end());

	PreallocatedDeque<int>::iterator it;
	it = x.erase(x.begin(), x.begin() + 2);
	LONGS_EQUAL(SIZE - 2, x.size());
	LONGS_EQUAL(2, x[0]);
	LONGS_EQUAL(3, x[1]);
	CHECK_EQUAL(x.begin(), it);

	it = x.erase(x.end() - 3, x.end());
	LONGS_EQUAL(SIZE - 5, x.size());
	LONGS_EQUAL(6, x.back());
	CHECK_EQUAL(x.end(), it);

	it = x.erase(x.begin() + 1, x.begin() + 3);
	LONGS_EQUAL(SIZE - 7, x.size());
	LONGS_EQUAL(2, x[0]);
	LONGS_EQUAL(5, x[1]);
	CHECK_EQUAL(x.begin() + 1, it);
}

TEST(PreallocatedDequeTest, erase_range_all)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedDeque<int>::iterator it;
	it = x.erase(x.begin(), x.end());
	LONGS_EQUAL(0, x.size());
	CHECK_EQUAL(x.end(), it);
}

TEST(PreallocatedDequeTest, operator_equal_true)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());

	int buf[ALLOC_SIZE / sizeof(int)];
	PreallocatedDeque<int> y(buf, sizeof buf);
	y.assign(a.begin(), a.end());

	LONGS_EQUAL(x.max_size(), y.max_size());
	CHECK_TRUE(x == y);
}

TEST(PreallocatedDequeTest, operator_equal_true_after_pop_back)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());

	int buf[ALLOC_SIZE / sizeof(int)];
	PreallocatedDeque<int> y(buf, sizeof buf);
	y.assign(a.begin(), a.end());

	x.back() = 100;
	y.back() = 101;
	x.pop_back();
	y.pop_back();
	CHECK_TRUE(x == y);
}

TEST(PreallocatedDequeTest, operator_equal_false)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const Array<int, SIZE> b = {0, 1, 2, 3, 4, 5, 6, 7, 8, 10};

	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());

	int buf[ALLOC_SIZE / sizeof(int)];
	PreallocatedDeque<int> y(buf, sizeof buf);
	y.assign(b.begin(), b.end());

	CHECK_FALSE(x == y);
}

TEST(PreallocatedDequeTest, operator_equal_false_diff_size)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());

	int buf[ALLOC_SIZE / sizeof(int)];
	PreallocatedDeque<int> y(buf, sizeof buf);
	y.assign(a.begin(), a.end() - 1);

	CHECK_FALSE(x == y);
}

TEST(PreallocatedDequeTest, operator_equal_false_diff_max_size)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());

	int buf[ALLOC_SIZE / sizeof(int) - 1];
	PreallocatedDeque<int> y(buf, sizeof buf);
	y.assign(a.begin(), a.end());

	CHECK_FALSE(x == y);
}

TEST(PreallocatedDequeTest, operator_notequal_true)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const Array<int, SIZE> b = {0, 1, 2, 3, 4, 5, 6, 7, 8, 10};

	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());

	int buf[ALLOC_SIZE / sizeof(int)];
	PreallocatedDeque<int> y(buf, sizeof buf);
	y.assign(b.begin(), b.end());

	CHECK_TRUE(x != y);
}

TEST(PreallocatedDequeTest, operator_notequal_false)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());

	int buf[ALLOC_SIZE / sizeof(int)];
	PreallocatedDeque<int> y(buf, sizeof buf);
	y.assign(a.begin(), a.end());

	CHECK_FALSE(x != y);
}

TEST(PreallocatedDequeTest, iterator_copy_ctor)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedDeque<int>::iterator it = x.begin();
	PreallocatedDeque<int>::iterator it2(it);
	CHECK_EQUAL(it, it2);
	PreallocatedDeque<int>::iterator it3;
	it3 = it;
	CHECK_EQUAL(it, it3);

	const PreallocatedDeque<int>& y = x;
	PreallocatedDeque<int>::const_iterator cit = y.begin();
	PreallocatedDeque<int>::const_iterator cit2(it);
	CHECK_EQUAL(cit, cit2);
	PreallocatedDeque<int>::const_iterator cit3;
	cit3 = it;
	CHECK_EQUAL(cit, cit3);
}

TEST(PreallocatedDequeTest, iterator_operator_plusequal)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedDeque<int>::iterator it = x.begin();
	PreallocatedDeque<int>::iterator i = it += 5;
	LONGS_EQUAL(5, *it);
	CHECK_EQUAL(x.begin() + 5, it);
	CHECK_EQUAL(it, i);
}

TEST(PreallocatedDequeTest, iterator_operator_plusequal_0)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedDeque<int>::iterator it = x.begin();
	it += 0;
	LONGS_EQUAL(0, *it);
	CHECK_EQUAL(x.begin(), it);
}

TEST(PreallocatedDequeTest, iterator_operator_plusequal_max)
{
	Array<int, ALLOC_SIZE / sizeof(int) - 1> a = {0};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedDeque<int>::iterator it = x.begin();
	it += x.max_size();
	CHECK_EQUAL(x.end(), it);
}

TEST(PreallocatedDequeTest, iterator_operator_plusequal_minus)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedDeque<int>::iterator it = x.end();
	PreallocatedDeque<int>::iterator i = it += -1;
	LONGS_EQUAL(9, *it);
	CHECK_EQUAL(x.begin() + 9, it);
	CHECK_EQUAL(it, i);
}

TEST(PreallocatedDequeTest, iterator_operator_plusequal_minus_max)
{
	Array<int, ALLOC_SIZE / sizeof(int) - 1> a = {0};
	for (std::size_t i = 0; i < a.size(); ++i) {
		a[i] = int(i);
	}
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedDeque<int>::iterator it = x.end();
	it += -static_cast<std::ptrdiff_t>(x.max_size());
	LONGS_EQUAL(0, *it);
	CHECK_EQUAL(x.begin(), it);
}

TEST(PreallocatedDequeTest, iterator_operator_plus)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedDeque<int>::iterator it = x.begin() + 5;
	LONGS_EQUAL(5, *it);
}

TEST(PreallocatedDequeTest, iterator_operator_plus_nonmember)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedDeque<int>::iterator it = 5 + x.begin();
	LONGS_EQUAL(5, *it);
	LONGS_EQUAL(1, *(-4 + it));
}

TEST(PreallocatedDequeTest, iterator_operator_incremant)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedDeque<int>::iterator it = x.begin();
	PreallocatedDeque<int>::iterator i = ++it;
	LONGS_EQUAL(1, *it);
	LONGS_EQUAL(1, *i);

	PreallocatedDeque<int>::iterator j = it++;
	LONGS_EQUAL(2, *it);
	LONGS_EQUAL(1, *j);
}

TEST(PreallocatedDequeTest, iterator_operator_minusqual)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedDeque<int>::iterator it = x.end();
	PreallocatedDeque<int>::iterator i = it -= 5;
	LONGS_EQUAL(5, *it);
	CHECK_EQUAL(x.end() - 5, it);
	CHECK_EQUAL(it, i);
}

TEST(PreallocatedDequeTest, iterator_operator_minusequal_0)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedDeque<int>::iterator it = x.begin();
	it -= 0;
	LONGS_EQUAL(0, *it);
	CHECK_EQUAL(x.begin(), it);
}

TEST(PreallocatedDequeTest, iterator_operator_minusqual_max)
{
	Array<int, ALLOC_SIZE / sizeof(int) - 1> a = {0};
	for (std::size_t i = 0; i < a.size(); ++i) {
		a[i] = int(i);
	}
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedDeque<int>::iterator it = x.end();
	it -= x.max_size();
	LONGS_EQUAL(0, *it);
	CHECK_EQUAL(x.begin(), it);
}

TEST(PreallocatedDequeTest, iterator_operator_minusequal_minus)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedDeque<int>::iterator it = x.begin();
	PreallocatedDeque<int>::iterator i = it -= -1;
	LONGS_EQUAL(1, *it);
	CHECK_EQUAL(x.begin() + 1, it);
	CHECK_EQUAL(it, i);
}

TEST(PreallocatedDequeTest, iterator_operator_minusequal_minus_max)
{
	Array<int, ALLOC_SIZE / sizeof(int) - 1> a = {0};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedDeque<int>::iterator it = x.begin();
	it -= -static_cast<std::ptrdiff_t>(x.max_size());
	CHECK_EQUAL(x.end(), it);
}

TEST(PreallocatedDequeTest, iterator_operator_minus)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedDeque<int>::iterator it = x.end() - 5;
	LONGS_EQUAL(5, *it);
}

TEST(PreallocatedDequeTest, iterator_operator_decremant)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedDeque<int>::iterator it = x.end();
	PreallocatedDeque<int>::iterator i = --it;
	LONGS_EQUAL(9, *it);
	LONGS_EQUAL(9, *i);

	PreallocatedDeque<int>::iterator j = it--;
	LONGS_EQUAL(8, *it);
	LONGS_EQUAL(9, *j);
}

struct A {
	int a;
	const char* b;
};

TEST(PreallocatedDequeTest, iterator_operator_arrow)
{
	A a = {1, "foo"};
	PreallocatedDeque<A> x(alloc_buf, ALLOC_SIZE);
	x.resize(1, a);
	PreallocatedDeque<A>::iterator it = x.begin();
	LONGS_EQUAL(1, it->a);
	STRCMP_EQUAL("foo", it->b);

	it->a = 10;
	LONGS_EQUAL(10, it->a);

	PreallocatedDeque<A>::const_iterator cit(it);
	LONGS_EQUAL(10, cit->a);
	STRCMP_EQUAL("foo", cit->b);
}

TEST(PreallocatedDequeTest, iterator_operator_asterisk)
{
	A a = {1, "foo"};
	PreallocatedDeque<A> x(alloc_buf, ALLOC_SIZE);
	x.resize(1, a);
	PreallocatedDeque<A>::iterator it = x.begin();
	LONGS_EQUAL(1, (*it).a);
	STRCMP_EQUAL("foo", (*it).b);

	(*it).a = 10;
	LONGS_EQUAL(10, (*it).a);

	PreallocatedDeque<A>::const_iterator cit(it);
	LONGS_EQUAL(10, (*cit).a);
	STRCMP_EQUAL("foo", (*cit).b);
}

TEST(PreallocatedDequeTest, iterator_operator_bracket_read)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedDeque<int>::iterator it = x.begin();
	LONGS_EQUAL(0, it[0]);
	LONGS_EQUAL(5, it[5]);
	LONGS_EQUAL(9, it[9]);
}

TEST(PreallocatedDequeTest, iterator_operator_bracket_write)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(10);
	PreallocatedDeque<int>::iterator it = x.begin();
	it[0] = 100;
	LONGS_EQUAL(100, it[0]);

	it[9] = 10;
	LONGS_EQUAL(10, it[9]);
}

TEST(PreallocatedDequeTest, iterator_operator_equal_true)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	PreallocatedDeque<int>::iterator it = x.begin();
	PreallocatedDeque<int>::iterator it2 = x.begin();
	PreallocatedDeque<int>::const_iterator cit(it);
	CHECK_TRUE(it == it2);
	CHECK_TRUE(cit == it);
	// CHECK_TRUE(it == cit); // compile error

	it = x.begin() + 5;
	it2 = x.begin() + 5;
	CHECK_TRUE(it == it2);
	cit = it;
	CHECK_TRUE(cit == it);

	it = x.end();
	it2 = x.end();
	CHECK_TRUE(it == it2);
	cit = it;
	CHECK_TRUE(cit == it);

}

TEST(PreallocatedDequeTest, iterator_operator_equal_false)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	PreallocatedDeque<int>::iterator it = x.begin();
	PreallocatedDeque<int>::iterator it2 = x.begin() + 1;
	CHECK_FALSE(it == it2);

	int buf[ALLOC_SIZE / sizeof(int)];
	PreallocatedDeque<int> y(buf, sizeof buf);
	y.resize(y.max_size());
	PreallocatedDeque<int>::iterator it3 = y.begin();
	CHECK_FALSE(it == it3);
}

TEST(PreallocatedDequeTest, iterator_operator_notequal_true)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	PreallocatedDeque<int>::iterator it = x.begin();
	PreallocatedDeque<int>::iterator it2 = x.begin() + 1;
	CHECK_TRUE(it != it2);

	it = x.begin();
	it2 = x.end();
	CHECK_TRUE(it != it2);
}

TEST(PreallocatedDequeTest, iterator_operator_notequal_false)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	PreallocatedDeque<int>::iterator it = x.begin();
	PreallocatedDeque<int>::iterator it2 = x.begin();
	CHECK_FALSE(it != it2);

	it = x.end();
	it2 = x.end();
	CHECK_FALSE(it != it2);
}

TEST(PreallocatedDequeTest, iterator_operator_less_true)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	PreallocatedDeque<int>::iterator it = x.begin();
	PreallocatedDeque<int>::iterator it2 = x.end();
	CHECK_TRUE(it < it2);
}

TEST(PreallocatedDequeTest, iterator_operator_less_true2)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.push_front(1);
	x.pop_back();
	LONGS_EQUAL(0, x.size());

	x.resize(x.max_size());
	PreallocatedDeque<int>::iterator it = x.begin();
	PreallocatedDeque<int>::iterator it2 = x.end();
	CHECK_TRUE(it < it2);
}

TEST(PreallocatedDequeTest, iterator_operator_less_false)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	PreallocatedDeque<int>::iterator it = x.begin();
	PreallocatedDeque<int>::iterator it2 = x.begin();
	CHECK_FALSE(it < it2);

	it = x.end();
	it2 = x.end();
	CHECK_FALSE(it < it2);

	it = x.end();
	it2 = x.begin();
	CHECK_FALSE(it < it2);
}

TEST(PreallocatedDequeTest, iterator_operator_less_false2)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.push_front(1);
	x.pop_back();
	LONGS_EQUAL(0, x.size());

	x.resize(SIZE);
	PreallocatedDeque<int>::iterator it = x.begin();
	PreallocatedDeque<int>::iterator it2 = x.begin();
	CHECK_FALSE(it < it2);

	it = x.end();
	it2 = x.end();
	CHECK_FALSE(it < it2);

	it = x.end();
	it2 = x.begin();
	CHECK_FALSE(it < it2);
}

TEST(PreallocatedDequeTest, iterator_operator_greater_true)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	PreallocatedDeque<int>::iterator it = x.end();
	PreallocatedDeque<int>::iterator it2 = x.begin();
	CHECK_TRUE(it > it2);
}

TEST(PreallocatedDequeTest, iterator_operator_greater_false)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	PreallocatedDeque<int>::iterator it = x.begin();
	PreallocatedDeque<int>::iterator it2 = x.begin();
	CHECK_FALSE(it > it2);

	it = x.end();
	it2 = x.end();
	CHECK_FALSE(it > it2);

	it = x.begin();
	it2 = x.end();
	CHECK_FALSE(it > it2);
}

TEST(PreallocatedDequeTest, iterator_operator_lessequal_true)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	PreallocatedDeque<int>::iterator it = x.begin();
	PreallocatedDeque<int>::iterator it2 = x.end();
	CHECK_TRUE(it <= it2);

	it = x.begin();
	it2 = x.begin();
	CHECK_TRUE(it <= it2);

	it = x.end();
	it2 = x.end();
	CHECK_TRUE(it <= it2);
}

TEST(PreallocatedDequeTest, iterator_operator_lessequal_false)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	PreallocatedDeque<int>::iterator it = x.end();
	PreallocatedDeque<int>::iterator it2 = x.begin();
	CHECK_FALSE(it <= it2);
}

TEST(PreallocatedDequeTest, iterator_operator_greaterequal_true)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	PreallocatedDeque<int>::iterator it = x.end();
	PreallocatedDeque<int>::iterator it2 = x.begin();
	CHECK_TRUE(it >= it2);

	it = x.begin();
	it2 = x.begin();
	CHECK_TRUE(it >= it2);

	it = x.end();
	it2 = x.end();
	CHECK_TRUE(it >= it2);
}

TEST(PreallocatedDequeTest, iterator_operator_greaterequal_false)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	PreallocatedDeque<int>::iterator it = x.begin();
	PreallocatedDeque<int>::iterator it2 = x.end();
	CHECK_FALSE(it >= it2);
}

TEST(PreallocatedDequeTest, iterator_difference)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	PreallocatedDeque<int>::iterator it = x.begin();
	PreallocatedDeque<int>::iterator it2 = x.end();
	LONGS_EQUAL(x.max_size(), it2 - it);
	LONGS_EQUAL(-static_cast<std::ptrdiff_t>(x.max_size()), it - it2);
}

TEST(PreallocatedDequeTest, iterator_difference2)
{
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.push_front(1);
	x.pop_back();
	LONGS_EQUAL(0, x.size());

	x.resize(x.max_size());
	PreallocatedDeque<int>::iterator it = x.begin();
	PreallocatedDeque<int>::iterator it2 = x.end();
	LONGS_EQUAL(x.max_size(), it2 - it);
	LONGS_EQUAL(-static_cast<std::ptrdiff_t>(x.max_size()), it - it2);
}

TEST(PreallocatedDequeTest, new_delete)
{
	PreallocatedDeque<int>* x = new PreallocatedDeque<int>(alloc_buf, ALLOC_SIZE);
	x->resize(SIZE);
	LONGS_EQUAL(SIZE, x->size());
	delete x;
}

#ifndef CPPELIB_NO_STD_ALGORITHM
TEST(PreallocatedDequeTest, algo_sort)
{
	const Array<int, SIZE> a = {1, 7, 0, 2, 5, 3, 9, 4, 6, 8};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	std::sort(x.begin(), x.end());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x.at(i));
	}

	std::sort(x.begin(), x.end(), std::greater<int>());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(SIZE - i - 1, x.at(i));
	}
}

TEST(PreallocatedDequeTest, algo_copy)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	std::copy(a.begin(), a.end(), std::back_inserter(x));
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x.at(i));
	}
}

TEST(PreallocatedDequeTest, algo_copy_exception)
{
	Array<int, ALLOC_SIZE / sizeof(int) - 1> a = {0};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(1);
	try {
		std::copy(a.begin(), a.end(), std::back_inserter(x));
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("PreallocatedDeque::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}
#endif

#ifndef CPPELIB_NO_STD_ITERATOR
TEST(PreallocatedDequeTest, rbegin_rend)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	std::size_t i = SIZE - 1;
	for (PreallocatedDeque<int>::reverse_iterator it = x.rbegin(); it != x.rend(); ++it, --i) {
		LONGS_EQUAL(a[i], *it);
	}
}

TEST(PreallocatedDequeTest, rbegin_rend_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedDeque<int> z(alloc_buf, ALLOC_SIZE);
	z.assign(a.begin(), a.end());
	const PreallocatedDeque<int>& x = z;
	std::size_t i = SIZE - 1;
	for (PreallocatedDeque<int>::const_reverse_iterator it = x.rbegin(); it != x.rend(); ++it, --i) {
		LONGS_EQUAL(a[i], *it);
	}
}
#endif


class DElem {
	int count;
public:
	volatile unsigned int data;
	static const unsigned int EXCEPTION_DATA = 0;

	DElem() : count(1), data(100) {}

	class Exception {};

	DElem(const DElem& x) : count(x.count), data(x.data)
	{
		if (x.data == EXCEPTION_DATA) {
			throw Exception();
		}
	}
	DElem& operator=(const DElem&)
	{
		return *this;
	}
	~DElem()
	{
		data = 0xDEADBEEF;
		--count;
		LONGS_EQUAL(0, count);
	}

	template <typename Iterator>
	static void checkElemsDestroyed(Iterator it, std::size_t num, std::size_t maxUsed)
	{
		for (std::size_t i = 0; i < num; ++i, ++it) {
			if (i < maxUsed) {
				LONGS_EQUAL_TEXT(0xDEADBEEF, it->data,
						StringFromFormat("%ld", i).asCharString());
			} else {
				CHECK_TRUE_TEXT(0xDEADBEEF != it->data,
						StringFromFormat("%ld", i).asCharString());
			}
		}
	}
};


TEST(PreallocatedDequeTest, default_ctor_DElem)
{
	PreallocatedDeque<DElem> x(alloc_buf, ALLOC_SIZE);
	DElem::checkElemsDestroyed(x.begin(), x.max_size(), 0);
}

TEST(PreallocatedDequeTest, ctor_DElem)
{
	DElem a;

	PreallocatedDeque<DElem> x(alloc_buf, ALLOC_SIZE);
	x.resize(2, a);
	LONGS_EQUAL(2, x.size());

	PreallocatedDeque<DElem>::iterator it = x.begin();
	x.clear();
	DElem::checkElemsDestroyed(it, x.max_size(), 2);

	a.data = DElem::EXCEPTION_DATA;
	try {
		int buf[ALLOC_SIZE];
		PreallocatedDeque<DElem> xx(buf, sizeof buf);
		xx.resize(1, a);
	}
	catch (const DElem::Exception&) {
		return;
	}
	FAIL("failed");

}

TEST(PreallocatedDequeTest, operator_assign_DElem)
{
	DElem a;

	int xbuf[SIZE] = {0};
	PreallocatedDeque<DElem> x(xbuf, sizeof xbuf);
	x.resize(2, a);

	int ybuf[SIZE] = {0};
	PreallocatedDeque<DElem> y(ybuf, sizeof ybuf);
	y.resize(1, a);

	y = x;
	LONGS_EQUAL(2, y.size());

	int zbuf[SIZE] = {0};
	PreallocatedDeque<DElem> z(zbuf, sizeof zbuf);
	z.resize(3, a);

	z = x;
	LONGS_EQUAL(2, z.size());

	PreallocatedDeque<DElem>::iterator it = z.begin();
	z.clear();
	DElem::checkElemsDestroyed(it, z.max_size(), 3);

}

TEST(PreallocatedDequeTest, push_back_pop_back_DElem)
{
	DElem a;

	PreallocatedDeque<DElem> x(alloc_buf, ALLOC_SIZE);
	x.push_back(a);
	x.push_back(a);
	x.pop_back();

	PreallocatedDeque<DElem>::iterator it = x.begin();
	x.clear();
	DElem::checkElemsDestroyed(it, x.max_size(), 2);

	int ybuf[ALLOC_SIZE] = {0};
	PreallocatedDeque<DElem> y(ybuf, sizeof ybuf);
	y.push_back(a);
	a.data = DElem::EXCEPTION_DATA;
	try {
		y.push_back(a);
	}
	catch (const DElem::Exception&) {
		LONGS_EQUAL(1, y.size());
		PreallocatedDeque<DElem>::iterator yit = y.begin();
		y.clear();
		DElem::checkElemsDestroyed(yit, y.max_size(), 1);
		return;
	}
	FAIL("failed");
}

TEST(PreallocatedDequeTest, insert_n_DElem)
{
	DElem a;

	PreallocatedDeque<DElem> x(alloc_buf, ALLOC_SIZE);
	x.insert(x.end(), 4, a);
	x.insert(x.begin() + 2, 1, a);
	x.insert(x.end(), 1, a);
	x.insert(x.end() - 2, 1, a);
	x.insert(x.end() - 1, 1, a);
	x.insert(x.begin() + 1, 1, a);
	LONGS_EQUAL(9, x.size());

	a.data = DElem::EXCEPTION_DATA;
	try {
		x.insert(x.end(), 1, a);
	}
	catch (const DElem::Exception&) {
		LONGS_EQUAL(9, x.size());
		PreallocatedDeque<DElem>::iterator it = x.begin();
		x.clear();
		DElem::checkElemsDestroyed(it, x.max_size(), 9);
		return;
	}
	FAIL("failed");
}

TEST(PreallocatedDequeTest, insert_range_DElem)
{
	DElem a;

	PreallocatedDeque<DElem> x(alloc_buf, ALLOC_SIZE);
	x.resize(4, a);

	int ybuf[ALLOC_SIZE] = {0};
	PreallocatedDeque<DElem> y(ybuf, sizeof ybuf);
	y.insert(y.end(), x.begin(), x.end());
	y.insert(y.begin() + 2, x.begin(), x.begin() + 1);
	y.insert(y.end(), x.begin(), x.begin() + 1);
	y.insert(y.end() - 2, x.begin(), x.begin() + 1);
	y.insert(y.end() - 1, x.begin(), x.begin() + 1);
	y.insert(y.begin() + 1, x.begin(), x.begin() + 1);
	LONGS_EQUAL(9, y.size());

	x[0].data = DElem::EXCEPTION_DATA;
	try {
		y.insert(y.end(), x.begin(), x.begin() + 1);
	}
	catch (const DElem::Exception&) {
		LONGS_EQUAL(9, y.size());
		PreallocatedDeque<DElem>::iterator it = y.begin();
		y.clear();
		DElem::checkElemsDestroyed(it, y.max_size(), 9);
		return;
	}
	FAIL("failed");
}

TEST(PreallocatedDequeTest, erase_range_DElem)
{
	DElem a;

	PreallocatedDeque<DElem> x(alloc_buf, ALLOC_SIZE);
	x.resize(5, a);

	x.erase(x.begin() + 1, x.begin() + 2);

	x.erase(x.end() - 2, x.end() - 1);

	x.erase(x.begin(), x.end());

}

#if (__cplusplus >= 201103L) || defined(_WIN32)
#include <memory>

TEST(PreallocatedDequeTest, shared_ptr_int)
{
	PreallocatedDeque<std::shared_ptr<int>> x(alloc_buf, ALLOC_SIZE);
	x.push_back(std::make_shared<int>(1));
	LONGS_EQUAL(1, *x[0]);
	*x[0] = 2;
	LONGS_EQUAL(2, *x[0]);
}

TEST(PreallocatedDequeTest, shared_ptr_DElem)
{
	DElem a;

	{
		PreallocatedDeque<std::shared_ptr<DElem>> x(alloc_buf, ALLOC_SIZE);

		x.push_back(std::make_shared<DElem>(a));
		x.push_back(std::make_shared<DElem>(a));

		x.pop_back();

	}

}
#endif

} // namespace PreallocatedDequeTest
