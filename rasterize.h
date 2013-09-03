#include "vec.h"
#include "array2d.h"
#include "util.h"
#include "triangle.h"

template <typename T>
T baryInterp(T const &a, T const &b, T const &c, Vec const &bary)
{
	return bary.x * a + bary.y * b + bary.z * c;
}

float zinterp(Triangle const &t, Vec const &bary)
{
	return baryInterp(t.a.z, t.b.z, t.c.z, bary);
}

// Interpolants must define operator+ and operator*(float, Interpolants)
// not sure how I like this but we'll see
// vecs should already be in image space
// (not just projection space)

template <typename Interpolants>
void rasterize(Triangle const &t, Interpolants *interps, Array2D<float> &zbuf, Array2D<Interpolants> &fragments)
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
				fragments(x, y) = baryInterp(interps[0], interps[1], interps[2], bary);
			}
		}
	}
}