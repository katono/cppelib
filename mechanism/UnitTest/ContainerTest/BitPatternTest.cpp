#include "CppUTest/TestHarness.h"
#include "Container/BitPattern.h"
#include <cstdint>

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
	LONGS_EQUAL(0, b.data());
}

TEST(BitPatternTest, ctor)
{
	BitPattern<std::uint32_t> b(0x12345678);
	LONGS_EQUAL(0x12345678, b.data());
}

TEST(BitPatternTest, size)
{
	BitPattern<std::uint32_t> b;
	LONGS_EQUAL(32, b.size());
}

TEST(BitPatternTest, copy_ctor)
{
	BitPattern<std::uint32_t> a(0x12345678);
	BitPattern<std::uint32_t> b(a);
	LONGS_EQUAL(0x12345678, b.data());
}

TEST(BitPatternTest, operator_assign)
{
	BitPattern<std::uint32_t> a(0x12345678);
	BitPattern<std::uint32_t> b;
	b = a;
	LONGS_EQUAL(0x12345678, b.data());
}

TEST(BitPatternTest, set_reset_all)
{
	BitPattern<std::uint32_t> b;
	b.set();
	LONGS_EQUAL(0xFFFFFFFF, b.data());

	b.reset();
	LONGS_EQUAL(0, b.data());
}

TEST(BitPatternTest, set_reset_bit)
{
	BitPattern<std::uint32_t> b;
	for (std::size_t i = 0; i < 32; i++) {
		b.set(i);
		LONGS_EQUAL((1 << i), b.data());
		b.reset(i);
		LONGS_EQUAL(0, b.data());
	}
}

TEST(BitPatternTest, set_reset_return_this)
{
	BitPattern<std::uint32_t> b;
	b.set(0).set(1).set(2).set(3);
	LONGS_EQUAL(0xF, b.data());

	b.reset(0).reset(1).reset(2).reset(3);
	LONGS_EQUAL(0x0, b.data());
}

TEST(BitPatternTest, set_precondition_failed)
{
	BitPattern<std::uint32_t> b;
	try {
		b.set(32);
	}
	catch (const std::exception& e) {
		STRCMP_CONTAINS("Pre-condition failed", e.what());
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
	catch (const std::exception& e) {
		STRCMP_CONTAINS("Pre-condition failed", e.what());
		return;
	}
	FAIL("failed");
}

TEST(BitPatternTest, flip_all)
{
	BitPattern<std::uint32_t> b(0x12345678);
	b.flip();
	LONGS_EQUAL(0xEDCBA987, b.data());
}

TEST(BitPatternTest, flip_bit)
{
	BitPattern<std::uint32_t> b;
	for (std::size_t i = 0; i < 32; i++) {
		b.flip(i);
		LONGS_EQUAL((1 << i), b.data());
		b.flip(i);
		LONGS_EQUAL(0, b.data());
	}
}

TEST(BitPatternTest, flip_return_this)
{
	BitPattern<std::uint32_t> b(0x12345678);
	b.flip(0).flip(1).flip(2).flip(3);
	LONGS_EQUAL(0x12345677, b.data());
}

TEST(BitPatternTest, flip_precondition_failed)
{
	BitPattern<std::uint32_t> b;
	try {
		b.flip(32);
	}
	catch (const std::exception& e) {
		STRCMP_CONTAINS("Pre-condition failed", e.what());
		return;
	}
	FAIL("failed");
}

TEST(BitPatternTest, operator_tilde)
{
	BitPattern<std::uint32_t> a(0x12345678);
	BitPattern<std::uint32_t> b = ~a;
	LONGS_EQUAL(0xEDCBA987, b.data());
}

TEST(BitPatternTest, operator_and_equal)
{
	BitPattern<std::uint32_t> a(0xAAAAAAAA);
	BitPattern<std::uint32_t> b(0x82824141);
	b &= a;
	LONGS_EQUAL(0x82820000, b.data());
}

TEST(BitPatternTest, operator_or_equal)
{
	BitPattern<std::uint32_t> a(0xAAAAAAAA);
	BitPattern<std::uint32_t> b(0x82824141);
	b |= a;
	LONGS_EQUAL(0xAAAAEBEB, b.data());
}

TEST(BitPatternTest, operator_xor_equal)
{
	BitPattern<std::uint32_t> a(0xAAAAAAAA);
	BitPattern<std::uint32_t> b(0x82824141);
	b ^= a;
	LONGS_EQUAL(0x2828EBEB, b.data());
}

TEST(BitPatternTest, operator_and_equal_return_this)
{
	BitPattern<std::uint32_t> a(0x3);
	BitPattern<std::uint32_t> b(0x2);
	(b &= a).set(0);
	LONGS_EQUAL(0x3, b.data());
}

TEST(BitPatternTest, operator_or_equal_return_this)
{
	BitPattern<std::uint32_t> a(0x3);
	BitPattern<std::uint32_t> b(0x2);
	(b |= a).reset(1);
	LONGS_EQUAL(0x1, b.data());
}

TEST(BitPatternTest, operator_xor_equal_return_this)
{
	BitPattern<std::uint32_t> a(0x3);
	BitPattern<std::uint32_t> b(0x2);
	(b ^= a).set(1);
	LONGS_EQUAL(0x3, b.data());
}

TEST(BitPatternTest, operator_left_right_shift_equal)
{
	BitPattern<std::uint32_t> b(0x1);
	for (std::size_t i = 0; i < 32; i++) {
		b <<= i;
		LONGS_EQUAL((1 << i), b.data());
		b >>= i;
		LONGS_EQUAL(1, b.data());
	}
}

TEST(BitPatternTest, operator_left_right_shift_equal_return_this)
{
	BitPattern<std::uint32_t> b(0x1);
	(b <<= 1).set(0);
	LONGS_EQUAL(0x3, b.data());
	(b >>= 1).reset(0);
	LONGS_EQUAL(0x0, b.data());
}

TEST(BitPatternTest, operator_left_right_shift)
{
	BitPattern<std::uint32_t> a(0x1);
	for (std::size_t i = 0; i < 32; i++) {
		BitPattern<std::uint32_t> b = a << i;
		LONGS_EQUAL((1 << i), b.data());
		BitPattern<std::uint32_t> c = b >> i;
		LONGS_EQUAL(1, c.data());
	}
}

TEST(BitPatternTest, operator_equal_true)
{
	BitPattern<std::uint32_t> a(0x12345678);
	BitPattern<std::uint32_t> b(0x12345678);
	bool f = (a == b);
	CHECK(f);
}

TEST(BitPatternTest, operator_equal_false)
{
	BitPattern<std::uint32_t> a(0x12345678);
	BitPattern<std::uint32_t> b(0x12345679);
	bool f = (a == b);
	CHECK_FALSE(f);
}

TEST(BitPatternTest, operator_not_equal_true)
{
	BitPattern<std::uint32_t> a(0x12345678);
	BitPattern<std::uint32_t> b(0x12345679);
	bool f = (a != b);
	CHECK(f);
}

TEST(BitPatternTest, operator_not_equal_false)
{
	BitPattern<std::uint32_t> a(0x12345678);
	BitPattern<std::uint32_t> b(0x12345678);
	bool f = (a != b);
	CHECK_FALSE(f);
}

TEST(BitPatternTest, test)
{
	BitPattern<std::uint32_t> a(0xAAAAAAAA);
	CHECK_FALSE(a.test(0));
	CHECK(a.test(1));
	CHECK_FALSE(a.test(2));
	CHECK(a.test(3));
}

TEST(BitPatternTest, test_precondition_failed)
{
	BitPattern<std::uint32_t> b;
	try {
		b.test(32);
	}
	catch (const std::exception& e) {
		STRCMP_CONTAINS("Pre-condition failed", e.what());
		return;
	}
	FAIL("failed");
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
	BitPattern<std::uint32_t> a(0xAAAAAAAA);
	BitPattern<std::uint32_t> b(0x82824141);
	BitPattern<std::uint32_t> c = b & a;
	LONGS_EQUAL(0x82820000, c.data());
}

TEST(BitPatternTest, operator_or)
{
	BitPattern<std::uint32_t> a(0xAAAAAAAA);
	BitPattern<std::uint32_t> b(0x82824141);
	BitPattern<std::uint32_t> c = b | a;
	LONGS_EQUAL(0xAAAAEBEB, c.data());
}

TEST(BitPatternTest, operator_xor)
{
	BitPattern<std::uint32_t> a(0xAAAAAAAA);
	BitPattern<std::uint32_t> b(0x82824141);
	BitPattern<std::uint32_t> c = b ^ a;
	LONGS_EQUAL(0x2828EBEB, c.data());
}

