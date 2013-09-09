#pragma once

#include "vec.h"
#include <limits>
#include <algorithm>

float const FltMin = std::numeric_limits<float>::min();
float const FltMax = std::numeric_limits<float>::max();

class Bounds
{
public:
	Vec mins;
	Vec maxes;

	// default ctor: empty
	Bounds() :
		mins(FltMax, FltMax, FltMax),
		maxes(FltMin, FltMin, FltMin)
	{
	}

	bool empty() const
	{
		return mins[0] > maxes[0];
	}

	void merge(Vec const &v)
	{
		mins = vmin(v, mins);
		maxes = vmax(v, maxes);
	}

	bool contains(Vec const &v) const
	{
		return v <= maxes && v >= mins;
	}

	void corners(Vec c[8]) const
	{
		Vec v[] = { mins, maxes };

		for (int x = 0; x < 2; ++x)
		for (int y = 0; y < 2; ++y)
		for (int z = 0; z < 2; ++z)
		{
			c[x * 4 + y * 2 + z] = Vec(v[x].x, v[y].y, v[z].z);
		}
	}

	bool intersects(Bounds const &other) const
	{
		Vec thisCorners[8];
		Vec otherCorners[8];
		corners(thisCorners);
		other.corners(otherCorners);

		bool us = std::any_of(otherCorners, otherCorners + 8, [&](Vec const &v) { return contains(v); });
		bool them = std::any_of(thisCorners, thisCorners + 8, [&](Vec const &v) { return other.contains(v); });

		return us || them;
	}

	template <typename Iter>
	static Bounds fromIterators(Iter begin, Iter end)
	{
		Bounds b;

		for (Iter it = begin; it != end; ++it)
		{
			b.merge(*it);
		}

		return b;
	}
};