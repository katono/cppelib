#ifndef CONTAINER_DOUBLY_LINKEDLIST_H_INCLUDED
#define CONTAINER_DOUBLY_LINKEDLIST_H_INCLUDED

#include <cstddef>
#ifndef NO_STD_ITERATOR
#include <iterator>
#endif
#include "Assertion/Assertion.h"

namespace Container {

class DoublyLinkedListNode {
private:
	DoublyLinkedListNode* m_nextListNode;
	DoublyLinkedListNode* m_prevListNode;

	template <typename T, typename Ref, typename Ptr, typename DoublyLinkedListNodePtr>
	friend class DoublyLinkedList_iterator;

	template <typename T>
	friend class DoublyLinkedList;

protected:
	DoublyLinkedListNode() : m_nextListNode(), m_prevListNode() {}
};

template <typename T, typename Ref, typename Ptr, typename DoublyLinkedListNodePtr>
class DoublyLinkedList_iterator {
public:
	typedef T value_type;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef DoublyLinkedList_iterator<T, T&, T*, DoublyLinkedListNode*> iterator;
	typedef DoublyLinkedList_iterator<T, const T&, const T*, const DoublyLinkedListNode*> const_iterator;
	typedef Ref reference;
	typedef const Ref const_reference;
	typedef Ptr pointer;
	typedef const Ptr const_pointer;
#ifndef NO_STD_ITERATOR
	typedef std::bidirectional_iterator_tag iterator_category;
#endif

	DoublyLinkedList_iterator() : m_node(0) {}

	DoublyLinkedList_iterator(const iterator& x) : m_node(x.m_node) {}

	DoublyLinkedList_iterator& operator=(const iterator& x)
	{
		m_node = x.m_node;
		return *this;
	}

	DoublyLinkedList_iterator& operator++()
	{
		CHECK_PRECOND(m_node != 0);
		m_node = m_node->m_nextListNode;
		return *this;
	}

	DoublyLinkedList_iterator& operator--()
	{
		CHECK_PRECOND(m_node != 0);
		m_node = m_node->m_prevListNode;
		return *this;
	}

	DoublyLinkedList_iterator operator++(int)
	{
		DoublyLinkedList_iterator tmp = *this;
		++*this;
		return tmp;
	}

	DoublyLinkedList_iterator operator--(int)
	{
		DoublyLinkedList_iterator tmp = *this;
		--*this;
		return tmp;
	}

	reference operator*() const
	{
		CHECK_PRECOND(m_node != 0);
		return *static_cast<pointer>(m_node);
	}

	pointer operator->() const
	{
		CHECK_PRECOND(m_node != 0);
		return static_cast<pointer>(m_node);
	}

	bool operator==(const DoublyLinkedList_iterator& x) const
	{
		return m_node == x.m_node;
	}

	bool operator!=(const DoublyLinkedList_iterator& x) const
	{
		return !(*this == x);
	}

private:
	template <typename U>
	friend class DoublyLinkedList;

	template <typename U, typename RefX, typename PtrX, typename DoublyLinkedListNodePtrX>
	friend class DoublyLinkedList_iterator;

	DoublyLinkedListNodePtr m_node;

	explicit DoublyLinkedList_iterator(DoublyLinkedListNodePtr node) : m_node(node) {}
};

template <typename T>
class DoublyLinkedList {
public:
	typedef T value_type;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef DoublyLinkedList_iterator<T, T&, T*, DoublyLinkedListNode*> iterator;
	typedef DoublyLinkedList_iterator<T, const T&, const T*, const DoublyLinkedListNode*> const_iterator;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
#ifndef NO_STD_ITERATOR
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
#endif

	DoublyLinkedList() : m_terminator()
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
		return *--end();
	}

	const_reference back() const
	{
		CHECK_PRECOND(!empty());
		return *--end();
	}

	void push_back(T& data)
	{
		insert(end(), data);
	}

	void pop_back()
	{
		CHECK_PRECOND(!empty());
		erase(--end());
	}

	void push_front(T& data)
	{
		insert(begin(), data);
	}

	void pop_front()
	{
		CHECK_PRECOND(!empty());
		erase(begin());
	}

	iterator insert(iterator pos, T& data)
	{
		CHECK_PRECOND(pos.m_node != 0);
		data.m_nextListNode = pos.m_node;
		data.m_prevListNode = pos.m_node->m_prevListNode;
		pos.m_node->m_prevListNode = &data;
		data.m_prevListNode->m_nextListNode = &data;
		return iterator(&data);
	}

	iterator erase(iterator pos)
	{
		CHECK_PRECOND(!empty());
		CHECK_PRECOND(pos.m_node != 0);
		DoublyLinkedListNode* node = pos.m_node->m_nextListNode;
		pos.m_node->m_prevListNode->m_nextListNode = pos.m_node->m_nextListNode;
		pos.m_node->m_nextListNode->m_prevListNode = pos.m_node->m_prevListNode;
		return iterator(node);
	}

	void splice(iterator pos, DoublyLinkedList& x)
	{
		CHECK_PRECOND(this != &x);
		if (x.empty()) {
			return;
		}
		splice(pos, x, x.begin(), x.end());
	}

	void splice(iterator pos, DoublyLinkedList& x, iterator i)
	{
		CHECK_PRECOND(i != end());
		CHECK_PRECOND(i != x.end());
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

	void splice(iterator pos, DoublyLinkedList& x, iterator first, iterator last)
	{
		CHECK_PRECOND(pos.m_node != 0);
		CHECK_PRECOND(first.m_node != 0);
		CHECK_PRECOND(last.m_node != 0);
		CHECK_PRECOND(first != end());
		CHECK_PRECOND(first != x.end());
		if (first == last) {
			return;
		}
		if (pos == last) {
			return;
		}
		last.m_node->m_prevListNode->m_nextListNode = pos.m_node;
		first.m_node->m_prevListNode->m_nextListNode = last.m_node;
		pos.m_node->m_prevListNode->m_nextListNode = first.m_node;

		DoublyLinkedListNode* tmp = pos.m_node->m_prevListNode;
		pos.m_node->m_prevListNode = last.m_node->m_prevListNode;
		last.m_node->m_prevListNode = first.m_node->m_prevListNode;
		first.m_node->m_prevListNode = tmp;
	}

	void swap(DoublyLinkedList& other)
	{
		if (this == &other) {
			return;
		}
		DoublyLinkedList tmp;
		tmp.splice(tmp.end(), other);
		other.splice(other.end(), *this);
		splice(end(), tmp);
	}

private:
	DoublyLinkedListNode m_terminator;

	DoublyLinkedList(const DoublyLinkedList& x);
	DoublyLinkedList& operator=(const DoublyLinkedList& x);
};

template <typename T>
void swap(DoublyLinkedList<T>& x, DoublyLinkedList<T>& y)
{
	x.swap(y);
}

}

#endif // CONTAINER_DOUBLY_LINKEDLIST_H_INCLUDED
