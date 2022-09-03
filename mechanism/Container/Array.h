#ifndef CONTAINER_ARRAY_H_INCLUDED
#define CONTAINER_ARRAY_H_INCLUDED

#include <cstddef>
#ifndef CPPELIB_NO_STD_ITERATOR
#include <iterator>
#endif
#include "ContainerException.h"

namespace Container {

/*!
 * @brief Similar as std::array of C++11
 * @tparam T Type of element
 * @tparam Size Number of elements
 */
template <typename T, std::size_t Size>
struct Array {
	typedef T value_type;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef value_type* iterator;
	typedef const value_type* const_iterator;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
#ifndef CPPELIB_NO_STD_ITERATOR
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
#endif

	//! @cond
	T m_buf[Size];
	//! @endcond

	size_type size() const
	{
		return Size;
	}

	size_type max_size() const
	{
		return size();
	}

	bool empty() const
	{
		return size() == 0;
	}

	reference operator[](size_type idx)
	{
		return *(begin() + idx);
	}

	const_reference operator[](size_type idx) const
	{
		return *(begin() + idx);
	}

	reference at(size_type idx)
	{
		if (idx >= size()) {
			CPPELIB_CONTAINER_THROW(OutOfRange("Array::at"));
		}
		return *(begin() + idx);
	}

	const_reference at(size_type idx) const
	{
		if (idx >= size()) {
			CPPELIB_CONTAINER_THROW(OutOfRange("Array::at"));
		}
		return *(begin() + idx);
	}

	pointer data()
	{
		return begin();
	}

	const_pointer data() const
	{
		return begin();
	}

	iterator begin()
	{
		return &m_buf[0];
	}

	const_iterator begin() const
	{
		return &m_buf[0];
	}

	iterator end()
	{
		return &m_buf[size()];
	}

	const_iterator end() const
	{
		return &m_buf[size()];
	}

#ifndef CPPELIB_NO_STD_ITERATOR
	reverse_iterator rbegin()
	{
		return reverse_iterator(end());
	}

	const_reverse_iterator rbegin() const
	{
		return const_reverse_iterator(end());
	}

	reverse_iterator rend()
	{
		return reverse_iterator(begin());
	}

	const_reverse_iterator rend() const
	{
		return const_reverse_iterator(begin());
	}
#endif

	reference front()
	{
		return *begin();
	}

	const_reference front() const
	{
		return *begin();
	}

	reference back()
	{
		return *(end() - 1);
	}

	const_reference back() const
	{
		return *(end() - 1);
	}

	void fill(const T& data)
	{
		for (iterator i = begin(); i < end(); ++i) {
			*i = data;
		}
	}

};

template <typename T, std::size_t Size>
bool operator==(const Array<T, Size>& x, const Array<T, Size>& y)
{
	for (std::size_t i = 0U; i < Size; ++i) {
		if (!(x[i] == y[i])) {
			return false;
		}
	}
	return true;
}

template <typename T, std::size_t Size>
bool operator!=(const Array<T, Size>& x, const Array<T, Size>& y)
{
	return !(x == y);
}

}

#endif // CONTAINER_ARRAY_H_INCLUDED
