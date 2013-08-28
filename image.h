#pragma once

#include "util.h"
#include "array2d.h"


// 2D array of RGBA values, 8 bits per channel
// arithmetic saturates on overflow
class ColorRGBA
{
public:
	ColorRGBA() :
		r(0), g(0), b(0), a(0xFF)
	{
	}

	ColorRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 0xFF) :
		r(r), g(g), b(b), a(a)
	{
	}

	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	unsigned char *asMemory()
	{ 
		return (unsigned char *)this;
	}

	ColorRGBA fromMemory(unsigned char *arr)
	{
		return ColorRGBA(arr[0], arr[1], arr[2], arr[3]);
	}
};

ColorRGBA operator+(ColorRGBA a, ColorRGBA b)
{
	ColorRGBA c;
	c.r = clampAdd<unsigned char, int>(a.r, b.r);
	c.g = clampAdd<unsigned char, int>(a.g, b.g);
	c.b = clampAdd<unsigned char, int>(a.b, b.b);
	c.a = clampAdd<unsigned char, int>(a.a, b.a);

	return c;
}

// converting (x, y) to the (row, col) that Array2D expects
class Image : public Array2D<ColorRGBA>
{
public:
	Image(size_t width, size_t height) :
		Array2D(height, width)
	{
	}

	ColorRGBA &operator()(size_t x, size_t y)
	{
		return Array2D::operator()(y, x);
	}
};