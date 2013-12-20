#pragma once

#include <cassert>

#include "array2d.h"

// utilities for texture mapping
// upper left hand corner of a pixel is (0, 0)
// converts from (x, y) that makes sense for textures
// to (row, col) used by array2d

template <typename T>
T nearestNeighbor(Array2D<T> const &tex, float x, float y)
{
	int xi = x;
	int yi = y;
	return tex(yi, xi);
}

template <typename T>
T bilinear(Array2D<T> const &tex, float x, float y)
{
	int x0 = x;
	int x1 = x0 + 1;
	int y0 = y;
	int y1 = y0 + 1;
	
	float right = x - x0;
	float left = 1 - right;
	float bottom = y - y0;
	float top = 1 - bottom;

	assert((x0 <= tex.columns()) && (y0 <= tex.rows()));

	if (x0 == tex.columns())
	{
		// coord is on right hand side of texture
		if (y0 == tex.rows())
		{
			// coord is on bottom too
			return tex(x0 - 1, y0 - 1);
		}
	
		// otherwise, on right but not bottom
		T v00 = tex(y0, x0 - 1);
		T v01 = tex(y1, x0 - 1);
		return top * v00 + bottom * v01;
	}
	else if (y0 == tex.rows())
	{
		// on bottom but not right
		T v00 = tex(y0 - 1, x0);
		T v10 = tex(y0 - 1, x1);
		return left * v00 + right * v10;
	}
	else
	{	
		T v00 = tex(y0, x0);
		T v10 = tex(y0, x1);
		T v01 = tex(y1, x0);
		T v11 = tex(y1, x1);

		T topval = left * v00 + right * v10;
		T bottomval = left * v01 + right * v11;

		T erp = top * topval + bottom * bottomval;
		return erp;
	}
}