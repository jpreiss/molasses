#pragma once

#include "vec.h"

// 2x2 matrix. Operates on the X, Y values of our 4-vecs
// and ignores the rest.

class Mat2D
{
public:
	Mat2D()
	{
	}

	Mat2D(float a, float b,
	      float c, float d) :
		a(a), b(b), c(c), d(d)
	{
	}

	float det()
	{
		return a * d - b * c;
	}

	Mat2D inverted()
	{
		float dinv = 1.0 / det();

		return Mat2D( dinv * d, -dinv * b,
		             -dinv * c,  dinv * a);
	}

	float a, b, 
	      c, d;
};

Vec operator*(Mat2D const &m, Vec const &v)
{
	float x = m.a * v.x + m.b * v.y;
	float y = m.c * v.x + m.d * v.y;

	return Vec(x, y, v.z, v.w);
}