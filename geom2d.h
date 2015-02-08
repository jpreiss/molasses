#pragma once

#include "vec2.h"

#include <cmath>

float turn_direction(vec2 a, vec2 b, vec2 c)
{
	return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

bool is_ccw(float direction) { return direction > 0; }
bool is_cw(float direction) { return direction < 0; }

float angle(vec2 a, vec2 b)
{
	return atan2(b.y - a.y, b.x - a.x);
}
