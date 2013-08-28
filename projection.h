#pragma once

#include "camera.h"
#include "image.h"


Mat parallelProjection(Camera const &camera, double worldToPixels)
{
	Mat toOrigin = Mat::translation(-camera.position);
	Mat view = Mat::fromRows(camera.x(), camera.up, camera.direction);
	Mat projection = worldToPixels * Mat::identity();
	Mat vp = projection * view * toOrigin;
	return vp;
}