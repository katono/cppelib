#include "Container/Array.h"
#ifndef CPPELIB_NO_STD_ALGORITHM
#include <algorithm>
#include <functional>
#endif
#include "CppUTest/TestHarness.h"

namespace ArrayTest {

using Container::Array;

TEST_GROUP(ArrayTest) {
	static const std::size_t SIZE = 10;
	void setup()
	{
	}
	void teardown()
	{
	}
};

TEST(ArrayTest, init0)
{
	Array<int, SIZE> a = {0};
	for (std::size_t i = 0; i < a.size(); ++i) {
		LONGS_EQUAL(0, a.at(i));
	}
}

TEST(ArrayTest, copy_ctor)
{
	Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	Array<int, SIZE> b(a);
	for (std::size_t i = 0; i < a.size(); ++i) {
		LONGS_EQUAL(a[i], b[i]);
	}
}

TEST(ArrayTest, operator_assign)
{
	Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	Array<int, SIZE> b;
	b = a;
	for (std::size_t i = 0; i < a.size(); ++i) {
		LONGS_EQUAL(a[i], b[i]);
	}
}

TEST(ArrayTest, size)
{
	Array<int, SIZE> a;
	LONGS_EQUAL(SIZE, a.size());
}

TEST(ArrayTest, max_size)
{
	Array<int, SIZE> a;
	LONGS_EQUAL(SIZE, a.max_size());
}

TEST(ArrayTest, empty)
{
	Array<int, SIZE> a;
	CHECK_FALSE(a.empty());
}

TEST(ArrayTest, operator_bracket_read)
{
	Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	LONGS_EQUAL(0, a[0]);
	LONGS_EQUAL(5, a[5]);
	LONGS_EQUAL(9, a[9]);
}

TEST(ArrayTest, operator_bracket_read_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	LONGS_EQUAL(0, a[0]);
	LONGS_EQUAL(5, a[5]);
	LONGS_EQUAL(9, a[9]);
}

TEST(ArrayTest, operator_bracket_write)
{
	Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	a[0] = 100;
	LONGS_EQUAL(100, a[0]);

	a[9] = 10;
	LONGS_EQUAL(10, a[9]);
}

TEST(ArrayTest, at_read)
{
	Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	LONGS_EQUAL(0, a.at(0));
	LONGS_EQUAL(5, a.at(5));
	LONGS_EQUAL(9, a.at(9));
}

TEST(ArrayTest, at_read_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	LONGS_EQUAL(0, a.at(0));
	LONGS_EQUAL(5, a.at(5));
	LONGS_EQUAL(9, a.at(9));
}

TEST(ArrayTest, at_write)
{
	Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	a.at(0) = 100;
	LONGS_EQUAL(100, a.at(0));

	a.at(9) = 10;
	LONGS_EQUAL(10, a.at(9));
}

TEST(ArrayTest, at_exception)
{
	Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	try {
		a.at(SIZE);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("Array::at", e.what());
		return;
	}
	FAIL("failed");
}

TEST(ArrayTest, at_exception_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	try {
		a.at(SIZE);
	}
	catch (const std::exception& e) {
		STRCMP_EQUAL("Array::at", e.what());
		return;
	}
	FAIL("failed");
}

TEST(ArrayTest, data_read)
{
	Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	LONGS_EQUAL(0, a.data()[0]);
	LONGS_EQUAL(5, a.data()[5]);
	LONGS_EQUAL(9, a.data()[9]);
}

TEST(ArrayTest, data_read_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	LONGS_EQUAL(0, a.data()[0]);
	LONGS_EQUAL(5, a.data()[5]);
	LONGS_EQUAL(9, a.data()[9]);
}

TEST(ArrayTest, data_write)
{
	Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	int* p = a.data();
	p[5] = 100;
	LONGS_EQUAL(100, a.data()[5]);
}

TEST(ArrayTest, begin_end)
{
	Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	std::size_t i = 0;
	for (Array<int, SIZE>::iterator it = a.begin(); it != a.end(); ++it, ++i) {
		LONGS_EQUAL(a[i], *it);
	}
}

TEST(ArrayTest, begin_end_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	std::size_t i = 0;
	for (Array<int, SIZE>::const_iterator it = a.begin(); it != a.end(); ++it, ++i) {
		LONGS_EQUAL(a[i], *it);
	}
}

TEST(ArrayTest, front_read)
{
	Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	LONGS_EQUAL(0, a.front());
}

TEST(ArrayTest, front_read_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	LONGS_EQUAL(0, a.front());
}

TEST(ArrayTest, front_write)
{
	Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	a.front() = 100;
	LONGS_EQUAL(100, a.front());
}

TEST(ArrayTest, back_read)
{
	Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	LONGS_EQUAL(9, a.back());
}

TEST(ArrayTest, back_read_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	LONGS_EQUAL(9, a.back());
}

TEST(ArrayTest, back_write)
{
	Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	a.back() = 100;
	LONGS_EQUAL(100, a.back());
}

TEST(ArrayTest, fill)
{
	Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	a.fill(100);
	for (std::size_t i = 0; i < a.size(); ++i) {
		LONGS_EQUAL(100, a.at(i));
	}
}

TEST(ArrayTest, operator_equal_true)
{
	Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	Array<int, SIZE> b = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	CHECK_TRUE(a == b);
}

TEST(ArrayTest, operator_equal_false)
{
	Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	Array<int, SIZE> b = {0, 1, 2, 3, 4, 5, 6, 7, 8, 10};
	CHECK_FALSE(a == b);
}

TEST(ArrayTest, operator_notequal_true)
{
	Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	Array<int, SIZE> b = {0, 1, 2, 3, 4, 5, 6, 7, 8, 10};
	CHECK_TRUE(a != b);
}

TEST(ArrayTest, operator_notequal_false)
{
	Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	Array<int, SIZE> b = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	CHECK_FALSE(a != b);
}

TEST(ArrayTest, new_delete)
{
	Array<int, SIZE>* a = new Array<int, SIZE>();
	LONGS_EQUAL(SIZE, a->size());
	delete a;
}

#ifndef CPPELIB_NO_STD_ALGORITHM
TEST(ArrayTest, algo_sort)
{
	Array<int, SIZE> a = {1, 7, 0, 2, 5, 3, 9, 4, 6, 8};
	std::sort(a.begin(), a.end());
	for (std::size_t i = 0; i < a.size(); ++i) {
		LONGS_EQUAL(i, a.at(i));
	}

	std::sort(a.begin(), a.end(), std::greater<int>());
	for (std::size_t i = 0; i < a.size(); ++i) {
		LONGS_EQUAL(SIZE - i - 1, a.at(i));
	}
}
#endif

#ifndef CPPELIB_NO_STD_ITERATOR
TEST(ArrayTest, rbegin_rend)
{
	Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	std::size_t i = SIZE - 1;
	for (Array<int, SIZE>::reverse_iterator it = a.rbegin(); it != a.rend(); ++it, --i) {
		LONGS_EQUAL(a[i], *it);
	}
}

TEST(ArrayTest, rbegin_rend_const)
{
	const Array<int, SIZE> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	std::size_t i = SIZE - 1;
	for (Array<int, SIZE>::const_reverse_iterator it = a.rbegin(); it != a.rend(); ++it, --i) {
		LONGS_EQUAL(a[i], *it);
	}
}
#endif

} // namespace ArrayTest
