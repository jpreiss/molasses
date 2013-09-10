#pragma once

#include "vec.h"
#include <limits>
#include <algorithm>

float const FltLow = std::numeric_limits<float>::lowest();
float const FltMax = std::numeric_limits<float>::max();

class Bounds
{
public:
	Vec mins;
	Vec maxes;

	// default ctor: empty
	Bounds() :
		mins(FltMax, FltMax, FltMax),
		maxes(FltLow, FltLow, FltLow)
	{
	}

	bool empty() const
	{
		return mins[0] > maxes[0] 
		    || mins[1] > maxes[1]
		    || mins[2] > maxes[2];
	}

	void merge(Vec const &v)
	{
		mins = vmin(v, mins);
		maxes = vmax(v, maxes);
	}

	bool contains(Vec const &v) const
	{
		return mins <= v && v <= maxes;
	}

	bool intersects(Bounds const &other) const
	{
		return !intersection(other).empty();
	}

	Bounds intersection(Bounds const &other) const
	{
		Bounds b;
		b.mins = vmax(mins, other.mins);
		b.maxes = vmin(maxes, other.maxes);
		return b;
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