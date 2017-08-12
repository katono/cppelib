#include "CppUTest/TestHarness.h"
#include "Container/FixedVector.h"
#include "Container/Array.h"
#include "Container/RingBuffer.h"
#ifndef NO_STD_CONTAINER
#include <deque>
#include <list>
#endif
#ifndef NO_STD_ALGORITHM
#include <algorithm>
#include <functional>
#endif

using namespace Container;

TEST_GROUP(FixedVectorTest) {
	static const size_t SIZE = 10;
	void setup()
	{
	}
	void teardown()
	{
	}
};

TEST(FixedVectorTest, size_0)
{
	FixedVector<int, SIZE> x;
	LONGS_EQUAL(0, x.size());
}

TEST(FixedVectorTest, size)
{
	FixedVector<int, SIZE> x(5);
	LONGS_EQUAL(5, x.size());
}

TEST(FixedVectorTest, available_size)
{
	FixedVector<int, SIZE> x(5);
	LONGS_EQUAL(SIZE - 5, x.available_size());
}

TEST(FixedVectorTest, max_size)
{
	FixedVector<int, SIZE> x;
	LONGS_EQUAL(SIZE, x.max_size());
}

TEST(FixedVectorTest, empty_true)
{
	FixedVector<int, SIZE> x;
	CHECK(x.empty());
}

TEST(FixedVectorTest, empty_false)
{
	FixedVector<int, SIZE> x(1);
	CHECK_FALSE(x.empty());
}

TEST(FixedVectorTest, full_true)
{
	FixedVector<int, SIZE> x(SIZE);
	CHECK(x.full());
}

TEST(FixedVectorTest, full_false)
{
	FixedVector<int, SIZE> x(SIZE - 1);
	CHECK_FALSE(x.full());
}

TEST(FixedVectorTest, clear)
{
	FixedVector<int, SIZE> x(SIZE);
	x.clear();
	CHECK(x.empty());
}

TEST(FixedVectorTest, operator_bracket_read)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	LONGS_EQUAL(0, x[0]);
	LONGS_EQUAL(5, x[5]);
	LONGS_EQUAL(9, x[9]);
}

TEST(FixedVectorTest, operator_bracket_read_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedVector<int, SIZE> x(a.begin(), a.end());
	LONGS_EQUAL(0, x[0]);
	LONGS_EQUAL(5, x[5]);
	LONGS_EQUAL(9, x[9]);
}

TEST(FixedVectorTest, operator_bracket_write)
{
	FixedVector<int, SIZE> x(10);
	x[0] = 100;
	LONGS_EQUAL(100, x[0]);

	x[9] = 10;
	LONGS_EQUAL(10, x[9]);
}

TEST(FixedVectorTest, at_read)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	LONGS_EQUAL(0, x.at(0));
	LONGS_EQUAL(5, x.at(5));
	LONGS_EQUAL(9, x.at(9));
}

TEST(FixedVectorTest, at_read_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedVector<int, SIZE> x(a.begin(), a.end());
	LONGS_EQUAL(0, x.at(0));
	LONGS_EQUAL(5, x.at(5));
	LONGS_EQUAL(9, x.at(9));
}

TEST(FixedVectorTest, at_write)
{
	FixedVector<int, SIZE> x(10);
	x.at(0) = 100;
	LONGS_EQUAL(100, x.at(0));

	x.at(9) = 10;
	LONGS_EQUAL(10, x.at(9));
}

TEST(FixedVectorTest, at_exception)
{
	FixedVector<int, SIZE> x;
	try {
		x.at(0);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("FixedVector::at", e.what());
		return;
	}
	FAIL("failed");
}

TEST(FixedVectorTest, at_exception_const)
{
	const FixedVector<int, SIZE> x;
	try {
		x.at(0);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("FixedVector::at", e.what());
		return;
	}
	FAIL("failed");
}

TEST(FixedVectorTest, data_read)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	LONGS_EQUAL(0, x.data()[0]);
	LONGS_EQUAL(5, x.data()[5]);
	LONGS_EQUAL(9, x.data()[9]);
}

TEST(FixedVectorTest, data_read_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedVector<int, SIZE> x(a.begin(), a.end());
	LONGS_EQUAL(0, x.data()[0]);
	LONGS_EQUAL(5, x.data()[5]);
	LONGS_EQUAL(9, x.data()[9]);
}

TEST(FixedVectorTest, data_write)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	int* p = x.data();
	p[5] = 100;
	LONGS_EQUAL(100, x.data()[5]);
}

TEST(FixedVectorTest, begin_end)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	size_t i = 0;
	for (FixedVector<int, SIZE>::iterator it = x.begin(); it != x.end(); ++it, ++i) {
		LONGS_EQUAL(a[i], *it);
	}
}

TEST(FixedVectorTest, begin_end_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedVector<int, SIZE> x(a.begin(), a.end());
	size_t i = 0;
	for (FixedVector<int, SIZE>::const_iterator it = x.begin(); it != x.end(); ++it, ++i) {
		LONGS_EQUAL(a[i], *it);
	}
}

TEST(FixedVectorTest, front_read)
{
	FixedVector<int, SIZE> x(1);
	LONGS_EQUAL(0, x.front());
}

TEST(FixedVectorTest, front_read_const)
{
	const FixedVector<int, SIZE> x(1);
	LONGS_EQUAL(0, x.front());
}

TEST(FixedVectorTest, front_write)
{
	FixedVector<int, SIZE> x(1);
	x.front() = 100;
	LONGS_EQUAL(100, x.front());
}

TEST(FixedVectorTest, back_read)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	LONGS_EQUAL(9, x.back());
}

TEST(FixedVectorTest, back_read_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedVector<int, SIZE> x(a.begin(), a.end());
	LONGS_EQUAL(9, x.back());
}

TEST(FixedVectorTest, back_write)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	x.back() = 100;
	LONGS_EQUAL(100, x.back());
}

TEST(FixedVectorTest, ctor_n_data)
{
	FixedVector<int, SIZE> x(SIZE, 100);
	LONGS_EQUAL(SIZE, x.size());
	for (FixedVector<int, SIZE>::iterator it = x.begin(); it != x.end(); ++it) {
		LONGS_EQUAL(100, *it);
	}
}

TEST(FixedVectorTest, ctor_n_data_exception)
{
	try {
		FixedVector<int, SIZE> x(SIZE + 1);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("Container::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(FixedVectorTest, ctor_range)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	LONGS_EQUAL(SIZE, x.size());
	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(FixedVectorTest, ctor_range_exception)
{
	const Array<int, SIZE + 1> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	try {
		FixedVector<int, SIZE> x(a.begin(), a.end());
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("Container::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(FixedVectorTest, copy_ctor)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedVector<int, SIZE> b(a.begin(), a.end());
	const FixedVector<int, SIZE> x(b);
	LONGS_EQUAL(SIZE, b.size());
	LONGS_EQUAL(SIZE, x.size());
	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(a[i], b[i]);
		LONGS_EQUAL(b[i], x[i]);
	}
}

TEST(FixedVectorTest, operator_assign)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedVector<int, SIZE> b(a.begin(), a.end());
	FixedVector<int, SIZE> x;
	x = b;
	LONGS_EQUAL(SIZE, x.size());
	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(b[i], x[i]);
	}
}

TEST(FixedVectorTest, resize_shorten)
{
	FixedVector<int, SIZE> x(5);
	x.resize(3);
	LONGS_EQUAL(3, x.size());
}

TEST(FixedVectorTest, resize_make_longer)
{
	FixedVector<int, SIZE> x(5);
	x.resize(8, 100);
	LONGS_EQUAL(8, x.size());
	size_t i;
	for (i = 0; i < 5; ++i) {
		LONGS_EQUAL(0, x[i]);
	}
	for (; i < x.size(); ++i) {
		LONGS_EQUAL(100, x[i]);
	}
}

TEST(FixedVectorTest, resize_exception)
{
	FixedVector<int, SIZE> x;
	try {
		x.resize(SIZE + 1);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("Container::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(FixedVectorTest, push_back)
{
	FixedVector<int, SIZE> x;
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

TEST(FixedVectorTest, push_back_exception)
{
	FixedVector<int, SIZE> x(SIZE - 1);
	x.push_back(100);
	LONGS_EQUAL(SIZE, x.size());

	try {
		x.push_back(101);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("Container::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(FixedVectorTest, pop_back)
{
	FixedVector<int, SIZE> x(3);
	x.pop_back();
	LONGS_EQUAL(2, x.size());
	x.pop_back();
	LONGS_EQUAL(1, x.size());
	x.pop_back();
	LONGS_EQUAL(0, x.size());
}

TEST(FixedVectorTest, assign_n)
{
	FixedVector<int, SIZE> x;
	x.assign(SIZE, 100);
	LONGS_EQUAL(SIZE, x.size());
	for (FixedVector<int, SIZE>::iterator it = x.begin(); it != x.end(); ++it) {
		LONGS_EQUAL(100, *it);
	}
}

TEST(FixedVectorTest, assign_n_exception)
{
	FixedVector<int, SIZE> x;
	try {
		x.assign(SIZE + 1, 100);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("Container::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(FixedVectorTest, assign_range)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x;
	x.assign(a.begin(), a.end());
	LONGS_EQUAL(SIZE, x.size());
	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(FixedVectorTest, assign_range_c_array)
{
	const int a[SIZE] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x;
	x.assign(&a[0], &a[SIZE]);
	LONGS_EQUAL(SIZE, x.size());
	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(FixedVectorTest, assign_range_FixedVector_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedVector<int, SIZE> b(a.begin(), a.end());
	FixedVector<int, SIZE> x;
	x.assign(b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(FixedVectorTest, assign_range_RingBuffer_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const RingBuffer<int, SIZE> b(a.begin(), a.end());
	FixedVector<int, SIZE> x;
	x.assign(b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

#ifndef NO_STD_CONTAINER
TEST(FixedVectorTest, assign_range_deque_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const std::deque<int> b(a.begin(), a.end());
	FixedVector<int, SIZE> x;
	x.assign(b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(FixedVectorTest, assign_range_list_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const std::list<int> b(a.begin(), a.end());
	FixedVector<int, SIZE> x;
	x.assign(b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}
#endif

TEST(FixedVectorTest, assign_range_exception)
{
	const Array<int, SIZE + 1> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	FixedVector<int, SIZE> x;
	try {
		x.assign(a.begin(), a.end());
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("Container::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(FixedVectorTest, insert)
{
	FixedVector<int, SIZE> x;
	FixedVector<int, SIZE>::iterator it;
	it = x.insert(x.end(), 1);
	CHECK_EQUAL(it, x.begin());
	LONGS_EQUAL(1, x.size());
	LONGS_EQUAL(1, x[0]);

	it = x.insert(x.begin(), 2);
	CHECK_EQUAL(it, x.begin());
	LONGS_EQUAL(2, x.size());
	LONGS_EQUAL(2, x[0]);
	LONGS_EQUAL(1, x[1]);

	it = x.insert(x.begin() + 1, 3);
	CHECK_EQUAL(it, x.begin() + 1);
	LONGS_EQUAL(3, x.size());
	LONGS_EQUAL(2, x[0]);
	LONGS_EQUAL(3, x[1]);
	LONGS_EQUAL(1, x[2]);

}

TEST(FixedVectorTest, insert_exception)
{
	FixedVector<int, SIZE> x(SIZE);
	try {
		x.insert(x.begin(), 100);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("Container::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(FixedVectorTest, insert_n)
{
	FixedVector<int, SIZE> x;
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

	x.insert(x.begin() + 1, 1U, 300);
	LONGS_EQUAL(6, x.size());
	LONGS_EQUAL(200, x[0]);
	LONGS_EQUAL(300, x[1]);
	LONGS_EQUAL(200, x[2]);
	LONGS_EQUAL(200, x[3]);
	LONGS_EQUAL(100, x[4]);
	LONGS_EQUAL(100, x[5]);

}

TEST(FixedVectorTest, insert_n_dispatch)
{
	FixedVector<int, SIZE> x;
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

	x.insert(x.begin() + 1, 1, 300);
	LONGS_EQUAL(6, x.size());
	LONGS_EQUAL(200, x[0]);
	LONGS_EQUAL(300, x[1]);
	LONGS_EQUAL(200, x[2]);
	LONGS_EQUAL(200, x[3]);
	LONGS_EQUAL(100, x[4]);
	LONGS_EQUAL(100, x[5]);

}

TEST(FixedVectorTest, insert_n_exception)
{
	FixedVector<int, SIZE> x;
	try {
		x.insert(x.begin(), SIZE + 1, 100);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("Container::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(FixedVectorTest, insert_range)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x;
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

	x.insert(x.begin() + 1, a.end() - 1, a.end());
	LONGS_EQUAL(6, x.size());
	LONGS_EQUAL(3, x[0]);
	LONGS_EQUAL(a.back(), x[1]);
	LONGS_EQUAL(4, x[2]);
	LONGS_EQUAL(0, x[3]);
	LONGS_EQUAL(1, x[4]);
	LONGS_EQUAL(2, x[5]);
}

TEST(FixedVectorTest, insert_range_c_array)
{
	const int a[SIZE] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x;
	x.insert(x.end(), &a[0], &a[SIZE]);
	LONGS_EQUAL(SIZE, x.size());
	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(FixedVectorTest, insert_range_FixedVector_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedVector<int, SIZE> b(a.begin(), a.end());
	FixedVector<int, SIZE> x;
	x.insert(x.end(), b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(FixedVectorTest, insert_range_RingBuffer_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const RingBuffer<int, SIZE> b(a.begin(), a.end());
	FixedVector<int, SIZE> x;
	x.insert(x.end(), b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

#ifndef NO_STD_CONTAINER
TEST(FixedVectorTest, insert_range_deque_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const std::deque<int> b(a.begin(), a.end());
	FixedVector<int, SIZE> x;
	x.insert(x.end(), b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(FixedVectorTest, insert_range_list_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const std::list<int> b(a.begin(), a.end());
	FixedVector<int, SIZE> x;
	x.insert(x.end(), b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}
#endif

TEST(FixedVectorTest, insert_range_exception)
{
	const Array<int, SIZE + 1> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	FixedVector<int, SIZE> x;
	try {
		x.insert(x.end(), a.begin(), a.end());
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("Container::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(FixedVectorTest, erase)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE>::iterator it;
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
}

TEST(FixedVectorTest, erase_range)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE>::iterator it;
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

TEST(FixedVectorTest, erase_range_all)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE>::iterator it;
	it = x.erase(x.begin(), x.end());
	LONGS_EQUAL(0, x.size());
	CHECK_EQUAL(x.end(), it);
}

TEST(FixedVectorTest, swap)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const Array<int, SIZE> b = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE> y(b.begin(), b.end());
	x.swap(y);
	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(x.size() - i - 1, x.at(i));
		LONGS_EQUAL(i, y.at(i));
	}
}

TEST(FixedVectorTest, swap_nonmember)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const Array<int, SIZE> b = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE> y(b.begin(), b.end());
	swap(x, y);
	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(x.size() - i - 1, x.at(i));
		LONGS_EQUAL(i, y.at(i));
	}
}

TEST(FixedVectorTest, operator_equal_true)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE> y(a.begin(), a.end());
	CHECK_TRUE(x == y);
}

TEST(FixedVectorTest, operator_equal_true_exists_garbage)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE> y(a.begin(), a.end());
	x.back() = 100;
	y.back() = 101;
	x.pop_back();
	y.pop_back();
	CHECK_TRUE(x == y);
}

TEST(FixedVectorTest, operator_equal_false)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const Array<int, SIZE> b = {0, 1, 2, 3, 4, 5, 6, 7, 8, 10};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE> y(b.begin(), b.end());
	CHECK_FALSE(x == y);
}

TEST(FixedVectorTest, operator_equal_false_diff_size)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE> y(a.begin(), a.end() - 1);
	CHECK_FALSE(x == y);
}

TEST(FixedVectorTest, operator_notequal_true)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const Array<int, SIZE> b = {0, 1, 2, 3, 4, 5, 6, 7, 8, 10};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE> y(b.begin(), b.end());
	CHECK_TRUE(x != y);
}

TEST(FixedVectorTest, operator_notequal_false)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE> y(a.begin(), a.end());
	CHECK_FALSE(x != y);
}

TEST(FixedVectorTest, new_delete)
{
	FixedVector<int, SIZE>* x = new FixedVector<int, SIZE>(SIZE);
	LONGS_EQUAL(SIZE, x->size());
	delete x;
}

#ifndef NO_STD_ALGORITHM
TEST(FixedVectorTest, algo_sort)
{
	const Array<int, SIZE> a = {1, 7, 0, 2, 5, 3, 9, 4, 6, 8};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	std::sort(x.begin(), x.end());
	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x.at(i));
	}

	std::sort(x.begin(), x.end(), std::greater<int>());
	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(SIZE - i - 1, x.at(i));
	}
}

TEST(FixedVectorTest, algo_copy)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x;
	std::copy(a.begin(), a.end(), std::back_inserter(x));
	LONGS_EQUAL(SIZE, x.size());
	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x.at(i));
	}
}

TEST(FixedVectorTest, algo_copy_exception)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(1);
	try {
		std::copy(a.begin(), a.end(), std::back_inserter(x));
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("Container::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}
#endif

#ifndef NO_STD_ITERATOR
TEST(FixedVectorTest, rbegin_rend)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	size_t i = SIZE - 1;
	for (FixedVector<int, SIZE>::reverse_iterator it = x.rbegin(); it != x.rend(); ++it, --i) {
		LONGS_EQUAL(a[i], *it);
	}
}

TEST(FixedVectorTest, rbegin_rend_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedVector<int, SIZE> x(a.begin(), a.end());
	size_t i = SIZE - 1;
	for (FixedVector<int, SIZE>::const_reverse_iterator it = x.rbegin(); it != x.rend(); ++it, --i) {
		LONGS_EQUAL(a[i], *it);
	}
}
#endif

