#pragma once

// Cut down from Will Perone's code (http://willperone.net/)

#include "mat.h"
#include "vec.h"
#include <cmath>

class Quat;

Quat operator-(Quat const &);
Quat operator+(Quat const &, Quat const&);
Quat operator-(Quat const &, Quat const&);
Quat operator*(Quat const &);
Quat operator/(Quat const &, Quat const &);
Quat operator*(float, Quat const &);
Quat operator/(Quat const &, float);

class Quat
{
public:
	float s;
	Vec v;

	Quat()
	{
	}

	Quat(float s, Vec const &v) : 
		s(s), v(v) 
	{
	}

	static inline Quat from_axis_angle(Vec const &axis, float angle)
	{	
		return Quat(cos(angle / 2), sin(angle / 2) * axis);
	}

	float length() const
	{	
		return sqrt(length2());
	}

	float length2() const
	{	
		return (float)(s * s + dot(v, v));
	}

	Quat normalized() const
	{
		return *this / length();
	}

	Quat conjugated() const
	{
		return Quat(s, -v);
	}

	Quat inverted() const
	{
		return Quat(s, -v) / length2();
	}

	Mat matrix() const
	{			
		return Mat(
			s,  -v.x, -v.y,-v.z,
			v.x,  s,  -v.z, v.y,
			v.y, v.z,    s,-v.x,
			v.z,-v.y,  v.x,   s);
	}

	void to_axis_angle(Vec &axis, float &angle) const
	{
		angle = acos(s);
		axis = v / sin(angle);
		angle *= 2;
	}
};

float dot(const Quat &q1, const Quat &q2) 
{   
	return dot(q1.v, q2.v) + q1.s * q2.s;
}

Quat lerp(const Quat &q1, const Quat &q2, float t) 
{	
	return ((1 - t) * q1 + t * q2).normalized();	
}

Quat slerp(const Quat &q1, const Quat &q2, float t) 
{
	Quat q3;
	float dp = dot(q1, q2);

	/*	dot = cos(theta)
		if (dot < 0), q1 and q2 are more than 90 degrees apart,
		so we can invert one to reduce spinning	*/
	if (dp < 0)
	{
		dp = -dp;
		q3 = -q2;
	} 
	else 
	{
		q3 = q2;
	}

	if (dp < 0.99f)
	{
		float angle = acos(dp);
		return ((sin(angle * (1-t)) * q1) + (sin(angle * t) * q3)) / sin(angle);
	} 
	else
	{
		// if the angle is small, use linear interpolation
		return lerp(q1, q3, t);
	}
}

Quat operator-(Quat const &q)				
{	
	return Quat(-q.s, -q.v);
}

Quat operator+(Quat const &a, Quat const &b)
{
	return Quat(a.s + b.s, a.v + b.v);
}

Quat operator-(Quat const &a, Quat const &b)
{
	return Quat(a.s - b.s, a.v - b.v);
}

Quat operator *(Quat const &a, Quat const &b)	
{
	return Quat(
		a.s * b.s - dot(a.v, b.v),
		Vec(
			a.v.y * b.v.z - a.v.z * b.v.y + a.s * b.v.x + a.v.x * b.s,
			a.v.z * b.v.x - a.v.x * b.v.z + a.s * b.v.y + a.v.y * b.s,
			a.v.x * b.v.y - a.v.y * b.v.x + a.s * b.v.z + a.v.z * b.s));
}

Quat operator/(Quat const &a, Quat const &q)
{	
	return a * q.inverted();
}

Quat operator *(float s, Quat const &q)	
{	
	return Quat(s * q.s, s * q.v);		
}

Quat operator /(Quat const &q, float s)
{	
	return (1.0 / s) * q;
}

// Quat must be unit
Vec operator*(Quat const &q, Vec const &v)
{   
	Quat V(0, v);
	return (q * V * q.conjugated()).v;
}