#ifndef CONTAINER_TYPE_TRAITS_H_INCLUDED
#define CONTAINER_TYPE_TRAITS_H_INCLUDED

namespace Container {

struct TrueType {};
struct FalseType {};

template <typename T> struct IsInteger {
	typedef FalseType Integral;
};

template <> struct IsInteger<char> {
	typedef TrueType Integral;
};

template <> struct IsInteger<signed char> {
	typedef TrueType Integral;
};

template <> struct IsInteger<unsigned char> {
	typedef TrueType Integral;
};

template <> struct IsInteger<short> {
	typedef TrueType Integral;
};

template <> struct IsInteger<unsigned short> {
	typedef TrueType Integral;
};

template <> struct IsInteger<int> {
	typedef TrueType Integral;
};

template <> struct IsInteger<unsigned int> {
	typedef TrueType Integral;
};

template <> struct IsInteger<long> {
	typedef TrueType Integral;
};

template <> struct IsInteger<unsigned long> {
	typedef TrueType Integral;
};

#if (__cplusplus >= 201103L) || !defined(CPPELIB_NO_LONG_LONG)
template <> struct IsInteger<long long> {
	typedef TrueType Integral;
};

template <> struct IsInteger<unsigned long long> {
	typedef TrueType Integral;
};
#endif

}

#endif // CONTAINER_TYPE_TRAITS_H_INCLUDED
