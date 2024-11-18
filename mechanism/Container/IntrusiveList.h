#ifndef CONTAINER_INTRUSIVE_LIST_H_INCLUDED
#define CONTAINER_INTRUSIVE_LIST_H_INCLUDED

#include <cstddef>
#ifndef CPPELIB_NO_STD_ITERATOR
#include <iterator>
#endif
#include "Assertion/Assertion.h"

namespace Container {

/*!
 * @brief Base class of the type of element of IntrusiveList
 *
 * @note This class has links.
 */
class IntrusiveListNode {
private:
	IntrusiveListNode* m_nextListNode;
	IntrusiveListNode* m_prevListNode;

	template <typename T, typename Ref, typename Ptr, typename IntrusiveListNodePtr>
	friend class IntrusiveList_iterator;

	template <typename T>
	friend class IntrusiveList;

protected:
	IntrusiveListNode() : m_nextListNode(), m_prevListNode() {}
};

/*!
 * @brief Bidirectional iterator used as IntrusiveList<T>::iterator or IntrusiveList<T>::const_iterator
 */
template <typename T, typename Ref, typename Ptr, typename IntrusiveListNodePtr>
class IntrusiveList_iterator {
public:
	typedef T value_type;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef IntrusiveList_iterator<T, T&, T*, IntrusiveListNode*> iterator;
	typedef IntrusiveList_iterator<T, const T&, const T*, const IntrusiveListNode*> const_iterator;
	typedef Ref reference;
	typedef const Ref const_reference;
	typedef Ptr pointer;
	typedef const Ptr const_pointer;
#ifndef CPPELIB_NO_STD_ITERATOR
	typedef std::bidirectional_iterator_tag iterator_category;
#endif

	IntrusiveList_iterator() : m_node(0) {}

	IntrusiveList_iterator(const iterator& x) : m_node(x.m_node) {} // cppcheck-suppress noExplicitConstructor

	IntrusiveList_iterator& operator=(const iterator& x)
	{
		m_node = x.m_node;
		return *this;
	}

	IntrusiveList_iterator& operator++()
	{
		DEBUG_ASSERT(m_node != 0);
		m_node = m_node->m_nextListNode;
		return *this;
	}

	IntrusiveList_iterator& operator--()
	{
		DEBUG_ASSERT(m_node != 0);
		m_node = m_node->m_prevListNode;
		return *this;
	}

	IntrusiveList_iterator operator++(int)
	{
		IntrusiveList_iterator tmp = *this;
		++*this;
		return tmp;
	}

	IntrusiveList_iterator operator--(int)
	{
		IntrusiveList_iterator tmp = *this;
		--*this;
		return tmp;
	}

	reference operator*() const
	{
		DEBUG_ASSERT(m_node != 0);
		return *static_cast<pointer>(m_node);
	}

	pointer operator->() const
	{
		DEBUG_ASSERT(m_node != 0);
		return static_cast<pointer>(m_node);
	}

	bool operator==(const IntrusiveList_iterator& x) const
	{
		return m_node == x.m_node;
	}

	bool operator!=(const IntrusiveList_iterator& x) const
	{
		return !(*this == x);
	}

private:
	template <typename U>
	friend class IntrusiveList;

	template <typename U, typename RefX, typename PtrX, typename IntrusiveListNodePtrX>
	friend class IntrusiveList_iterator;

	IntrusiveListNodePtr m_node;

	explicit IntrusiveList_iterator(IntrusiveListNodePtr node) : m_node(node) {}
};

/*!
 * @brief STL-like intrusive doubly linked list
 * @tparam T Type of element (that must be derived from IntrusiveListNode)
 *
 * @note The caller needs to prepare elements.
 *       IntrusiveList does not allocate and release elements.
 */
template <typename T>
class IntrusiveList {
public:
	typedef T value_type;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef IntrusiveList_iterator<T, T&, T*, IntrusiveListNode*> iterator;
	typedef IntrusiveList_iterator<T, const T&, const T*, const IntrusiveListNode*> const_iterator;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
#ifndef CPPELIB_NO_STD_ITERATOR
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
#endif

	IntrusiveList() : m_terminator()
	{
		m_terminator.m_nextListNode = &m_terminator;
		m_terminator.m_prevListNode = &m_terminator;
	}

	bool empty() const
	{
		return m_terminator.m_nextListNode == &m_terminator;
	}

	size_type size() const
	{
		size_type num = 0U;
		for (const_iterator i = begin(); i != end(); ++i) {
			++num;
		}
		return num;
	}

	iterator begin()
	{
		return iterator(m_terminator.m_nextListNode);
	}

	const_iterator begin() const
	{
		return const_iterator(m_terminator.m_nextListNode);
	}

	iterator end()
	{
		return iterator(&m_terminator);
	}

	const_iterator end() const
	{
		return const_iterator(&m_terminator);
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
		return *--end();
	}

	const_reference back() const
	{
		DEBUG_ASSERT(!empty());
		return *--end();
	}

	void push_back(T& data)
	{
		insert(end(), data);
	}

	void pop_back()
	{
		DEBUG_ASSERT(!empty());
		erase(--end());
	}

	void push_front(T& data)
	{
		insert(begin(), data);
	}

	void pop_front()
	{
		DEBUG_ASSERT(!empty());
		erase(begin());
	}

	iterator insert(iterator pos, T& data)
	{
		DEBUG_ASSERT(pos.m_node != 0);
		data.m_nextListNode = pos.m_node;
		data.m_prevListNode = pos.m_node->m_prevListNode;
		pos.m_node->m_prevListNode = &data;
		data.m_prevListNode->m_nextListNode = &data;
		return iterator(&data);
	}

	iterator erase(iterator pos)
	{
		DEBUG_ASSERT(!empty());
		DEBUG_ASSERT(pos.m_node != 0);
		IntrusiveListNode* node = pos.m_node->m_nextListNode;
		pos.m_node->m_prevListNode->m_nextListNode = pos.m_node->m_nextListNode;
		pos.m_node->m_nextListNode->m_prevListNode = pos.m_node->m_prevListNode;
		return iterator(node);
	}

	void splice(iterator pos, IntrusiveList& x)
	{
		DEBUG_ASSERT(this != &x);
		if (x.empty()) {
			return;
		}
		splice(pos, x, x.begin(), x.end());
	}

	void splice(iterator pos, IntrusiveList& x, iterator i)
	{
		DEBUG_ASSERT(i != end());
		DEBUG_ASSERT(i != x.end());
		if (pos == i) {
			return;
		}
		iterator j = i;
		++j;
		if (pos == j) {
			return;
		}
		splice(pos, x, i, j);
	}

	void splice(iterator pos, IntrusiveList& x, iterator first, iterator last) // cppcheck-suppress constParameterReference
	{
		DEBUG_ASSERT(pos.m_node != 0);
		DEBUG_ASSERT(first.m_node != 0);
		DEBUG_ASSERT(last.m_node != 0);
		DEBUG_ASSERT(first != end());
		DEBUG_ASSERT(first != x.end());
		(void) x;
		if (first == last) {
			return;
		}
		if (pos == last) {
			return;
		}
		last.m_node->m_prevListNode->m_nextListNode = pos.m_node;
		first.m_node->m_prevListNode->m_nextListNode = last.m_node;
		pos.m_node->m_prevListNode->m_nextListNode = first.m_node;

		IntrusiveListNode* tmp = pos.m_node->m_prevListNode;
		pos.m_node->m_prevListNode = last.m_node->m_prevListNode;
		last.m_node->m_prevListNode = first.m_node->m_prevListNode;
		first.m_node->m_prevListNode = tmp;
	}

	void swap(IntrusiveList& other)
	{
		if (this == &other) {
			return;
		}
		IntrusiveList tmp;
		tmp.splice(tmp.end(), other);
		other.splice(other.end(), *this);
		splice(end(), tmp);
	}

private:
	IntrusiveListNode m_terminator;

	IntrusiveList(const IntrusiveList& x);
	IntrusiveList& operator=(const IntrusiveList& x);
};

template <typename T>
void swap(IntrusiveList<T>& x, IntrusiveList<T>& y)
{
	x.swap(y);
}

}

#endif // CONTAINER_INTRUSIVE_LIST_H_INCLUDED
