#ifndef CONTAINER_LIST_H_INCLUDED
#define CONTAINER_LIST_H_INCLUDED

#include <stddef.h>
#ifndef NO_STD_ITERATOR
#include <iterator>
#endif
#include "DesignByContract/Assertion.h"

namespace Container {

class ListItem {
private:
	ListItem* m_nextListItem;
	ListItem* m_prevListItem;

	template <typename T, typename Ref, typename Ptr, typename ListItemPtr>
	friend class List_iterator;

	template <typename T>
	friend class List;
};

template <typename T, typename Ref, typename Ptr, typename ListItemPtr>
class List_iterator {
public:
	typedef T value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef List_iterator<T, T&, T*, ListItem*> iterator;
	typedef List_iterator<T, const T&, const T*, const ListItem*> const_iterator;
	typedef Ref reference;
	typedef const Ref const_reference;
	typedef Ptr pointer;
	typedef const Ptr const_pointer;
	typedef ListItemPtr ListItemPointer;
#ifndef NO_STD_ITERATOR
	typedef std::bidirectional_iterator_tag iterator_category;
#endif

	List_iterator() : m_item(0) {}

	List_iterator(const iterator& x) : m_item(x.m_item) {}

	List_iterator& operator++()
	{
		DBC_PRE(m_item != 0);
		m_item = m_item->m_nextListItem;
		return *this;
	}

	List_iterator& operator--()
	{
		DBC_PRE(m_item != 0);
		m_item = m_item->m_prevListItem;
		return *this;
	}

	List_iterator operator++(int)
	{
		List_iterator tmp = *this;
		++*this;
		return tmp;
	}

	List_iterator operator--(int)
	{
		List_iterator tmp = *this;
		--*this;
		return tmp;
	}

	reference operator*() const
	{
		DBC_PRE(m_item != 0);
		return *static_cast<pointer>(m_item);
	}

	pointer operator->() const
	{
		DBC_PRE(m_item != 0);
		return static_cast<pointer>(m_item);
	}

	bool operator==(const List_iterator& x) const
	{
		return m_item == x.m_item;
	}

	bool operator!=(const List_iterator& x) const
	{
		return !(*this == x);
	}

private:
	template <typename U>
	friend class List;

	template <typename U, typename RefX, typename PtrX, typename ListItemPtrX>
	friend class List_iterator;

	ListItemPointer m_item;

	List_iterator(ListItemPointer item) : m_item(item) {}
};

template <typename T>
class List {
public:
	typedef T value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef List_iterator<T, T&, T*, ListItem*> iterator;
	typedef List_iterator<T, const T&, const T*, const ListItem*> const_iterator;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
#ifndef NO_STD_ITERATOR
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
#endif

	List()
	{
		m_terminator.m_nextListItem = &m_terminator;
		m_terminator.m_prevListItem = &m_terminator;
	}

	bool empty() const
	{
		return m_terminator.m_nextListItem == &m_terminator;
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
		return iterator(m_terminator.m_nextListItem);
	}

	const_iterator begin() const
	{
		return const_iterator(m_terminator.m_nextListItem);
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
		return *--end();
	}

	const_reference back() const
	{
		DBC_PRE(!empty());
		return *--end();
	}

	void push_back(value_type& data)
	{
		insert(end(), data);
	}

	void pop_back()
	{
		DBC_PRE(!empty());
		erase(--end());
	}

	void push_front(value_type& data)
	{
		insert(begin(), data);
	}

	void pop_front()
	{
		DBC_PRE(!empty());
		erase(begin());
	}

	iterator insert(iterator pos, value_type& data)
	{
		DBC_PRE(pos.m_item != 0);
		data.m_nextListItem = pos.m_item;
		data.m_prevListItem = pos.m_item->m_prevListItem;
		pos.m_item->m_prevListItem = &data;
		data.m_prevListItem->m_nextListItem = &data;
		return iterator(&data);
	}

	iterator erase(iterator pos)
	{
		DBC_PRE(!empty());
		DBC_PRE(pos.m_item != 0);
		ListItem* item = pos.m_item->m_nextListItem;
		pos.m_item->m_prevListItem->m_nextListItem = pos.m_item->m_nextListItem;
		pos.m_item->m_nextListItem->m_prevListItem = pos.m_item->m_prevListItem;
		return iterator(item);
	}

	void splice(iterator pos, List& x)
	{
		DBC_PRE(this != &x);
		if (x.empty()) {
			return;
		}
		splice(pos, x, x.begin(), x.end());
	}

	void splice(iterator pos, List& x, iterator i)
	{
		DBC_PRE(i != end());
		DBC_PRE(i != x.end());
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

	void splice(iterator pos, List& x, iterator first, iterator last)
	{
		(void)x;
		DBC_PRE(pos.m_item != 0);
		DBC_PRE(first.m_item != 0);
		DBC_PRE(last.m_item != 0);
		DBC_PRE(first != end());
		DBC_PRE(first != x.end());
		if (first == last) {
			return;
		}
		if (pos == last) {
			return;
		}
		last.m_item->m_prevListItem->m_nextListItem = pos.m_item;
		first.m_item->m_prevListItem->m_nextListItem = last.m_item;
		pos.m_item->m_prevListItem->m_nextListItem = first.m_item;

		ListItem* tmp = pos.m_item->m_prevListItem;
		pos.m_item->m_prevListItem = last.m_item->m_prevListItem;
		last.m_item->m_prevListItem = first.m_item->m_prevListItem;
		first.m_item->m_prevListItem = tmp;
	}

	void swap(List& other)
	{
		if (this == &other) {
			return;
		}
		List tmp;
		tmp.splice(tmp.end(), other);
		other.splice(other.end(), *this);
		splice(end(), tmp);
	}

private:
	ListItem m_terminator;

	List(const List& x);
	List& operator=(const List& x);
};

template <typename T>
void swap(List<T>& x, List<T>& y)
{
	x.swap(y);
}

}

#endif // CONTAINER_LIST_H_INCLUDED
