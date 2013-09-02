#include "vec.h"
#include "array2d.h"
#include "image.h"
#include "util.h"
#include "triangle.h"

float zinterp(Triangle const &t, Vec const &bary)
{
	float z =
		t.a.z * bary.x +
		t.b.z * bary.y +
		t.c.z * bary.z;

	return z;
}

// vecs should already be in image space
// (not just projection space)
void rasterize(Triangle const &t, Array2D<float> &zbuf, Image &view, ColorRGBA color)
{	
	auto bbox = bounds(t.asVecs(), t.asVecs() + 3);
	Vec mins = bbox.first;
	Vec maxes = bbox.second;

	// need shadow rules for perfect int coords
	for (int x = ceil(mins.x); x <= floor(maxes.x); ++x)
	{
		for (int y = ceil(mins.y); y <= floor(maxes.y); ++y)
		{
			Vec pt(x, y, 0);

			Vec bary = t.barycentricXY(pt);

			if (bary.x < 0 || bary.y < 0 || bary.z < 0)
			{
				// outside tri
				continue;
			}
		
			pt.z = zinterp(t, bary);

			// z test
			if (pt.z < zbuf(x, y))
			{
				zbuf(x, y) = pt.z;
				view(x, y) = color;
			}
		}
	}
}