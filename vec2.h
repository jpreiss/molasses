#pragma once

struct vec2
{
	// no ctors, so MSVC will return it in registers
	float x;
	float y;

	static vec2 zero() { return {0, 0}; }
};

vec2 operator-(vec2 v)
{
	return {-v.x, -v.y};
}

vec2 operator+(vec2 a, vec2 b)
{
	return {a.x + b.x, a.y + b.y};
}

vec2 operator-(vec2 a, vec2 b)
{
	return {a.x - b.x, a.y - b.y};
}

float dot(vec2 a, vec2 b)
{
	return a.x * b.x + a.y * b.y;
}

bool y_lt(vec2 a, vec2 b)
{
	return a.y < b.y;
}

