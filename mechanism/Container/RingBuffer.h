#ifndef CONTAINER_RING_BUFFER_H_INCLUDED
#define CONTAINER_RING_BUFFER_H_INCLUDED

#include <cstddef>
#ifndef NO_STD_ITERATOR
#include <iterator>
#endif
#include "Array.h"
#include "ContainerException.h"
#include "private/TypeTraits.h"
#include "DesignByContract/Assertion.h"

namespace Container {

template <typename T, std::size_t MaxSize>
class RingBuffer;

template <typename T, typename Ref, typename Ptr, typename RBPtr, std::size_t MaxSize>
class RingBuffer_iterator {
public:
	typedef T value_type;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef RingBuffer_iterator<T, T&, T*, RingBuffer<T, MaxSize>*, MaxSize> iterator;
	typedef RingBuffer_iterator<T, const T&, const T*, const RingBuffer<T, MaxSize>*, MaxSize> const_iterator;
	typedef Ref reference;
	typedef const Ref const_reference;
	typedef Ptr pointer;
	typedef const Ptr const_pointer;
#ifndef NO_STD_ITERATOR
	typedef std::random_access_iterator_tag iterator_category;
#endif

	RingBuffer_iterator() : m_rb(0), m_idx(0U) {}

	RingBuffer_iterator(const iterator& x) : m_rb(x.m_rb), m_idx(x.m_idx) {}

	RingBuffer_iterator& operator=(const iterator& x)
	{
		m_rb = x.m_rb;
		m_idx = x.m_idx;
		return *this;
	}

	RingBuffer_iterator& operator+=(difference_type n)
	{
		DBC_PRE(m_rb != 0);
		DBC_PRE((m_rb->begin() <= *this) && (*this <= m_rb->end()));
		if (n < 0) {
			return operator-=(-n);
		}

		const size_type un = static_cast<size_type>(n);
		DBC_PRE((*this - m_rb->begin()) + un < BufSize);
		m_idx = m_rb->next_idx(m_idx, un);
		DBC_ASSERT((m_rb->begin() <= *this) && (*this <= m_rb->end()));
		return *this;
	}

	RingBuffer_iterator operator+(difference_type n) const
	{
		RingBuffer_iterator tmp = *this;
		return tmp += n;
	}

	RingBuffer_iterator& operator-=(difference_type n)
	{
		DBC_PRE(m_rb != 0);
		DBC_PRE((m_rb->begin() <= *this) && (*this <= m_rb->end()));
		if (n < 0) {
			return operator+=(-n);
		}
		DBC_PRE((*this - m_rb->begin()) >= n);

		const size_type un = static_cast<size_type>(n);
		m_idx = m_rb->prev_idx(m_idx, un);
		DBC_ASSERT((m_rb->begin() <= *this) && (*this <= m_rb->end()));
		return *this;
	}

	difference_type operator-(const RingBuffer_iterator& x) const
	{
		DBC_PRE(m_rb != 0);
		DBC_PRE(m_rb == x.m_rb);
		DBC_PRE((m_rb->begin() <= *this) && (*this <= m_rb->end()));
		DBC_PRE((x.m_rb->begin() <= x) && (x <= x.m_rb->end()));
		if (*this >= x) {
			return static_cast<difference_type>(m_rb->distance(x.m_idx, m_idx));
		}
		return -static_cast<difference_type>(m_rb->distance(m_idx, x.m_idx));
	}

	RingBuffer_iterator operator-(difference_type n) const
	{
		RingBuffer_iterator tmp = *this;
		return tmp -= n;
	}

	RingBuffer_iterator& operator++()
	{
		return operator+=(1);
	}

	RingBuffer_iterator& operator--()
	{
		return operator-=(1);
	}

	RingBuffer_iterator operator++(int)
	{
		RingBuffer_iterator tmp = *this;
		++*this;
		return tmp;
	}

	RingBuffer_iterator operator--(int)
	{
		RingBuffer_iterator tmp = *this;
		--*this;
		return tmp;
	}

	reference operator*() const
	{
		DBC_PRE(m_rb != 0);
		return m_rb->m_buf[m_idx];
	}

	pointer operator->() const
	{
		DBC_PRE(m_rb != 0);
		return &m_rb->m_buf[m_idx];
	}

	reference operator[](difference_type n) const
	{
		return *(*this + n);
	}

	bool operator==(const RingBuffer_iterator& x) const
	{
		DBC_PRE(m_rb == x.m_rb);
		return m_idx == x.m_idx;
	}

	bool operator!=(const RingBuffer_iterator& x) const
	{
		return !(*this == x);
	}

	bool operator<(const RingBuffer_iterator& x) const
	{
		DBC_PRE(m_rb != 0);
		DBC_PRE(m_rb == x.m_rb);
		return
			m_rb->distance(m_rb->m_begin, m_idx) <
			m_rb->distance(x.m_rb->m_begin, x.m_idx);
	}

	bool operator>(const RingBuffer_iterator& x) const
	{
		return x < *this;
	}

	bool operator<=(const RingBuffer_iterator& x) const
	{
		return !(x < *this);
	}

	bool operator>=(const RingBuffer_iterator& x) const
	{
		return !(*this < x);
	}

private:
	template <typename U, std::size_t N>
	friend class RingBuffer;

	template <typename U, typename RefX, typename PtrX, typename RBPtrX, std::size_t N>
	friend class RingBuffer_iterator;

	static const size_type BufSize = MaxSize + 1U;

	RBPtr m_rb;
	size_type m_idx;

	RingBuffer_iterator(RBPtr rb, size_type idx) : m_rb(rb), m_idx(idx) {}
};

template <typename T, typename Ref, typename Ptr, typename RBPtr, std::size_t MaxSize>
RingBuffer_iterator<T, Ref, Ptr, RBPtr, MaxSize>
operator+(std::ptrdiff_t n, const RingBuffer_iterator<T, Ref, Ptr, RBPtr, MaxSize>& x)
{
	return x + n;
}

template <typename T, std::size_t MaxSize>
class RingBuffer {
public:
	typedef T value_type;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef RingBuffer_iterator<T, T&, T*, RingBuffer*, MaxSize> iterator;
	typedef RingBuffer_iterator<T, const T&, const T*, const RingBuffer*, MaxSize> const_iterator;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
#ifndef NO_STD_ITERATOR
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
#endif

private:
	static const size_type BufSize = MaxSize + 1U;

	size_type m_begin;
	size_type m_end;
	Array<T, BufSize> m_buf;

	class BadAlloc : public Container::BadAlloc {
	public:
		BadAlloc() : Container::BadAlloc() {}
		const char* what() const throw()
		{
			return "RingBuffer::BadAlloc";
		}
	};
public:
	RingBuffer() : m_begin(0U), m_end(0U) {}

	RingBuffer(size_type n, const T& data = T())
	: m_begin(0U), m_end(0U)
	{
		assign(n, data);
	}

	template <typename InputIterator>
	RingBuffer(InputIterator first, InputIterator last)
	: m_begin(0U), m_end(0U)
	{
		assign(first, last);
	}

	size_type size() const
	{
		return distance(m_begin, m_end);
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
		return m_begin == m_end;
	}

	bool full() const
	{
		return size() == max_size();
	}

	void clear()
	{
		m_begin = m_end;
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
			throw OutOfRange("RingBuffer::at");
		}
		return *(begin() + idx);
	}

	const_reference at(size_type idx) const
	{
		if (idx >= size()) {
			throw OutOfRange("RingBuffer::at");
		}
		return *(begin() + idx);
	}

	iterator begin()
	{
		return iterator(this, m_begin);
	}

	const_iterator begin() const
	{
		return const_iterator(this, m_begin);
	}

	iterator end()
	{
		return iterator(this, m_end);
	}

	const_iterator end() const
	{
		return const_iterator(this, m_end);
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

	void resize(size_type n, const T& data = T())
	{
		if (size() >= n) {
			m_end = prev_idx(m_end, size() - n);
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
		m_buf[m_end] = data;
		m_end = next_idx(m_end);
	}

	void pop_back()
	{
		DBC_PRE(!empty());
		m_end = prev_idx(m_end);
	}

	void push_front(const T& data)
	{
		if (full()) {
			throw BadAlloc();
		}
		m_begin = prev_idx(m_begin);
		m_buf[m_begin] = data;
	}

	void pop_front()
	{
		DBC_PRE(!empty());
		m_begin = next_idx(m_begin);
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
		DBC_PRE((begin() <= pos) && (pos <= end()));
		return insert_n(pos, 1U, data);
	}

	void insert(iterator pos, size_type n, const T& data)
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
		const size_type n = static_cast<size_type>(last - first);
		if ((first - begin()) >= (end() - last)) {
			move_forward(last.m_idx, m_end, n);
			m_end = prev_idx(m_end, n);
			return first;
		}
		move_backward(m_begin, first.m_idx, n);
		m_begin = next_idx(m_begin, n);
		return last;
	}

	void swap(RingBuffer& other)
	{
		if (this == &other) {
			return;
		}
		m_buf.swap(other.m_buf);

		size_type tmp_begin = m_begin;
		m_begin = other.m_begin;
		other.m_begin = tmp_begin;

		size_type tmp_end = m_end;
		m_end = other.m_end;
		other.m_end = tmp_end;
	}

private:
	template <typename U, typename Ref, typename Ptr, typename RBPtr, std::size_t N>
	friend class RingBuffer_iterator;

	template <typename U, std::size_t N>
	friend bool operator==(const RingBuffer<U, N>& x, const RingBuffer<U, N>& y);

	size_type next_idx(size_type idx, size_type un = 1U) const
	{
		if (idx + un < BufSize) {
			return idx + un;
		}
		return idx + un - BufSize;
	}

	size_type prev_idx(size_type idx, size_type un = 1U) const
	{
		if (idx >= un) {
			return idx - un;
		}
		return BufSize + idx - un;
	}

	size_type distance(size_type first_idx, size_type last_idx) const
	{
		if (first_idx <= last_idx) {
			return last_idx - first_idx;
		}
		return BufSize - first_idx + last_idx;
	}

	void move_backward(size_type first, size_type last, size_type n)
	{
		const size_type stop = prev_idx(first);
		for (size_type i = prev_idx(last); i != stop; i = prev_idx(i)) {
			m_buf[next_idx(i, n)] = m_buf[i];
		}
	}

	void move_forward(size_type first, size_type last, size_type n)
	{
		for (size_type i = first; i != last; i = next_idx(i)) {
			m_buf[prev_idx(i, n)] = m_buf[i];
		}
	}

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

	iterator insert_n(iterator pos, size_type n, const T& data)
	{
		if (available_size() < n) {
			throw BadAlloc();
		}
		const size_type idx = pos.m_idx;
		if ((size() / 2U) < static_cast<size_type>(pos - begin())) {
			move_backward(idx, m_end, n);
			for (size_type i = 0U; i < n; ++i) {
				m_buf[next_idx(idx, i)] = data;
			}
			m_end = next_idx(m_end, n);
			return pos;
		}
		move_forward(m_begin, idx, n);
		for (size_type i = 0U; i < n; ++i) {
			m_buf[prev_idx(idx, n - i)] = data;
		}
		m_begin = prev_idx(m_begin, n);
		return pos - n;
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
		size_type idx = pos.m_idx;
		if ((size() / 2U) < static_cast<size_type>(pos - begin())) {
			move_backward(idx, m_end, n);
			for (; first != last; ++first) {
				m_buf[idx] = *first;
				idx = next_idx(idx);
			}
			m_end = next_idx(m_end, n);
			return;
		}
		move_forward(m_begin, idx, n);
		idx = prev_idx(idx, n);
		for (; first != last; ++first) {
			m_buf[idx] = *first;
			idx = next_idx(idx);
		}
		m_begin = prev_idx(m_begin, n);
	}

};

template <typename T, std::size_t MaxSize>
bool operator==(const RingBuffer<T, MaxSize>& x, const RingBuffer<T, MaxSize>& y)
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
bool operator!=(const RingBuffer<T, MaxSize>& x, const RingBuffer<T, MaxSize>& y)
{
	return !(x == y);
}

template <typename T, std::size_t MaxSize>
void swap(RingBuffer<T, MaxSize>& x, RingBuffer<T, MaxSize>& y)
{
	x.swap(y);
}

}

#endif // CONTAINER_RING_BUFFER_H_INCLUDED
