#pragma once

#include "camera.h"
#include "mat.h"

Mat view(Camera const &camera)
{
	Mat trans = Mat::translation(-camera.position);
	Mat rot = Mat::fromRows33(camera.x(), camera.up, camera.direction);
	Mat view = rot * trans;
	return view;
}

// fov in radians
// projects the view frustum into a box with z in [0, 1], xy in [-1, 1]
Mat projection(double fov, double near, double far)
{
	float xy = 1 / tan(fov / 2);

	Mat projection = Mat(xy, 0,  0,                0,
		                 0,  xy, 0,                0,
						 0,  0,  1 / (far - near), near / (far - near),
						 0,  0,  -1,                0);

	return projection;
}

Mat normalizedToScreen(int width, int height)
{
	Mat scale = Mat::diagonal(height / 2, height / 2, 1);
	Mat shift = Mat::translation(Vec(width / 2, height / 2, 0));
	Mat toScreen = shift * scale;
	return toScreen;
}