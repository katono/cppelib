#include "Container/BitPattern.h"
#include <cstdint>
#include "CppUTest/TestHarness.h"

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
	BitPattern<std::uint32_t> b;
	UNSIGNED_LONGS_EQUAL(0, b);
}

TEST(BitPatternTest, ctor)
{
	BitPattern<std::uint32_t> b(0x12345678);
	UNSIGNED_LONGS_EQUAL(0x12345678, b);
}

TEST(BitPatternTest, size)
{
	BitPattern<std::uint32_t> b;
	UNSIGNED_LONGS_EQUAL(32, b.size());
}

TEST(BitPatternTest, copy_ctor)
{
	BitPattern<std::uint32_t> a(0x12345678);
	BitPattern<std::uint32_t> b(a);
	UNSIGNED_LONGS_EQUAL(0x12345678, b);
}

TEST(BitPatternTest, operator_assign)
{
	const BitPattern<std::uint32_t> a(0x12345678);
	BitPattern<std::uint32_t> b;
	b = a;
	UNSIGNED_LONGS_EQUAL(0x12345678, b);

	std::uint32_t c = a;
	UNSIGNED_LONGS_EQUAL(0x12345678, c);

	BitPattern<std::uint32_t> d;
	d = 0x12345678;
	UNSIGNED_LONGS_EQUAL(0x12345678, d);
}

TEST(BitPatternTest, set_reset_all)
{
	BitPattern<std::uint32_t> b;
	b.set();
	UNSIGNED_LONGS_EQUAL(0xFFFFFFFF, b);

	b.reset();
	UNSIGNED_LONGS_EQUAL(0, b);
}

TEST(BitPatternTest, set_reset_bit)
{
	BitPattern<std::uint32_t> b;
	for (std::size_t i = 0; i < 32; i++) {
		b.set(i);
		UNSIGNED_LONGS_EQUAL((1U << i), b);
		b.reset(i);
		UNSIGNED_LONGS_EQUAL(0, b);
	}
}

TEST(BitPatternTest, set_reset_return_this)
{
	BitPattern<std::uint32_t> b;
	b.set(0).set(1).set(2).set(3);
	UNSIGNED_LONGS_EQUAL(0xF, b);

	b.reset(0).reset(1).reset(2).reset(3);
	UNSIGNED_LONGS_EQUAL(0x0, b);
}

TEST(BitPatternTest, flip_all)
{
	BitPattern<std::uint32_t> b(0x12345678);
	b.flip();
	UNSIGNED_LONGS_EQUAL(0xEDCBA987, b);
}

TEST(BitPatternTest, flip_bit)
{
	BitPattern<std::uint32_t> b;
	for (std::size_t i = 0; i < 32; i++) {
		b.flip(i);
		UNSIGNED_LONGS_EQUAL((1U << i), b);
		b.flip(i);
		UNSIGNED_LONGS_EQUAL(0, b);
	}
}

TEST(BitPatternTest, flip_return_this)
{
	BitPattern<std::uint32_t> b(0x12345678);
	b.flip(0).flip(1).flip(2).flip(3);
	UNSIGNED_LONGS_EQUAL(0x12345677, b);
}

TEST(BitPatternTest, operator_tilde)
{
	const BitPattern<std::uint32_t> a(0x12345678);
	BitPattern<std::uint32_t> b = ~a;
	UNSIGNED_LONGS_EQUAL(0xEDCBA987, b);

	std::uint32_t c = ~a;
	UNSIGNED_LONGS_EQUAL(0xEDCBA987, c);

	BitPattern<std::uint32_t> d;
	d = ~0x12345678U;
	UNSIGNED_LONGS_EQUAL(0xEDCBA987, d);
}

TEST(BitPatternTest, operator_and_equal)
{
	const BitPattern<std::uint32_t> a(0xAAAAAAAA);
	BitPattern<std::uint32_t> b(0x82824141);
	b &= a;
	UNSIGNED_LONGS_EQUAL(0x82820000, b);

	std::uint32_t c = 0x82824141;
	c &= a;
	UNSIGNED_LONGS_EQUAL(0x82820000, c);

	BitPattern<std::uint32_t> d(0x82824141);
	d &= 0xAAAAAAAA;
	UNSIGNED_LONGS_EQUAL(0x82820000, d);
}

TEST(BitPatternTest, operator_or_equal)
{
	const BitPattern<std::uint32_t> a(0xAAAAAAAA);
	BitPattern<std::uint32_t> b(0x82824141);
	b |= a;
	UNSIGNED_LONGS_EQUAL(0xAAAAEBEB, b);

	std::uint32_t c = 0x82824141;
	c |= a;
	UNSIGNED_LONGS_EQUAL(0xAAAAEBEB, c);

	BitPattern<std::uint32_t> d(0x82824141);
	d |= 0xAAAAAAAA;
	UNSIGNED_LONGS_EQUAL(0xAAAAEBEB, d);
}

TEST(BitPatternTest, operator_xor_equal)
{
	const BitPattern<std::uint32_t> a(0xAAAAAAAA);
	BitPattern<std::uint32_t> b(0x82824141);
	b ^= a;
	UNSIGNED_LONGS_EQUAL(0x2828EBEB, b);

	std::uint32_t c = 0x82824141;
	c ^= a;
	UNSIGNED_LONGS_EQUAL(0x2828EBEB, c);

	BitPattern<std::uint32_t> d(0x82824141);
	d ^= 0xAAAAAAAA;
	UNSIGNED_LONGS_EQUAL(0x2828EBEB, d);
}

TEST(BitPatternTest, operator_left_right_shift_equal)
{
	BitPattern<std::uint32_t> b(0x1);
	for (std::size_t i = 0; i < 32; i++) {
		b <<= i;
		UNSIGNED_LONGS_EQUAL((1U << i), b);
		b >>= i;
		UNSIGNED_LONGS_EQUAL(1, b);
	}
}

TEST(BitPatternTest, operator_left_right_shift)
{
	BitPattern<std::uint32_t> a(0x1);
	for (std::size_t i = 0; i < 32; i++) {
		BitPattern<std::uint32_t> b = a << i;
		UNSIGNED_LONGS_EQUAL((1U << i), b);
		BitPattern<std::uint32_t> c = b >> i;
		UNSIGNED_LONGS_EQUAL(1, c);
	}
}

TEST(BitPatternTest, operator_equal_true)
{
	const BitPattern<std::uint32_t> a(0x12345678);
	BitPattern<std::uint32_t> b(0x12345678);
	bool f = (a == b);
	CHECK(f);

	bool g = (a == 0x12345678);
	CHECK(g);
}

TEST(BitPatternTest, operator_equal_false)
{
	const BitPattern<std::uint32_t> a(0x12345678);
	BitPattern<std::uint32_t> b(0x12345679);
	bool f = (a == b);
	CHECK_FALSE(f);

	bool g = (a == 0x12345679);
	CHECK_FALSE(g);
}

TEST(BitPatternTest, operator_not_equal_true)
{
	const BitPattern<std::uint32_t> a(0x12345678);
	BitPattern<std::uint32_t> b(0x12345679);
	bool f = (a != b);
	CHECK(f);

	bool g = (a != 0x12345679);
	CHECK(g);
}

TEST(BitPatternTest, operator_not_equal_false)
{
	const BitPattern<std::uint32_t> a(0x12345678);
	BitPattern<std::uint32_t> b(0x12345678);
	bool f = (a != b);
	CHECK_FALSE(f);

	bool g = (a != 0x12345678);
	CHECK_FALSE(g);
}

TEST(BitPatternTest, test)
{
	BitPattern<std::uint32_t> a(0xAAAAAAAA);
	CHECK_FALSE(a.test(0));
	CHECK(a.test(1));
	CHECK_FALSE(a.test(2));
	CHECK(a.test(3));
}

TEST(BitPatternTest, all_true)
{
	BitPattern<std::uint32_t> a(0xFFFFFFFF);
	CHECK(a.all());
}

TEST(BitPatternTest, all_false)
{
	BitPattern<std::uint32_t> a(0xFFFFFFFE);
	CHECK_FALSE(a.all());
}

TEST(BitPatternTest, any_true)
{
	BitPattern<std::uint32_t> a(0x1);
	CHECK(a.any());
}

TEST(BitPatternTest, any_false)
{
	BitPattern<std::uint32_t> a(0x0);
	CHECK_FALSE(a.any());
}

TEST(BitPatternTest, none_true)
{
	BitPattern<std::uint32_t> a(0x0);
	CHECK(a.none());
}

TEST(BitPatternTest, none_false)
{
	BitPattern<std::uint32_t> a(0x1);
	CHECK_FALSE(a.none());
}

TEST(BitPatternTest, operator_and)
{
	const BitPattern<std::uint32_t> a(0xAAAAAAAA);
	BitPattern<std::uint32_t> b(0x82824141);
	BitPattern<std::uint32_t> c = b & a;
	UNSIGNED_LONGS_EQUAL(0x82820000, c);

	std::uint32_t d = b & a;
	UNSIGNED_LONGS_EQUAL(0x82820000, d);

	BitPattern<std::uint32_t> e = b & 0xAAAAAAAA;
	UNSIGNED_LONGS_EQUAL(0x82820000, e);

	std::uint32_t f = b & 0xAAAAAAAA;
	UNSIGNED_LONGS_EQUAL(0x82820000, f);
}

TEST(BitPatternTest, operator_or)
{
	const BitPattern<std::uint32_t> a(0xAAAAAAAA);
	BitPattern<std::uint32_t> b(0x82824141);
	BitPattern<std::uint32_t> c = b | a;
	UNSIGNED_LONGS_EQUAL(0xAAAAEBEB, c);

	std::uint32_t d = b | a;
	UNSIGNED_LONGS_EQUAL(0xAAAAEBEB, d);

	BitPattern<std::uint32_t> e = b | 0xAAAAAAAA;
	UNSIGNED_LONGS_EQUAL(0xAAAAEBEB, e);

	std::uint32_t f = b | 0xAAAAAAAA;
	UNSIGNED_LONGS_EQUAL(0xAAAAEBEB, f);
}

TEST(BitPatternTest, operator_xor)
{
	const BitPattern<std::uint32_t> a(0xAAAAAAAA);
	BitPattern<std::uint32_t> b(0x82824141);
	BitPattern<std::uint32_t> c = b ^ a;
	UNSIGNED_LONGS_EQUAL(0x2828EBEB, c);

	std::uint32_t d = b ^ a;
	UNSIGNED_LONGS_EQUAL(0x2828EBEB, d);

	BitPattern<std::uint32_t> e = b ^ 0xAAAAAAAA;
	UNSIGNED_LONGS_EQUAL(0x2828EBEB, e);

	std::uint32_t f = b ^ 0xAAAAAAAA;
	UNSIGNED_LONGS_EQUAL(0x2828EBEB, f);
}

#ifndef NDEBUG
TEST(BitPatternTest, set_precondition_failed)
{
	BitPattern<std::uint32_t> b;
	try {
		b.set(32);
	}
	catch (const Assertion::Error& e) {
		STRCMP_CONTAINS("failed", e.message());
		return;
	}
	FAIL("failed");
}

TEST(BitPatternTest, reset_precondition_failed)
{
	BitPattern<std::uint32_t> b;
	try {
		b.reset(32);
	}
	catch (const Assertion::Error& e) {
		STRCMP_CONTAINS("failed", e.message());
		return;
	}
	FAIL("failed");
}

TEST(BitPatternTest, flip_precondition_failed)
{
	BitPattern<std::uint32_t> b;
	try {
		b.flip(32);
	}
	catch (const Assertion::Error& e) {
		STRCMP_CONTAINS("failed", e.message());
		return;
	}
	FAIL("failed");
}

TEST(BitPatternTest, test_precondition_failed)
{
	BitPattern<std::uint32_t> b;
	try {
		b.test(32);
	}
	catch (const Assertion::Error& e) {
		STRCMP_CONTAINS("failed", e.message());
		return;
	}
	FAIL("failed");
}
#endif
