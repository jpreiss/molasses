#pragma once

#include <limits>

template <typename T>
T max(T const &a, T const &b)
{
	return a < b ? b : a;
}

template <typename T>
T min(T const &a, T const &b)
{
	return a > b ? b : a;
}

template <typename T, typename Big>
T clampAdd(T a, T b)
{
	Big add = (Big)a + (Big)b;
	T max = std::numeric_limits<T>::max();
	T min = std::numeric_limits<T>::min();

	if (add < (Big)min)
		return min;

	if (add > (Big)max)
		return max;

	return (T)add;
}

template <typename T, typename Big>
T clampSubtract(T a, T b)
{
	Big subtract = (Big)a - (Big)b;
	T max = std::numeric_limits<T>::max();
	T min = std::numeric_limits<T>::min();

	if (subtract < (Big)min)
		return min;

	if (subtract > (Big)max)
		return max;

	return (T)subtract;
}

static const double PI = 3.14159265358979323846264338327950288;

double radians(double degrees)
{
	return degrees * (PI / 180.0);
}

double degrees(double radians)
{
	return radians * (180.0 / PI);
}