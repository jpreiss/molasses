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
// aspect is width/height
// projects the view frustum into a box with z in [0, 1], xy in [-1, 1]
Mat projection(float fov, float aspect, float near, float far)
{
	float x = 1 / tan(fov / 2);
	float y = aspect * x;

	Mat projection = Mat(x, 0, 0,                  0,
		                 0, y, 0,                  0,
						 0, 0, far / (far - near), - far * near / (far - near),
						 0, 0, 1,                  0);

	return projection;
}

Mat normalizedToScreen(int width, int height)
{
	Mat scale = Mat::diagonal(width / 2, height / 2, 1);
	Mat shift = Mat::translation(Vec(width / 2, height / 2, 0));
	Mat toScreen = shift * scale;
	return toScreen;
}