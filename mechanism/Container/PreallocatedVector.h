#ifndef CONTAINER_PREALLOCATED_VECTOR_H_INCLUDED
#define CONTAINER_PREALLOCATED_VECTOR_H_INCLUDED

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
 * @brief STL-like vector container using pre-allocated buffer
 * @tparam T Type of element
 *
 * Almost all the method specification is similar as STL vector,
 * but this container can not expand the capacity.
 * Over capacity addition of element throws the exception derived from std::exception.
 */
template <typename T>
class PreallocatedVector {
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
	T* m_buf;
	size_type m_buf_size;
	size_type m_end;

	class BadAlloc : public Container::BadAlloc {
	public:
		BadAlloc() : Container::BadAlloc() {}
		const char* what() const CONTAINER_NOEXCEPT
		{
			return "PreallocatedVector::BadAlloc";
		}
	};

	PreallocatedVector(const PreallocatedVector& x);

public:
	/*!
	 * @brief Default constructor
	 * @attention If you use default constructor, you need to call init() before other method call.
	 */
	PreallocatedVector()
	: m_buf(0), m_buf_size(0U), m_end(0U)
	{
	}

	/*!
	 * @brief Constructor
	 * @param preallocated_buffer Pre-allocated buffer by caller
	 * @param buffer_size Size of preallocated_buffer
	 * @attention preallocated_buffer must be aligned on the boundary of type T.
	 */
	PreallocatedVector(void* preallocated_buffer, size_type buffer_size)
	: m_buf(static_cast<T*>(preallocated_buffer)), m_buf_size(buffer_size), m_end(0U)
	{
	}

	/*!
	 * @brief Destructor
	 * @note All elements are erased, but pre-allocated buffer is not released.
	 */
	~PreallocatedVector()
	{
		destroy_range(begin(), end());
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

	PreallocatedVector& operator=(const PreallocatedVector& x)
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
		return m_buf_size / sizeof(T);
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
			CONTAINER_THROW(OutOfRange("PreallocatedVector::at"));
		}
		return *(begin() + idx);
	}

	const_reference at(size_type idx) const
	{
		if (idx >= size()) {
			CONTAINER_THROW(OutOfRange("PreallocatedVector::at"));
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
			CONTAINER_THROW(BadAlloc());
		}
		const size_type rest = n - size();
		for (size_type i = 0U; i < rest; ++i) {
			push_back(data);
		}
	}

	void push_back(const T& data)
	{
		if (full()) {
			CONTAINER_THROW(BadAlloc());
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
			CONTAINER_THROW(BadAlloc());
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
			CONTAINER_THROW(BadAlloc());
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
	template <typename U>
	friend bool operator==(const PreallocatedVector<U>& x, const PreallocatedVector<U>& y);

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
			CONTAINER_THROW(BadAlloc());
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
			CONTAINER_THROW(BadAlloc());
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

template <typename T>
bool operator==(const PreallocatedVector<T>& x, const PreallocatedVector<T>& y)
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
bool operator!=(const PreallocatedVector<T>& x, const PreallocatedVector<T>& y)
{
	return !(x == y);
}

}

#endif // CONTAINER_PREALLOCATED_VECTOR_H_INCLUDED
