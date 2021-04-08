#ifndef CONTAINER_FIXED_VECTOR_H_INCLUDED
#define CONTAINER_FIXED_VECTOR_H_INCLUDED

#include <cstddef>
#ifndef NO_STD_ITERATOR
#include <iterator>
#endif
#include "ContainerException.h"
#include "private/TypeTraits.h"
#include "private/Construct.h"
#include "Assertion/Assertion.h"

namespace Container {

/*!
 * @brief STL-like vector container with fixed capacity
 * @tparam T Type of element
 * @tparam MaxSize Max of elements that can be stored (that is decided at compile-time)
 *
 * Almost all the method specification is similar as STL vector,
 * but this container can not expand the capacity.
 * Over capacity addition of element throws the exception derived from std::exception.
 */
template <typename T, std::size_t MaxSize>
class FixedVector {
public:
	typedef T value_type;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
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
	union InternalBuf {
		double dummyForAlignment;
		char buf[sizeof(T) * MaxSize];
	};
	InternalBuf m_realBuf;
	T (&m_virtualBuf)[MaxSize];
	size_type m_end;

	class BadAlloc : public Container::BadAlloc {
	public:
		BadAlloc() : Container::BadAlloc() {}
		const char* what() const throw()
		{
			return "FixedVector::BadAlloc";
		}
	};

public:
	FixedVector()
	: m_realBuf(), m_virtualBuf(*reinterpret_cast<T(*)[MaxSize]>(&m_realBuf)), m_end(0U)
	{}

	FixedVector(size_type n, const T& data = T())
	: m_realBuf(), m_virtualBuf(*reinterpret_cast<T(*)[MaxSize]>(&m_realBuf)), m_end(0U)
	{
		assign(n, data);
	}

	template <typename InputIterator>
	FixedVector(InputIterator first, InputIterator last)
	: m_realBuf(), m_virtualBuf(*reinterpret_cast<T(*)[MaxSize]>(&m_realBuf)), m_end(0U)
	{
		assign(first, last);
	}

	FixedVector(const FixedVector& x)
	: m_realBuf(), m_virtualBuf(*reinterpret_cast<T(*)[MaxSize]>(&m_realBuf)), m_end(0U)
	{
		assign(x.begin(), x.end());
	}

	~FixedVector()
	{
		destroy_range(begin(), end());
	}

	FixedVector& operator=(const FixedVector& x)
	{
		if (this != &x) {
			assign(x.begin(), x.end());
		}
		return *this;
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
		return size() == max_size();
	}

	void clear()
	{
		destroy_range(begin(), end());
		m_end = 0U;
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
		return &m_virtualBuf[0];
	}

	const_iterator begin() const
	{
		return &m_virtualBuf[0];
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
		DEBUG_ASSERT(!empty());
		return *begin();
	}

	const_reference front() const
	{
		DEBUG_ASSERT(!empty());
		return *begin();
	}

	reference back()
	{
		DEBUG_ASSERT(!empty());
		return *(end() - 1);
	}

	const_reference back() const
	{
		DEBUG_ASSERT(!empty());
		return *(end() - 1);
	}

	void resize(size_type n, const T& data = T())
	{
		if (size() >= n) {
			destroy_range(begin() + n, end());
			m_end = n;
			return;
		}

		if (max_size() < n) {
			throw BadAlloc();
		}
		const size_type rest = n - size();
		for (size_type i = 0U; i < rest; ++i) {
			push_back(data);
		}
	}

	void push_back(const T& data)
	{
		if (full()) {
			throw BadAlloc();
		}
		construct(&*end(), data);
		++m_end;
	}

	void pop_back()
	{
		DEBUG_ASSERT(!empty());
		destroy(&*(end() - 1));
		--m_end;
	}

	void assign(size_type n, const T& data)
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
		size_type n = 0U;
		for (InputIterator i = first; i != last; ++i) {
			++n;
		}
		if (max_size() < n) {
			throw BadAlloc();
		}
		clear();
		for (; first != last; ++first) {
			push_back(*first);
		}
	}

	iterator insert(iterator pos, const T& data)
	{
		DEBUG_ASSERT((begin() <= pos) && (pos <= end()));
		insert_n(pos, 1U, data);
		return pos;
	}

	void insert(iterator pos, size_type n, const T& data)
	{
		DEBUG_ASSERT((begin() <= pos) && (pos <= end()));
		insert_n(pos, n, data);
	}

	template <typename InputIterator>
	void insert(iterator pos, InputIterator first, InputIterator last)
	{
		DEBUG_ASSERT((begin() <= pos) && (pos <= end()));
		typedef typename IsInteger<InputIterator>::Integral Integral;
		insert_dispatch(pos, first, last, Integral());
	}

	iterator erase(iterator pos)
	{
		DEBUG_ASSERT((begin() <= pos) && (pos < end()));
		return erase(pos, pos + 1);
	}

	iterator erase(iterator first, iterator last)
	{
		DEBUG_ASSERT(first < last);
		DEBUG_ASSERT((begin() <= first) && (first < end()));
		DEBUG_ASSERT((begin() <= last) && (last <= end()));
		const difference_type n = last - first;
		for (iterator i = last; i != end(); ++i) {
			*(i - n) = *i;
		}
		destroy_range(end() - n, end());
		m_end -= n;
		return first;
	}

private:
	template <typename U, std::size_t N>
	friend bool operator==(const FixedVector<U, N>& x, const FixedVector<U, N>& y);

	template <typename Integer>
	void insert_dispatch(iterator pos, Integer n, Integer data, TrueType)
	{
		insert_n(pos, static_cast<size_type>(n), static_cast<T>(data));
	}

	template <typename InputIterator>
	void insert_dispatch(iterator pos, InputIterator first, InputIterator last, FalseType)
	{
		insert_range(pos, first, last);
	}

	void insert_n(iterator pos, size_type n, const T& data)
	{
		if (available_size() < n) {
			throw BadAlloc();
		}

		const size_type num_elems_pos_to_end = end() - pos;
		iterator old_end = end();
		if (num_elems_pos_to_end > n) {
			for (size_type i = 0U; i < n; ++i) {
				construct(&*end());
				++m_end;
			}
			for (iterator it = old_end - 1; it != pos - 1; --it) {
				*(it + n) = *it;
			}
			for (iterator it = pos; it != pos + n; ++it) {
				*it = data;
			}
		} else {
			for (size_type i = 0U; i < n - num_elems_pos_to_end; ++i) {
				construct(&*end(), data);
				++m_end;
			}
			for (iterator it = pos; it != pos + num_elems_pos_to_end; ++it) {
				construct(&*end(), *it);
				++m_end;
			}
			for (iterator it = pos; it != old_end; ++it) {
				*it = data;
			}
		}
	}

	template <typename InputIterator>
	void insert_range(iterator pos, InputIterator first, InputIterator last)
	{
		size_type n = 0U;
		for (InputIterator i = first; i != last; ++i) {
			++n;
		}
		if (available_size() < n) {
			throw BadAlloc();
		}

		const size_type num_elems_pos_to_end = end() - pos;
		iterator old_end = end();
		if (num_elems_pos_to_end > n) {
			for (size_type i = 0U; i < n; ++i) {
				construct(&*end());
				++m_end;
			}
			for (iterator it = old_end - 1; it != pos - 1; --it) {
				*(it + n) = *it;
			}
			for (; first != last; ++pos, ++first) {
				*pos = *first;
			}
		} else {
			InputIterator mid = first;
			for (size_type i = 0U; i < num_elems_pos_to_end; ++i) {
				++mid;
			}
			for (size_type i = 0U; i < n - num_elems_pos_to_end; ++i) {
				construct(&*end(), *mid);
				++mid;
				++m_end;
			}
			for (iterator it = pos; it != pos + num_elems_pos_to_end; ++it) {
				construct(&*end(), *it);
				++m_end;
			}
			for (iterator it = pos; it != old_end; ++it, ++first) {
				*it = *first;
			}
		}
	}

};

template <typename T, std::size_t MaxSize>
bool operator==(const FixedVector<T, MaxSize>& x, const FixedVector<T, MaxSize>& y)
{
	if (x.size() != y.size()) {
		return false;
	}
	for (std::size_t i = 0U; i < x.size(); ++i) {
		if (!(x[i] == y[i])) {
			return false;
		}
	}
	return true;
}

template <typename T, std::size_t MaxSize>
bool operator!=(const FixedVector<T, MaxSize>& x, const FixedVector<T, MaxSize>& y)
{
	return !(x == y);
}

}

#endif // CONTAINER_FIXED_VECTOR_H_INCLUDED
