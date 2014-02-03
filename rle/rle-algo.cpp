#include "rle-algo.hpp"
template <typename T>
int RLE<T>::MaxRunSize()
{
	static_assert(false, "RLE not supported for this type.");
}

template <>
int RLE<char>::MaxRunSize()
{
	return 127;
}

template <>
int RLE<short>::MaxRunSize()
{
	return 32767;
}

template <>
int RLE<int>::MaxRunSize()
{
	return 2147483647;
}
