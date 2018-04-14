#include "Container/FixedVector.h"
#include "Container/Array.h"
#include "Container/FixedDeque.h"
#ifndef NO_STD_CONTAINER
#include <deque>
#include <list>
#endif
#ifndef NO_STD_ALGORITHM
#include <algorithm>
#include <functional>
#endif
#include "CppUTest/TestHarness.h"

using Container::FixedVector;
using Container::Array;
using Container::FixedDeque;

TEST_GROUP(FixedVectorTest) {
	static const std::size_t SIZE = 10;
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
		STRCMP_EQUAL("FixedVector::BadAlloc", e.what());
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
		STRCMP_EQUAL("FixedVector::BadAlloc", e.what());
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

TEST(FixedVectorTest, operator_assign_self)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	x = x;
	LONGS_EQUAL(SIZE, x.size());
	for (size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(a[i], x[i]);
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
		STRCMP_EQUAL("FixedVector::BadAlloc", e.what());
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
		STRCMP_EQUAL("FixedVector::BadAlloc", e.what());
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
		STRCMP_EQUAL("FixedVector::BadAlloc", e.what());
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

TEST(FixedVectorTest, assign_range_FixedDeque_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedDeque<int, SIZE> b(a.begin(), a.end());
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
		STRCMP_EQUAL("FixedVector::BadAlloc", e.what());
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
		STRCMP_EQUAL("FixedVector::BadAlloc", e.what());
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
		STRCMP_EQUAL("FixedVector::BadAlloc", e.what());
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

	x.insert(x.begin() + 3, a.end() - 2, a.end());
	LONGS_EQUAL(7, x.size());
	LONGS_EQUAL(3, x[0]);
	LONGS_EQUAL(4, x[1]);
	LONGS_EQUAL(0, x[2]);
	LONGS_EQUAL(8, x[3]);
	LONGS_EQUAL(9, x[4]);
	LONGS_EQUAL(1, x[5]);
	LONGS_EQUAL(2, x[6]);
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

TEST(FixedVectorTest, insert_range_FixedDeque_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedDeque<int, SIZE> b(a.begin(), a.end());
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
		STRCMP_EQUAL("FixedVector::BadAlloc", e.what());
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

TEST(FixedVectorTest, operator_equal_true)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE> y(a.begin(), a.end());
	CHECK_TRUE(x == y);
}

TEST(FixedVectorTest, operator_equal_true_after_pop_back)
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

TEST(FixedVectorTest, iterator_copy_ctor)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE>::iterator it = x.begin();
	FixedVector<int, SIZE>::iterator it2(it);
	CHECK_EQUAL(it, it2);
	FixedVector<int, SIZE>::iterator it3;
	it3 = it;
	CHECK_EQUAL(it, it3);

	const FixedVector<int, SIZE>& y = x;
	FixedVector<int, SIZE>::const_iterator cit = y.begin();
	FixedVector<int, SIZE>::const_iterator cit2(it);
	CHECK_EQUAL(cit, cit2);
	FixedVector<int, SIZE>::const_iterator cit3;
	cit3 = it;
	CHECK_EQUAL(cit, cit3);
}

TEST(FixedVectorTest, iterator_operator_plusequal)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE>::iterator it = x.begin();
	FixedVector<int, SIZE>::iterator i = it += 5;
	LONGS_EQUAL(5, *it);
	CHECK_EQUAL(x.begin() + 5, it);
	CHECK_EQUAL(it, i);
}

TEST(FixedVectorTest, iterator_operator_plusequal_0)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE>::iterator it = x.begin();
	it += 0;
	LONGS_EQUAL(0, *it);
	CHECK_EQUAL(x.begin(), it);
}

TEST(FixedVectorTest, iterator_operator_plusequal_max)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE>::iterator it = x.begin();
	it += SIZE;
	CHECK_EQUAL(x.end(), it);
}

TEST(FixedVectorTest, iterator_operator_plusequal_minus)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE>::iterator it = x.end();
	FixedVector<int, SIZE>::iterator i = it += -1;
	LONGS_EQUAL(9, *it);
	CHECK_EQUAL(x.begin() + 9, it);
	CHECK_EQUAL(it, i);
}

TEST(FixedVectorTest, iterator_operator_plusequal_minus_max)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE>::iterator it = x.end();
	it += -static_cast<std::ptrdiff_t>(SIZE);
	LONGS_EQUAL(0, *it);
	CHECK_EQUAL(x.begin(), it);
}

TEST(FixedVectorTest, iterator_operator_plus)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE>::iterator it = x.begin() + 5;
	LONGS_EQUAL(5, *it);
}

TEST(FixedVectorTest, iterator_operator_plus_nonmember)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE>::iterator it = 5 + x.begin();
	LONGS_EQUAL(5, *it);
	LONGS_EQUAL(1, *(-4 + it));
}

TEST(FixedVectorTest, iterator_operator_incremant)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE>::iterator it = x.begin();
	FixedVector<int, SIZE>::iterator i = ++it;
	LONGS_EQUAL(1, *it);
	LONGS_EQUAL(1, *i);

	FixedVector<int, SIZE>::iterator j = it++;
	LONGS_EQUAL(2, *it);
	LONGS_EQUAL(1, *j);
}

TEST(FixedVectorTest, iterator_operator_minusqual)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE>::iterator it = x.end();
	FixedVector<int, SIZE>::iterator i = it -= 5;
	LONGS_EQUAL(5, *it);
	CHECK_EQUAL(x.end() - 5, it);
	CHECK_EQUAL(it, i);
}

TEST(FixedVectorTest, iterator_operator_minusequal_0)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE>::iterator it = x.begin();
	it -= 0;
	LONGS_EQUAL(0, *it);
	CHECK_EQUAL(x.begin(), it);
}

TEST(FixedVectorTest, iterator_operator_minusqual_max)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE>::iterator it = x.end();
	it -= SIZE;
	LONGS_EQUAL(0, *it);
	CHECK_EQUAL(x.begin(), it);
}

TEST(FixedVectorTest, iterator_operator_minusequal_minus)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE>::iterator it = x.begin();
	FixedVector<int, SIZE>::iterator i = it -= -1;
	LONGS_EQUAL(1, *it);
	CHECK_EQUAL(x.begin() + 1, it);
	CHECK_EQUAL(it, i);
}

TEST(FixedVectorTest, iterator_operator_minusequal_minus_max)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE>::iterator it = x.begin();
	it -= -static_cast<std::ptrdiff_t>(SIZE);
	CHECK_EQUAL(x.end(), it);
}

TEST(FixedVectorTest, iterator_operator_minus)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE>::iterator it = x.end() - 5;
	LONGS_EQUAL(5, *it);
}

TEST(FixedVectorTest, iterator_operator_decremant)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE>::iterator it = x.end();
	FixedVector<int, SIZE>::iterator i = --it;
	LONGS_EQUAL(9, *it);
	LONGS_EQUAL(9, *i);

	FixedVector<int, SIZE>::iterator j = it--;
	LONGS_EQUAL(8, *it);
	LONGS_EQUAL(9, *j);
}

struct A {
	int a;
	const char* b;
};

TEST(FixedVectorTest, iterator_operator_arrow)
{
	A a = {1, "foo"};
	FixedVector<A, SIZE> x(1, a);
	FixedVector<A, SIZE>::iterator it = x.begin();
	LONGS_EQUAL(1, it->a);
	STRCMP_EQUAL("foo", it->b);

	it->a = 10;
	LONGS_EQUAL(10, it->a);

	FixedVector<A, SIZE>::const_iterator cit(it);
	LONGS_EQUAL(10, cit->a);
	STRCMP_EQUAL("foo", cit->b);
}

TEST(FixedVectorTest, iterator_operator_asterisk)
{
	A a = {1, "foo"};
	FixedVector<A, SIZE> x(1, a);
	FixedVector<A, SIZE>::iterator it = x.begin();
	LONGS_EQUAL(1, (*it).a);
	STRCMP_EQUAL("foo", (*it).b);

	(*it).a = 10;
	LONGS_EQUAL(10, (*it).a);

	FixedVector<A, SIZE>::const_iterator cit(it);
	LONGS_EQUAL(10, (*cit).a);
	STRCMP_EQUAL("foo", (*cit).b);
}

TEST(FixedVectorTest, iterator_operator_bracket_read)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	FixedVector<int, SIZE> x(a.begin(), a.end());
	FixedVector<int, SIZE>::iterator it = x.begin();
	LONGS_EQUAL(0, it[0]);
	LONGS_EQUAL(5, it[5]);
	LONGS_EQUAL(9, it[9]);
}

TEST(FixedVectorTest, iterator_operator_bracket_write)
{
	FixedVector<int, SIZE> x(10);
	FixedVector<int, SIZE>::iterator it = x.begin();
	it[0] = 100;
	LONGS_EQUAL(100, it[0]);

	it[9] = 10;
	LONGS_EQUAL(10, it[9]);
}

TEST(FixedVectorTest, iterator_operator_equal_true)
{
	FixedVector<int, SIZE> x(SIZE);
	FixedVector<int, SIZE>::iterator it = x.begin();
	FixedVector<int, SIZE>::iterator it2 = x.begin();
	FixedVector<int, SIZE>::const_iterator cit(it);
	CHECK_TRUE(it == it2);
	CHECK_TRUE(cit == it);
	CHECK_TRUE(it == cit);

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

TEST(FixedVectorTest, iterator_operator_equal_false)
{
	FixedVector<int, SIZE> x(SIZE);
	FixedVector<int, SIZE>::iterator it = x.begin();
	FixedVector<int, SIZE>::iterator it2 = x.begin() + 1;
	CHECK_FALSE(it == it2);
}

TEST(FixedVectorTest, iterator_operator_notequal_true)
{
	FixedVector<int, SIZE> x(SIZE);
	FixedVector<int, SIZE>::iterator it = x.begin();
	FixedVector<int, SIZE>::iterator it2 = x.begin() + 1;
	CHECK_TRUE(it != it2);

	it = x.begin();
	it2 = x.end();
	CHECK_TRUE(it != it2);
}

TEST(FixedVectorTest, iterator_operator_notequal_false)
{
	FixedVector<int, SIZE> x(SIZE);
	FixedVector<int, SIZE>::iterator it = x.begin();
	FixedVector<int, SIZE>::iterator it2 = x.begin();
	CHECK_FALSE(it != it2);

	it = x.end();
	it2 = x.end();
	CHECK_FALSE(it != it2);
}

TEST(FixedVectorTest, iterator_operator_less_true)
{
	FixedVector<int, SIZE> x(SIZE);
	FixedVector<int, SIZE>::iterator it = x.begin();
	FixedVector<int, SIZE>::iterator it2 = x.end();
	CHECK_TRUE(it < it2);
}

TEST(FixedVectorTest, iterator_operator_less_false)
{
	FixedVector<int, SIZE> x(SIZE);
	FixedVector<int, SIZE>::iterator it = x.begin();
	FixedVector<int, SIZE>::iterator it2 = x.begin();
	CHECK_FALSE(it < it2);

	it = x.end();
	it2 = x.end();
	CHECK_FALSE(it < it2);

	it = x.end();
	it2 = x.begin();
	CHECK_FALSE(it < it2);
}

TEST(FixedVectorTest, iterator_operator_greater_true)
{
	FixedVector<int, SIZE> x(SIZE);
	FixedVector<int, SIZE>::iterator it = x.end();
	FixedVector<int, SIZE>::iterator it2 = x.begin();
	CHECK_TRUE(it > it2);
}

TEST(FixedVectorTest, iterator_operator_greater_false)
{
	FixedVector<int, SIZE> x(SIZE);
	FixedVector<int, SIZE>::iterator it = x.begin();
	FixedVector<int, SIZE>::iterator it2 = x.begin();
	CHECK_FALSE(it > it2);

	it = x.end();
	it2 = x.end();
	CHECK_FALSE(it > it2);

	it = x.begin();
	it2 = x.end();
	CHECK_FALSE(it > it2);
}

TEST(FixedVectorTest, iterator_operator_lessequal_true)
{
	FixedVector<int, SIZE> x(SIZE);
	FixedVector<int, SIZE>::iterator it = x.begin();
	FixedVector<int, SIZE>::iterator it2 = x.end();
	CHECK_TRUE(it <= it2);

	it = x.begin();
	it2 = x.begin();
	CHECK_TRUE(it <= it2);

	it = x.end();
	it2 = x.end();
	CHECK_TRUE(it <= it2);
}

TEST(FixedVectorTest, iterator_operator_lessequal_false)
{
	FixedVector<int, SIZE> x(SIZE);
	FixedVector<int, SIZE>::iterator it = x.end();
	FixedVector<int, SIZE>::iterator it2 = x.begin();
	CHECK_FALSE(it <= it2);
}

TEST(FixedVectorTest, iterator_operator_greaterequal_true)
{
	FixedVector<int, SIZE> x(SIZE);
	FixedVector<int, SIZE>::iterator it = x.end();
	FixedVector<int, SIZE>::iterator it2 = x.begin();
	CHECK_TRUE(it >= it2);

	it = x.begin();
	it2 = x.begin();
	CHECK_TRUE(it >= it2);

	it = x.end();
	it2 = x.end();
	CHECK_TRUE(it >= it2);
}

TEST(FixedVectorTest, iterator_operator_greaterequal_false)
{
	FixedVector<int, SIZE> x(SIZE);
	FixedVector<int, SIZE>::iterator it = x.begin();
	FixedVector<int, SIZE>::iterator it2 = x.end();
	CHECK_FALSE(it >= it2);
}

TEST(FixedVectorTest, iterator_difference)
{
	FixedVector<int, SIZE> x(SIZE);
	FixedVector<int, SIZE>::iterator it = x.begin();
	FixedVector<int, SIZE>::iterator it2 = x.end();
	LONGS_EQUAL(SIZE, it2 - it);
	LONGS_EQUAL(-static_cast<std::ptrdiff_t>(SIZE), it - it2);
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
		STRCMP_EQUAL("FixedVector::BadAlloc", e.what());
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


class VElem {
	int count;
public:
	unsigned int data;
	static const unsigned int EXCEPTION_DATA = 0;

	VElem() : count(1), data(100) {}

	class Exception {};

	VElem(const VElem& x) : count(x.count), data(x.data)
	{
		if (x.data == EXCEPTION_DATA) {
			throw Exception();
		}
	}
	VElem& operator=(const VElem&)
	{
		return *this;
	}
	~VElem()
	{
		data = 0xDEADBEEF;
		--count;
		LONGS_EQUAL(0, count);
	}

	static void checkElemsDestroyed(const VElem* array, std::size_t num, std::size_t maxUsed)
	{
		for (std::size_t i = 0; i < num; ++i) {
			if (i < maxUsed) {
				LONGS_EQUAL_TEXT(0xDEADBEEF, array[i].data,
						StringFromFormat("%ld", i).asCharString());
			} else {
				CHECK_TRUE_TEXT(0xDEADBEEF != array[i].data,
						StringFromFormat("%ld", i).asCharString());
			}
		}
	}
};

TEST(FixedVectorTest, default_ctor_VElem)
{
	FixedVector<VElem, SIZE> x;
	VElem::checkElemsDestroyed(&x[0], SIZE, 0);
}

TEST(FixedVectorTest, ctor_VElem)
{
	VElem a;

	FixedVector<VElem, SIZE> x(2, a);
	LONGS_EQUAL(2, x.size());

	FixedVector<VElem, SIZE> y(x.begin(), x.end());
	LONGS_EQUAL(2, y.size());

	FixedVector<VElem, SIZE> z(x);
	LONGS_EQUAL(2, z.size());

	x.clear();
	VElem::checkElemsDestroyed(&x[0], SIZE, 2);

	a.data = VElem::EXCEPTION_DATA;
	try {
		FixedVector<VElem, SIZE> xx(1, a);
	}
	catch (const VElem::Exception&) {
		return;
	}
	FAIL("failed");
}

TEST(FixedVectorTest, operator_assign_VElem)
{
	VElem a;

	FixedVector<VElem, SIZE> x(2, a);

	FixedVector<VElem, SIZE> y(1, a);

	y = x;
	LONGS_EQUAL(2, y.size());

	FixedVector<VElem, SIZE> z(3, a);

	z = x;
	LONGS_EQUAL(2, z.size());

	z.clear();
	VElem::checkElemsDestroyed(&z[0], SIZE, 3);
}

TEST(FixedVectorTest, push_back_pop_back_VElem)
{
	VElem a;

	FixedVector<VElem, SIZE> x;
	x.push_back(a);
	x.push_back(a);
	x.pop_back();
	LONGS_EQUAL(1, x.size());
	x.clear();
	VElem::checkElemsDestroyed(&x[0], SIZE, 2);

	FixedVector<VElem, SIZE> y;
	y.push_back(a);
	a.data = VElem::EXCEPTION_DATA;
	try {
		y.push_back(a);
	}
	catch (const VElem::Exception&) {
		LONGS_EQUAL(1, y.size());
		y.clear();
		VElem::checkElemsDestroyed(&y[0], SIZE, 1);
		return;
	}
	FAIL("failed");
}

TEST(FixedVectorTest, insert_n_VElem)
{
	VElem a;

	FixedVector<VElem, SIZE> x;
	x.insert(x.end(), 2, a);
	x.insert(x.begin(), 1, a);
	x.insert(x.end() - 1, 2, a);
	LONGS_EQUAL(5, x.size());

	a.data = VElem::EXCEPTION_DATA;
	try {
		x.insert(x.end(), 1, a);
	}
	catch (const VElem::Exception&) {
		LONGS_EQUAL(5, x.size());
		x.clear();
		VElem::checkElemsDestroyed(&x[0], SIZE, 5);
		return;
	}
	FAIL("failed");
}

TEST(FixedVectorTest, insert_range_VElem)
{
	VElem a;

	FixedVector<VElem, SIZE> x(2, a);
	FixedVector<VElem, SIZE> y;
	y.insert(y.end(), x.begin(), x.end());
	y.insert(y.begin(), x.begin(), x.begin() + 1);
	y.insert(y.end() - 1, x.begin(), x.begin() + 2);
	LONGS_EQUAL(5, y.size());

	x[0].data = VElem::EXCEPTION_DATA;
	try {
		y.insert(y.end(), x.begin(), x.begin() + 1);
	}
	catch (const VElem::Exception&) {
		LONGS_EQUAL(5, y.size());
		y.clear();
		VElem::checkElemsDestroyed(&y[0], SIZE, 5);
		return;
	}
	FAIL("failed");
}

TEST(FixedVectorTest, erase_range_VElem)
{
	VElem a;

	FixedVector<VElem, SIZE> x(4, a);

	x.erase(x.begin() + 1, x.begin() + 2);

	x.erase(x.begin(), x.end());

	VElem::checkElemsDestroyed(&x[0], SIZE, 4);
}

#if (__cplusplus >= 201103L) || defined(_WIN32)
#include <memory>

TEST(FixedVectorTest, shared_ptr_int)
{
	FixedVector<std::shared_ptr<int>, SIZE> x;
	x.push_back(std::make_shared<int>(1));
	LONGS_EQUAL(1, *x[0]);
	*x[0] = 2;
	LONGS_EQUAL(2, *x[0]);
}

TEST(FixedVectorTest, shared_ptr_VElem)
{
	VElem a;

	{
		FixedVector<std::shared_ptr<VElem>, SIZE> x;

		x.push_back(std::make_shared<VElem>(a));
		x.push_back(std::make_shared<VElem>(a));

		x.pop_back();

	}

}
#endif
