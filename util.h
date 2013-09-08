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

template <typename T>
T clamp(T const &val, T const &minimum, T const &maximum)
{
	return max(min(val, maximum), minimum);
}

template <typename T, typename Big>
T clampToLimits(Big big)
{
	T max = std::numeric_limits<T>::max();
	T min = std::numeric_limits<T>::min();

	if (big < (Big)min)
		return min;

	if (big > (Big)max)
		return max;

	return (T)big;
}

template <typename T, typename Big>
T clampAdd(T a, T b)
{
	Big add = (Big)a + (Big)b;
	return clampToLimits<T, Big>(add);
}

template <typename T, typename Big>
T clampSubtract(T a, T b)
{
	Big subtract = (Big)a - (Big)b;
	return clampToLimits<T, Big>(subtract);
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

bool almostEqual(float a, float b)
{
	return abs(a - b) <= std::numeric_limits<float>::epsilon();
}