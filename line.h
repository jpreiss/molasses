#pragma once

#include "vec.h"

class Line
{
public:
	Line(Vec const &s, Vec const &e) : start(s), end(e) {}
	Vec start;
	Vec end;

	float length2()
	{
		return (end - start).length2();
	}

	float length()
	{
		return (end - start).length();
	}
};

/* Not needed yet, wrote before I decided to ditch scan line rasterizer
   for barycentric

bool isOnLeft(Vec const &start, Vec const &end, Vec const &v)
{
	Vec start2D(start.x, start.y, 0);
	Vec end2D(end.x, end.y, 0);

	Vec normal = cross(Vec(0, 0, 1), end2D - start2D);
	Vec test = v - start;
	float dp = dot(normal, test);
	return dp > 0;
}

bool onSameSide(Line const &l, Vec const &a, Vec const &b)
{
	// not sure all this projecting to 2D is necessary

	Vec start2D(l.start.x, l.start.y, 0);
	Vec end2D(l.end.x, l.end.y, 0);
	Vec a2D(a.x, a.y, 0);
	Vec b2D(b.x, b.y, 0);
	
	Vec dir = end2D - start2D;
	Vec toA = a2D - start2D;
	Vec toB = b2D - start2D;

	return dot(toA.normalTo(dir), toB.normalTo(dir)) > 0;
}
*/