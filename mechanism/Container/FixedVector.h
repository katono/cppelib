#ifndef CONTAINER_FIXED_VECTOR_H_INCLUDED
#define CONTAINER_FIXED_VECTOR_H_INCLUDED

#include <stddef.h>
#ifndef NO_STD_ITERATOR
#include <iterator>
#endif
#include "Array.h"
#include "ContainerException.h"
#include "private/TypeTraits.h"
#include "DesignByContract/Assertion.h"

namespace Container {

template <typename T, size_t MaxSize>
class FixedVector {
public:
	typedef T value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef value_type* iterator;
	typedef const value_type* const_iterator;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
#ifndef NO_STD_ITERATOR
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
#endif

private:
	Array<value_type, MaxSize> m_buf;
	size_type m_end;

public:
	FixedVector() : m_end(0U) {}

	FixedVector(size_type n, const value_type& data = value_type())
	: m_end(0U)
	{
		assign(n, data);
	}

	template <typename InputIterator>
	FixedVector(InputIterator first, InputIterator last)
	: m_end(0U)
	{
		assign(first, last);
	}

	size_type size() const
	{
		return m_end;
	}

	size_type max_size() const
	{
		return MaxSize;
	}

	size_type available_size() const
	{
		return max_size() - size();
	}

	bool empty() const
	{
		return size() == 0;
	}

	bool full() const
	{
		return m_end == MaxSize;
	}

	void clear()
	{
		m_end = 0U;
	}

	reference operator[](size_type idx)
	{
		DBC_PRE(idx < size());
		return *(begin() + idx);
	}

	const_reference operator[](size_type idx) const
	{
		DBC_PRE(idx < size());
		return *(begin() + idx);
	}

	reference at(size_type idx)
	{
		if (idx >= size()) {
			throw OutOfRange("FixedVector::at");
		}
		return *(begin() + idx);
	}

	const_reference at(size_type idx) const
	{
		if (idx >= size()) {
			throw OutOfRange("FixedVector::at");
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
		return m_buf.begin();
	}

	const_iterator begin() const
	{
		return m_buf.begin();
	}

	iterator end()
	{
		return begin() + m_end;
	}

	const_iterator end() const
	{
		return begin() + m_end;
	}

#ifndef NO_STD_ITERATOR
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
		DBC_PRE(!empty());
		return *begin();
	}

	const_reference front() const
	{
		DBC_PRE(!empty());
		return *begin();
	}

	reference back()
	{
		DBC_PRE(!empty());
		return *(end() - 1);
	}

	const_reference back() const
	{
		DBC_PRE(!empty());
		return *(end() - 1);
	}

	void resize(size_type n, const value_type& data = value_type())
	{
		if (size() >= n) {
			m_end = n;
			return;
		}

		if (max_size() < n) {
			throw BadAlloc();
		}
		for (size_type i = m_end; i < n; ++i) {
			*(begin() + i) = data;
		}
		m_end = n;
	}

	void push_back(const value_type& data)
	{
		if (full()) {
			throw BadAlloc();
		}
		*(begin() + m_end) = data;
		++m_end;
	}

	void pop_back()
	{
		DBC_PRE(!empty());
		--m_end;
	}

	void assign(size_type n, const value_type& data)
	{
		if (max_size() < n) {
			throw BadAlloc();
		}
		clear();
		resize(n, data);
	}

	template <typename InputIterator>
	void assign(InputIterator first, InputIterator last)
	{
#ifdef NO_STD_ITERATOR
		size_type n = 0U;
		for (InputIterator i = first; i != last; ++i) {
			++n;
		}
#else
		size_type n = static_cast<size_type>(std::distance(first, last));
#endif
		if (max_size() < n) {
			throw BadAlloc();
		}
		clear();
		for (; first != last; ++first) {
			push_back(*first);
		}
	}

	iterator insert(iterator pos, const value_type& data)
	{
		DBC_PRE((begin() <= pos) && (pos <= end()));
		insert_n(pos, 1U, data);
		return pos;
	}

	void insert(iterator pos, size_type n, const value_type& data)
	{
		DBC_PRE((begin() <= pos) && (pos <= end()));
		insert_n(pos, n, data);
	}

	template <typename InputIterator>
	void insert(iterator pos, InputIterator first, InputIterator last)
	{
		DBC_PRE((begin() <= pos) && (pos <= end()));
		typedef typename IsInteger<InputIterator>::Integral Integral;
		insert_dispatch(pos, first, last, Integral());
	}

	iterator erase(iterator pos)
	{
		DBC_PRE((begin() <= pos) && (pos < end()));
		return erase(pos, pos + 1);
	}

	iterator erase(iterator first, iterator last)
	{
		DBC_PRE(first < last);
		DBC_PRE((begin() <= first) && (first < end()));
		DBC_PRE((begin() <= last) && (last <= end()));
		const difference_type n = last - first;
		move_forward(last, end(), n);
		m_end -= n;
		return first;
	}

	void swap(FixedVector& other)
	{
		if (this == &other) {
			return;
		}
		m_buf.swap(other.m_buf);
		size_type tmp = m_end;
		m_end = other.m_end;
		other.m_end = tmp;
	}

private:
	template <typename U, size_t N>
	friend bool operator==(const FixedVector<U, N>& x, const FixedVector<U, N>& y);

	void move_backward(iterator first, iterator last, difference_type n)
	{
		for (iterator i = last - 1; i != first - 1; --i) {
			*(i + n) = *i;
		}
	}

	void move_forward(iterator first, iterator last, difference_type n)
	{
		for (iterator i = first; i != last; ++i) {
			*(i - n) = *i;
		}
	}

	template <typename Integer>
	void insert_dispatch(iterator pos, Integer n, Integer data, TrueType)
	{
		insert_n(pos, static_cast<size_type>(n), static_cast<value_type>(data));
	}

	template <typename InputIterator>
	void insert_dispatch(iterator pos, InputIterator first, InputIterator last, FalseType)
	{
		insert_range(pos, first, last);
	}

	void insert_n(iterator pos, size_type n, const value_type& data)
	{
		if (available_size() < n) {
			throw BadAlloc();
		}
		move_backward(pos, end(), static_cast<difference_type>(n));
		for (iterator i = pos; i < pos + n; ++i) {
			*i = data;
		}
		m_end += n;
	}

	template <typename InputIterator>
	void insert_range(iterator pos, InputIterator first, InputIterator last)
	{
#ifdef NO_STD_ITERATOR
		size_type n = 0U;
		for (InputIterator i = first; i != last; ++i) {
			++n;
		}
#else
		size_type n = static_cast<size_type>(std::distance(first, last));
#endif
		if (available_size() < n) {
			throw BadAlloc();
		}
		move_backward(pos, end(), static_cast<difference_type>(n));
		for (; first != last; ++first) {
			*pos = *first;
			++pos;
		}
		m_end += n;
	}

};

template <typename T, size_t MaxSize>
bool operator==(const FixedVector<T, MaxSize>& x, const FixedVector<T, MaxSize>& y)
{
	if (x.size() != y.size()) {
		return false;
	}
	for (size_t i = 0U; i < x.size(); ++i) {
		if (!(x[i] == y[i])) {
			return false;
		}
	}
	return true;
}

template <typename T, size_t MaxSize>
bool operator!=(const FixedVector<T, MaxSize>& x, const FixedVector<T, MaxSize>& y)
{
	return !(x == y);
}

template <typename T, size_t MaxSize>
void swap(FixedVector<T, MaxSize>& x, FixedVector<T, MaxSize>& y)
{
	x.swap(y);
}

}

#endif // CONTAINER_FIXED_VECTOR_H_INCLUDED
