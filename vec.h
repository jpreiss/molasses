#pragma once

#include <math.h>
#include "util.h"

// 4-element vector.  Want to SIMD some day.

// fwd declaration so we can use non-member functions in member functions
class Vec;
float dot(Vec const &, Vec const &);
Vec operator-(Vec const &, Vec const &);
Vec operator/(Vec const &, float);
Vec operator*(float, Vec const &);

class Vec
{
public:
	Vec() :
		x(0), y(0), z(0), w(1)
	{
	}

	Vec(float x, float y, float z, float w = 1) :
		x(x), y(y), z(z), w(w)
	{
	}

	Vec(float const *arr) :
		x(arr[0]), y(arr[1]), z(arr[2]), w(arr[3])
	{
	}

	float length2() const
	{
		return dot(*this, *this);
	}

	float length() const
	{
		return sqrt(length2());
	}

	Vec normalized() const
	{
		return *this / length();
	}

	Vec projectedTo(Vec const &other) const
	{
		Vec otherNorm = other.normalized();
		float dp = dot(*this, otherNorm);
		return dp * otherNorm;
	}

	Vec normalTo(Vec const &other) const
	{
		return *this - this->projectedTo(other);
	}

	float const *asMemory() const
	{
		return reinterpret_cast<float const *>(this);
	}

	float const &operator[](int i) const
	{
		return asMemory()[i];
	}
	
	float x;
	float y;
	float z;
	float w;
};

Vec operator -(Vec const &a)
{
	return Vec(-a.x, -a.y, -a.z);
}

Vec operator *(float s, Vec const &a)
{
	return Vec(s * a.x, s * a.y, s * a.z, s * a.w);
}

Vec operator /(Vec const &a, float s)
{
	return (1.0 / s) * a;
}

Vec operator +(Vec const &a, Vec const &b)
{
	return Vec(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vec operator -(Vec const &a, Vec const &b)
{
	return Vec(a.x - b.x, a.y - b.y, a.z - b.z);
}

float dot(Vec const &a, Vec const &b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec cross(Vec const &a, Vec const &b)
{
	return Vec(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x);
}

Vec vmax(Vec const &a, Vec const &b)
{
	return Vec(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
}

Vec vmin(Vec const &a, Vec const &b)
{
	return Vec(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
}

template <typename Iter>
std::pair<Vec, Vec> bounds(Iter begin, Iter end)
{
	const float fmax = std::numeric_limits<float>::max();
	const float fmin = std::numeric_limits<float>::min();

	Vec mins(fmax, fmax, fmax);
	Vec maxes(fmin, fmin, fmin);

	for (Iter it = begin; it != end; ++it)
	{
		mins = vmin(mins, *it);
		maxes = vmax(maxes, *it);
	}

	return std::make_pair(mins, maxes);
}