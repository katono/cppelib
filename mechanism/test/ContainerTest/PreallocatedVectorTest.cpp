#include "Container/PreallocatedVector.h"
#include "Container/Array.h"
#include "Container/FixedVector.h"
#include "Container/FixedDeque.h"
#ifndef NO_STD_CONTAINER
#include <deque>
#include <list>
#endif
#ifndef NO_STD_ALGORITHM
#include <algorithm>
#include <functional>
#endif
#include <cstdlib>
#include "CppUTest/TestHarness.h"

namespace PreallocatedVectorTest {

using Container::PreallocatedVector;
using Container::Array;
using Container::FixedVector;
using Container::FixedDeque;

TEST_GROUP(PreallocatedVectorTest) {
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
};

TEST(PreallocatedVectorTest, init_only_one_time)
{
	PreallocatedVector<int> x;
	x.init(alloc_buf, ALLOC_SIZE);
	x.resize(1);
	LONGS_EQUAL(1, x.size());
	POINTERS_EQUAL(alloc_buf, &x[0]);

	int b[100];
	x.init(b, sizeof b); // do nothing
	LONGS_EQUAL(1, x.size());
	POINTERS_EQUAL(alloc_buf, &x[0]);
}

TEST(PreallocatedVectorTest, size_0)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	LONGS_EQUAL(0, x.size());
}

TEST(PreallocatedVectorTest, size)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(5);
	LONGS_EQUAL(5, x.size());
}

TEST(PreallocatedVectorTest, max_size)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	LONGS_EQUAL(ALLOC_SIZE / sizeof(int), x.max_size());
}

TEST(PreallocatedVectorTest, available_size)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(5);
	LONGS_EQUAL(x.max_size() - 5, x.available_size());
}

TEST(PreallocatedVectorTest, empty_true)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	CHECK(x.empty());
}

TEST(PreallocatedVectorTest, empty_false)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(1);
	CHECK_FALSE(x.empty());
}

TEST(PreallocatedVectorTest, full_true)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	CHECK(x.full());
}

TEST(PreallocatedVectorTest, full_false)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size() - 1);
	CHECK_FALSE(x.full());
}

TEST(PreallocatedVectorTest, clear)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	x.clear();
	CHECK(x.empty());
}

TEST(PreallocatedVectorTest, operator_bracket_read)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	LONGS_EQUAL(0, x[0]);
	LONGS_EQUAL(5, x[5]);
	LONGS_EQUAL(9, x[9]);
}

TEST(PreallocatedVectorTest, operator_bracket_read_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> z(alloc_buf, ALLOC_SIZE);
	z.assign(a.begin(), a.end());
	const PreallocatedVector<int>& x = z;
	LONGS_EQUAL(0, x[0]);
	LONGS_EQUAL(5, x[5]);
	LONGS_EQUAL(9, x[9]);
}

TEST(PreallocatedVectorTest, operator_bracket_write)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(10);
	x[0] = 100;
	LONGS_EQUAL(100, x[0]);

	x[9] = 10;
	LONGS_EQUAL(10, x[9]);
}

TEST(PreallocatedVectorTest, at_read)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	LONGS_EQUAL(0, x.at(0));
	LONGS_EQUAL(5, x.at(5));
	LONGS_EQUAL(9, x.at(9));
}

TEST(PreallocatedVectorTest, at_read_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> z(alloc_buf, ALLOC_SIZE);
	z.assign(a.begin(), a.end());
	const PreallocatedVector<int>& x = z;
	LONGS_EQUAL(0, x.at(0));
	LONGS_EQUAL(5, x.at(5));
	LONGS_EQUAL(9, x.at(9));
}

TEST(PreallocatedVectorTest, at_write)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(10);
	x.at(0) = 100;
	LONGS_EQUAL(100, x.at(0));

	x.at(9) = 10;
	LONGS_EQUAL(10, x.at(9));
}

TEST(PreallocatedVectorTest, at_exception)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	try {
		x.at(0);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("PreallocatedVector::at", e.what());
		return;
	}
	FAIL("failed");
}

TEST(PreallocatedVectorTest, at_exception_const)
{
	const PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	try {
		x.at(0);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("PreallocatedVector::at", e.what());
		return;
	}
	FAIL("failed");
}

TEST(PreallocatedVectorTest, data_read)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	LONGS_EQUAL(0, x.data()[0]);
	LONGS_EQUAL(5, x.data()[5]);
	LONGS_EQUAL(9, x.data()[9]);
}

TEST(PreallocatedVectorTest, data_read_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> z(alloc_buf, ALLOC_SIZE);
	z.assign(a.begin(), a.end());
	const PreallocatedVector<int>& x = z;
	LONGS_EQUAL(0, x.data()[0]);
	LONGS_EQUAL(5, x.data()[5]);
	LONGS_EQUAL(9, x.data()[9]);
}

TEST(PreallocatedVectorTest, data_write)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	int* p = x.data();
	p[5] = 100;
	LONGS_EQUAL(100, x.data()[5]);
}

TEST(PreallocatedVectorTest, begin_end)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	std::size_t i = 0;
	for (PreallocatedVector<int>::iterator it = x.begin(); it != x.end(); ++it, ++i) {
		LONGS_EQUAL(a[i], *it);
	}
}

TEST(PreallocatedVectorTest, begin_end_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> z(alloc_buf, ALLOC_SIZE);
	z.assign(a.begin(), a.end());
	const PreallocatedVector<int>& x = z;
	std::size_t i = 0;
	for (PreallocatedVector<int>::const_iterator it = x.begin(); it != x.end(); ++it, ++i) {
		LONGS_EQUAL(a[i], *it);
	}
}

TEST(PreallocatedVectorTest, front_read)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(1);
	LONGS_EQUAL(0, x.front());
}

TEST(PreallocatedVectorTest, front_read_const)
{
	PreallocatedVector<int> z(alloc_buf, ALLOC_SIZE);
	z.resize(1);
	const PreallocatedVector<int>& x = z;
	LONGS_EQUAL(0, x.front());
}

TEST(PreallocatedVectorTest, front_write)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(1);
	x.front() = 100;
	LONGS_EQUAL(100, x.front());
}

TEST(PreallocatedVectorTest, back_read)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	LONGS_EQUAL(9, x.back());
}

TEST(PreallocatedVectorTest, back_read_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> z(alloc_buf, ALLOC_SIZE);
	z.assign(a.begin(), a.end());
	const PreallocatedVector<int>& x = z;
	LONGS_EQUAL(9, x.back());
}

TEST(PreallocatedVectorTest, back_write)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	x.back() = 100;
	LONGS_EQUAL(100, x.back());
}

TEST(PreallocatedVectorTest, operator_assign)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	int buf[SIZE];
	PreallocatedVector<int> b(buf, sizeof buf);
	b.assign(a.begin(), a.end());
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x = b;
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(b[i], x[i]);
	}
}

TEST(PreallocatedVectorTest, operator_assign_self)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	x = x;
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(a[i], x[i]);
	}
}

TEST(PreallocatedVectorTest, resize_shorten)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(5);
	x.resize(3);
	LONGS_EQUAL(3, x.size());
}

TEST(PreallocatedVectorTest, resize_make_longer)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
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

TEST(PreallocatedVectorTest, resize_exception)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	try {
		x.resize(x.max_size() + 1);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("PreallocatedVector::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(PreallocatedVectorTest, push_back)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
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

TEST(PreallocatedVectorTest, push_back_exception)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size() - 1);
	x.push_back(100);
	LONGS_EQUAL(x.max_size(), x.size());

	try {
		x.push_back(101);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("PreallocatedVector::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(PreallocatedVectorTest, pop_back)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(3);
	x.pop_back();
	LONGS_EQUAL(2, x.size());
	x.pop_back();
	LONGS_EQUAL(1, x.size());
	x.pop_back();
	LONGS_EQUAL(0, x.size());
}

TEST(PreallocatedVectorTest, assign_n)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(x.max_size(), 100);
	LONGS_EQUAL(x.max_size(), x.size());
	for (PreallocatedVector<int>::iterator it = x.begin(); it != x.end(); ++it) {
		LONGS_EQUAL(100, *it);
	}
}

TEST(PreallocatedVectorTest, assign_n_exception)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	try {
		x.assign(x.max_size() + 1, 100);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("PreallocatedVector::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(PreallocatedVectorTest, assign_range)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(PreallocatedVectorTest, assign_range_c_array)
{
	const int a[SIZE] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(&a[0], &a[SIZE]);
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(PreallocatedVectorTest, assign_range_PreallocatedVector_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	int buf[SIZE];
	PreallocatedVector<int> b(buf, sizeof buf);
	b.assign(a.begin(), a.end());
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(PreallocatedVectorTest, assign_range_FixedVector_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedVector<int, SIZE> b(a.begin(), a.end());
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(PreallocatedVectorTest, assign_range_FixedDeque_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedDeque<int, SIZE> b(a.begin(), a.end());
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

#ifndef NO_STD_CONTAINER
TEST(PreallocatedVectorTest, assign_range_deque_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const std::deque<int> b(a.begin(), a.end());
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(PreallocatedVectorTest, assign_range_list_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const std::list<int> b(a.begin(), a.end());
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}
#endif

TEST(PreallocatedVectorTest, assign_range_exception)
{
	Array<int, ALLOC_SIZE / sizeof(int) + 1> a = {0};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	try {
		x.assign(a.begin(), a.end());
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("PreallocatedVector::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(PreallocatedVectorTest, insert)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	PreallocatedVector<int>::iterator it;
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

TEST(PreallocatedVectorTest, insert_exception)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	try {
		x.insert(x.begin(), 100);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("PreallocatedVector::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(PreallocatedVectorTest, insert_n)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
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

TEST(PreallocatedVectorTest, insert_n_dispatch)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
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

TEST(PreallocatedVectorTest, insert_n_exception)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	try {
		x.insert(x.begin(), x.max_size() + 1, 100);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("PreallocatedVector::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(PreallocatedVectorTest, insert_range)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
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

TEST(PreallocatedVectorTest, insert_range_c_array)
{
	const int a[SIZE] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.insert(x.end(), &a[0], &a[SIZE]);
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(PreallocatedVectorTest, insert_range_PreallocatedVector_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	int buf[SIZE];
	PreallocatedVector<int> b(buf, sizeof buf);
	b.assign(a.begin(), a.end());
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.insert(x.end(), b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(PreallocatedVectorTest, insert_range_FixedVector_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedVector<int, SIZE> b(a.begin(), a.end());
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.insert(x.end(), b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(PreallocatedVectorTest, insert_range_FixedDeque_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const FixedDeque<int, SIZE> b(a.begin(), a.end());
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.insert(x.end(), b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

#ifndef NO_STD_CONTAINER
TEST(PreallocatedVectorTest, insert_range_deque_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const std::deque<int> b(a.begin(), a.end());
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.insert(x.end(), b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}

TEST(PreallocatedVectorTest, insert_range_list_iter)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const std::list<int> b(a.begin(), a.end());
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.insert(x.end(), b.begin(), b.end());
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x[i]);
	}
}
#endif

TEST(PreallocatedVectorTest, insert_range_exception)
{
	Array<int, ALLOC_SIZE / sizeof(int) + 1> a = {0};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	try {
		x.insert(x.end(), a.begin(), a.end());
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("PreallocatedVector::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}

TEST(PreallocatedVectorTest, erase)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedVector<int>::iterator it;
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

TEST(PreallocatedVectorTest, erase_range)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedVector<int>::iterator it;
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

TEST(PreallocatedVectorTest, erase_range_all)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedVector<int>::iterator it;
	it = x.erase(x.begin(), x.end());
	LONGS_EQUAL(0, x.size());
	CHECK_EQUAL(x.end(), it);
}

TEST(PreallocatedVectorTest, operator_equal_true)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());

	int buf[ALLOC_SIZE / sizeof(int)];
	PreallocatedVector<int> y(buf, sizeof buf);
	y.assign(a.begin(), a.end());

	LONGS_EQUAL(x.max_size(), y.max_size());
	CHECK_TRUE(x == y);
}

TEST(PreallocatedVectorTest, operator_equal_true_after_pop_back)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());

	int buf[ALLOC_SIZE / sizeof(int)];
	PreallocatedVector<int> y(buf, sizeof buf);
	y.assign(a.begin(), a.end());

	x.back() = 100;
	y.back() = 101;
	x.pop_back();
	y.pop_back();
	CHECK_TRUE(x == y);
}

TEST(PreallocatedVectorTest, operator_equal_false)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const Array<int, SIZE> b = {0, 1, 2, 3, 4, 5, 6, 7, 8, 10};

	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());

	int buf[ALLOC_SIZE / sizeof(int)];
	PreallocatedVector<int> y(buf, sizeof buf);
	y.assign(b.begin(), b.end());

	CHECK_FALSE(x == y);
}

TEST(PreallocatedVectorTest, operator_equal_false_diff_size)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());

	int buf[ALLOC_SIZE / sizeof(int)];
	PreallocatedVector<int> y(buf, sizeof buf);
	y.assign(a.begin(), a.end() - 1);

	CHECK_FALSE(x == y);
}

TEST(PreallocatedVectorTest, operator_equal_false_diff_max_size)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());

	int buf[ALLOC_SIZE / sizeof(int) - 1];
	PreallocatedVector<int> y(buf, sizeof buf);
	y.assign(a.begin(), a.end());

	CHECK_FALSE(x == y);
}

TEST(PreallocatedVectorTest, operator_notequal_true)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	const Array<int, SIZE> b = {0, 1, 2, 3, 4, 5, 6, 7, 8, 10};

	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());

	int buf[ALLOC_SIZE / sizeof(int)];
	PreallocatedVector<int> y(buf, sizeof buf);
	y.assign(b.begin(), b.end());

	CHECK_TRUE(x != y);
}

TEST(PreallocatedVectorTest, operator_notequal_false)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());

	int buf[ALLOC_SIZE / sizeof(int)];
	PreallocatedVector<int> y(buf, sizeof buf);
	y.assign(a.begin(), a.end());

	CHECK_FALSE(x != y);
}

TEST(PreallocatedVectorTest, iterator_copy_ctor)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedVector<int>::iterator it = x.begin();
	PreallocatedVector<int>::iterator it2(it);
	CHECK_EQUAL(it, it2);
	PreallocatedVector<int>::iterator it3;
	it3 = it;
	CHECK_EQUAL(it, it3);

	const PreallocatedVector<int>& y = x;
	PreallocatedVector<int>::const_iterator cit = y.begin();
	PreallocatedVector<int>::const_iterator cit2(it);
	CHECK_EQUAL(cit, cit2);
	PreallocatedVector<int>::const_iterator cit3;
	cit3 = it;
	CHECK_EQUAL(cit, cit3);
}

TEST(PreallocatedVectorTest, iterator_operator_plusequal)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedVector<int>::iterator it = x.begin();
	PreallocatedVector<int>::iterator i = it += 5;
	LONGS_EQUAL(5, *it);
	CHECK_EQUAL(x.begin() + 5, it);
	CHECK_EQUAL(it, i);
}

TEST(PreallocatedVectorTest, iterator_operator_plusequal_0)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedVector<int>::iterator it = x.begin();
	it += 0;
	LONGS_EQUAL(0, *it);
	CHECK_EQUAL(x.begin(), it);
}

TEST(PreallocatedVectorTest, iterator_operator_plusequal_max)
{
	Array<int, ALLOC_SIZE / sizeof(int)> a = {0};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedVector<int>::iterator it = x.begin();
	it += x.max_size();
	CHECK_EQUAL(x.end(), it);
}

TEST(PreallocatedVectorTest, iterator_operator_plusequal_minus)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedVector<int>::iterator it = x.end();
	PreallocatedVector<int>::iterator i = it += -1;
	LONGS_EQUAL(9, *it);
	CHECK_EQUAL(x.begin() + 9, it);
	CHECK_EQUAL(it, i);
}

TEST(PreallocatedVectorTest, iterator_operator_plusequal_minus_max)
{
	Array<int, ALLOC_SIZE / sizeof(int)> a = {0};
	for (std::size_t i = 0; i < a.size(); ++i) {
		a[i] = int(i);
	}
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedVector<int>::iterator it = x.end();
	it += -static_cast<std::ptrdiff_t>(x.max_size());
	LONGS_EQUAL(0, *it);
	CHECK_EQUAL(x.begin(), it);
}

TEST(PreallocatedVectorTest, iterator_operator_plus)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedVector<int>::iterator it = x.begin() + 5;
	LONGS_EQUAL(5, *it);
}

TEST(PreallocatedVectorTest, iterator_operator_plus_nonmember)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedVector<int>::iterator it = 5 + x.begin();
	LONGS_EQUAL(5, *it);
	LONGS_EQUAL(1, *(-4 + it));
}

TEST(PreallocatedVectorTest, iterator_operator_incremant)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedVector<int>::iterator it = x.begin();
	PreallocatedVector<int>::iterator i = ++it;
	LONGS_EQUAL(1, *it);
	LONGS_EQUAL(1, *i);

	PreallocatedVector<int>::iterator j = it++;
	LONGS_EQUAL(2, *it);
	LONGS_EQUAL(1, *j);
}

TEST(PreallocatedVectorTest, iterator_operator_minusqual)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedVector<int>::iterator it = x.end();
	PreallocatedVector<int>::iterator i = it -= 5;
	LONGS_EQUAL(5, *it);
	CHECK_EQUAL(x.end() - 5, it);
	CHECK_EQUAL(it, i);
}

TEST(PreallocatedVectorTest, iterator_operator_minusequal_0)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedVector<int>::iterator it = x.begin();
	it -= 0;
	LONGS_EQUAL(0, *it);
	CHECK_EQUAL(x.begin(), it);
}

TEST(PreallocatedVectorTest, iterator_operator_minusqual_max)
{
	Array<int, ALLOC_SIZE / sizeof(int)> a = {0};
	for (std::size_t i = 0; i < a.size(); ++i) {
		a[i] = int(i);
	}
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedVector<int>::iterator it = x.end();
	it -= x.max_size();
	LONGS_EQUAL(0, *it);
	CHECK_EQUAL(x.begin(), it);
}

TEST(PreallocatedVectorTest, iterator_operator_minusequal_minus)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedVector<int>::iterator it = x.begin();
	PreallocatedVector<int>::iterator i = it -= -1;
	LONGS_EQUAL(1, *it);
	CHECK_EQUAL(x.begin() + 1, it);
	CHECK_EQUAL(it, i);
}

TEST(PreallocatedVectorTest, iterator_operator_minusequal_minus_max)
{
	Array<int, ALLOC_SIZE / sizeof(int)> a = {0};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedVector<int>::iterator it = x.begin();
	it -= -static_cast<std::ptrdiff_t>(x.max_size());
	CHECK_EQUAL(x.end(), it);
}

TEST(PreallocatedVectorTest, iterator_operator_minus)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedVector<int>::iterator it = x.end() - 5;
	LONGS_EQUAL(5, *it);
}

TEST(PreallocatedVectorTest, iterator_operator_decremant)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedVector<int>::iterator it = x.end();
	PreallocatedVector<int>::iterator i = --it;
	LONGS_EQUAL(9, *it);
	LONGS_EQUAL(9, *i);

	PreallocatedVector<int>::iterator j = it--;
	LONGS_EQUAL(8, *it);
	LONGS_EQUAL(9, *j);
}

struct A {
	int a;
	const char* b;
};

TEST(PreallocatedVectorTest, iterator_operator_arrow)
{
	A a = {1, "foo"};
	PreallocatedVector<A> x(alloc_buf, ALLOC_SIZE);
	x.resize(1, a);
	PreallocatedVector<A>::iterator it = x.begin();
	LONGS_EQUAL(1, it->a);
	STRCMP_EQUAL("foo", it->b);

	it->a = 10;
	LONGS_EQUAL(10, it->a);

	PreallocatedVector<A>::const_iterator cit(it);
	LONGS_EQUAL(10, cit->a);
	STRCMP_EQUAL("foo", cit->b);
}

TEST(PreallocatedVectorTest, iterator_operator_asterisk)
{
	A a = {1, "foo"};
	PreallocatedVector<A> x(alloc_buf, ALLOC_SIZE);
	x.resize(1, a);
	PreallocatedVector<A>::iterator it = x.begin();
	LONGS_EQUAL(1, (*it).a);
	STRCMP_EQUAL("foo", (*it).b);

	(*it).a = 10;
	LONGS_EQUAL(10, (*it).a);

	PreallocatedVector<A>::const_iterator cit(it);
	LONGS_EQUAL(10, (*cit).a);
	STRCMP_EQUAL("foo", (*cit).b);
}

TEST(PreallocatedVectorTest, iterator_operator_bracket_read)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	PreallocatedVector<int>::iterator it = x.begin();
	LONGS_EQUAL(0, it[0]);
	LONGS_EQUAL(5, it[5]);
	LONGS_EQUAL(9, it[9]);
}

TEST(PreallocatedVectorTest, iterator_operator_bracket_write)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(10);
	PreallocatedVector<int>::iterator it = x.begin();
	it[0] = 100;
	LONGS_EQUAL(100, it[0]);

	it[9] = 10;
	LONGS_EQUAL(10, it[9]);
}

TEST(PreallocatedVectorTest, iterator_operator_equal_true)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	PreallocatedVector<int>::iterator it = x.begin();
	PreallocatedVector<int>::iterator it2 = x.begin();
	PreallocatedVector<int>::const_iterator cit(it);
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

TEST(PreallocatedVectorTest, iterator_operator_equal_false)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	PreallocatedVector<int>::iterator it = x.begin();
	PreallocatedVector<int>::iterator it2 = x.begin() + 1;
	CHECK_FALSE(it == it2);
}

TEST(PreallocatedVectorTest, iterator_operator_notequal_true)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	PreallocatedVector<int>::iterator it = x.begin();
	PreallocatedVector<int>::iterator it2 = x.begin() + 1;
	CHECK_TRUE(it != it2);

	it = x.begin();
	it2 = x.end();
	CHECK_TRUE(it != it2);
}

TEST(PreallocatedVectorTest, iterator_operator_notequal_false)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	PreallocatedVector<int>::iterator it = x.begin();
	PreallocatedVector<int>::iterator it2 = x.begin();
	CHECK_FALSE(it != it2);

	it = x.end();
	it2 = x.end();
	CHECK_FALSE(it != it2);
}

TEST(PreallocatedVectorTest, iterator_operator_less_true)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	PreallocatedVector<int>::iterator it = x.begin();
	PreallocatedVector<int>::iterator it2 = x.end();
	CHECK_TRUE(it < it2);
}

TEST(PreallocatedVectorTest, iterator_operator_less_false)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	PreallocatedVector<int>::iterator it = x.begin();
	PreallocatedVector<int>::iterator it2 = x.begin();
	CHECK_FALSE(it < it2);

	it = x.end();
	it2 = x.end();
	CHECK_FALSE(it < it2);

	it = x.end();
	it2 = x.begin();
	CHECK_FALSE(it < it2);
}

TEST(PreallocatedVectorTest, iterator_operator_greater_true)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	PreallocatedVector<int>::iterator it = x.end();
	PreallocatedVector<int>::iterator it2 = x.begin();
	CHECK_TRUE(it > it2);
}

TEST(PreallocatedVectorTest, iterator_operator_greater_false)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	PreallocatedVector<int>::iterator it = x.begin();
	PreallocatedVector<int>::iterator it2 = x.begin();
	CHECK_FALSE(it > it2);

	it = x.end();
	it2 = x.end();
	CHECK_FALSE(it > it2);

	it = x.begin();
	it2 = x.end();
	CHECK_FALSE(it > it2);
}

TEST(PreallocatedVectorTest, iterator_operator_lessequal_true)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	PreallocatedVector<int>::iterator it = x.begin();
	PreallocatedVector<int>::iterator it2 = x.end();
	CHECK_TRUE(it <= it2);

	it = x.begin();
	it2 = x.begin();
	CHECK_TRUE(it <= it2);

	it = x.end();
	it2 = x.end();
	CHECK_TRUE(it <= it2);
}

TEST(PreallocatedVectorTest, iterator_operator_lessequal_false)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	PreallocatedVector<int>::iterator it = x.end();
	PreallocatedVector<int>::iterator it2 = x.begin();
	CHECK_FALSE(it <= it2);
}

TEST(PreallocatedVectorTest, iterator_operator_greaterequal_true)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	PreallocatedVector<int>::iterator it = x.end();
	PreallocatedVector<int>::iterator it2 = x.begin();
	CHECK_TRUE(it >= it2);

	it = x.begin();
	it2 = x.begin();
	CHECK_TRUE(it >= it2);

	it = x.end();
	it2 = x.end();
	CHECK_TRUE(it >= it2);
}

TEST(PreallocatedVectorTest, iterator_operator_greaterequal_false)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	PreallocatedVector<int>::iterator it = x.begin();
	PreallocatedVector<int>::iterator it2 = x.end();
	CHECK_FALSE(it >= it2);
}

TEST(PreallocatedVectorTest, iterator_difference)
{
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(x.max_size());
	PreallocatedVector<int>::iterator it = x.begin();
	PreallocatedVector<int>::iterator it2 = x.end();
	LONGS_EQUAL(x.max_size(), it2 - it);
	LONGS_EQUAL(-static_cast<std::ptrdiff_t>(x.max_size()), it - it2);
}

TEST(PreallocatedVectorTest, new_delete)
{
	PreallocatedVector<int>* x = new PreallocatedVector<int>(alloc_buf, ALLOC_SIZE);
	x->resize(SIZE);
	LONGS_EQUAL(SIZE, x->size());
	delete x;
}

#ifndef NO_STD_ALGORITHM
TEST(PreallocatedVectorTest, algo_sort)
{
	const Array<int, SIZE> a = {1, 7, 0, 2, 5, 3, 9, 4, 6, 8};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
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

TEST(PreallocatedVectorTest, algo_copy)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	std::copy(a.begin(), a.end(), std::back_inserter(x));
	LONGS_EQUAL(SIZE, x.size());
	for (std::size_t i = 0; i < x.size(); ++i) {
		LONGS_EQUAL(i, x.at(i));
	}
}

TEST(PreallocatedVectorTest, algo_copy_exception)
{
	Array<int, ALLOC_SIZE / sizeof(int)> a = {0};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.resize(1);
	try {
		std::copy(a.begin(), a.end(), std::back_inserter(x));
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("PreallocatedVector::BadAlloc", e.what());
		return;
	}
	FAIL("failed");
}
#endif

#ifndef NO_STD_ITERATOR
TEST(PreallocatedVectorTest, rbegin_rend)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> x(alloc_buf, ALLOC_SIZE);
	x.assign(a.begin(), a.end());
	std::size_t i = SIZE - 1;
	for (PreallocatedVector<int>::reverse_iterator it = x.rbegin(); it != x.rend(); ++it, --i) {
		LONGS_EQUAL(a[i], *it);
	}
}

TEST(PreallocatedVectorTest, rbegin_rend_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	PreallocatedVector<int> z(alloc_buf, ALLOC_SIZE);
	z.assign(a.begin(), a.end());
	const PreallocatedVector<int>& x = z;
	std::size_t i = SIZE - 1;
	for (PreallocatedVector<int>::const_reverse_iterator it = x.rbegin(); it != x.rend(); ++it, --i) {
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

TEST(PreallocatedVectorTest, default_ctor_VElem)
{
	PreallocatedVector<VElem> x(alloc_buf, ALLOC_SIZE);
	VElem::checkElemsDestroyed(&x[0], x.max_size(), 0);
}

TEST(PreallocatedVectorTest, ctor_VElem)
{
	VElem a;

	PreallocatedVector<VElem> x(alloc_buf, ALLOC_SIZE);
	x.resize(2, a);
	LONGS_EQUAL(2, x.size());

	x.clear();
	VElem::checkElemsDestroyed(&x[0], x.max_size(), 2);

	a.data = VElem::EXCEPTION_DATA;
	try {
		int buf[SIZE];
		PreallocatedVector<VElem> xx(buf, sizeof buf);
		xx.resize(1, a);
	}
	catch (const VElem::Exception&) {
		return;
	}
	FAIL("failed");
}

TEST(PreallocatedVectorTest, operator_assign_VElem)
{
	VElem a;

	int xbuf[SIZE] = {0};
	PreallocatedVector<VElem> x(xbuf, sizeof xbuf);
	x.resize(2, a);

	int ybuf[SIZE] = {0};
	PreallocatedVector<VElem> y(ybuf, sizeof ybuf);
	y.resize(1, a);

	y = x;
	LONGS_EQUAL(2, y.size());

	int zbuf[SIZE] = {0};
	PreallocatedVector<VElem> z(zbuf, sizeof zbuf);
	z.resize(3, a);

	z = x;
	LONGS_EQUAL(2, z.size());

	z.clear();
	VElem::checkElemsDestroyed(&z[0], z.max_size(), 3);
}

TEST(PreallocatedVectorTest, push_back_pop_back_VElem)
{
	VElem a;

	PreallocatedVector<VElem> x(alloc_buf, ALLOC_SIZE);
	x.push_back(a);
	x.push_back(a);
	x.pop_back();
	LONGS_EQUAL(1, x.size());
	x.clear();
	VElem::checkElemsDestroyed(&x[0], x.max_size(), 2);

	int ybuf[SIZE] = {0};
	PreallocatedVector<VElem> y(ybuf, sizeof ybuf);
	y.push_back(a);
	a.data = VElem::EXCEPTION_DATA;
	try {
		y.push_back(a);
	}
	catch (const VElem::Exception&) {
		LONGS_EQUAL(1, y.size());
		y.clear();
		VElem::checkElemsDestroyed(&y[0], y.max_size(), 1);
		return;
	}
	FAIL("failed");
}

TEST(PreallocatedVectorTest, insert_n_VElem)
{
	VElem a;

	PreallocatedVector<VElem> x(alloc_buf, ALLOC_SIZE);
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
		VElem::checkElemsDestroyed(&x[0], x.max_size(), 5);
		return;
	}
	FAIL("failed");
}

TEST(PreallocatedVectorTest, insert_range_VElem)
{
	VElem a;

	PreallocatedVector<VElem> x(alloc_buf, ALLOC_SIZE);
	x.resize(2, a);

	int ybuf[ALLOC_SIZE] = {0};
	PreallocatedVector<VElem> y(ybuf, sizeof ybuf);
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
		VElem::checkElemsDestroyed(&y[0], y.max_size(), 5);
		return;
	}
	FAIL("failed");
}

TEST(PreallocatedVectorTest, erase_range_VElem)
{
	VElem a;

	PreallocatedVector<VElem> x(alloc_buf, ALLOC_SIZE);
	x.resize(4, a);

	x.erase(x.begin() + 1, x.begin() + 2);

	x.erase(x.begin(), x.end());

	VElem::checkElemsDestroyed(&x[0], x.max_size(), 4);
}

#if (__cplusplus >= 201103L) || defined(_WIN32)
#include <memory>

TEST(PreallocatedVectorTest, shared_ptr_int)
{
	PreallocatedVector<std::shared_ptr<int>> x(alloc_buf, ALLOC_SIZE);
	x.push_back(std::make_shared<int>(1));
	LONGS_EQUAL(1, *x[0]);
	*x[0] = 2;
	LONGS_EQUAL(2, *x[0]);
}

TEST(PreallocatedVectorTest, shared_ptr_VElem)
{
	VElem a;

	{
		PreallocatedVector<std::shared_ptr<VElem>> x(alloc_buf, ALLOC_SIZE);

		x.push_back(std::make_shared<VElem>(a));
		x.push_back(std::make_shared<VElem>(a));

		x.pop_back();

	}

}
#endif

} // namespace PreallocatedVectorTest
