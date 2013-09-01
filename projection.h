#pragma once

#include "camera.h"
#include "image.h"


Mat parallelProjection(Camera const &camera)
{
	Mat toOrigin = Mat::translation(-camera.position);
	Mat view = Mat::fromRows33(camera.x(), camera.up, camera.direction);
	Mat vp = view * toOrigin;
	return vp;
}

// fov in radians
// projects the view frustum into a box with z in [0, 1], xy in [-1, 1]
Mat perspectiveProjection(Camera const &camera, double fov, double near, double far)
{
	Mat toOrigin = Mat::translation(-camera.position);
	Mat view = Mat::fromRows33(camera.x(), camera.up, camera.direction);
	float xy = 1 / tan(fov / 2);

//#define THEIRS

#ifdef THEIRS
	float nearmap = - far / (far - near);
	float farmap  = - (far * near) / (far - near);
	Mat projection = Mat(xy, 0,  0,       0,
		                 0,  xy, 0,       0,
						 0,  0,  nearmap, farmap,
						 0,  0,  -1,       0);

#else
	Mat projection = Mat(xy, 0,  0,                0,
		                 0,  xy, 0,                0,
						 0,  0,  1 / (far - near), near / (far - near),
						 0,  0,  1,                0);
#endif

	return projection * (view * toOrigin);
}