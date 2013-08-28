#pragma once

#include "vec.h"
#include "mat.h"

class Camera
{
public:
	Vec position;
	Vec direction;
	Vec up;
	Vec x() const
	{
		return cross(direction, up);
	}
	float fov;
};