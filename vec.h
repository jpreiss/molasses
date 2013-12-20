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

	Vec(float x, float y, float z = 0, float w = 1) :
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
		float len2 = length2();
		if (len2 == 0)
		{
			return *this;
		}
		return *this / sqrt(len2);
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

bool operator<(Vec const &a, Vec const &b)
{
	return a.x < b.x && a.y < b.y && a.z < b.z;
}

bool operator>(Vec const &a, Vec const &b)
{
	return b < a;
}

bool operator<=(Vec const &a, Vec const &b)
{
	return a.x <= b.x && a.y <= b.y && a.z <= b.z;
}

bool operator>=(Vec const &a, Vec const &b)
{
	return b <= a;
}

// spherical coordinates:
// azimuth from 0 to 2pi
// elevation from 0 (ceiling) to pi (floor)
// so elevations match image coords for environment maps
// radius
Vec toSpherical(Vec const &cartesian)
{
	float radius = cartesian.length();
	Vec cnorm = cartesian / radius;
	float elevation = acos(cnorm.z);
	float azimuth = atan2(cnorm.y, cnorm.x) + PI;
	return Vec(azimuth, elevation, radius);
}