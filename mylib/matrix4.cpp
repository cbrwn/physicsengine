#include "matrix4.h"

#include <cmath>
#include <cassert>

#include "matrix3.h"

Matrix4::Matrix4()
{
	// initialise to identity
	//for (int i = 0; i < 4; ++i)
	//	for (int j = 0; j < 4; ++j)
	//		if (j == i)
	//			// cell is diagonal if i = j
	//			m2d[i][j] = 1.0f;
	//		else
	//			m2d[i][j] = 0.0f;
	m2d[0][0] = 1.0f;
	m2d[0][1] = 0.0f;
	m2d[0][2] = 0.0f;
	m2d[0][3] = 0.0f;

	m2d[1][0] = 0.0f;
	m2d[1][1] = 1.0f;
	m2d[1][2] = 0.0f;
	m2d[1][3] = 0.0f;

	m2d[2][0] = 0.0f;
	m2d[2][1] = 0.0f;
	m2d[2][2] = 1.0f;
	m2d[2][3] = 0.0f;

	m2d[3][0] = 0.0f;
	m2d[3][1] = 0.0f;
	m2d[3][2] = 0.0f;
	m2d[3][3] = 1.0f;
}

Matrix4::Matrix4(float m00, float m01, float m02, float m03, float m10,
	float m11, float m12, float m13, float m20, float m21, float m22,
	float m23, float m30, float m31, float m32, float m33)
{
	// copy values individually
	m2d[0][0] = m00;
	m2d[0][1] = m01;
	m2d[0][2] = m02;
	m2d[0][3] = m03;

	m2d[1][0] = m10;
	m2d[1][1] = m11;
	m2d[1][2] = m12;
	m2d[1][3] = m13;

	m2d[2][0] = m20;
	m2d[2][1] = m21;
	m2d[2][2] = m22;
	m2d[2][3] = m23;

	m2d[3][0] = m30;
	m2d[3][1] = m31;
	m2d[3][2] = m32;
	m2d[3][3] = m33;
}

Matrix4::Matrix4(float* values)
{
	// copy values from an array
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			m2d[i][j] = values[i * 4 + j];
}

Matrix4::Matrix4(Matrix4 const& other)
{
	// copy values from other matrix's array
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			m2d[i][j] = other.m2d[i][j];
}

Matrix4& Matrix4::operator=(Matrix4 const& other)
{
	// copy values from other matrix's array
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			m2d[i][j] = other.m2d[i][j];
	return *this;
}

// make all 3 rotation matrices and combine them
void Matrix4::setRotate(Vector3 const& v)
{
	Matrix4 xRot, yRot, zRot;
	xRot.setRotateX(v.x);
	yRot.setRotateY(v.y);
	zRot.setRotateZ(v.z);

	Matrix4 result = xRot * yRot * zRot;
	(*this) = result;
}

void Matrix4::setRotateX(float a)
{
	m2d[1][1] = cosf(a);
	m2d[1][2] = sinf(a);

	m2d[2][1] = -sinf(a);
	m2d[2][2] = cosf(a);
}

void Matrix4::setRotateY(float a)
{
	m2d[0][0] = cosf(a);
	m2d[0][2] = -sinf(a);

	m2d[2][0] = sinf(a);
	m2d[2][2] = cosf(a);
}

void Matrix4::setRotateZ(float a)
{
	m2d[0][0] = cosf(a);
	m2d[0][1] = sinf(a);

	m2d[1][0] = -sinf(a);
	m2d[1][1] = cosf(a);
}

void Matrix4::setPosition(float x, float y, float z)
{
	m2d[3][0] = x;
	m2d[3][1] = y;
	m2d[3][2] = z;
}

// just call other setPosition function using the vector's values
void Matrix4::setPosition(Vector3 p)
{
	setPosition(p.x, p.y, p.z);
}

void Matrix4::setScale(float x, float y, float z)
{
	m2d[0][0] = x;
	m2d[1][1] = y;
	m2d[2][2] = z;
}

// just call other setScale function using the vector's values
void Matrix4::setScale(Vector3 s)
{
	setScale(s.x, s.y, s.z);
}

Matrix4& Matrix4::transpose()
{
	// transpose this matrix
	Matrix4 n = this->transposed();
	(*this) = Matrix4(n);
	return *this;
}

// swippy swap rows and columns
Matrix4 Matrix4::transposed()
{
	// make a new matrix and put this matrix transposed into it
	Matrix4 result;
	for (int i = 0; i < 4; ++i)
	{
		Vector4 col = (*this)[i];
		result.m2d[0][i] = col.x;
		result.m2d[1][i] = col.y;
		result.m2d[2][i] = col.z;
		result.m2d[3][i] = col.w;
	}
	return result;
}

Matrix4 Matrix4::inverse()
{
	// make the matrix of minors
	Matrix4 minors;

	// for each element
	//		ignore its row and column
	//		get determinant from leftover items
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			// grab a 2d matrix of the values that aren't in this
			// row and column
			Matrix3 vals;
			int added = 0;
			for (int y = 0; y < 4; ++y)
			{
				for (int x = 0; x < 4; ++x)
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

	// turn minor matrix into cofactor matrix
	// by swapping the signs on alternating cells
	Matrix4 cofactor(minors);
	for (int y = 0; y < 4; ++y)
	{
		int a = (y % 2);
		for (int x = 0; x < 4; ++x)
		{
			if ((x % 2) == a)
				cofactor.m2d[y][x] *= -1.0f;
		}
	}

	// "adjugate" - just transpose that matrix
	cofactor.transpose();

	// find the determinant of this matrix using the minors
	float detVals[4];
	for (int i = 0; i < 4; ++i)
		detVals[i] = minors.m2d[i][0] * m2d[i][0];

	float determinant = -1 * detVals[0] + detVals[1] - detVals[2] + detVals[3];

	return cofactor * (1.0f / determinant);
}

// allow indexing like an array
Vector4& Matrix4::operator[](int index)
{
	// make sure the index is within the bounds
	assert(index >= 0 && index <= 3);
	// grab the column
	return mvec[index];
}

// stick position values into a vector
Vector3 Matrix4::getPosition()
{
	return Vector3(m2d[3][0], m2d[3][1], m2d[3][2]);
}

// forward direction vector
Vector3 Matrix4::getForward()
{
	return Vector3(m2d[2][0], m2d[2][1], m2d[2][2]);
}

// up direction vector
Vector3 Matrix4::getUp()
{
	return Vector3(m2d[1][0], m2d[1][1], m2d[1][2]);
}

// right direction vector
Vector3 Matrix4::getRight()
{
	return Vector3(m2d[0][0], m2d[0][1], m2d[0][2]);
}

// grab euler angles out of this
Vector3 Matrix4::getAngles()
{
	float yaw = 0.0f;
	float roll = 0.0f;
	float pitch = 0.0f;

	if (m2d[0][0] == 1.0f)
	{
		yaw = atan2f(m2d[0][2], m2d[2][3]);
	}
	else if (m2d[0][0] == -1.0f)
	{
		yaw = atan2f(m2d[0][2], m2d[2][3]);
	}
	else
	{
		yaw = atan2f(-m2d[2][0], m2d[0][0]);
		pitch = asinf(m2d[1][0]);
		roll = atan2f(-m2d[1][2], m2d[1][1]);
	}
	return { pitch, yaw, roll };
}

// multiply by a column vector
Vector4 Matrix4::operator*(Vector4 const& v)
{
	Vector4 result;
	for (int i = 0; i < 4; ++i)
		result[i] = mvec[0][i] * v.x + mvec[1][i] * v.y + mvec[2][i] * v.z +
		mvec[3][i] * v.w;
	return result;
}

// matrix multiplication
Matrix4 Matrix4::operator*(Matrix4 const& other)
{
	Matrix4 result;

	// nicer looking but slower :(
	//for (int i = 0; i < 4; ++i)
	//	for (int j = 0; j < 4; ++j)
	//		result.m2d[j][i] = m2d[0][i] * other.m2d[j][0] +
	//		m2d[1][i] * other.m2d[j][1] +
	//		m2d[2][i] * other.m2d[j][2] +
	//		m2d[3][i] * other.m2d[j][3];

	// here's 1000 lines of multiplying things
	// how exciting
	result.m2d[0][0] = m2d[0][0] * other.m2d[0][0] + m2d[1][0] * 
		other.m2d[0][1] + m2d[2][0] * other.m2d[0][2] + m2d[3][0] * 
		other.m2d[0][3];
	result.m2d[1][0] = m2d[0][0] * other.m2d[1][0] + m2d[1][0] * 
		other.m2d[1][1] + m2d[2][0] * other.m2d[1][2] + m2d[3][0] * 
		other.m2d[1][3];
	result.m2d[2][0] = m2d[0][0] * other.m2d[2][0] + m2d[1][0] * 
		other.m2d[2][1] + m2d[2][0] * other.m2d[2][2] + m2d[3][0] * 
		other.m2d[2][3];
	result.m2d[3][0] = m2d[0][0] * other.m2d[3][0] + m2d[1][0] * 
		other.m2d[3][1] + m2d[2][0] * other.m2d[3][2] + m2d[3][0] * 
		other.m2d[3][3];

	result.m2d[0][1] = m2d[0][1] * other.m2d[0][0] + m2d[1][1] * 
		other.m2d[0][1] + m2d[2][1] * other.m2d[0][2] + m2d[3][1] * 
		other.m2d[0][3];
	result.m2d[1][1] = m2d[0][1] * other.m2d[1][0] + m2d[1][1] * 
		other.m2d[1][1] + m2d[2][1] * other.m2d[1][2] + m2d[3][1] * 
		other.m2d[1][3];
	result.m2d[2][1] = m2d[0][1] * other.m2d[2][0] + m2d[1][1] * 
		other.m2d[2][1] + m2d[2][1] * other.m2d[2][2] + m2d[3][1] * 
		other.m2d[2][3];
	result.m2d[3][1] = m2d[0][1] * other.m2d[3][0] + m2d[1][1] * 
		other.m2d[3][1] + m2d[2][1] * other.m2d[3][2] + m2d[3][1] * 
		other.m2d[3][3];

	result.m2d[0][2] = m2d[0][2] * other.m2d[0][0] + m2d[1][2] * 
		other.m2d[0][1] + m2d[2][2] * other.m2d[0][2] + m2d[3][2] * 
		other.m2d[0][3];
	result.m2d[1][2] = m2d[0][2] * other.m2d[1][0] + m2d[1][2] * 
		other.m2d[1][1] + m2d[2][2] * other.m2d[1][2] + m2d[3][2] * 
		other.m2d[1][3];
	result.m2d[2][2] = m2d[0][2] * other.m2d[2][0] + m2d[1][2] * 
		other.m2d[2][1] + m2d[2][2] * other.m2d[2][2] + m2d[3][2] * 
		other.m2d[2][3];
	result.m2d[3][2] = m2d[0][2] * other.m2d[3][0] + m2d[1][2] * 
		other.m2d[3][1] + m2d[2][2] * other.m2d[3][2] + m2d[3][2] * 
		other.m2d[3][3];

	result.m2d[0][3] = m2d[0][3] * other.m2d[0][0] + m2d[1][3] * 
		other.m2d[0][1] + m2d[2][3] * other.m2d[0][2] + m2d[3][3] * 
		other.m2d[0][3];
	result.m2d[1][3] = m2d[0][3] * other.m2d[1][0] + m2d[1][3] * 
		other.m2d[1][1] + m2d[2][3] * other.m2d[1][2] + m2d[3][3] * 
		other.m2d[1][3];
	result.m2d[2][3] = m2d[0][3] * other.m2d[2][0] + m2d[1][3] * 
		other.m2d[2][1] + m2d[2][3] * other.m2d[2][2] + m2d[3][3] * 
		other.m2d[2][3];
	result.m2d[3][3] = m2d[0][3] * other.m2d[3][0] + m2d[1][3] * 
		other.m2d[3][1] + m2d[2][3] * other.m2d[3][2] + m2d[3][3] * 
		other.m2d[3][3];

	return result;
}

// multiply each value by a float
Matrix4 Matrix4::operator*(float f)
{
	Matrix4 result;
	for (int i = 0; i < 16; ++i)
		result.m[i] = m[i] * f;
	return result;
}

// cast to a float array
Matrix4::operator float*()
{
	return (float*)&m;
}

// just copied from glm::lookAt
// is that illegal?
// this explains it
// https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/gluLookAt.xml
Matrix4 Matrix4::lookAt(Vector3 start, Vector3 dest, Vector3 up)
{
	Vector3 a = (dest - start).normalised();
	Vector3 b = a.cross(up).normalised();
	Vector3 c = b.cross(a);

	Matrix4 result;

	result.m2d[0][0] = b.x;
	result.m2d[1][0] = b.y;
	result.m2d[2][0] = b.z;

	result.m2d[0][1] = c.x;
	result.m2d[1][1] = c.y;
	result.m2d[2][1] = c.z;

	result.m2d[0][2] = -a.x;
	result.m2d[1][2] = -a.y;
	result.m2d[2][2] = -a.z;

	result.m2d[3][0] = -b.dot(start);
	result.m2d[3][1] = -c.dot(start);
	result.m2d[3][2] =  a.dot(start);

	return result;
}
