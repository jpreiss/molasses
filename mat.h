#pragma once

#include "vec.h"

// 4x4 row-major matrix.  Want to SIMD some day.

class Mat
{
public:
	float const &operator()(size_t row, size_t column) const
	{
		return arr[row][column];
	}

	float &operator()(size_t row, size_t column)
	{
		return arr[row][column];
	}

	Vec col(int col) const
	{
		return Vec(arr[0][col], arr[1][col], arr[2][col], arr[3][col]);
	}

	Vec row(int row) const
	{
		return Vec(arr[row]);
	}

	void setCol(int col, Vec const &v)
	{
		for (int row = 0; row < 4; ++row)
		{
			arr[row][col] = v[row];
		}
	}

	void setRow(int row, Vec const &v)
	{
		for (int col = 0; col < 4; ++col)
		{
			arr[row][col] = v[col];
		}
	}

	static Mat identity()
	{
		Mat m;

		for (int row = 0; row < 4; ++row)
		for (int col = 0; col < 4; ++col)
		{
			m(row, col) = (row == col ? 1.0f : 0.0f);
		}

		return m;
	}

	static Mat translation(Vec const &v)
	{
		Mat m = Mat::identity();
		
		for (int row = 0; row < 3; ++row)
		{
			m(row, 3) = v[row];
		}

		return m;
	}

	static Mat fromColumns(Vec const &x, Vec const &y, Vec const &z, Vec const &translation = Vec())
	{
		Mat m;
		m.setCol(0, x);
		m.setCol(1, y);
		m.setCol(2, z);
		m.setCol(3, translation);
		m.arr[3][3] = 1.0;

		return m;
	}

	static Mat fromRows(Vec const &x, Vec const &y, Vec const &z, Vec const &translation = Vec())
	{
		Mat m;
		m.setRow(0, x);
		m.setRow(1, y);
		m.setRow(2, z);
		m.setRow(3, translation);
		m.arr[3][3] = 1.0;

		return m;
	}

	float arr[4][4];
};

Mat operator *(float s, Mat const &b)
{
	Mat m;

	for (int row = 0; row < 4; ++row)
	for (int col = 0; col < 4; ++col)
	{
		m(row, col) = s * b(row, col);
	}

	return m;
}

Vec operator *(Mat const &m, Vec const &v)
{
	return Vec(
		dot(m.row(0), v),
		dot(m.row(1), v),
		dot(m.row(2), v),
		dot(m.row(3), v));
}

Mat operator *(Mat const &a, Mat const &b)
{
	Mat m;

	for (int row = 0; row < 4; ++row)
	for (int col = 0; col < 4; ++col)
	{
		m(row, col) = dot(a.row(row), b.col(col));
	}

	return m;
}