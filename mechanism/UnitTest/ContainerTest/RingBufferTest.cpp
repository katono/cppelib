#include "Container/RingBuffer.h"
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
#include "CppUTestExt/MockSupport.h"

using Container::RingBuffer;
using Container::Array;
using Container::FixedVector;


TEST_GROUP(RingBufferTest) {
	static const std::size_t SIZE = 10;
	void setup()
	{
	}
	void teardown()
	{
		mock().checkExpectations();
		mock().clear();
	}

	SimpleString StringFrom(RingBuffer<int, SIZE>::iterator value)
	{
		return StringFromFormat("%p", (void *)&*value);
	}
	SimpleString StringFrom(RingBuffer<int, SIZE>::const_iterator value)
	{
		return StringFromFormat("%p", (void *)&*value);
	}
};

TEST(RingBufferTest, size_0)
{
	RingBuffer<int, SIZE> x;
	LONGS_EQUAL(0, x.size());
}

TEST(RingBufferTest, size)
{
	RingBuffer<int, SIZE> x(5);
	LONGS_EQUAL(5, x.size());
}

TEST(RingBufferTest, available_size)
{
	RingBuffer<int, SIZE> x(5);
	LONGS_EQUAL(SIZE - 5, x.available_size());
}

TEST(RingBufferTest, max_size)
{
	RingBuffer<int, SIZE> x;
	LONGS_EQUAL(SIZE, x.max_size());
}

TEST(RingBufferTest, empty_true)
{
	RingBuffer<int, SIZE> x;
	CHECK(x.empty());
}

TEST(RingBufferTest, empty_false)
{
	RingBuffer<int, SIZE> x(1);
	CHECK_FALSE(x.empty());
}

TEST(RingBufferTest, full_true)
{
	RingBuffer<int, SIZE> x(SIZE);
	CHECK(x.full());
}

TEST(RingBufferTest, full_false)
{
	RingBuffer<int, SIZE> x(SIZE - 2);
	CHECK_FALSE(x.full());
}

TEST(RingBufferTest, clear)
{
	RingBuffer<int, SIZE> x(SIZE);
	x.clear();
	CHECK(x.empty());
}

TEST(RingBufferTest, operator_bracket_read)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	LONGS_EQUAL(0, x[0]);
	LONGS_EQUAL(5, x[5]);
	LONGS_EQUAL(9, x[9]);
}

TEST(RingBufferTest, operator_bracket_read_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const RingBuffer<int, SIZE + 1> x(a.begin(), a.end());
	LONGS_EQUAL(0, x[0]);
	LONGS_EQUAL(5, x[5]);
	LONGS_EQUAL(9, x[9]);
}

TEST(RingBufferTest, operator_bracket_write)
{
	RingBuffer<int, SIZE> x(10);
	x[0] = 100;
	LONGS_EQUAL(100, x[0]);

	x[9] = 10;
	LONGS_EQUAL(10, x[9]);
}

TEST(RingBufferTest, at_read)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	LONGS_EQUAL(0, x.at(0));
	LONGS_EQUAL(5, x.at(5));
	LONGS_EQUAL(9, x.at(9));
}

TEST(RingBufferTest, at_read_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const RingBuffer<int, SIZE> x(a.begin(), a.end());
	LONGS_EQUAL(0, x.at(0));
	LONGS_EQUAL(5, x.at(5));
	LONGS_EQUAL(9, x.at(9));
}

TEST(RingBufferTest, at_write)
{
	RingBuffer<int, SIZE> x(10);
	x.at(0) = 100;
	LONGS_EQUAL(100, x.at(0));

	x.at(9) = 10;
	LONGS_EQUAL(10, x.at(9));
}

TEST(RingBufferTest, at_exception)
{
	RingBuffer<int, SIZE> x;
	try {
		x.at(0);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("RingBuffer::at", e.what());
		return;
	}
	FAIL("failed");
}

TEST(RingBufferTest, at_exception_const)
{
	const RingBuffer<int, SIZE> x;
	try {
		x.at(0);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("RingBuffer::at", e.what());
		return;
	}
	FAIL("failed");
}

TEST(RingBufferTest, begin_end)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	std::size_t i = 0;
	for (RingBuffer<int, SIZE>::iterator it = x.begin(); it != x.end(); ++it, ++i) {
		LONGS_EQUAL(a[i], *it);
	}
}

TEST(RingBufferTest, begin_end_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const RingBuffer<int, SIZE> x(a.begin(), a.end());
	std::size_t i = 0;
	for (RingBuffer<int, SIZE>::const_iterator it = x.begin(); it != x.end(); ++it, ++i) {
		LONGS_EQUAL(a[i], *it);
	}
}

TEST(RingBufferTest, front_read)
{
	RingBuffer<int, SIZE> x(1);
	LONGS_EQUAL(0, x.front());
}

TEST(RingBufferTest, front_read_const)
{
	const RingBuffer<int, SIZE> x(1);
	LONGS_EQUAL(0, x.front());
}

TEST(RingBufferTest, front_write)
{
	RingBuffer<int, SIZE> x(1);
	x.front() = 100;
	LONGS_EQUAL(100, x.front());
}

TEST(RingBufferTest, back_read)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	LONGS_EQUAL(9, x.back());
}

TEST(RingBufferTest, back_read_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const RingBuffer<int, SIZE> x(a.begin(), a.end());
	LONGS_EQUAL(9, x.back());
}

TEST(RingBufferTest, back_write)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	x.back() = 100;
	LONGS_EQUAL(100, x.back());
}

TEST(RingBufferTest, ctor_n_data)
{
	RingBuffer<int, SIZE> x(SIZE, 100);
	LONGS_EQUAL(SIZE, x.size());
	for (RingBuffer<int, SIZE>::iterator it = x.begin(); it != x.end(); ++it) {
		LONGS_EQUAL(100, *it);
	}
}

TEST(RingBufferTest, ctor_n_data_exception)
{
	try {
		RingBuffer<int, SIZE> x(SIZE + 1);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("RingBuffer::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(RingBufferTest, ctor_range)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(RingBufferTest, ctor_range_exception)
{
	const Array<int, SIZE + 1> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	try {
		RingBuffer<int, SIZE> x(a.begin(), a.end());
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("RingBuffer::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(RingBufferTest, copy_ctor)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const RingBuffer<int, SIZE> b(a.begin(), a.end());
	const RingBuffer<int, SIZE> x(b);
	LONGS_EQUAL(SIZE, b.size());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(a[i], b[i]);
		LONGS_EQUAL(b[i], x[i]);
	}
}

TEST(RingBufferTest, operator_assign)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const RingBuffer<int, SIZE> b(a.begin(), a.end());
	RingBuffer<int, SIZE> x;
	x = b;
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(b[i], x[i]);
	}
}

TEST(RingBufferTest, operator_assign_self)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	x = x;
	LONGS_EQUAL(SIZE, x.size());
	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(a[i], x[i]);
	}
}

TEST(RingBufferTest, resize_shorten)
{
	RingBuffer<int, SIZE> x(5);
	x.resize(3);
	LONGS_EQUAL(3, x.size());
}

TEST(RingBufferTest, resize_make_longer)
{
	RingBuffer<int, SIZE> x(5);
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

TEST(RingBufferTest, resize_exception)
{
	RingBuffer<int, SIZE> x;
	try {
		x.resize(SIZE + 1);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("RingBuffer::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(RingBufferTest, push_back)
{
	RingBuffer<int, SIZE> x;
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

TEST(RingBufferTest, push_back_pop_front)
{
	RingBuffer<int, SIZE> x;
	for (std::size_t i = 0; i < SIZE * 100; ++i) {
		x.push_back(i);
		LONGS_EQUAL(i, x.front());
		x.pop_front();
		LONGS_EQUAL(0, x.size());
	}
}

TEST(RingBufferTest, push_back_exception)
{
	RingBuffer<int, SIZE> x(SIZE - 1);
	x.push_back(100);
	LONGS_EQUAL(SIZE, x.size());

	try {
		x.push_back(101);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("RingBuffer::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(RingBufferTest, pop_back)
{
	RingBuffer<int, SIZE> x(3);
	x.pop_back();
	LONGS_EQUAL(2, x.size());
	x.pop_back();
	LONGS_EQUAL(1, x.size());
	x.pop_back();
	LONGS_EQUAL(0, x.size());
}

TEST(RingBufferTest, push_front)
{
	RingBuffer<int, SIZE> x;
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

TEST(RingBufferTest, push_front_pop_back)
{
	RingBuffer<int, SIZE> x;
	for (std::size_t i = 0; i < SIZE * 100; ++i) {
		x.push_front(i);
		LONGS_EQUAL(i, x.back());
		x.pop_back();
		LONGS_EQUAL(0, x.size());
	}
}

TEST(RingBufferTest, push_front_exception)
{
	RingBuffer<int, SIZE> x(SIZE - 1);
	x.push_front(100);
	LONGS_EQUAL(SIZE, x.size());

	try {
		x.push_front(101);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("RingBuffer::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(RingBufferTest, pop_front)
{
	RingBuffer<int, SIZE> x(3);
	x.pop_front();
	LONGS_EQUAL(2, x.size());
	x.pop_front();
	LONGS_EQUAL(1, x.size());
	x.pop_front();
	LONGS_EQUAL(0, x.size());
}

TEST(RingBufferTest, assign_n)
{
	RingBuffer<int, SIZE> x;
	x.assign(SIZE, 100);
	LONGS_EQUAL(SIZE, x.size());
	for (RingBuffer<int, SIZE>::iterator it = x.begin(); it != x.end(); ++it) {
		LONGS_EQUAL(100, *it);
	}
}

TEST(RingBufferTest, assign_n_exception)
{
	RingBuffer<int, SIZE> x;
	try {
		x.assign(SIZE + 1, 100);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("RingBuffer::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(RingBufferTest, assign_range)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x;
	x.assign(a.begin(), a.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(RingBufferTest, assign_range_c_array)
{
	const int a[SIZE] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x;
	x.assign(&a[0], &a[SIZE]);
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(RingBufferTest, assign_range_FixedVector_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedVector<int, SIZE> b(a.begin(), a.end());
	RingBuffer<int, SIZE> x;
	x.assign(b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(RingBufferTest, assign_range_RingBuffer_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const RingBuffer<int, SIZE> b(a.begin(), a.end());
	RingBuffer<int, SIZE> x;
	x.assign(b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

#ifndef NO_STD_CONTAINER
TEST(RingBufferTest, assign_range_deque_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const std::deque<int> b(a.begin(), a.end());
	RingBuffer<int, SIZE> x;
	x.assign(b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(RingBufferTest, assign_range_list_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const std::list<int> b(a.begin(), a.end());
	RingBuffer<int, SIZE> x;
	x.assign(b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}
#endif

TEST(RingBufferTest, assign_range_exception)
{
	const Array<int, SIZE + 1> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	RingBuffer<int, SIZE> x;
	try {
		x.assign(a.begin(), a.end());
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("RingBuffer::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(RingBufferTest, insert)
{
	RingBuffer<int, SIZE> x;
	RingBuffer<int, SIZE>::iterator it;
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

TEST(RingBufferTest, insert2)
{
	RingBuffer<int, SIZE> x;
	x.push_front(1);
	x.pop_back();
	LONGS_EQUAL(0, x.size());

	RingBuffer<int, SIZE>::iterator it;
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

TEST(RingBufferTest, insert_exception)
{
	RingBuffer<int, SIZE> x(SIZE);
	try {
		x.insert(x.begin(), 100);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("RingBuffer::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(RingBufferTest, insert_n)
{
	RingBuffer<int, SIZE> x;
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

TEST(RingBufferTest, insert_n2)
{
	RingBuffer<int, SIZE> x;
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

TEST(RingBufferTest, insert_n_dispatch)
{
	RingBuffer<int, SIZE> x;
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

TEST(RingBufferTest, insert_n_dispatch2)
{
	RingBuffer<int, SIZE> x;
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

TEST(RingBufferTest, insert_n_exception)
{
	RingBuffer<int, SIZE> x;
	try {
		x.insert(x.begin(), SIZE + 1, 100);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("RingBuffer::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(RingBufferTest, insert_range)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x;
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

TEST(RingBufferTest, insert_range2)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x;
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

TEST(RingBufferTest, insert_range_c_array)
{
	const int a[SIZE] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x;
	x.insert(x.end(), &a[0], &a[SIZE]);
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(RingBufferTest, insert_range_FixedVector_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedVector<int, SIZE> b(a.begin(), a.end());
	RingBuffer<int, SIZE> x;
	x.insert(x.end(), b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(RingBufferTest, insert_range_RingBuffer_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const RingBuffer<int, SIZE> b(a.begin(), a.end());
	RingBuffer<int, SIZE> x;
	x.insert(x.end(), b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

#ifndef NO_STD_CONTAINER
TEST(RingBufferTest, insert_range_deque_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const std::deque<int> b(a.begin(), a.end());
	RingBuffer<int, SIZE> x;
	x.insert(x.end(), b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(RingBufferTest, insert_range_list_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const std::list<int> b(a.begin(), a.end());
	RingBuffer<int, SIZE> x;
	x.insert(x.end(), b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}
#endif

TEST(RingBufferTest, insert_range_exception)
{
	const Array<int, SIZE + 1> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	RingBuffer<int, SIZE> x;
	try {
		x.insert(x.end(), a.begin(), a.end());
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("RingBuffer::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(RingBufferTest, erase)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	RingBuffer<int, SIZE>::iterator it;
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

TEST(RingBufferTest, erase_range)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	RingBuffer<int, SIZE>::iterator it;
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

TEST(RingBufferTest, erase_range2)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x;
	x.push_front(1);
	x.pop_back();
	LONGS_EQUAL(0, x.size());
	x.assign(a.begin(), a.end());

	RingBuffer<int, SIZE>::iterator it;
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

TEST(RingBufferTest, erase_range_all)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	RingBuffer<int, SIZE>::iterator it;
	it = x.erase(x.begin(), x.end());
	LONGS_EQUAL(0, x.size());
	CHECK_EQUAL(x.end(), it);
}

TEST(RingBufferTest, operator_equal_true)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	RingBuffer<int, SIZE> y(a.begin(), a.end());
	CHECK_TRUE(x == y);
}

TEST(RingBufferTest, operator_equal_true_after_pop_back)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	RingBuffer<int, SIZE> y(a.begin(), a.end());
	x.back() = 100;
	y.back() = 101;
	x.pop_back();
	y.pop_back();
	CHECK_TRUE(x == y);
}

TEST(RingBufferTest, operator_equal_false)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const Array<int, SIZE> b = {0, 1, 2, 3, 4, 5, 6, 7, 8, 10};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	RingBuffer<int, SIZE> y(b.begin(), b.end());
	CHECK_FALSE(x == y);
}

TEST(RingBufferTest, operator_equal_false_diff_size)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	RingBuffer<int, SIZE> y(a.begin(), a.end() - 1);
	CHECK_FALSE(x == y);
}

TEST(RingBufferTest, operator_notequal_true)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const Array<int, SIZE> b = {0, 1, 2, 3, 4, 5, 6, 7, 8, 10};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	RingBuffer<int, SIZE> y(b.begin(), b.end());
	CHECK_TRUE(x != y);
}

TEST(RingBufferTest, operator_notequal_false)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	RingBuffer<int, SIZE> y(a.begin(), a.end());
	CHECK_FALSE(x != y);
}

TEST(RingBufferTest, iterator_copy_ctor)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	RingBuffer<int, SIZE>::iterator it = x.begin();
	RingBuffer<int, SIZE>::iterator it2(it);
	CHECK_EQUAL(it, it2);
	RingBuffer<int, SIZE>::iterator it3;
	it3 = it;
	CHECK_EQUAL(it, it3);

	const RingBuffer<int, SIZE>& y = x;
	RingBuffer<int, SIZE>::const_iterator cit = y.begin();
	RingBuffer<int, SIZE>::const_iterator cit2(it);
	CHECK_EQUAL(cit, cit2);
	RingBuffer<int, SIZE>::const_iterator cit3;
	cit3 = it;
	CHECK_EQUAL(cit, cit3);
}

TEST(RingBufferTest, iterator_operator_plusequal)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	RingBuffer<int, SIZE>::iterator it = x.begin();
	RingBuffer<int, SIZE>::iterator i = it += 5;
	LONGS_EQUAL(5, *it);
	CHECK_EQUAL(x.begin() + 5, it);
	CHECK_EQUAL(it, i);
}

TEST(RingBufferTest, iterator_operator_plusequal_0)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	RingBuffer<int, SIZE>::iterator it = x.begin();
	it += 0;
	LONGS_EQUAL(0, *it);
	CHECK_EQUAL(x.begin(), it);
}

TEST(RingBufferTest, iterator_operator_plusequal_max)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	RingBuffer<int, SIZE>::iterator it = x.begin();
	it += SIZE;
	CHECK_EQUAL(x.end(), it);
}

TEST(RingBufferTest, iterator_operator_plusequal_minus)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	RingBuffer<int, SIZE>::iterator it = x.end();
	RingBuffer<int, SIZE>::iterator i = it += -1;
	LONGS_EQUAL(9, *it);
	CHECK_EQUAL(x.begin() + 9, it);
	CHECK_EQUAL(it, i);
}

TEST(RingBufferTest, iterator_operator_plusequal_minus_max)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	RingBuffer<int, SIZE>::iterator it = x.end();
	it += -static_cast<std::ptrdiff_t>(SIZE);
	LONGS_EQUAL(0, *it);
	CHECK_EQUAL(x.begin(), it);
}

TEST(RingBufferTest, iterator_operator_plus)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	RingBuffer<int, SIZE>::iterator it = x.begin() + 5;
	LONGS_EQUAL(5, *it);
}

TEST(RingBufferTest, iterator_operator_plus_nonmember)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	RingBuffer<int, SIZE>::iterator it = 5 + x.begin();
	LONGS_EQUAL(5, *it);
	LONGS_EQUAL(1, *(-4 + it));
}

TEST(RingBufferTest, iterator_operator_incremant)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	RingBuffer<int, SIZE>::iterator it = x.begin();
	RingBuffer<int, SIZE>::iterator i = ++it;
	LONGS_EQUAL(1, *it);
	LONGS_EQUAL(1, *i);

	RingBuffer<int, SIZE>::iterator j = it++;
	LONGS_EQUAL(2, *it);
	LONGS_EQUAL(1, *j);
}

TEST(RingBufferTest, iterator_operator_minusqual)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	RingBuffer<int, SIZE>::iterator it = x.end();
	RingBuffer<int, SIZE>::iterator i = it -= 5;
	LONGS_EQUAL(5, *it);
	CHECK_EQUAL(x.end() - 5, it);
	CHECK_EQUAL(it, i);
}

TEST(RingBufferTest, iterator_operator_minusequal_0)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	RingBuffer<int, SIZE>::iterator it = x.begin();
	it -= 0;
	LONGS_EQUAL(0, *it);
	CHECK_EQUAL(x.begin(), it);
}

TEST(RingBufferTest, iterator_operator_minusqual_max)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	RingBuffer<int, SIZE>::iterator it = x.end();
	it -= SIZE;
	LONGS_EQUAL(0, *it);
	CHECK_EQUAL(x.begin(), it);
}

TEST(RingBufferTest, iterator_operator_minusequal_minus)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	RingBuffer<int, SIZE>::iterator it = x.begin();
	RingBuffer<int, SIZE>::iterator i = it -= -1;
	LONGS_EQUAL(1, *it);
	CHECK_EQUAL(x.begin() + 1, it);
	CHECK_EQUAL(it, i);
}

TEST(RingBufferTest, iterator_operator_minusequal_minus_max)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	RingBuffer<int, SIZE>::iterator it = x.begin();
	it -= -static_cast<std::ptrdiff_t>(SIZE);
	CHECK_EQUAL(x.end(), it);
}

TEST(RingBufferTest, iterator_operator_minus)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	RingBuffer<int, SIZE>::iterator it = x.end() - 5;
	LONGS_EQUAL(5, *it);
}

TEST(RingBufferTest, iterator_operator_decremant)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	RingBuffer<int, SIZE>::iterator it = x.end();
	RingBuffer<int, SIZE>::iterator i = --it;
	LONGS_EQUAL(9, *it);
	LONGS_EQUAL(9, *i);

	RingBuffer<int, SIZE>::iterator j = it--;
	LONGS_EQUAL(8, *it);
	LONGS_EQUAL(9, *j);
}

struct A {
	int a;
	const char* b;
};

TEST(RingBufferTest, iterator_operator_arrow)
{
	A a = {1, "foo"};
	RingBuffer<A, SIZE> x(1, a);
	RingBuffer<A, SIZE>::iterator it = x.begin();
	LONGS_EQUAL(1, it->a);
	STRCMP_EQUAL("foo", it->b);

	it->a = 10;
	LONGS_EQUAL(10, it->a);

	RingBuffer<A, SIZE>::const_iterator cit(it);
	LONGS_EQUAL(10, cit->a);
	STRCMP_EQUAL("foo", cit->b);
}

TEST(RingBufferTest, iterator_operator_asterisk)
{
	A a = {1, "foo"};
	RingBuffer<A, SIZE> x(1, a);
	RingBuffer<A, SIZE>::iterator it = x.begin();
	LONGS_EQUAL(1, (*it).a);
	STRCMP_EQUAL("foo", (*it).b);

	(*it).a = 10;
	LONGS_EQUAL(10, (*it).a);

	RingBuffer<A, SIZE>::const_iterator cit(it);
	LONGS_EQUAL(10, (*cit).a);
	STRCMP_EQUAL("foo", (*cit).b);
}

TEST(RingBufferTest, iterator_operator_bracket_read)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	RingBuffer<int, SIZE>::iterator it = x.begin();
	LONGS_EQUAL(0, it[0]);
	LONGS_EQUAL(5, it[5]);
	LONGS_EQUAL(9, it[9]);
}

TEST(RingBufferTest, iterator_operator_bracket_write)
{
	RingBuffer<int, SIZE> x(10);
	RingBuffer<int, SIZE>::iterator it = x.begin();
	it[0] = 100;
	LONGS_EQUAL(100, it[0]);

	it[9] = 10;
	LONGS_EQUAL(10, it[9]);
}

TEST(RingBufferTest, iterator_operator_equal_true)
{
	RingBuffer<int, SIZE> x(SIZE);
	RingBuffer<int, SIZE>::iterator it = x.begin();
	RingBuffer<int, SIZE>::iterator it2 = x.begin();
	RingBuffer<int, SIZE>::const_iterator cit(it);
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

TEST(RingBufferTest, iterator_operator_equal_false)
{
	RingBuffer<int, SIZE> x(SIZE);
	RingBuffer<int, SIZE>::iterator it = x.begin();
	RingBuffer<int, SIZE>::iterator it2 = x.begin() + 1;
	CHECK_FALSE(it == it2);

	RingBuffer<int, SIZE> y(SIZE);
	RingBuffer<int, SIZE>::iterator it3 = y.begin();
	CHECK_FALSE(it == it3);
}

TEST(RingBufferTest, iterator_operator_notequal_true)
{
	RingBuffer<int, SIZE> x(SIZE);
	RingBuffer<int, SIZE>::iterator it = x.begin();
	RingBuffer<int, SIZE>::iterator it2 = x.begin() + 1;
	CHECK_TRUE(it != it2);

	it = x.begin();
	it2 = x.end();
	CHECK_TRUE(it != it2);
}

TEST(RingBufferTest, iterator_operator_notequal_false)
{
	RingBuffer<int, SIZE> x(SIZE);
	RingBuffer<int, SIZE>::iterator it = x.begin();
	RingBuffer<int, SIZE>::iterator it2 = x.begin();
	CHECK_FALSE(it != it2);

	it = x.end();
	it2 = x.end();
	CHECK_FALSE(it != it2);
}

TEST(RingBufferTest, iterator_operator_less_true)
{
	RingBuffer<int, SIZE> x(SIZE);
	RingBuffer<int, SIZE>::iterator it = x.begin();
	RingBuffer<int, SIZE>::iterator it2 = x.end();
	CHECK_TRUE(it < it2);
}

TEST(RingBufferTest, iterator_operator_less_true2)
{
	RingBuffer<int, SIZE> x;
	x.push_front(1);
	x.pop_back();
	LONGS_EQUAL(0, x.size());

	x.resize(SIZE);
	RingBuffer<int, SIZE>::iterator it = x.begin();
	RingBuffer<int, SIZE>::iterator it2 = x.end();
	CHECK_TRUE(it < it2);
}

TEST(RingBufferTest, iterator_operator_less_false)
{
	RingBuffer<int, SIZE> x(SIZE);
	RingBuffer<int, SIZE>::iterator it = x.begin();
	RingBuffer<int, SIZE>::iterator it2 = x.begin();
	CHECK_FALSE(it < it2);

	it = x.end();
	it2 = x.end();
	CHECK_FALSE(it < it2);

	it = x.end();
	it2 = x.begin();
	CHECK_FALSE(it < it2);
}

TEST(RingBufferTest, iterator_operator_less_false2)
{
	RingBuffer<int, SIZE> x;
	x.push_front(1);
	x.pop_back();
	LONGS_EQUAL(0, x.size());

	x.resize(SIZE);
	RingBuffer<int, SIZE>::iterator it = x.begin();
	RingBuffer<int, SIZE>::iterator it2 = x.begin();
	CHECK_FALSE(it < it2);

	it = x.end();
	it2 = x.end();
	CHECK_FALSE(it < it2);

	it = x.end();
	it2 = x.begin();
	CHECK_FALSE(it < it2);
}

TEST(RingBufferTest, iterator_operator_greater_true)
{
	RingBuffer<int, SIZE> x(SIZE);
	RingBuffer<int, SIZE>::iterator it = x.end();
	RingBuffer<int, SIZE>::iterator it2 = x.begin();
	CHECK_TRUE(it > it2);
}

TEST(RingBufferTest, iterator_operator_greater_false)
{
	RingBuffer<int, SIZE> x(SIZE);
	RingBuffer<int, SIZE>::iterator it = x.begin();
	RingBuffer<int, SIZE>::iterator it2 = x.begin();
	CHECK_FALSE(it > it2);

	it = x.end();
	it2 = x.end();
	CHECK_FALSE(it > it2);

	it = x.begin();
	it2 = x.end();
	CHECK_FALSE(it > it2);
}

TEST(RingBufferTest, iterator_operator_lessequal_true)
{
	RingBuffer<int, SIZE> x(SIZE);
	RingBuffer<int, SIZE>::iterator it = x.begin();
	RingBuffer<int, SIZE>::iterator it2 = x.end();
	CHECK_TRUE(it <= it2);

	it = x.begin();
	it2 = x.begin();
	CHECK_TRUE(it <= it2);

	it = x.end();
	it2 = x.end();
	CHECK_TRUE(it <= it2);
}

TEST(RingBufferTest, iterator_operator_lessequal_false)
{
	RingBuffer<int, SIZE> x(SIZE);
	RingBuffer<int, SIZE>::iterator it = x.end();
	RingBuffer<int, SIZE>::iterator it2 = x.begin();
	CHECK_FALSE(it <= it2);
}

TEST(RingBufferTest, iterator_operator_greaterequal_true)
{
	RingBuffer<int, SIZE> x(SIZE);
	RingBuffer<int, SIZE>::iterator it = x.end();
	RingBuffer<int, SIZE>::iterator it2 = x.begin();
	CHECK_TRUE(it >= it2);

	it = x.begin();
	it2 = x.begin();
	CHECK_TRUE(it >= it2);

	it = x.end();
	it2 = x.end();
	CHECK_TRUE(it >= it2);
}

TEST(RingBufferTest, iterator_operator_greaterequal_false)
{
	RingBuffer<int, SIZE> x(SIZE);
	RingBuffer<int, SIZE>::iterator it = x.begin();
	RingBuffer<int, SIZE>::iterator it2 = x.end();
	CHECK_FALSE(it >= it2);
}

TEST(RingBufferTest, iterator_difference)
{
	RingBuffer<int, SIZE> x(SIZE);
	RingBuffer<int, SIZE>::iterator it = x.begin();
	RingBuffer<int, SIZE>::iterator it2 = x.end();
	LONGS_EQUAL(SIZE, it2 - it);
	LONGS_EQUAL(-static_cast<std::ptrdiff_t>(SIZE), it - it2);
}

TEST(RingBufferTest, iterator_difference2)
{
	RingBuffer<int, SIZE> x;
	x.push_front(1);
	x.pop_back();
	LONGS_EQUAL(0, x.size());

	x.resize(SIZE);
	RingBuffer<int, SIZE>::iterator it = x.begin();
	RingBuffer<int, SIZE>::iterator it2 = x.end();
	LONGS_EQUAL(SIZE, it2 - it);
	LONGS_EQUAL(-static_cast<std::ptrdiff_t>(SIZE), it - it2);
}

TEST(RingBufferTest, new_delete)
{
	RingBuffer<int, SIZE>* x = new RingBuffer<int, SIZE>(SIZE);
	LONGS_EQUAL(SIZE, x->size());
	delete x;
}

#ifndef NO_STD_ALGORITHM
TEST(RingBufferTest, algo_sort)
{
	const Array<int, SIZE> a = {1, 7, 0, 2, 5, 3, 9, 4, 6, 8};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	std::sort(x.begin(), x.end());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x.at(i));
	}

	std::sort(x.begin(), x.end(), std::greater<int>());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(SIZE - i - 1, x.at(i));
	}
}

TEST(RingBufferTest, algo_copy)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x;
	std::copy(a.begin(), a.end(), std::back_inserter(x));
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x.at(i));
	}
}

TEST(RingBufferTest, algo_copy_exception)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(1);
	try {
		std::copy(a.begin(), a.end(), std::back_inserter(x));
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("RingBuffer::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}
#endif

#ifndef NO_STD_ITERATOR
TEST(RingBufferTest, rbegin_rend)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RingBuffer<int, SIZE> x(a.begin(), a.end());
	std::size_t i = SIZE - 1;
	for (RingBuffer<int, SIZE>::reverse_iterator it = x.rbegin(); it != x.rend(); ++it, --i) {
		LONGS_EQUAL(a[i], *it);
	}
}

TEST(RingBufferTest, rbegin_rend_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const RingBuffer<int, SIZE> x(a.begin(), a.end());
	std::size_t i = SIZE - 1;
	for (RingBuffer<int, SIZE>::const_reverse_iterator it = x.rbegin(); it != x.rend(); ++it, --i) {
		LONGS_EQUAL(a[i], *it);
	}
}
#endif


class C {
	int count;
public:
	C() : count(1)
	{
		mock().actualCall("C_default_ctor");
	}
	C(const C&) : count(1)
	{
		mock().actualCall("C_copy_ctor");
	}
	C& operator=(const C&)
	{
		mock().actualCall("C_operator_assign");
		return *this;
	}
	~C()
	{
		--count;
		mock().actualCall("C_dtor");
		LONGS_EQUAL(0, count);
	}
};

TEST(RingBufferTest, default_ctor_C)
{
	mock().expectNCalls(0, "C_default_ctor");
	mock().expectNCalls(0, "C_dtor");
	RingBuffer<C, SIZE> x;
}

TEST(RingBufferTest, ctor_C)
{
	mock().expectNCalls(1, "C_default_ctor");
	mock().expectNCalls(1, "C_dtor");
	C c;

	mock().expectNCalls(2, "C_copy_ctor");
	mock().expectNCalls(2, "C_dtor");
	RingBuffer<C, SIZE> x(2, c);
	LONGS_EQUAL(2, x.size());

	mock().expectNCalls(2, "C_copy_ctor");
	mock().expectNCalls(2, "C_dtor");
	RingBuffer<C, SIZE> y(x.begin(), x.end());
	LONGS_EQUAL(2, y.size());

	mock().expectNCalls(2, "C_copy_ctor");
	mock().expectNCalls(2, "C_dtor");
	RingBuffer<C, SIZE> z(x);
	LONGS_EQUAL(2, z.size());
}

TEST(RingBufferTest, operator_assign_C)
{
	mock().expectNCalls(1, "C_default_ctor");
	mock().expectNCalls(1, "C_dtor");
	C c;

	mock().expectNCalls(2, "C_copy_ctor");
	mock().expectNCalls(2, "C_dtor");
	RingBuffer<C, SIZE> x(2, c);

	mock().expectNCalls(1, "C_copy_ctor");
	RingBuffer<C, SIZE> y(1, c);

	mock().expectNCalls(1, "C_operator_assign");
	mock().expectNCalls(1, "C_copy_ctor");
	mock().expectNCalls(2, "C_dtor");
	y = x;
	LONGS_EQUAL(2, y.size());

	mock().expectNCalls(3, "C_copy_ctor");
	RingBuffer<C, SIZE> z(3, c);

	mock().expectNCalls(2, "C_operator_assign");
	mock().expectNCalls(3, "C_dtor");
	z = x;
	LONGS_EQUAL(2, z.size());
}

TEST(RingBufferTest, push_back_pop_back_C)
{
	mock().expectNCalls(1, "C_default_ctor");
	C c;

	mock().expectNCalls(2, "C_copy_ctor");
	RingBuffer<C, SIZE> x;
	x.push_back(c);
	x.push_back(c);

	mock().expectNCalls(1, "C_dtor");
	x.pop_back();

	mock().expectNCalls(1, "C_dtor");
	mock().expectNCalls(1, "C_dtor");
}

TEST(RingBufferTest, insert_n_C)
{
	mock().expectNCalls(1, "C_default_ctor");
	C c;

	mock().expectNCalls(4, "C_copy_ctor");
	RingBuffer<C, SIZE> x;
	x.insert(x.end(), 4, c);

	mock().expectNCalls(1, "C_copy_ctor");
	mock().expectNCalls(1, "C_operator_assign");
	mock().expectNCalls(1, "C_operator_assign");
	x.insert(x.begin() + 2, 1, c);

	mock().expectNCalls(1, "C_copy_ctor");
	x.insert(x.end(), 1, c);

	mock().expectNCalls(1, "C_copy_ctor");
	mock().expectNCalls(1, "C_operator_assign");
	mock().expectNCalls(1, "C_operator_assign");
	x.insert(x.end() - 2, 1, c);

	mock().expectNCalls(7, "C_dtor");
	mock().expectNCalls(1, "C_dtor");
}

TEST(RingBufferTest, insert_range_C)
{
	mock().expectNCalls(1, "C_default_ctor");
	C c;

	mock().expectNCalls(4, "C_copy_ctor");
	RingBuffer<C, SIZE> x(4, c);

	RingBuffer<C, SIZE> y;

	mock().expectNCalls(4, "C_copy_ctor");
	y.insert(y.end(), x.begin(), x.end());

	mock().expectNCalls(1, "C_copy_ctor");
	mock().expectNCalls(1, "C_operator_assign");
	mock().expectNCalls(1, "C_operator_assign");
	y.insert(y.begin() + 2, x.begin(), x.begin() + 1);

	mock().expectNCalls(1, "C_copy_ctor");
	y.insert(y.end(), x.begin(), x.begin() + 1);

	mock().expectNCalls(1, "C_copy_ctor");
	mock().expectNCalls(1, "C_operator_assign");
	mock().expectNCalls(1, "C_operator_assign");
	y.insert(y.end() - 2, x.begin(), x.begin() + 1);

	mock().expectNCalls(7, "C_dtor");
	mock().expectNCalls(4, "C_dtor");
	mock().expectNCalls(1, "C_dtor");
}

TEST(RingBufferTest, erase_range_C)
{
	mock().expectNCalls(1, "C_default_ctor");
	C c;

	mock().expectNCalls(5, "C_copy_ctor");
	RingBuffer<C, SIZE> x(5, c);

	mock().expectNCalls(1, "C_operator_assign");
	mock().expectNCalls(1, "C_dtor");
	x.erase(x.begin() + 1, x.begin() + 2);

	mock().expectNCalls(1, "C_operator_assign");
	mock().expectNCalls(1, "C_dtor");
	x.erase(x.end() - 2, x.end() - 1);

	mock().expectNCalls(3, "C_dtor");
	x.erase(x.begin(), x.end());

	mock().expectNCalls(1, "C_dtor");
}

#if (__cplusplus >= 201103L) || defined(_WIN32)
#include <memory>

TEST(RingBufferTest, shared_ptr_int)
{
	RingBuffer<std::shared_ptr<int>, SIZE> x;
	x.push_back(std::make_shared<int>(1));
	LONGS_EQUAL(1, *x[0]);
	*x[0] = 2;
	LONGS_EQUAL(2, *x[0]);
}

TEST(RingBufferTest, shared_ptr_C)
{
	mock().expectNCalls(1, "C_default_ctor");
	C c;

	{
		RingBuffer<std::shared_ptr<C>, SIZE> x;

		mock().expectNCalls(2, "C_copy_ctor");
		x.push_back(std::make_shared<C>(c));
		x.push_back(std::make_shared<C>(c));

		mock().expectNCalls(1, "C_dtor");
		x.pop_back();

		mock().expectNCalls(1, "C_dtor");
	}

	mock().expectNCalls(1, "C_dtor");
}
#endif
