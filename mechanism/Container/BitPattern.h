#ifndef CONTAINER_BIT_PATTERN_H_INCLUDED
#define CONTAINER_BIT_PATTERN_H_INCLUDED

#include <cstddef>
#include "DesignByContract/Assertion.h"

namespace Container {

template <typename T>
class BitPattern {
private:
	T m_data;

public:
	BitPattern() : m_data(0U) {}
	explicit BitPattern(T pattern) : m_data(pattern) {}

	std::size_t size() const
	{
		return sizeof m_data * 8U;
	}

	BitPattern& set()
	{
		m_data = ~static_cast<T>(0U);
		return *this;
	}

	BitPattern& set(std::size_t pos)
	{
		DBC_PRE(pos < (sizeof m_data * 8U));
		m_data |= (1U << pos);
		return *this;
	}

	BitPattern& reset()
	{
		m_data = 0U;
		return *this;
	}

	BitPattern& reset(std::size_t pos)
	{
		DBC_PRE(pos < (sizeof m_data * 8U));
		m_data &= ~(1U << pos);
		return *this;
	}

	BitPattern& flip()
	{
		m_data = ~m_data;
		return *this;
	}

	BitPattern& flip(std::size_t pos)
	{
		DBC_PRE(pos < (sizeof m_data * 8U));
		m_data ^= (1U << pos);
		return *this;
	}

	T data() const
	{
		return m_data;
	}

	bool test(std::size_t pos) const
	{
		DBC_PRE(pos < (sizeof m_data * 8U));
		return (m_data & (1U << pos)) != 0U;
	}

	bool all() const
	{
		return m_data == ~static_cast<T>(0U);
	}

	bool any() const
	{
		return m_data != 0U;
	}

	bool none() const
	{
		return m_data == 0U;
	}

	BitPattern& operator=(const T& x)
	{
		m_data = x;
		return *this;
	}

	BitPattern operator~() const
	{
		return BitPattern(*this).flip();
	}

	BitPattern& operator&=(const BitPattern& x)
	{
		m_data &= x.m_data;
		return *this;
	}

	BitPattern& operator&=(const T& x)
	{
		m_data &= x;
		return *this;
	}

	BitPattern& operator|=(const BitPattern& x)
	{
		m_data |= x.m_data;
		return *this;
	}

	BitPattern& operator|=(const T& x)
	{
		m_data |= x;
		return *this;
	}

	BitPattern& operator^=(const BitPattern& x)
	{
		m_data ^= x.m_data;
		return *this;
	}

	BitPattern& operator^=(const T& x)
	{
		m_data ^= x;
		return *this;
	}

	BitPattern operator&(const BitPattern& x)
	{
		return BitPattern(*this) &= x;
	}

	BitPattern operator&(const T& x)
	{
		return BitPattern(*this) &= x;
	}

	BitPattern operator|(const BitPattern& x)
	{
		return BitPattern(*this) |= x;
	}

	BitPattern operator|(const T& x)
	{
		return BitPattern(*this) |= x;
	}

	BitPattern operator^(const BitPattern& x)
	{
		return BitPattern(*this) ^= x;
	}

	BitPattern operator^(const T& x)
	{
		return BitPattern(*this) ^= x;
	}

	BitPattern& operator<<=(std::size_t pos)
	{
		m_data <<= pos;
		return *this;
	}

	BitPattern& operator>>=(std::size_t pos)
	{
		m_data >>= pos;
		return *this;
	}

	BitPattern operator<<(std::size_t pos) const
	{
		return BitPattern(*this) <<= pos;
	}

	BitPattern operator>>(std::size_t pos) const
	{
		return BitPattern(*this) >>= pos;
	}

	bool operator==(const BitPattern& x) const
	{
		return m_data == x.m_data;
	}

	bool operator==(const T& x) const
	{
		return m_data == x;
	}

	bool operator!=(const BitPattern& x) const
	{
		return m_data != x.m_data;
	}

	bool operator!=(const T& x) const
	{
		return m_data != x;
	}

};

}

#endif // CONTAINER_BIT_PATTERN_H_INCLUDED
