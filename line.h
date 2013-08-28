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