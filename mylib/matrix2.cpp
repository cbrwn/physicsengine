#include "matrix2.h"

#include <cmath>
#include <cassert>

Matrix2::Matrix2()
{
	// initialise to identity
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
			if (j == i)
				// cell is diagonal if i = j
				m2d[i][j] = 1.0f;
			else
				m2d[i][j] = 0.0f;
}

Matrix2::Matrix2(float m00, float m01, float m10, float m11)
{
	// copy values individually
	m2d[0][0] = m00;
	m2d[0][1] = m01;

	m2d[1][0] = m10;
	m2d[1][1] = m11;
}

Matrix2::Matrix2(float* values)
{
	// copy values from an array
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
			m2d[i][j] = values[i * 2 + j];
}

Matrix2::Matrix2(Matrix2 const& other)
{
	// copy values from other matrix's array
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
			m2d[i][j] = other.m2d[i][j];
}

Matrix2& Matrix2::operator=(Matrix2 const& other)
{
	// copy values from other matrix's array
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
			m2d[i][j] = other.m2d[i][j];
	return *this;
}

// turn this angle into a transformation matrix
void Matrix2::setRotate(float angle)
{
	m[0] = cosf(angle);
	m[1] = sinf(angle);

	m[2] = -sinf(angle);
	m[3] = cosf(angle);
}

Matrix2& Matrix2::transpose()
{
	// transpose this matrix
	Matrix2 n = this->transposed();
	(*this) = Matrix2(n);
	return *this;
}

Matrix2 Matrix2::transposed()
{
	// make a new matrix and put this matrix transposed into it
	Matrix2 result;
	for (int i = 0; i < 2; ++i)
	{
		Vector2 col = (*this)[i];
		result.m2d[0][i] = col.x;
		result.m2d[1][i] = col.y;
	}
	return result;
}

// determinant is used in getting the inverse
float Matrix2::getDeterminant()
{
	return (m[0] * m[3]) - (m[1] * m[2]);
}

// super duper easy to get the inverse of a 2x2 matrix
Matrix2 Matrix2::inverse()
{
	float determinant = getDeterminant();

	// basically just from:
	// https://www.mathsisfun.com/algebra/matrix-inverse.html
	Matrix2 swapped;
	swapped.m[0] = m[3];
	swapped.m[1] = -m[1];
	swapped.m[2] = -m[2];
	swapped.m[3] = m[0];

	return swapped * (1.0f / determinant);
}

// allow indexing the matrix like an array
Vector2& Matrix2::operator[](int index)
{
	// make sure the index isn't out of bounds
	assert(index >= 0 && index <= 2);
	// grab the column
	return mvec[index];
}

Vector2 Matrix2::operator*(Vector2 const& v)
{
	Vector2 result;
	result.x = m[0] * v.x + m[2] * v.y;
	result.y = m[1] * v.x + m[3] * v.y;
	return result;
}

// multiply this with another matrix
Matrix2 Matrix2::operator*(Matrix2 const& other)
{
	Matrix2 result;

	Matrix2 ot = ((Matrix2)other).transposed();
	for (int i = 0; i < 2; ++i) // columns
		for (int j = 0; j < 2; ++j) // rows
			result[i][j] = (*this)[i].dot(ot[j]);
	return result;
}

// multiply each value by f
Matrix2 Matrix2::operator*(float f)
{
	Matrix2 result;
	for (int i = 0; i < 4; ++i)
		result.m[i] = m[i] * f;
	return result;
}

// casting to a float array
Matrix2::operator float*()
{
	return (float*)&m;
}
