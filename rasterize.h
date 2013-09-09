#include "vec.h"
#include "array2d.h"
#include "util.h"
#include "shader.h"

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

Vec triNormal(Vec const &a, Vec const &b, Vec const &c)
{
	return cross(b - a, c - a);
}

// Interpolants must define operator+ and operator*(float, Interpolants)
// not sure how I like this but we'll see
// Vecs should be in projection space

template <typename VertexOut, typename FragmentOut, typename FragmentShader>
void rasterize(
	VertexOut const &a, VertexOut const &b, VertexOut const &c,
	FragmentShader const &shadeFragment,
	Array2D<float> &zbuf, Array2D<FragmentOut> &fragments, 
	Mat const &toScreen,
	VertexGlobal const &global)
{	
	Vec ws(a.vertex.w, b.vertex.w, c.vertex.w);

	Vec vtxProj[] = {
		a.vertex / a.vertex.w,
		b.vertex / b.vertex.w,
		c.vertex / c.vertex.w,
	};
	
	Vec vtxScreen[] = {
		toScreen * (a.vertex / a.vertex.w),
		toScreen * (b.vertex / b.vertex.w),
		toScreen * (c.vertex / c.vertex.w),
	};
	
	// incorrecat, overly agressive bounding box culling
	// until i get real clipping
	Bounds viewCube;
	viewCube.merge(Vec(-1, -1, -1));
	viewCube.merge(Vec(1, 1, 1));

	if (!viewCube.contains(vtxProj[0]) ||
	    !viewCube.contains(vtxProj[1]) ||
		!viewCube.contains(vtxProj[2]))
	{
		return;
	}

	// backface culling. in screen space, we only need to test z
	Vec normal = triNormal(vtxScreen[0], vtxScreen[1], vtxScreen[2]);
	if (normal.z < 0)
	{
		return;
	}

	Mat2D fromBary(vtxScreen[0].x - vtxScreen[2].x, vtxScreen[1].x - vtxScreen[2].x,
	               vtxScreen[0].y - vtxScreen[2].y, vtxScreen[1].y - vtxScreen[2].y);

	Mat2D toBary = fromBary.inverted();

	Bounds bbox = Bounds::fromIterators(vtxScreen, vtxScreen + 3);

	// need shadow rules for perfect int coords
	for (int x = ceil(bbox.mins.x); x <= floor(bbox.maxes.x); ++x)
	{
		for (int y = ceil(bbox.mins.y); y <= floor(bbox.maxes.y); ++y)
		{
			Vec pt(x, y, 0);
			Vec bary = toBary * (pt - vtxScreen[2]);
			bary.z = 1 - (bary.x + bary.y);

			if (bary.x < 0 || bary.y < 0 || bary.z < 0)
			{
				// outside tri
				continue;
			}
		
			// early z test
			double zinterp = baryInterp(
				vtxScreen[0].z, vtxScreen[1].z, vtxScreen[2].z, bary);
			
			if (x < zbuf.columns() && y < zbuf.rows() && zinterp < zbuf(y, x))
			{
				zbuf(y, x) = zinterp;
				VertexOut interpolant = perspectiveCorrectBaryInterp(a, b, c, bary, ws);			
				fragments(y, x) = shadeFragment(interpolant, global).color;
			}
		}
	}
}