#include "Container/BitPattern.h"
#include <climits>
#include "CppUTest/TestHarness.h"

// Reason not to use std::uint32_t is to test with C++98.
#if UINT_MAX == 0xFFFFFFFF
typedef unsigned int U32;
#else
typedef unsigned long U32;
#if ULONG_MAX != 0xFFFFFFFF
#define U32_IS_NOT_32BIT
#endif
#endif

namespace BitPatternTest {

using Container::BitPattern;

TEST_GROUP(BitPatternTest) {
	void setup()
	{
	}
	void teardown()
	{
	}
};

TEST(BitPatternTest, default_ctor)
{
	BitPattern<U32> b;
	UNSIGNED_LONGS_EQUAL(0, b);
}

TEST(BitPatternTest, ctor)
{
	BitPattern<U32> b(0x12345678);
	UNSIGNED_LONGS_EQUAL(0x12345678, b);
}

#ifndef U32_IS_NOT_32BIT
TEST(BitPatternTest, size)
{
	BitPattern<U32> b;
	UNSIGNED_LONGS_EQUAL(32, b.size());
}
#endif

TEST(BitPatternTest, copy_ctor)
{
	BitPattern<U32> a(0x12345678);
	BitPattern<U32> b(a);
	UNSIGNED_LONGS_EQUAL(0x12345678, b);
}

TEST(BitPatternTest, operator_assign)
{
	const BitPattern<U32> a(0x12345678);
	BitPattern<U32> b;
	b = a;
	UNSIGNED_LONGS_EQUAL(0x12345678, b);

	U32 c = a;
	UNSIGNED_LONGS_EQUAL(0x12345678, c);

	BitPattern<U32> d;
	d = 0x12345678;
	UNSIGNED_LONGS_EQUAL(0x12345678, d);
}

TEST(BitPatternTest, set_reset_all)
{
	BitPattern<U32> b;
	b.set();
#ifndef U32_IS_NOT_32BIT
	UNSIGNED_LONGS_EQUAL(0xFFFFFFFF, b);
#endif

	b.reset();
	UNSIGNED_LONGS_EQUAL(0, b);
}

TEST(BitPatternTest, set_reset_bit)
{
	BitPattern<U32> b;
	for (std::size_t i = 0; i < 32; i++) {
		b.set(i);
		UNSIGNED_LONGS_EQUAL((1U << i), b);
		b.reset(i);
		UNSIGNED_LONGS_EQUAL(0, b);
	}
}

TEST(BitPatternTest, set_reset_return_this)
{
	BitPattern<U32> b;
	b.set(0).set(1).set(2).set(3);
	UNSIGNED_LONGS_EQUAL(0xF, b);

	b.reset(0).reset(1).reset(2).reset(3);
	UNSIGNED_LONGS_EQUAL(0x0, b);
}

TEST(BitPatternTest, flip_all)
{
	BitPattern<U32> b(0x12345678);
	b.flip();
#ifndef U32_IS_NOT_32BIT
	UNSIGNED_LONGS_EQUAL(0xEDCBA987, b);
#endif
}

TEST(BitPatternTest, flip_bit)
{
	BitPattern<U32> b;
	for (std::size_t i = 0; i < 32; i++) {
		b.flip(i);
		UNSIGNED_LONGS_EQUAL((1U << i), b);
		b.flip(i);
		UNSIGNED_LONGS_EQUAL(0, b);
	}
}

TEST(BitPatternTest, flip_return_this)
{
	BitPattern<U32> b(0x12345678);
	b.flip(0).flip(1).flip(2).flip(3);
	UNSIGNED_LONGS_EQUAL(0x12345677, b);
}

#ifndef U32_IS_NOT_32BIT
TEST(BitPatternTest, operator_tilde)
{
	const BitPattern<U32> a(0x12345678);
	BitPattern<U32> b = ~a;
	UNSIGNED_LONGS_EQUAL(0xEDCBA987, b);

	U32 c = ~a;
	UNSIGNED_LONGS_EQUAL(0xEDCBA987, c);

	BitPattern<U32> d;
	d = ~0x12345678U;
	UNSIGNED_LONGS_EQUAL(0xEDCBA987, d);
}
#endif

TEST(BitPatternTest, operator_and_equal)
{
	const BitPattern<U32> a(0xAAAAAAAA);
	BitPattern<U32> b(0x82824141);
	b &= a;
	UNSIGNED_LONGS_EQUAL(0x82820000, b);

	U32 c = 0x82824141;
	c &= a;
	UNSIGNED_LONGS_EQUAL(0x82820000, c);

	BitPattern<U32> d(0x82824141);
	d &= 0xAAAAAAAA;
	UNSIGNED_LONGS_EQUAL(0x82820000, d);
}

TEST(BitPatternTest, operator_or_equal)
{
	const BitPattern<U32> a(0xAAAAAAAA);
	BitPattern<U32> b(0x82824141);
	b |= a;
	UNSIGNED_LONGS_EQUAL(0xAAAAEBEB, b);

	U32 c = 0x82824141;
	c |= a;
	UNSIGNED_LONGS_EQUAL(0xAAAAEBEB, c);

	BitPattern<U32> d(0x82824141);
	d |= 0xAAAAAAAA;
	UNSIGNED_LONGS_EQUAL(0xAAAAEBEB, d);
}

TEST(BitPatternTest, operator_xor_equal)
{
	const BitPattern<U32> a(0xAAAAAAAA);
	BitPattern<U32> b(0x82824141);
	b ^= a;
	UNSIGNED_LONGS_EQUAL(0x2828EBEB, b);

	U32 c = 0x82824141;
	c ^= a;
	UNSIGNED_LONGS_EQUAL(0x2828EBEB, c);

	BitPattern<U32> d(0x82824141);
	d ^= 0xAAAAAAAA;
	UNSIGNED_LONGS_EQUAL(0x2828EBEB, d);
}

TEST(BitPatternTest, operator_left_right_shift_equal)
{
	BitPattern<U32> b(0x1);
	for (std::size_t i = 0; i < 32; i++) {
		b <<= i;
		UNSIGNED_LONGS_EQUAL((1U << i), b);
		b >>= i;
		UNSIGNED_LONGS_EQUAL(1, b);
	}
}

TEST(BitPatternTest, operator_left_right_shift)
{
	BitPattern<U32> a(0x1);
	for (std::size_t i = 0; i < 32; i++) {
		BitPattern<U32> b = a << i;
		UNSIGNED_LONGS_EQUAL((1U << i), b);
		BitPattern<U32> c = b >> i;
		UNSIGNED_LONGS_EQUAL(1, c);
	}
}

TEST(BitPatternTest, operator_equal_true)
{
	const BitPattern<U32> a(0x12345678);
	BitPattern<U32> b(0x12345678);
	bool f = (a == b);
	CHECK(f);

	bool g = (a == 0x12345678);
	CHECK(g);
}

TEST(BitPatternTest, operator_equal_false)
{
	const BitPattern<U32> a(0x12345678);
	BitPattern<U32> b(0x12345679);
	bool f = (a == b);
	CHECK_FALSE(f);

	bool g = (a == 0x12345679);
	CHECK_FALSE(g);
}

TEST(BitPatternTest, operator_not_equal_true)
{
	const BitPattern<U32> a(0x12345678);
	BitPattern<U32> b(0x12345679);
	bool f = (a != b);
	CHECK(f);

	bool g = (a != 0x12345679);
	CHECK(g);
}

TEST(BitPatternTest, operator_not_equal_false)
{
	const BitPattern<U32> a(0x12345678);
	BitPattern<U32> b(0x12345678);
	bool f = (a != b);
	CHECK_FALSE(f);

	bool g = (a != 0x12345678);
	CHECK_FALSE(g);
}

TEST(BitPatternTest, test)
{
	BitPattern<U32> a(0xAAAAAAAA);
	CHECK_FALSE(a.test(0));
	CHECK(a.test(1));
	CHECK_FALSE(a.test(2));
	CHECK(a.test(3));
}

#ifndef U32_IS_NOT_32BIT
TEST(BitPatternTest, all_true)
{
	BitPattern<U32> a(0xFFFFFFFF);
	CHECK(a.all());
}
#endif

TEST(BitPatternTest, all_false)
{
	BitPattern<U32> a(0xFFFFFFFE);
	CHECK_FALSE(a.all());
}

TEST(BitPatternTest, any_true)
{
	BitPattern<U32> a(0x1);
	CHECK(a.any());
}

TEST(BitPatternTest, any_false)
{
	BitPattern<U32> a(0x0);
	CHECK_FALSE(a.any());
}

TEST(BitPatternTest, none_true)
{
	BitPattern<U32> a(0x0);
	CHECK(a.none());
}

TEST(BitPatternTest, none_false)
{
	BitPattern<U32> a(0x1);
	CHECK_FALSE(a.none());
}

TEST(BitPatternTest, operator_and)
{
	const BitPattern<U32> a(0xAAAAAAAA);
	BitPattern<U32> b(0x82824141);
	BitPattern<U32> c = b & a;
	UNSIGNED_LONGS_EQUAL(0x82820000, c);

	U32 d = b & a;
	UNSIGNED_LONGS_EQUAL(0x82820000, d);

	BitPattern<U32> e = b & 0xAAAAAAAA;
	UNSIGNED_LONGS_EQUAL(0x82820000, e);

	U32 f = b & 0xAAAAAAAA;
	UNSIGNED_LONGS_EQUAL(0x82820000, f);
}

TEST(BitPatternTest, operator_or)
{
	const BitPattern<U32> a(0xAAAAAAAA);
	BitPattern<U32> b(0x82824141);
	BitPattern<U32> c = b | a;
	UNSIGNED_LONGS_EQUAL(0xAAAAEBEB, c);

	U32 d = b | a;
	UNSIGNED_LONGS_EQUAL(0xAAAAEBEB, d);

	BitPattern<U32> e = b | 0xAAAAAAAA;
	UNSIGNED_LONGS_EQUAL(0xAAAAEBEB, e);

	U32 f = b | 0xAAAAAAAA;
	UNSIGNED_LONGS_EQUAL(0xAAAAEBEB, f);
}

TEST(BitPatternTest, operator_xor)
{
	const BitPattern<U32> a(0xAAAAAAAA);
	BitPattern<U32> b(0x82824141);
	BitPattern<U32> c = b ^ a;
	UNSIGNED_LONGS_EQUAL(0x2828EBEB, c);

	U32 d = b ^ a;
	UNSIGNED_LONGS_EQUAL(0x2828EBEB, d);

	BitPattern<U32> e = b ^ 0xAAAAAAAA;
	UNSIGNED_LONGS_EQUAL(0x2828EBEB, e);

	U32 f = b ^ 0xAAAAAAAA;
	UNSIGNED_LONGS_EQUAL(0x2828EBEB, f);
}

#ifndef NDEBUG
#ifndef U32_IS_NOT_32BIT
TEST(BitPatternTest, set_precondition_failed)
{
	BitPattern<U32> b;
	try {
		b.set(32);
	}
	catch (const Assertion::Failure& e) {
		STRCMP_CONTAINS("failed", e.message());
		return;
	}
	FAIL("failed");
}

TEST(BitPatternTest, reset_precondition_failed)
{
	BitPattern<U32> b;
	try {
		b.reset(32);
	}
	catch (const Assertion::Failure& e) {
		STRCMP_CONTAINS("failed", e.message());
		return;
	}
	FAIL("failed");
}

TEST(BitPatternTest, flip_precondition_failed)
{
	BitPattern<U32> b;
	try {
		b.flip(32);
	}
	catch (const Assertion::Failure& e) {
		STRCMP_CONTAINS("failed", e.message());
		return;
	}
	FAIL("failed");
}

TEST(BitPatternTest, test_precondition_failed)
{
	BitPattern<U32> b;
	try {
		b.test(32);
	}
	catch (const Assertion::Failure& e) {
		STRCMP_CONTAINS("failed", e.message());
		return;
	}
	FAIL("failed");
}
#endif
#endif

TEST(BitPatternTest, test_8bit)
{
	BitPattern<unsigned char> b;
	UNSIGNED_LONGS_EQUAL(8, b.size());
	UNSIGNED_LONGS_EQUAL(0x0, b);

	b.set(0).set(1).set(2).set(3);
	UNSIGNED_LONGS_EQUAL(0xF, b);
	CHECK_FALSE(b.all());
	b.set();
	UNSIGNED_LONGS_EQUAL(0xFF, b);
	CHECK(b.all());

	b.reset(0).reset(1).reset(2).reset(3);
	UNSIGNED_LONGS_EQUAL(0xF0, b);
	b.reset();
	UNSIGNED_LONGS_EQUAL(0x0, b);

	b.flip(0).flip(1).flip(2).flip(3);
	UNSIGNED_LONGS_EQUAL(0xF, b);
	b.flip();
	UNSIGNED_LONGS_EQUAL(0xF0, b);

	CHECK(b.test(7));
	CHECK_FALSE(b.test(0));
}

TEST(BitPatternTest, test_16bit)
{
	BitPattern<unsigned short> b;
	UNSIGNED_LONGS_EQUAL(16, b.size());
	UNSIGNED_LONGS_EQUAL(0x0, b);

	b.set(0).set(1).set(2).set(3);
	UNSIGNED_LONGS_EQUAL(0xF, b);
	CHECK_FALSE(b.all());
	b.set();
	UNSIGNED_LONGS_EQUAL(0xFFFF, b);
	CHECK(b.all());

	b.reset(0).reset(1).reset(2).reset(3);
	UNSIGNED_LONGS_EQUAL(0xFFF0, b);
	b.reset();
	UNSIGNED_LONGS_EQUAL(0x0, b);

	b.flip(0).flip(1).flip(2).flip(3);
	UNSIGNED_LONGS_EQUAL(0xF, b);
	b.flip();
	UNSIGNED_LONGS_EQUAL(0xFFF0, b);

	CHECK(b.test(15));
	CHECK_FALSE(b.test(0));
}

#if (__cplusplus >= 201103L) || !defined(CPPELIB_NO_LONG_LONG)
TEST(BitPatternTest, test_64bit)
{
	BitPattern<unsigned long long> b;
	UNSIGNED_LONGS_EQUAL(64, b.size());
	UNSIGNED_LONGLONGS_EQUAL(0x0, b);

	b.set(0).set(1).set(2).set(3);
	UNSIGNED_LONGLONGS_EQUAL(0xF, b);
	CHECK_FALSE(b.all());
	b.set();
	UNSIGNED_LONGLONGS_EQUAL(0xFFFFFFFFFFFFFFFF, b);
	CHECK(b.all());

	b.reset(0).reset(1).reset(2).reset(3);
	UNSIGNED_LONGLONGS_EQUAL(0xFFFFFFFFFFFFFFF0, b);
	b.reset();
	UNSIGNED_LONGLONGS_EQUAL(0x0, b);

	b.flip(0).flip(1).flip(2).flip(3);
	UNSIGNED_LONGLONGS_EQUAL(0xF, b);
	b.flip();
	UNSIGNED_LONGLONGS_EQUAL(0xFFFFFFFFFFFFFFF0, b);

	CHECK(b.test(63));
	CHECK_FALSE(b.test(0));
}
#endif

} // namespace BitPatternTest
