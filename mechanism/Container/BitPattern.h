#ifndef CONTAINER_BIT_PATTERN_H_INCLUDED
#define CONTAINER_BIT_PATTERN_H_INCLUDED

#include <cstddef>
#include "Assertion/Assertion.h"

namespace Container {

/*!
 * @brief The container with bitwise operation like std::bitset
 *
 * Size of BitPattern<T> is the bit width of type T.
 *
 * @tparam T Unsigned integer type with any width
 */
template <typename T>
class BitPattern {
private:
	T m_data;

public:
	BitPattern() : m_data(0U) {}
	BitPattern(T pattern) : m_data(pattern) {} // cppcheck-suppress noExplicitConstructor

	std::size_t size() const
	{
		return sizeof m_data * 8U;
	}

	BitPattern& set()
	{
		m_data = static_cast<T>(~static_cast<T>(0U));
		return *this;
	}

	BitPattern& set(std::size_t pos)
	{
		DEBUG_ASSERT(pos < (sizeof m_data * 8U));
		m_data |= static_cast<T>(static_cast<T>(1U) << pos);
		return *this;
	}

	BitPattern& reset()
	{
		m_data = 0U;
		return *this;
	}

	BitPattern& reset(std::size_t pos)
	{
		DEBUG_ASSERT(pos < (sizeof m_data * 8U));
		m_data &= static_cast<T>(~(static_cast<T>(1U) << pos));
		return *this;
	}

	BitPattern& flip()
	{
		m_data = static_cast<T>(~m_data);
		return *this;
	}

	BitPattern& flip(std::size_t pos)
	{
		DEBUG_ASSERT(pos < (sizeof m_data * 8U));
		m_data ^= static_cast<T>(static_cast<T>(1U) << pos);
		return *this;
	}

	operator const T&() const
	{
		return m_data;
	}

	operator T&()
	{
		return m_data;
	}

	bool test(std::size_t pos) const
	{
		DEBUG_ASSERT(pos < (sizeof m_data * 8U));
		return (m_data & static_cast<T>(static_cast<T>(1U) << pos)) != 0U;
	}

	bool all() const
	{
		return m_data == static_cast<T>(~static_cast<T>(0U));
	}

	bool any() const
	{
		return m_data != 0U;
	}

	bool none() const
	{
		return m_data == 0U;
	}

};

}

#endif // CONTAINER_BIT_PATTERN_H_INCLUDED
