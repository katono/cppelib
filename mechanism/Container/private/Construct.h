#ifndef CONTAINER_CONSTRUCT_H_INCLUDED
#define CONTAINER_CONSTRUCT_H_INCLUDED

namespace Container {

template <typename T1, typename T2>
void construct(T1* p, const T2& val)
{
	new(p) T1(val);
}

template <typename T>
void construct(T* p)
{
	new(p) T();
}

template <typename T>
void destroy(T* p)
{
	p->~T();
}

template <typename ForwardIterator>
void destroy(ForwardIterator first, ForwardIterator last)
{
	for (; first != last; ++first) {
		destroy(&*first);
	}
}

}

#endif // CONTAINER_CONSTRUCT_H_INCLUDED
