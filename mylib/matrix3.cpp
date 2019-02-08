#include "matrix3.h"

#include <cmath>
#include <cassert>

#include "matrix2.h"

Matrix3::Matrix3()
{
	// initialise to identity
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			if (j == i)
				// cell is diagonal if i = j
				m2d[i][j] = 1.0f;
			else
				m2d[i][j] = 0.0f;
}

Matrix3::Matrix3(float m00, float m01, float m02, float m10, float m11, 
	float m12, float m20, float m21, float m22)
{
	// copy values individually
	m2d[0][0] = m00;
	m2d[0][1] = m01;
	m2d[0][2] = m02;

	m2d[1][0] = m10;
	m2d[1][1] = m11;
	m2d[1][2] = m12;

	m2d[2][0] = m20;
	m2d[2][1] = m21;
	m2d[2][2] = m22;
}

Matrix3::Matrix3(float* values)
{
	// copy values from an array
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			m2d[i][j] = values[i * 3 + j];
}

Matrix3::Matrix3(Matrix3 const& other)
{
	// copy values from other matrix's array
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			m2d[i][j] = other.m2d[i][j];
}

Matrix3& Matrix3::operator=(Matrix3 const& other)
{
	// copy values from other matrix's array
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			m2d[i][j] = other.m2d[i][j];
	return *this;
}

void Matrix3::setRotateX(float a)
{
	m2d[1][1] = cosf(a);
	m2d[1][2] = sinf(a);

	m2d[2][1] = -sinf(a);
	m2d[2][2] = cosf(a);
}

void Matrix3::setRotateY(float a)
{
	m2d[0][0] = cosf(a);
	m2d[0][2] = -sinf(a);

	m2d[2][0] = sinf(a);
	m2d[2][2] = cosf(a);
}

void Matrix3::setRotateZ(float a)
{
	m2d[0][0] = cosf(a);
	m2d[0][1] = sinf(a);

	m2d[1][0] = -sinf(a);
	m2d[1][1] = cosf(a);
}

void Matrix3::setPosition(float x, float y)
{
	m2d[2][0] = x;
	m2d[2][1] = y;
}

// just call the other setPosition with this vector's values
void Matrix3::setPosition(Vector2 p)
{
	setPosition(p.x, p.y);
}

void Matrix3::setScale(float x, float y)
{
	m2d[0][0] = x;
	m2d[1][1] = y;
}

// just call the other setScale with this vector's values
void Matrix3::setScale(Vector2 s)
{
	setScale(s.x, s.y);
}

Matrix3& Matrix3::transpose()
{
	// transpose this matrix
	Matrix3 n = this->transposed();
	(*this) = Matrix3(n);
	return *this;
}

// swappy swap the rows and columns
Matrix3 Matrix3::transposed()
{
	// make a new matrix and put this matrix transposed into it
	Matrix3 result;
	for (int i = 0; i < 3; ++i)
	{
		Vector3 col = (*this)[i];
		result.m2d[0][i] = col.x;
		result.m2d[1][i] = col.y;
		result.m2d[2][i] = col.z;
	}
	return result;
}

// get the determinant for use in inverses
float Matrix3::getDeterminant()
{
	// make the matrix of minors
	Matrix3 minors;

	// for each element
	//		ignore its row and column
	//		get determinant from leftover items
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			// grab a 2d matrix of the values that aren't in this
			// row and column
			Matrix2 vals;
			int added = 0;
			for (int y = 0; y < 3; ++y)
			{
				for (int x = 0; x < 3; ++x)
				{
					if (x == j || y == i)
						continue;
					vals.m[added] = m2d[y][x];
					added++;
				}
			}
			minors.m2d[i][j] = vals.getDeterminant();
		}
	}

	// find the determinant of this matrix using the minors
	float detVals[3];
	for (int i = 0; i < 3; ++i)
		detVals[i] = minors.m2d[i][0] * m2d[i][0];

	return detVals[0] - detVals[1] + detVals[2];
}

Matrix3 Matrix3::inverse()
{
	// make the matrix of minors
	Matrix3 minors;

	// for each element
	//		ignore its row and column
	//		get determinant from leftover items
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			// grab a 2d matrix of the values that aren't in this
			// row and column
			Matrix2 vals;
			int added = 0;
			for (int y = 0; y < 3; ++y)
			{
				for (int x = 0; x < 3; ++x)
				{
					if (x == j || y == i)
						continue;
					vals.m[added] = m2d[y][x];
					added++;
				}
			}
			// that matrix's determinant is this element's value
			minors.m2d[i][j] = vals.getDeterminant();
		}
	}

	// turn minor matrix into cofactor matrix
	// by swapping the signs on alternating cells
	Matrix3 cofactor(minors);
	for (int i = 1; i < 9; i += 2)
		cofactor.m[i] = -minors.m[i];

	// "adjugate" - just transpose that matrix
	cofactor.transpose();

	// find the determinant of this matrix using the minors
	float detVals[3];
	for (int i = 0; i < 3; ++i)
		detVals[i] = minors.m2d[i][0] * m2d[i][0];
	float determinant = detVals[0] - detVals[1] + detVals[2];

	// result is the transposed cofactor matrix multiplied by the inverse of
	//	the determinant
	return cofactor * (1.0f / determinant);
}

// allow indexing this like an array
Vector3& Matrix3::operator[](int index)
{
	// make sure the index is within bounds
	assert(index >= 0 && index <= 2);
	// grab the column
	return mvec[index];
}

// grab the position values
Vector2 Matrix3::getPosition()
{
	return Vector2(m2d[2][0], m2d[2][1]);
}

// up direction vector
Vector2 Matrix3::getUp()
{
	return Vector2(m2d[1][0], m2d[1][1]);
}

// right direction vector
Vector2 Matrix3::getRight()
{
	return Vector2(m2d[0][0], m2d[0][1]);
}

// multiply by volumn vector
Vector3 Matrix3::operator*(Vector3 const& v)
{
	Vector3 result;
	for (int i = 0; i < 3; ++i)
		result[i] = mvec[0][i] * v.x + mvec[1][i] * v.y + mvec[2][i] * v.z;
	return result;
}

// matrix multiplication
Matrix3 Matrix3::operator*(Matrix3 const& other)
{
	Matrix3 result;

	// while shorter, I think this is slower than writing it out manually
	// (which I've done in Matrix4)
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			result.m2d[j][i] = m2d[0][i] * other.m2d[j][0] + 
			m2d[1][i] * other.m2d[j][1] + m2d[2][i] * other.m2d[j][2];

	return result;
}

// multiply each element by a float
Matrix3 Matrix3::operator*(float f)
{
	Matrix3 result;
	for (int i = 0; i < 9; ++i)
		result.m[i] = m[i] * f;
	return result;
}

// cast to a float array
Matrix3::operator float*()
{
	return (float*)&m;
}
