#include "Container/FixedDeque.h"
#include "Container/Array.h"
#include "Container/FixedVector.h"
#ifndef NO_STD_CONTAINER
#include <deque>
#include <list>
#endif
#ifndef NO_STD_ALGORITHM
#include <algorithm>
#include <functional>
#endif
#include "CppUTest/TestHarness.h"

using Container::FixedDeque;
using Container::Array;
using Container::FixedVector;


TEST_GROUP(FixedDequeTest) {
	static const std::size_t SIZE = 10;
	void setup()
	{
	}
	void teardown()
	{
	}

	SimpleString StringFrom(FixedDeque<int, SIZE>::iterator value)
	{
		return StringFromFormat("%p", (void *)&*value);
	}
	SimpleString StringFrom(FixedDeque<int, SIZE>::const_iterator value)
	{
		return StringFromFormat("%p", (void *)&*value);
	}
};

TEST(FixedDequeTest, size_0)
{
	FixedDeque<int, SIZE> x;
	LONGS_EQUAL(0, x.size());
}

TEST(FixedDequeTest, size)
{
	FixedDeque<int, SIZE> x(5);
	LONGS_EQUAL(5, x.size());
}

TEST(FixedDequeTest, available_size)
{
	FixedDeque<int, SIZE> x(5);
	LONGS_EQUAL(SIZE - 5, x.available_size());
}

TEST(FixedDequeTest, max_size)
{
	FixedDeque<int, SIZE> x;
	LONGS_EQUAL(SIZE, x.max_size());
}

TEST(FixedDequeTest, empty_true)
{
	FixedDeque<int, SIZE> x;
	CHECK(x.empty());
}

TEST(FixedDequeTest, empty_false)
{
	FixedDeque<int, SIZE> x(1);
	CHECK_FALSE(x.empty());
}

TEST(FixedDequeTest, full_true)
{
	FixedDeque<int, SIZE> x(SIZE);
	CHECK(x.full());
}

TEST(FixedDequeTest, full_false)
{
	FixedDeque<int, SIZE> x(SIZE - 2);
	CHECK_FALSE(x.full());
}

TEST(FixedDequeTest, clear)
{
	FixedDeque<int, SIZE> x(SIZE);
	x.clear();
	CHECK(x.empty());
}

TEST(FixedDequeTest, operator_bracket_read)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	LONGS_EQUAL(0, x[0]);
	LONGS_EQUAL(5, x[5]);
	LONGS_EQUAL(9, x[9]);
}

TEST(FixedDequeTest, operator_bracket_read_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedDeque<int, SIZE + 1> x(a.begin(), a.end());
	LONGS_EQUAL(0, x[0]);
	LONGS_EQUAL(5, x[5]);
	LONGS_EQUAL(9, x[9]);
}

TEST(FixedDequeTest, operator_bracket_write)
{
	FixedDeque<int, SIZE> x(10);
	x[0] = 100;
	LONGS_EQUAL(100, x[0]);

	x[9] = 10;
	LONGS_EQUAL(10, x[9]);
}

TEST(FixedDequeTest, at_read)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	LONGS_EQUAL(0, x.at(0));
	LONGS_EQUAL(5, x.at(5));
	LONGS_EQUAL(9, x.at(9));
}

TEST(FixedDequeTest, at_read_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedDeque<int, SIZE> x(a.begin(), a.end());
	LONGS_EQUAL(0, x.at(0));
	LONGS_EQUAL(5, x.at(5));
	LONGS_EQUAL(9, x.at(9));
}

TEST(FixedDequeTest, at_write)
{
	FixedDeque<int, SIZE> x(10);
	x.at(0) = 100;
	LONGS_EQUAL(100, x.at(0));

	x.at(9) = 10;
	LONGS_EQUAL(10, x.at(9));
}

TEST(FixedDequeTest, at_exception)
{
	FixedDeque<int, SIZE> x;
	try {
		x.at(0);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("FixedDeque::at", e.what());
		return;
	}
	FAIL("failed");
}

TEST(FixedDequeTest, at_exception_const)
{
	const FixedDeque<int, SIZE> x;
	try {
		x.at(0);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("FixedDeque::at", e.what());
		return;
	}
	FAIL("failed");
}

TEST(FixedDequeTest, begin_end)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	std::size_t i = 0;
	for (FixedDeque<int, SIZE>::iterator it = x.begin(); it != x.end(); ++it, ++i) {
		LONGS_EQUAL(a[i], *it);
	}
}

TEST(FixedDequeTest, begin_end_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedDeque<int, SIZE> x(a.begin(), a.end());
	std::size_t i = 0;
	for (FixedDeque<int, SIZE>::const_iterator it = x.begin(); it != x.end(); ++it, ++i) {
		LONGS_EQUAL(a[i], *it);
	}
}

TEST(FixedDequeTest, front_read)
{
	FixedDeque<int, SIZE> x(1);
	LONGS_EQUAL(0, x.front());
}

TEST(FixedDequeTest, front_read_const)
{
	const FixedDeque<int, SIZE> x(1);
	LONGS_EQUAL(0, x.front());
}

TEST(FixedDequeTest, front_write)
{
	FixedDeque<int, SIZE> x(1);
	x.front() = 100;
	LONGS_EQUAL(100, x.front());
}

TEST(FixedDequeTest, back_read)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	LONGS_EQUAL(9, x.back());
}

TEST(FixedDequeTest, back_read_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedDeque<int, SIZE> x(a.begin(), a.end());
	LONGS_EQUAL(9, x.back());
}

TEST(FixedDequeTest, back_write)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	x.back() = 100;
	LONGS_EQUAL(100, x.back());
}

TEST(FixedDequeTest, ctor_n_data)
{
	FixedDeque<int, SIZE> x(SIZE, 100);
	LONGS_EQUAL(SIZE, x.size());
	for (FixedDeque<int, SIZE>::iterator it = x.begin(); it != x.end(); ++it) {
		LONGS_EQUAL(100, *it);
	}
}

TEST(FixedDequeTest, ctor_n_data_exception)
{
	try {
		FixedDeque<int, SIZE> x(SIZE + 1);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("FixedDeque::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(FixedDequeTest, ctor_range)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(FixedDequeTest, ctor_range_exception)
{
	const Array<int, SIZE + 1> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	try {
		FixedDeque<int, SIZE> x(a.begin(), a.end());
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("FixedDeque::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(FixedDequeTest, copy_ctor)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedDeque<int, SIZE> b(a.begin(), a.end());
	const FixedDeque<int, SIZE> x(b);
	LONGS_EQUAL(SIZE, b.size());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(a[i], b[i]);
		LONGS_EQUAL(b[i], x[i]);
	}
}

TEST(FixedDequeTest, operator_assign)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedDeque<int, SIZE> b(a.begin(), a.end());
	FixedDeque<int, SIZE> x;
	x = b;
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(b[i], x[i]);
	}
}

TEST(FixedDequeTest, operator_assign_self)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	x = x;
	LONGS_EQUAL(SIZE, x.size());
	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(a[i], x[i]);
	}
}

TEST(FixedDequeTest, resize_shorten)
{
	FixedDeque<int, SIZE> x(5);
	x.resize(3);
	LONGS_EQUAL(3, x.size());
}

TEST(FixedDequeTest, resize_make_longer)
{
	FixedDeque<int, SIZE> x(5);
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

TEST(FixedDequeTest, resize_exception)
{
	FixedDeque<int, SIZE> x;
	try {
		x.resize(SIZE + 1);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("FixedDeque::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(FixedDequeTest, push_back)
{
	FixedDeque<int, SIZE> x;
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

TEST(FixedDequeTest, push_back_pop_front)
{
	FixedDeque<int, SIZE> x;
	for (std::size_t i = 0; i < SIZE * 100; ++i) {
		x.push_back(i);
		LONGS_EQUAL(i, x.front());
		x.pop_front();
		LONGS_EQUAL(0, x.size());
	}
}

TEST(FixedDequeTest, push_back_exception)
{
	FixedDeque<int, SIZE> x(SIZE - 1);
	x.push_back(100);
	LONGS_EQUAL(SIZE, x.size());

	try {
		x.push_back(101);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("FixedDeque::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(FixedDequeTest, pop_back)
{
	FixedDeque<int, SIZE> x(3);
	x.pop_back();
	LONGS_EQUAL(2, x.size());
	x.pop_back();
	LONGS_EQUAL(1, x.size());
	x.pop_back();
	LONGS_EQUAL(0, x.size());
}

TEST(FixedDequeTest, push_front)
{
	FixedDeque<int, SIZE> x;
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

TEST(FixedDequeTest, push_front_pop_back)
{
	FixedDeque<int, SIZE> x;
	for (std::size_t i = 0; i < SIZE * 100; ++i) {
		x.push_front(i);
		LONGS_EQUAL(i, x.back());
		x.pop_back();
		LONGS_EQUAL(0, x.size());
	}
}

TEST(FixedDequeTest, push_front_exception)
{
	FixedDeque<int, SIZE> x(SIZE - 1);
	x.push_front(100);
	LONGS_EQUAL(SIZE, x.size());

	try {
		x.push_front(101);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("FixedDeque::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(FixedDequeTest, pop_front)
{
	FixedDeque<int, SIZE> x(3);
	x.pop_front();
	LONGS_EQUAL(2, x.size());
	x.pop_front();
	LONGS_EQUAL(1, x.size());
	x.pop_front();
	LONGS_EQUAL(0, x.size());
}

TEST(FixedDequeTest, assign_n)
{
	FixedDeque<int, SIZE> x;
	x.assign(SIZE, 100);
	LONGS_EQUAL(SIZE, x.size());
	for (FixedDeque<int, SIZE>::iterator it = x.begin(); it != x.end(); ++it) {
		LONGS_EQUAL(100, *it);
	}
}

TEST(FixedDequeTest, assign_n_exception)
{
	FixedDeque<int, SIZE> x;
	try {
		x.assign(SIZE + 1, 100);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("FixedDeque::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(FixedDequeTest, assign_range)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x;
	x.assign(a.begin(), a.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(FixedDequeTest, assign_range_c_array)
{
	const int a[SIZE] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x;
	x.assign(&a[0], &a[SIZE]);
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(FixedDequeTest, assign_range_FixedVector_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedVector<int, SIZE> b(a.begin(), a.end());
	FixedDeque<int, SIZE> x;
	x.assign(b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(FixedDequeTest, assign_range_FixedDeque_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedDeque<int, SIZE> b(a.begin(), a.end());
	FixedDeque<int, SIZE> x;
	x.assign(b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

#ifndef NO_STD_CONTAINER
TEST(FixedDequeTest, assign_range_deque_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const std::deque<int> b(a.begin(), a.end());
	FixedDeque<int, SIZE> x;
	x.assign(b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(FixedDequeTest, assign_range_list_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const std::list<int> b(a.begin(), a.end());
	FixedDeque<int, SIZE> x;
	x.assign(b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}
#endif

TEST(FixedDequeTest, assign_range_exception)
{
	const Array<int, SIZE + 1> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	FixedDeque<int, SIZE> x;
	try {
		x.assign(a.begin(), a.end());
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("FixedDeque::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(FixedDequeTest, insert)
{
	FixedDeque<int, SIZE> x;
	FixedDeque<int, SIZE>::iterator it;
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

TEST(FixedDequeTest, insert2)
{
	FixedDeque<int, SIZE> x;
	x.push_front(1);
	x.pop_back();
	LONGS_EQUAL(0, x.size());

	FixedDeque<int, SIZE>::iterator it;
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

TEST(FixedDequeTest, insert_exception)
{
	FixedDeque<int, SIZE> x(SIZE);
	try {
		x.insert(x.begin(), 100);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("FixedDeque::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(FixedDequeTest, insert_n)
{
	FixedDeque<int, SIZE> x;
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

TEST(FixedDequeTest, insert_n2)
{
	FixedDeque<int, SIZE> x;
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

TEST(FixedDequeTest, insert_n_dispatch)
{
	FixedDeque<int, SIZE> x;
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

TEST(FixedDequeTest, insert_n_dispatch2)
{
	FixedDeque<int, SIZE> x;
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

TEST(FixedDequeTest, insert_n_exception)
{
	FixedDeque<int, SIZE> x;
	try {
		x.insert(x.begin(), SIZE + 1, 100);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("FixedDeque::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(FixedDequeTest, insert_range)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x;
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

TEST(FixedDequeTest, insert_range2)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x;
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

TEST(FixedDequeTest, insert_range_c_array)
{
	const int a[SIZE] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x;
	x.insert(x.end(), &a[0], &a[SIZE]);
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(FixedDequeTest, insert_range_FixedVector_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedVector<int, SIZE> b(a.begin(), a.end());
	FixedDeque<int, SIZE> x;
	x.insert(x.end(), b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(FixedDequeTest, insert_range_FixedDeque_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedDeque<int, SIZE> b(a.begin(), a.end());
	FixedDeque<int, SIZE> x;
	x.insert(x.end(), b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

#ifndef NO_STD_CONTAINER
TEST(FixedDequeTest, insert_range_deque_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const std::deque<int> b(a.begin(), a.end());
	FixedDeque<int, SIZE> x;
	x.insert(x.end(), b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(FixedDequeTest, insert_range_list_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const std::list<int> b(a.begin(), a.end());
	FixedDeque<int, SIZE> x;
	x.insert(x.end(), b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}
#endif

TEST(FixedDequeTest, insert_range_exception)
{
	const Array<int, SIZE + 1> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	FixedDeque<int, SIZE> x;
	try {
		x.insert(x.end(), a.begin(), a.end());
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("FixedDeque::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(FixedDequeTest, erase)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	FixedDeque<int, SIZE>::iterator it;
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

TEST(FixedDequeTest, erase_range)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	FixedDeque<int, SIZE>::iterator it;
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

TEST(FixedDequeTest, erase_range2)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x;
	x.push_front(1);
	x.pop_back();
	LONGS_EQUAL(0, x.size());
	x.assign(a.begin(), a.end());

	FixedDeque<int, SIZE>::iterator it;
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

TEST(FixedDequeTest, erase_range_all)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	FixedDeque<int, SIZE>::iterator it;
	it = x.erase(x.begin(), x.end());
	LONGS_EQUAL(0, x.size());
	CHECK_EQUAL(x.end(), it);
}

TEST(FixedDequeTest, operator_equal_true)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	FixedDeque<int, SIZE> y(a.begin(), a.end());
	CHECK_TRUE(x == y);
}

TEST(FixedDequeTest, operator_equal_true_after_pop_back)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	FixedDeque<int, SIZE> y(a.begin(), a.end());
	x.back() = 100;
	y.back() = 101;
	x.pop_back();
	y.pop_back();
	CHECK_TRUE(x == y);
}

TEST(FixedDequeTest, operator_equal_false)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const Array<int, SIZE> b = {0, 1, 2, 3, 4, 5, 6, 7, 8, 10};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	FixedDeque<int, SIZE> y(b.begin(), b.end());
	CHECK_FALSE(x == y);
}

TEST(FixedDequeTest, operator_equal_false_diff_size)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	FixedDeque<int, SIZE> y(a.begin(), a.end() - 1);
	CHECK_FALSE(x == y);
}

TEST(FixedDequeTest, operator_notequal_true)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const Array<int, SIZE> b = {0, 1, 2, 3, 4, 5, 6, 7, 8, 10};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	FixedDeque<int, SIZE> y(b.begin(), b.end());
	CHECK_TRUE(x != y);
}

TEST(FixedDequeTest, operator_notequal_false)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	FixedDeque<int, SIZE> y(a.begin(), a.end());
	CHECK_FALSE(x != y);
}

TEST(FixedDequeTest, iterator_copy_ctor)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	FixedDeque<int, SIZE>::iterator it = x.begin();
	FixedDeque<int, SIZE>::iterator it2(it);
	CHECK_EQUAL(it, it2);
	FixedDeque<int, SIZE>::iterator it3;
	it3 = it;
	CHECK_EQUAL(it, it3);

	const FixedDeque<int, SIZE>& y = x;
	FixedDeque<int, SIZE>::const_iterator cit = y.begin();
	FixedDeque<int, SIZE>::const_iterator cit2(it);
	CHECK_EQUAL(cit, cit2);
	FixedDeque<int, SIZE>::const_iterator cit3;
	cit3 = it;
	CHECK_EQUAL(cit, cit3);
}

TEST(FixedDequeTest, iterator_operator_plusequal)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	FixedDeque<int, SIZE>::iterator it = x.begin();
	FixedDeque<int, SIZE>::iterator i = it += 5;
	LONGS_EQUAL(5, *it);
	CHECK_EQUAL(x.begin() + 5, it);
	CHECK_EQUAL(it, i);
}

TEST(FixedDequeTest, iterator_operator_plusequal_0)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	FixedDeque<int, SIZE>::iterator it = x.begin();
	it += 0;
	LONGS_EQUAL(0, *it);
	CHECK_EQUAL(x.begin(), it);
}

TEST(FixedDequeTest, iterator_operator_plusequal_max)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	FixedDeque<int, SIZE>::iterator it = x.begin();
	it += SIZE;
	CHECK_EQUAL(x.end(), it);
}

TEST(FixedDequeTest, iterator_operator_plusequal_minus)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	FixedDeque<int, SIZE>::iterator it = x.end();
	FixedDeque<int, SIZE>::iterator i = it += -1;
	LONGS_EQUAL(9, *it);
	CHECK_EQUAL(x.begin() + 9, it);
	CHECK_EQUAL(it, i);
}

TEST(FixedDequeTest, iterator_operator_plusequal_minus_max)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	FixedDeque<int, SIZE>::iterator it = x.end();
	it += -static_cast<std::ptrdiff_t>(SIZE);
	LONGS_EQUAL(0, *it);
	CHECK_EQUAL(x.begin(), it);
}

TEST(FixedDequeTest, iterator_operator_plus)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	FixedDeque<int, SIZE>::iterator it = x.begin() + 5;
	LONGS_EQUAL(5, *it);
}

TEST(FixedDequeTest, iterator_operator_plus_nonmember)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	FixedDeque<int, SIZE>::iterator it = 5 + x.begin();
	LONGS_EQUAL(5, *it);
	LONGS_EQUAL(1, *(-4 + it));
}

TEST(FixedDequeTest, iterator_operator_incremant)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	FixedDeque<int, SIZE>::iterator it = x.begin();
	FixedDeque<int, SIZE>::iterator i = ++it;
	LONGS_EQUAL(1, *it);
	LONGS_EQUAL(1, *i);

	FixedDeque<int, SIZE>::iterator j = it++;
	LONGS_EQUAL(2, *it);
	LONGS_EQUAL(1, *j);
}

TEST(FixedDequeTest, iterator_operator_minusqual)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	FixedDeque<int, SIZE>::iterator it = x.end();
	FixedDeque<int, SIZE>::iterator i = it -= 5;
	LONGS_EQUAL(5, *it);
	CHECK_EQUAL(x.end() - 5, it);
	CHECK_EQUAL(it, i);
}

TEST(FixedDequeTest, iterator_operator_minusequal_0)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	FixedDeque<int, SIZE>::iterator it = x.begin();
	it -= 0;
	LONGS_EQUAL(0, *it);
	CHECK_EQUAL(x.begin(), it);
}

TEST(FixedDequeTest, iterator_operator_minusqual_max)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	FixedDeque<int, SIZE>::iterator it = x.end();
	it -= SIZE;
	LONGS_EQUAL(0, *it);
	CHECK_EQUAL(x.begin(), it);
}

TEST(FixedDequeTest, iterator_operator_minusequal_minus)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	FixedDeque<int, SIZE>::iterator it = x.begin();
	FixedDeque<int, SIZE>::iterator i = it -= -1;
	LONGS_EQUAL(1, *it);
	CHECK_EQUAL(x.begin() + 1, it);
	CHECK_EQUAL(it, i);
}

TEST(FixedDequeTest, iterator_operator_minusequal_minus_max)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	FixedDeque<int, SIZE>::iterator it = x.begin();
	it -= -static_cast<std::ptrdiff_t>(SIZE);
	CHECK_EQUAL(x.end(), it);
}

TEST(FixedDequeTest, iterator_operator_minus)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	FixedDeque<int, SIZE>::iterator it = x.end() - 5;
	LONGS_EQUAL(5, *it);
}

TEST(FixedDequeTest, iterator_operator_decremant)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	FixedDeque<int, SIZE>::iterator it = x.end();
	FixedDeque<int, SIZE>::iterator i = --it;
	LONGS_EQUAL(9, *it);
	LONGS_EQUAL(9, *i);

	FixedDeque<int, SIZE>::iterator j = it--;
	LONGS_EQUAL(8, *it);
	LONGS_EQUAL(9, *j);
}

struct A {
	int a;
	const char* b;
};

TEST(FixedDequeTest, iterator_operator_arrow)
{
	A a = {1, "foo"};
	FixedDeque<A, SIZE> x(1, a);
	FixedDeque<A, SIZE>::iterator it = x.begin();
	LONGS_EQUAL(1, it->a);
	STRCMP_EQUAL("foo", it->b);

	it->a = 10;
	LONGS_EQUAL(10, it->a);

	FixedDeque<A, SIZE>::const_iterator cit(it);
	LONGS_EQUAL(10, cit->a);
	STRCMP_EQUAL("foo", cit->b);
}

TEST(FixedDequeTest, iterator_operator_asterisk)
{
	A a = {1, "foo"};
	FixedDeque<A, SIZE> x(1, a);
	FixedDeque<A, SIZE>::iterator it = x.begin();
	LONGS_EQUAL(1, (*it).a);
	STRCMP_EQUAL("foo", (*it).b);

	(*it).a = 10;
	LONGS_EQUAL(10, (*it).a);

	FixedDeque<A, SIZE>::const_iterator cit(it);
	LONGS_EQUAL(10, (*cit).a);
	STRCMP_EQUAL("foo", (*cit).b);
}

TEST(FixedDequeTest, iterator_operator_bracket_read)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	FixedDeque<int, SIZE>::iterator it = x.begin();
	LONGS_EQUAL(0, it[0]);
	LONGS_EQUAL(5, it[5]);
	LONGS_EQUAL(9, it[9]);
}

TEST(FixedDequeTest, iterator_operator_bracket_write)
{
	FixedDeque<int, SIZE> x(10);
	FixedDeque<int, SIZE>::iterator it = x.begin();
	it[0] = 100;
	LONGS_EQUAL(100, it[0]);

	it[9] = 10;
	LONGS_EQUAL(10, it[9]);
}

TEST(FixedDequeTest, iterator_operator_equal_true)
{
	FixedDeque<int, SIZE> x(SIZE);
	FixedDeque<int, SIZE>::iterator it = x.begin();
	FixedDeque<int, SIZE>::iterator it2 = x.begin();
	FixedDeque<int, SIZE>::const_iterator cit(it);
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

TEST(FixedDequeTest, iterator_operator_equal_false)
{
	FixedDeque<int, SIZE> x(SIZE);
	FixedDeque<int, SIZE>::iterator it = x.begin();
	FixedDeque<int, SIZE>::iterator it2 = x.begin() + 1;
	CHECK_FALSE(it == it2);

	FixedDeque<int, SIZE> y(SIZE);
	FixedDeque<int, SIZE>::iterator it3 = y.begin();
	CHECK_FALSE(it == it3);
}

TEST(FixedDequeTest, iterator_operator_notequal_true)
{
	FixedDeque<int, SIZE> x(SIZE);
	FixedDeque<int, SIZE>::iterator it = x.begin();
	FixedDeque<int, SIZE>::iterator it2 = x.begin() + 1;
	CHECK_TRUE(it != it2);

	it = x.begin();
	it2 = x.end();
	CHECK_TRUE(it != it2);
}

TEST(FixedDequeTest, iterator_operator_notequal_false)
{
	FixedDeque<int, SIZE> x(SIZE);
	FixedDeque<int, SIZE>::iterator it = x.begin();
	FixedDeque<int, SIZE>::iterator it2 = x.begin();
	CHECK_FALSE(it != it2);

	it = x.end();
	it2 = x.end();
	CHECK_FALSE(it != it2);
}

TEST(FixedDequeTest, iterator_operator_less_true)
{
	FixedDeque<int, SIZE> x(SIZE);
	FixedDeque<int, SIZE>::iterator it = x.begin();
	FixedDeque<int, SIZE>::iterator it2 = x.end();
	CHECK_TRUE(it < it2);
}

TEST(FixedDequeTest, iterator_operator_less_true2)
{
	FixedDeque<int, SIZE> x;
	x.push_front(1);
	x.pop_back();
	LONGS_EQUAL(0, x.size());

	x.resize(SIZE);
	FixedDeque<int, SIZE>::iterator it = x.begin();
	FixedDeque<int, SIZE>::iterator it2 = x.end();
	CHECK_TRUE(it < it2);
}

TEST(FixedDequeTest, iterator_operator_less_false)
{
	FixedDeque<int, SIZE> x(SIZE);
	FixedDeque<int, SIZE>::iterator it = x.begin();
	FixedDeque<int, SIZE>::iterator it2 = x.begin();
	CHECK_FALSE(it < it2);

	it = x.end();
	it2 = x.end();
	CHECK_FALSE(it < it2);

	it = x.end();
	it2 = x.begin();
	CHECK_FALSE(it < it2);
}

TEST(FixedDequeTest, iterator_operator_less_false2)
{
	FixedDeque<int, SIZE> x;
	x.push_front(1);
	x.pop_back();
	LONGS_EQUAL(0, x.size());

	x.resize(SIZE);
	FixedDeque<int, SIZE>::iterator it = x.begin();
	FixedDeque<int, SIZE>::iterator it2 = x.begin();
	CHECK_FALSE(it < it2);

	it = x.end();
	it2 = x.end();
	CHECK_FALSE(it < it2);

	it = x.end();
	it2 = x.begin();
	CHECK_FALSE(it < it2);
}

TEST(FixedDequeTest, iterator_operator_greater_true)
{
	FixedDeque<int, SIZE> x(SIZE);
	FixedDeque<int, SIZE>::iterator it = x.end();
	FixedDeque<int, SIZE>::iterator it2 = x.begin();
	CHECK_TRUE(it > it2);
}

TEST(FixedDequeTest, iterator_operator_greater_false)
{
	FixedDeque<int, SIZE> x(SIZE);
	FixedDeque<int, SIZE>::iterator it = x.begin();
	FixedDeque<int, SIZE>::iterator it2 = x.begin();
	CHECK_FALSE(it > it2);

	it = x.end();
	it2 = x.end();
	CHECK_FALSE(it > it2);

	it = x.begin();
	it2 = x.end();
	CHECK_FALSE(it > it2);
}

TEST(FixedDequeTest, iterator_operator_lessequal_true)
{
	FixedDeque<int, SIZE> x(SIZE);
	FixedDeque<int, SIZE>::iterator it = x.begin();
	FixedDeque<int, SIZE>::iterator it2 = x.end();
	CHECK_TRUE(it <= it2);

	it = x.begin();
	it2 = x.begin();
	CHECK_TRUE(it <= it2);

	it = x.end();
	it2 = x.end();
	CHECK_TRUE(it <= it2);
}

TEST(FixedDequeTest, iterator_operator_lessequal_false)
{
	FixedDeque<int, SIZE> x(SIZE);
	FixedDeque<int, SIZE>::iterator it = x.end();
	FixedDeque<int, SIZE>::iterator it2 = x.begin();
	CHECK_FALSE(it <= it2);
}

TEST(FixedDequeTest, iterator_operator_greaterequal_true)
{
	FixedDeque<int, SIZE> x(SIZE);
	FixedDeque<int, SIZE>::iterator it = x.end();
	FixedDeque<int, SIZE>::iterator it2 = x.begin();
	CHECK_TRUE(it >= it2);

	it = x.begin();
	it2 = x.begin();
	CHECK_TRUE(it >= it2);

	it = x.end();
	it2 = x.end();
	CHECK_TRUE(it >= it2);
}

TEST(FixedDequeTest, iterator_operator_greaterequal_false)
{
	FixedDeque<int, SIZE> x(SIZE);
	FixedDeque<int, SIZE>::iterator it = x.begin();
	FixedDeque<int, SIZE>::iterator it2 = x.end();
	CHECK_FALSE(it >= it2);
}

TEST(FixedDequeTest, iterator_difference)
{
	FixedDeque<int, SIZE> x(SIZE);
	FixedDeque<int, SIZE>::iterator it = x.begin();
	FixedDeque<int, SIZE>::iterator it2 = x.end();
	LONGS_EQUAL(SIZE, it2 - it);
	LONGS_EQUAL(-static_cast<std::ptrdiff_t>(SIZE), it - it2);
}

TEST(FixedDequeTest, iterator_difference2)
{
	FixedDeque<int, SIZE> x;
	x.push_front(1);
	x.pop_back();
	LONGS_EQUAL(0, x.size());

	x.resize(SIZE);
	FixedDeque<int, SIZE>::iterator it = x.begin();
	FixedDeque<int, SIZE>::iterator it2 = x.end();
	LONGS_EQUAL(SIZE, it2 - it);
	LONGS_EQUAL(-static_cast<std::ptrdiff_t>(SIZE), it - it2);
}

TEST(FixedDequeTest, new_delete)
{
	FixedDeque<int, SIZE>* x = new FixedDeque<int, SIZE>(SIZE);
	LONGS_EQUAL(SIZE, x->size());
	delete x;
}

#ifndef NO_STD_ALGORITHM
TEST(FixedDequeTest, algo_sort)
{
	const Array<int, SIZE> a = {1, 7, 0, 2, 5, 3, 9, 4, 6, 8};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	std::sort(x.begin(), x.end());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x.at(i));
	}

	std::sort(x.begin(), x.end(), std::greater<int>());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(SIZE - i - 1, x.at(i));
	}
}

TEST(FixedDequeTest, algo_copy)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x;
	std::copy(a.begin(), a.end(), std::back_inserter(x));
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x.at(i));
	}
}

TEST(FixedDequeTest, algo_copy_exception)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(1);
	try {
		std::copy(a.begin(), a.end(), std::back_inserter(x));
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("FixedDeque::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}
#endif

#ifndef NO_STD_ITERATOR
TEST(FixedDequeTest, rbegin_rend)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedDeque<int, SIZE> x(a.begin(), a.end());
	std::size_t i = SIZE - 1;
	for (FixedDeque<int, SIZE>::reverse_iterator it = x.rbegin(); it != x.rend(); ++it, --i) {
		LONGS_EQUAL(a[i], *it);
	}
}

TEST(FixedDequeTest, rbegin_rend_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedDeque<int, SIZE> x(a.begin(), a.end());
	std::size_t i = SIZE - 1;
	for (FixedDeque<int, SIZE>::const_reverse_iterator it = x.rbegin(); it != x.rend(); ++it, --i) {
		LONGS_EQUAL(a[i], *it);
	}
}
#endif


class RBElem {
	int count;
public:
	unsigned int data;
	static const unsigned int EXCEPTION_DATA = 0;

	RBElem() : count(1), data(100) {}

	class Exception {};

	RBElem(const RBElem& x) : count(x.count), data(x.data)
	{
		if (x.data == EXCEPTION_DATA) {
			throw Exception();
		}
	}
	RBElem& operator=(const RBElem&)
	{
		return *this;
	}
	~RBElem()
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
						StringFromFormat("%d", i).asCharString());
			} else {
				CHECK_TRUE_TEXT(0xDEADBEEF != it->data,
						StringFromFormat("%d", i).asCharString());
			}
		}
	}
};


TEST(FixedDequeTest, default_ctor_RBElem)
{
	FixedDeque<RBElem, SIZE> x;
	RBElem::checkElemsDestroyed(x.begin(), SIZE, 0);
}

TEST(FixedDequeTest, ctor_RBElem)
{
	RBElem a;

	FixedDeque<RBElem, SIZE> x(2, a);
	LONGS_EQUAL(2, x.size());

	FixedDeque<RBElem, SIZE> y(x.begin(), x.end());
	LONGS_EQUAL(2, y.size());

	FixedDeque<RBElem, SIZE> z(x);
	LONGS_EQUAL(2, z.size());

	FixedDeque<RBElem, SIZE>::iterator it = x.begin();
	x.clear();
	RBElem::checkElemsDestroyed(it, SIZE, 2);

	a.data = RBElem::EXCEPTION_DATA;
	try {
		FixedDeque<RBElem, SIZE> xx(1, a);
	}
	catch (const RBElem::Exception&) {
		return;
	}
	FAIL("failed");

}

TEST(FixedDequeTest, operator_assign_RBElem)
{
	RBElem a;

	FixedDeque<RBElem, SIZE> x(2, a);

	FixedDeque<RBElem, SIZE> y(1, a);

	y = x;
	LONGS_EQUAL(2, y.size());

	FixedDeque<RBElem, SIZE> z(3, a);

	z = x;
	LONGS_EQUAL(2, z.size());

	FixedDeque<RBElem, SIZE>::iterator it = z.begin();
	z.clear();
	RBElem::checkElemsDestroyed(it, SIZE, 3);

}

TEST(FixedDequeTest, push_back_pop_back_RBElem)
{
	RBElem a;

	FixedDeque<RBElem, SIZE> x;
	x.push_back(a);
	x.push_back(a);
	x.pop_back();

	FixedDeque<RBElem, SIZE>::iterator it = x.begin();
	x.clear();
	RBElem::checkElemsDestroyed(it, SIZE, 2);

	FixedDeque<RBElem, SIZE> y;
	y.push_back(a);
	a.data = RBElem::EXCEPTION_DATA;
	try {
		y.push_back(a);
	}
	catch (const RBElem::Exception&) {
		LONGS_EQUAL(1, y.size());
		FixedDeque<RBElem, SIZE>::iterator yit = y.begin();
		y.clear();
		RBElem::checkElemsDestroyed(yit, SIZE, 1);
		return;
	}
	FAIL("failed");
}

TEST(FixedDequeTest, insert_n_RBElem)
{
	RBElem a;

	FixedDeque<RBElem, SIZE> x;
	x.insert(x.end(), 4, a);
	x.insert(x.begin() + 2, 1, a);
	x.insert(x.end(), 1, a);
	x.insert(x.end() - 2, 1, a);
	x.insert(x.end() - 1, 1, a);
	x.insert(x.begin() + 1, 1, a);
	LONGS_EQUAL(9, x.size());

	a.data = RBElem::EXCEPTION_DATA;
	try {
		x.insert(x.end(), 1, a);
	}
	catch (const RBElem::Exception&) {
		LONGS_EQUAL(9, x.size());
		FixedDeque<RBElem, SIZE>::iterator it = x.begin();
		x.clear();
		RBElem::checkElemsDestroyed(it, SIZE, 9);
		return;
	}
	FAIL("failed");
}

TEST(FixedDequeTest, insert_range_RBElem)
{
	RBElem a;

	FixedDeque<RBElem, SIZE> x(4, a);
	FixedDeque<RBElem, SIZE> y;
	y.insert(y.end(), x.begin(), x.end());
	y.insert(y.begin() + 2, x.begin(), x.begin() + 1);
	y.insert(y.end(), x.begin(), x.begin() + 1);
	y.insert(y.end() - 2, x.begin(), x.begin() + 1);
	y.insert(y.end() - 1, x.begin(), x.begin() + 1);
	y.insert(y.begin() + 1, x.begin(), x.begin() + 1);
	LONGS_EQUAL(9, y.size());

	x[0].data = RBElem::EXCEPTION_DATA;
	try {
		y.insert(y.end(), x.begin(), x.begin() + 1);
	}
	catch (const RBElem::Exception&) {
		LONGS_EQUAL(9, y.size());
		FixedDeque<RBElem, SIZE>::iterator it = y.begin();
		y.clear();
		RBElem::checkElemsDestroyed(it, SIZE, 9);
		return;
	}
	FAIL("failed");
}

TEST(FixedDequeTest, erase_range_RBElem)
{
	RBElem a;

	FixedDeque<RBElem, SIZE> x(5, a);

	x.erase(x.begin() + 1, x.begin() + 2);

	x.erase(x.end() - 2, x.end() - 1);

	x.erase(x.begin(), x.end());

}

#if (__cplusplus >= 201103L) || defined(_WIN32)
#include <memory>

TEST(FixedDequeTest, shared_ptr_int)
{
	FixedDeque<std::shared_ptr<int>, SIZE> x;
	x.push_back(std::make_shared<int>(1));
	LONGS_EQUAL(1, *x[0]);
	*x[0] = 2;
	LONGS_EQUAL(2, *x[0]);
}

TEST(FixedDequeTest, shared_ptr_RBElem)
{
	RBElem a;

	{
		FixedDeque<std::shared_ptr<RBElem>, SIZE> x;

		x.push_back(std::make_shared<RBElem>(a));
		x.push_back(std::make_shared<RBElem>(a));

		x.pop_back();

	}

}
#endif
