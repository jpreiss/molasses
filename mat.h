#pragma once

#include "vec.h"

// 4x4 row-major matrix.  Want to SIMD some day.

class Mat
{
public:
	Mat()
	{
		for (int row = 0; row < 4; ++row)
		for (int col = 0; col < 4; ++col)
		{
			arr[row][col] = 0;
		}
	}

	Mat(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33)
	{
		arr[0][0] = m00; arr[0][1] = m01; arr[0][2] = m02; arr[0][3] = m03;
		arr[1][0] = m10; arr[1][1] = m11; arr[1][2] = m12; arr[1][3] = m13;
		arr[2][0] = m20; arr[2][1] = m21; arr[2][2] = m22; arr[2][3] = m23;
		arr[3][0] = m30; arr[3][1] = m31; arr[3][2] = m32; arr[3][3] = m33;
	}

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
		for (int row = 0; row < 3; ++row)
		{
			arr[row][col] = v[row];
		}
	}

	void setRow(int row, Vec const &v)
	{
		for (int col = 0; col < 3; ++col)
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
		m.setCol(3, v);
		m.arr[3][3] = 1.0;

		return m;
	}

	static Mat fromColumnsAffine(Vec const &x, Vec const &y, Vec const &z, Vec const &translation = Vec())
	{
		Mat m;
		m.setCol(0, x);
		m.setCol(1, y);
		m.setCol(2, z);
		m.setCol(3, translation);
		m.arr[3][3] = 1;

		return m;
	}

	static Mat fromRows33(Vec const &x, Vec const &y, Vec const &z)
	{
		Mat m;
		m.setRow(0, x);
		m.setRow(1, y);
		m.setRow(2, z);
		m.arr[3][3] = 1.0;

		return m;
	}

	static Mat diagonal(float x, float y, float z)
	{
		Mat m;
		m.arr[0][0] = x;
		m.arr[1][1] = y;
		m.arr[2][2] = z;
		m.arr[3][3] = 1;

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
	float result[] = {0, 0, 0, 0};

	for (int row = 0; row < 4; ++row)
	{
		for (int i = 0; i < 4; ++i)
		{
			result[row] += m(row, i) * v[i];
		}
	}

	return Vec(result);
}

Mat operator *(Mat const &a, Mat const &b)
{
	Mat m;

	for (int row = 0; row < 4; ++row)
	{
		for (int col = 0; col < 4; ++col)
		{
			m(row, col) = 0;

			for (int i = 0; i < 4; ++i)
			{
				m(row, col) += a(row, i) * b(i, col);
			}
		}
	}

	return m;
}