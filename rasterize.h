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
	int const width = zbuf.columns();
	int const height = zbuf.rows();

	Vec ws(a.vertex.w, b.vertex.w, c.vertex.w);

	Vec vtxProj[] = {
		a.vertex / abs(a.vertex.w),
		b.vertex / abs(b.vertex.w),
		c.vertex / abs(c.vertex.w),
	};
	
	Vec vtxScreen[] = {
		toScreen * vtxProj[0],
		toScreen * vtxProj[1],
		toScreen * vtxProj[2],
	};
	
	// incorrecat, overly agressive bounding box culling
	// until i get real clipping
	Bounds viewCube;
	viewCube.merge(Vec(-1, -1, 0));
	viewCube.merge(Vec(1, 1, 1));

	Bounds object = Bounds::fromIterators(vtxProj, vtxProj + 3);

	// partially outside
	if (!viewCube.contains(vtxProj[0]) ||
	    !viewCube.contains(vtxProj[1]) ||
		!viewCube.contains(vtxProj[2]))
	{
		//return;
	}

	// completely outside
	if (!viewCube.intersects(object))
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

	int xmin = max(0, (int)ceil(bbox.mins.x));
	int xmax = min(width, (int)ceil(bbox.maxes.x));
	int ymin = max(0, (int)ceil(bbox.mins.y));
	int ymax = min(height, (int)ceil(bbox.maxes.y));

	// need shadow rules for perfect int coords
	for (int x = xmin; x < xmax; ++x)
	{
		for (int y = ymin; y < ymax; ++y)
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

			if (zinterp < 0)
			{
				continue;
			}
			
			if (x < zbuf.columns() && y < zbuf.rows() && zinterp < zbuf(height - y, x))
			{
				zbuf(height - y, x) = zinterp;
				VertexOut interpolant = perspectiveCorrectBaryInterp(a, b, c, bary, ws);			
				fragments(height - y, x) = shadeFragment(interpolant, global).color;
			}
		}
	}
}