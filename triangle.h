#pragma once

#include "vec.h"
#include "mat2d.h"
#include "mat.h"

class Triangle
{
public:
	Vec a;
	Vec b;
	Vec c;

	Triangle (Vec const &a, Vec const &b, Vec const &c) :
		a(a), b(b), c(c)
	{
	}

	Vec const *asVecs() const
	{
		return reinterpret_cast<Vec const *>(this);
	}
	
	Vec *asVecs()
	{
		return reinterpret_cast<Vec *>(this);
	}

	Vec barycentricXY(Vec const &v) const
	{
		Mat2D baryInv(a.x - c.x, b.x - c.x,
		              a.y - c.y, b.y - c.y);

		Mat2D bary = baryInv.inverted();

		Vec baryAB = bary * (v - c);
		baryAB.z = 1 - baryAB.x - baryAB.y;
		
		return baryAB;
	}
};

Triangle operator*(Mat const &m, Triangle const &t)
{
	return Triangle(m * t.a, m * t.b, m * t.c);
}