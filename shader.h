#pragma once

#include "vec.h"
#include "mat.h"
#include "mat2d.h"
#include "image.h"
#include <functional>

class VertexGlobal
{
public:
	Mat modelView;
	Mat modelViewProjection;
	Mat normal;
};

class VertexIn
{
public:
	Vec vertex;
	Vec normal;
	Vec coord;
};

VertexIn operator+(VertexIn const &a, VertexIn const &b)
{
	VertexIn out;
	out.vertex = a.vertex + b.vertex;
	out.normal = a.normal + b.normal;
	out.coord = a.coord + b.coord;
	return out;
}

VertexIn operator*(float s, VertexIn const &a)
{
	VertexIn out;
	out.vertex = s * a.vertex;
	out.normal = s * a.normal;
	out.coord = s * a.coord;
	return out;
}

class VertexOutCore
{
public:
	Vec vertex;
};

// you must define operator+(T const &, T const &) and operator*(float, T const &) for your VertexOut class
VertexOutCore operator+(VertexOutCore const &a, VertexOutCore const &b)
{
	VertexOutCore out;
	out.vertex = a.vertex + b.vertex;
}

VertexOutCore operator*(float s, VertexOutCore const &a)
{
	VertexOutCore out;
	out.vertex = s * a.vertex;
}

class FragmentOut
{
public:
	ColorRGBA color;
};

// VertexOut should inherit from VertexOutCore
template <typename VertexOut>
class VertexShader : std::function<VertexOut(VertexIn const &, VertexGlobal const &)>
{
};

template <typename VertexOut>
class FragmentShader : std::function<FragmentOut(VertexIn const &vertex)>
{
};
