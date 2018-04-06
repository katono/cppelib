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
	: m_realBuf(), m_virtualBuf(*reinterpret_cast<T(*)[MaxSize]>(&m_realBuf)), m_end(x.m_end)
	{
		for (std::size_t i = 0U; i < x.size(); ++i) {
			construct(&operator[](i), x[i]);
		}
	}

	~FixedVector()
	{
		destroy(begin(), end());
	}

	FixedVector& operator=(const FixedVector& x)
	{
		if (this == &x) {
			return *this;
		}
		if (size() < x.size()) {
			for (std::size_t i = 0U; i < x.size(); ++i) {
				if (i < size()) {
					operator[](i) = x[i];
				} else {
					construct(&operator[](i), x[i]);
				}
			}
		} else {
			const std::size_t n = x.size();
			for (std::size_t i = 0U; i < n; ++i) {
				operator[](i) = x[i];
			}
			destroy(begin() + n, end());
		}
		m_end = x.m_end;
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
		return m_end == MaxSize;
	}

	void clear()
	{
		destroy(begin(), end());
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
		CHECK_PRECOND(!empty());
		return *begin();
	}

	const_reference front() const
	{
		CHECK_PRECOND(!empty());
		return *begin();
	}

	reference back()
	{
		CHECK_PRECOND(!empty());
		return *(end() - 1);
	}

	const_reference back() const
	{
		CHECK_PRECOND(!empty());
		return *(end() - 1);
	}

	void resize(size_type n, const T& data = T())
	{
		if (size() >= n) {
			destroy(begin() + n, end());
			m_end = n;
			return;
		}

		if (max_size() < n) {
			throw BadAlloc();
		}
		for (size_type i = m_end; i < n; ++i) {
			construct(&*(begin() + i), data);
		}
		m_end = n;
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
		CHECK_PRECOND(!empty());
		--m_end;
		destroy(&*end());
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

	iterator insert(iterator pos, const T& data)
	{
		CHECK_PRECOND((begin() <= pos) && (pos <= end()));
		insert_n(pos, 1U, data);
		return pos;
	}

	void insert(iterator pos, size_type n, const T& data)
	{
		CHECK_PRECOND((begin() <= pos) && (pos <= end()));
		insert_n(pos, n, data);
	}

	template <typename InputIterator>
	void insert(iterator pos, InputIterator first, InputIterator last)
	{
		CHECK_PRECOND((begin() <= pos) && (pos <= end()));
		typedef typename IsInteger<InputIterator>::Integral Integral;
		insert_dispatch(pos, first, last, Integral());
	}

	iterator erase(iterator pos)
	{
		CHECK_PRECOND((begin() <= pos) && (pos < end()));
		return erase(pos, pos + 1);
	}

	iterator erase(iterator first, iterator last)
	{
		CHECK_PRECOND(first < last);
		CHECK_PRECOND((begin() <= first) && (first < end()));
		CHECK_PRECOND((begin() <= last) && (last <= end()));
		const difference_type n = last - first;
		for (iterator i = last; i != end(); ++i) {
			*(i - n) = *i;
		}
		destroy(end() - n, end());
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

		for (iterator i = end() - 1; i != pos - 1; --i) {
			if ((i + n) < end()) {
				*(i + n) = *i;
			} else {
				construct(&*(i + n), *i);
			}
		}

		for (iterator i = pos; i < pos + n; ++i) {
			if (i < end()) {
				*i = data;
			} else {
				construct(&*i, data);
			}
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

		for (iterator i = end() - 1; i != pos - 1; --i) {
			if ((i + n) < end()) {
				*(i + n) = *i;
			} else {
				construct(&*(i + n), *i);
			}
		}

		for (; first != last; ++first) {
			if (pos < end()) {
				*pos = *first;
			} else {
				construct(&*pos, *first);
			}
			++pos;
		}

		m_end += n;
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
