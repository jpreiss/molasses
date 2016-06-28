#pragma once

#include "array2d.h"
#include "image.h"
#include "vec2.h"
#include "util.h"


// Draws a non-anti-aliased line to the image.
//
void drawLine2D(Vec const &start, Vec const &end, Array2D<ColorRGBA> &image, ColorRGBA color)
{
	// TODO replace with Bresenham's Algorithm to avoid floating point

	float x0 = start.x, y0 = start.y;
	float x1 = end.x,   y1 = end.y;

	float dx = x1 - x0;
	float dy = y1 - y0;

	float pixels = max(abs(dx), abs(dy));
	float xstep = dx / pixels;
	float ystep = dy / pixels;

	float x = x0;
	float y = y0;

	for (int i = 0; i < (int)pixels; ++i)
	{
		int xr = x + 0.5;
		int yr = y + 0.5;
		if ((xr >= 0) && (xr < image.columns()) && (yr >= 0) && (yr < image.rows()))
		{
			image(yr, xr) = color;
		}
		x += xstep;
		y += ystep;
	}
}
