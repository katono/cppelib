#ifndef CONTAINER_TYPE_TRAITS_H_INCLUDED
#define CONTAINER_TYPE_TRAITS_H_INCLUDED

namespace Container {

struct true_type {};
struct false_type {};

template <typename T> struct IsInteger {
	typedef false_type Integral;
};

template <> struct IsInteger<char> {
	typedef true_type Integral;
};

template <> struct IsInteger<signed char> {
	typedef true_type Integral;
};

template <> struct IsInteger<unsigned char> {
	typedef true_type Integral;
};

template <> struct IsInteger<short> {
	typedef true_type Integral;
};

template <> struct IsInteger<unsigned short> {
	typedef true_type Integral;
};

template <> struct IsInteger<int> {
	typedef true_type Integral;
};

template <> struct IsInteger<unsigned int> {
	typedef true_type Integral;
};

template <> struct IsInteger<long> {
	typedef true_type Integral;
};

template <> struct IsInteger<unsigned long> {
	typedef true_type Integral;
};

#ifdef USE_LONG_LONG
template <> struct IsInteger<long long> {
	typedef true_type Integral;
};

template <> struct IsInteger<unsigned long long> {
	typedef true_type Integral;
};
#endif

}

#endif // CONTAINER_TYPE_TRAITS_H_INCLUDED
