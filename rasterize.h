#include "vec.h"
#include "array2d.h"
#include "util.h"
#include "triangle.h"

template <typename T>
T perspectiveCorrectBaryInterp(T const &a, T const &b, T const &c, Vec const &bary, Vec const &ws)
{
	Vec invWs(1 / ws.x, 1 / ws.y, 1 / ws.z);

	float invWInterp = baryInterp(invWs.x, invWs.y, invWs.z, bary);

	T val = 
		(invWs.x * bary.x * a) + 
		(invWs.y * bary.y * b) + 
		(invWs.z * bary.z * c);

	T correct = (1.0 / invWInterp) * val;

	return correct;
}

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
void rasterize(Triangle const &t, Interpolants *interps, Array2D<float> &zbuf, Array2D<Interpolants> &fragments, Mat const &toScreen)
{	
	Vec ws(t.a.w, t.b.w, t.c.w);

	Triangle tscreen = t;
	tscreen.a = tscreen.a / tscreen.a.w;
	tscreen.b = tscreen.b / tscreen.b.w;
	tscreen.c = tscreen.c / tscreen.c.w;
	tscreen = toScreen * tscreen;

	auto bbox = bounds(tscreen.asVecs(), tscreen.asVecs() + 3);
	Vec mins = bbox.first;
	Vec maxes = bbox.second;

	// need shadow rules for perfect int coords
	for (int x = ceil(mins.x); x <= floor(maxes.x); ++x)
	{
		for (int y = ceil(mins.y); y <= floor(maxes.y); ++y)
		{
			Vec pt(x, y, 0);

			Vec bary = tscreen.barycentricXY(pt);

			if (bary.x < 0 || bary.y < 0 || bary.z < 0)
			{
				// outside tri
				continue;
			}
		
			pt.z = zinterp(tscreen, bary);

			// z test
			if (pt.z < zbuf(x, y))
			{
				zbuf(x, y) = pt.z;
				fragments(x, y) = perspectiveCorrectBaryInterp(interps[0], interps[1], interps[2], bary, ws);
			}
		}
	}
}