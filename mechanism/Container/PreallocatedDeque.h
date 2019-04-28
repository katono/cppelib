#ifndef CONTAINER_PREALLOCATED_DEQUE_H_INCLUDED
#define CONTAINER_PREALLOCATED_DEQUE_H_INCLUDED

#include <cstddef>
#ifndef NO_STD_ITERATOR
#include <iterator>
#endif
#include "ContainerException.h"
#include "private/TypeTraits.h"
#include "private/Construct.h"
#include "Assertion/Assertion.h"

namespace Container {

template <typename T>
class PreallocatedDeque;

/*!
 * @brief Random-access iterator used as PreallocatedDeque<T>::iterator or PreallocatedDeque<T>::const_iterator
 */
template <typename T, typename Ref, typename Ptr, typename DeqPtr>
class PreallocatedDeque_iterator {
public:
	typedef T value_type;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef PreallocatedDeque_iterator<T, T&, T*, PreallocatedDeque<T>*> iterator;
	typedef PreallocatedDeque_iterator<T, const T&, const T*, const PreallocatedDeque<T>*> const_iterator;
	typedef Ref reference;
	typedef const Ref const_reference;
	typedef Ptr pointer;
	typedef const Ptr const_pointer;
#ifndef NO_STD_ITERATOR
	typedef std::random_access_iterator_tag iterator_category;
#endif

	PreallocatedDeque_iterator() : m_deq(0), m_idx(0U) {}

	PreallocatedDeque_iterator(const iterator& x) : m_deq(x.m_deq), m_idx(x.m_idx) {}

	PreallocatedDeque_iterator& operator=(const iterator& x)
	{
		m_deq = x.m_deq;
		m_idx = x.m_idx;
		return *this;
	}

	PreallocatedDeque_iterator& operator+=(difference_type n)
	{
		DEBUG_ASSERT(m_deq != 0);
		if (n < 0) {
			return operator-=(-n);
		}

		const size_type un = static_cast<size_type>(n);
		m_idx = m_deq->next_idx(m_idx, un);
		return *this;
	}

	PreallocatedDeque_iterator operator+(difference_type n) const
	{
		PreallocatedDeque_iterator tmp = *this;
		return tmp += n;
	}

	PreallocatedDeque_iterator& operator-=(difference_type n)
	{
		DEBUG_ASSERT(m_deq != 0);
		if (n < 0) {
			return operator+=(-n);
		}

		const size_type un = static_cast<size_type>(n);
		m_idx = m_deq->prev_idx(m_idx, un);
		return *this;
	}

	difference_type operator-(const PreallocatedDeque_iterator& x) const
	{
		DEBUG_ASSERT(m_deq != 0);
		DEBUG_ASSERT(m_deq == x.m_deq);
		if (*this >= x) {
			return static_cast<difference_type>(m_deq->distance_idx(x.m_idx, m_idx));
		}
		return -static_cast<difference_type>(m_deq->distance_idx(m_idx, x.m_idx));
	}

	PreallocatedDeque_iterator operator-(difference_type n) const
	{
		PreallocatedDeque_iterator tmp = *this;
		return tmp -= n;
	}

	PreallocatedDeque_iterator& operator++()
	{
		return operator+=(1);
	}

	PreallocatedDeque_iterator& operator--()
	{
		return operator-=(1);
	}

	PreallocatedDeque_iterator operator++(int)
	{
		PreallocatedDeque_iterator tmp = *this;
		++*this;
		return tmp;
	}

	PreallocatedDeque_iterator operator--(int)
	{
		PreallocatedDeque_iterator tmp = *this;
		--*this;
		return tmp;
	}

	reference operator*() const
	{
		DEBUG_ASSERT(m_deq != 0);
		return m_deq->m_buf[m_idx];
	}

	pointer operator->() const
	{
		DEBUG_ASSERT(m_deq != 0);
		return &m_deq->m_buf[m_idx];
	}

	reference operator[](difference_type n) const
	{
		return *(*this + n);
	}

	bool operator==(const PreallocatedDeque_iterator& x) const
	{
		return (m_deq == x.m_deq) && (m_idx == x.m_idx);
	}

	bool operator!=(const PreallocatedDeque_iterator& x) const
	{
		return !(*this == x);
	}

	bool operator<(const PreallocatedDeque_iterator& x) const
	{
		DEBUG_ASSERT(m_deq != 0);
		DEBUG_ASSERT(m_deq == x.m_deq);
		return
			m_deq->distance_idx(m_deq->m_begin, m_idx) <
			m_deq->distance_idx(x.m_deq->m_begin, x.m_idx);
	}

	bool operator>(const PreallocatedDeque_iterator& x) const
	{
		return x < *this;
	}

	bool operator<=(const PreallocatedDeque_iterator& x) const
	{
		return !(x < *this);
	}

	bool operator>=(const PreallocatedDeque_iterator& x) const
	{
		return !(*this < x);
	}

private:
	template <typename U>
	friend class PreallocatedDeque;

	template <typename U, typename RefX, typename PtrX, typename DeqPtrX>
	friend class PreallocatedDeque_iterator;

	DeqPtr m_deq;
	size_type m_idx;

	PreallocatedDeque_iterator(DeqPtr deq, size_type idx) : m_deq(deq), m_idx(idx) {}
};

template <typename T, typename Ref, typename Ptr, typename DeqPtr>
PreallocatedDeque_iterator<T, Ref, Ptr, DeqPtr>
operator+(std::ptrdiff_t n, const PreallocatedDeque_iterator<T, Ref, Ptr, DeqPtr>& x)
{
	return x + n;
}

/*!
 * @brief STL-like deque container using pre-allocated buffer
 * @tparam T Type of element
 *
 * Almost all the method specification is similar as STL deque,
 * but this container can not expand the capacity.
 * Over capacity addition of element throws the exception derived from std::exception.
 */
template <typename T>
class PreallocatedDeque {
public:
	typedef T value_type;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef PreallocatedDeque_iterator<T, T&, T*, PreallocatedDeque*> iterator;
	typedef PreallocatedDeque_iterator<T, const T&, const T*, const PreallocatedDeque*> const_iterator;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
#ifndef NO_STD_ITERATOR
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
#endif

private:
	T* m_buf;
	size_type m_buf_size;
	size_type m_begin;
	size_type m_end;

	class BadAlloc : public Container::BadAlloc {
	public:
		BadAlloc() : Container::BadAlloc() {}
		const char* what() const throw()
		{
			return "PreallocatedDeque::BadAlloc";
		}
	};

	PreallocatedDeque(const PreallocatedDeque& x);

public:
	/*!
	 * @brief Default constructor
	 * @attention If you use default constructor, you need to call init() before other method call.
	 */
	PreallocatedDeque()
	: m_buf(0), m_buf_size(0U), m_begin(0U), m_end(0U)
	{}

	/*!
	 * @brief Constructor
	 * @param preallocated_buffer Pre-allocated buffer by caller
	 * @param buffer_size Size of preallocated_buffer
	 * @attention preallocated_buffer must be aligned on the boundary of type T.
	 */
	PreallocatedDeque(void* preallocated_buffer, size_type buffer_size)
	: m_buf(static_cast<T*>(preallocated_buffer)), m_buf_size(buffer_size), m_begin(0U), m_end(0U)
	{
	}

	/*!
	 * @brief Destructor
	 * @note All elements are erased, but pre-allocated buffer is not released.
	 */
	~PreallocatedDeque()
	{
		destroy(begin(), end());
	}

	/*!
	 * @brief Initialize
	 * @param preallocated_buffer Pre-allocated buffer by caller
	 * @param buffer_size Size of preallocated_buffer
	 * @attention preallocated_buffer must be aligned on the boundary of type T.
	 * @attention If you use default constructor, you need to call init() before other method call.
	 * @note Pre-allocated buffer can be set only one time.
	 */
	void init(void* preallocated_buffer, size_type buffer_size)
	{
		if (m_buf != 0) {
			return;
		}
		m_buf = static_cast<T*>(preallocated_buffer);
		m_buf_size = buffer_size;
	}

	PreallocatedDeque& operator=(const PreallocatedDeque& x)
	{
		if (this != &x) {
			assign(x.begin(), x.end());
		}
		return *this;
	}

	size_type size() const
	{
		return distance_idx(m_begin, m_end);
	}

	size_type max_size() const
	{
		return (m_buf_size / sizeof(T)) - 1U;
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
		destroy(begin(), end());
		m_end = m_begin;
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
			throw OutOfRange("PreallocatedDeque::at");
		}
		return *(begin() + idx);
	}

	const_reference at(size_type idx) const
	{
		if (idx >= size()) {
			throw OutOfRange("PreallocatedDeque::at");
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
			destroy(begin() + n, end());
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
		construct(&*end(), data);
		m_end = next_idx(m_end);
	}

	void pop_back()
	{
		DEBUG_ASSERT(!empty());
		destroy(&*(end() - 1));
		m_end = prev_idx(m_end);
	}

	void push_front(const T& data)
	{
		if (full()) {
			throw BadAlloc();
		}
		construct(&*(begin() - 1), data);
		m_begin = prev_idx(m_begin);
	}

	void pop_front()
	{
		DEBUG_ASSERT(!empty());
		destroy(&*begin());
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
		return insert_n(pos, 1U, data);
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
		const size_type n = static_cast<size_type>(last - first);
		if ((first - begin()) >= (end() - last)) {
			// move the end side
			for (iterator i = last; i != end(); ++i) {
				*(i - n) = *i;
			}
			destroy(end() - n, end());
			m_end = prev_idx(m_end, n);
			return first;
		} else {
			// move the begin side
			iterator stop = begin() - 1;
			for (iterator i = first - 1; i != stop; --i) {
				*(i + n) = *i;
			}
			destroy(begin(), begin() + n);
			m_begin = next_idx(m_begin, n);
			return last;
		}
	}

private:
	template <typename U, typename Ref, typename Ptr, typename DeqPtr>
	friend class PreallocatedDeque_iterator;

	template <typename U>
	friend bool operator==(const PreallocatedDeque<U>& x, const PreallocatedDeque<U>& y);

	size_type num_elems_in_buf() const
	{
		return m_buf_size / sizeof(T);
	}

	size_type next_idx(size_type idx, size_type un = 1U) const
	{
		if (idx + un < num_elems_in_buf()) {
			return idx + un;
		}
		// wraparound
		return idx + un - num_elems_in_buf();
	}

	size_type prev_idx(size_type idx, size_type un = 1U) const
	{
		if (idx >= un) {
			return idx - un;
		}
		// wraparound
		return num_elems_in_buf() + idx - un;
	}

	size_type distance_idx(size_type first_idx, size_type last_idx) const
	{
		if (first_idx <= last_idx) {
			return last_idx - first_idx;
		}
		// wraparound
		return num_elems_in_buf() - first_idx + last_idx;
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

		if ((size() / 2U) < static_cast<size_type>(pos - begin())) {
			// move the end side
			const size_type num_elems_pos_to_end = end() - pos;
			iterator old_end = end();
			if (num_elems_pos_to_end > n) {
				for (size_type i = 0U; i < n; ++i) {
					construct(&*end());
					m_end = next_idx(m_end);
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
					m_end = next_idx(m_end);
				}
				for (iterator it = pos; it != pos + num_elems_pos_to_end; ++it) {
					construct(&*end(), *it);
					m_end = next_idx(m_end);
				}
				for (iterator it = pos; it != old_end; ++it) {
					*it = data;
				}
			}
			return pos;
		} else {
			// move the begin side
			const size_type num_elems_beg_to_pos = pos - begin();
			iterator old_begin = begin();
			if (num_elems_beg_to_pos > n) {
				for (size_type i = 0U; i < n; ++i) {
					construct(&*(begin() - 1));
					m_begin = prev_idx(m_begin);
				}
				for (iterator it = old_begin; it != pos; ++it) {
					*(it - n) = *it;
				}
				for (iterator it = pos - n; it != pos; ++it) {
					*it = data;
				}
			} else {
				for (size_type i = 0U; i < n - num_elems_beg_to_pos; ++i) {
					construct(&*(begin() - 1), data);
					m_begin = prev_idx(m_begin);
				}
				for (iterator it = pos - 1; it != old_begin - 1; --it) {
					construct(&*(begin() - 1), *it);
					m_begin = prev_idx(m_begin);
				}
				for (iterator it = old_begin; it != pos; ++it) {
					*it = data;
				}
			}
			return pos - n;
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

		if ((size() / 2U) < static_cast<size_type>(pos - begin())) {
			// move the end side
			const size_type num_elems_pos_to_end = end() - pos;
			iterator old_end = end();
			if (num_elems_pos_to_end > n) {
				for (size_type i = 0U; i < n; ++i) {
					construct(&*end());
					m_end = next_idx(m_end);
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
					m_end = next_idx(m_end);
				}
				for (iterator it = pos; it != pos + num_elems_pos_to_end; ++it) {
					construct(&*end(), *it);
					m_end = next_idx(m_end);
				}
				for (iterator it = pos; it != old_end; ++it, ++first) {
					*it = *first;
				}
			}
		} else {
			// move the begin side
			const size_type num_elems_beg_to_pos = pos - begin();
			iterator old_begin = begin();
			if (num_elems_beg_to_pos > n) {
				for (size_type i = 0U; i < n; ++i) {
					construct(&*(begin() - 1));
					m_begin = prev_idx(m_begin);
				}
				for (iterator it = old_begin; it != pos; ++it) {
					*(it - n) = *it;
				}
				for (iterator it = pos - n; it != pos; ++it, ++first) {
					*it = *first;
				}
			} else {
				InputIterator mid = first;
				for (size_type i = 0U; i < n - num_elems_beg_to_pos; ++i) {
					++mid;
				}
				InputIterator p = mid;
				for (size_type i = 0U; i < n - num_elems_beg_to_pos; ++i) {
					construct(&*(begin() - 1), *--p);
					m_begin = prev_idx(m_begin);
				}
				for (iterator it = pos - 1; it != old_begin - 1; --it) {
					construct(&*(begin() - 1), *it);
					m_begin = prev_idx(m_begin);
				}
				for (iterator it = old_begin; it != pos; ++it, ++mid) {
					*it = *mid;
				}
			}
		}
	}

};

template <typename T>
bool operator==(const PreallocatedDeque<T>& x, const PreallocatedDeque<T>& y)
{
	if (x.max_size() != y.max_size()) {
		return false;
	}
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

template <typename T>
bool operator!=(const PreallocatedDeque<T>& x, const PreallocatedDeque<T>& y)
{
	return !(x == y);
}

}

#endif // CONTAINER_PREALLOCATED_DEQUE_H_INCLUDED
