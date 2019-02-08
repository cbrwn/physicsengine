#pragma once

#ifdef MYLIB_DYNAMIC
	#ifdef MYLIB_EXPORT
		#define MYLIB_SPEC __declspec(dllexport)
	#else
		#define MYLIB_SPEC __declspec(dllimport)
	#endif
#else
	#define MYLIB_SPEC 
#endif

#include "vector3.h"
#include "vector4.h"

struct Matrix4
{
	/***
	 * @brief Makes a 4D identity matrix
	 */
	MYLIB_SPEC Matrix4();
	/***
	 * @brief Makes a 4D matrix with given values
	 * 
	 * @param m00 Value to be put in row 0, column 0
	 * @param m01 Value to be put in row 1, column 0
	 * @param m02 Value to be put in row 2, column 0
	 * @param m03 Value to be put in row 3, column 0
	 * @param m10 Value to be put in row 0, column 1
	 * @param m11 Value to be put in row 1, column 1
	 * @param m12 Value to be put in row 2, column 1
	 * @param m13 Value to be put in row 3, column 1
	 * @param m20 Value to be put in row 0, column 2
	 * @param m21 Value to be put in row 1, column 2
	 * @param m22 Value to be put in row 2, column 2
	 * @param m23 Value to be put in row 3, column 2
	 * @param m30 Value to be put in row 0, column 3
	 * @param m31 Value to be put in row 1, column 3
	 * @param m32 Value to be put in row 2, column 3
	 * @param m33 Value to be put in row 3, column 3
	 */
	MYLIB_SPEC Matrix4(float m00, float m01, float m02, float m03, float m10, 
		float m11, float m12, float m13, float m20, float m21, float m22, 
		float m23, float m30, float m31, float m32, float m33);
	/***
	 * @brief Makes a 4D matrix from an array of values
	 * 
	 * @param values Array of floats, hopefully at least 16
	 */
	MYLIB_SPEC explicit Matrix4(float* values);

	// copy constructors
	MYLIB_SPEC Matrix4(Matrix4 const& other);
	MYLIB_SPEC Matrix4& operator=(Matrix4 const& other);

	/***
	 * @brief Makes a rotation matrix by combining x, y and z rotation matrices
	 * 
	 * @param v Vector containing the x, y and z angles
	 */
	MYLIB_SPEC void setRotate(Vector3 const& v);
	/***
	 * @brief Sets values to make this a rotation matrix on the x axis
	 * 
	 * @param a Angle to set the rotation matrix to
	 */
	MYLIB_SPEC void setRotateX(float a);
	/***
	 * @brief Sets values to make this a rotation matrix on the y axis
	 * 
	 * @param a Angle to set the rotation matrix to
	 */
	MYLIB_SPEC void setRotateY(float a);
	/***
	 * @brief Sets values to make this a rotation matrix on the z axis
	 * 
	 * @param a Angle to set the rotation matrix to
	 */
	MYLIB_SPEC void setRotateZ(float a);

	/***
	 * @brief Sets the position values of this matrix
	 * 
	 * @param x x-position to set
	 * @param y y-position to set
	 * @param z z-position to set
	 */
	MYLIB_SPEC void setPosition(float x, float y, float z);
	/***
	 * @brief Sets the position values of this matrix
	 * 
	 * @param p Position to set the values to
	 */
	MYLIB_SPEC void setPosition(Vector3 p);

	/***
	 * @brief Sets the scale values of this matrix
	 * 
	 * @param x Scale value of the x axis
	 * @param y Scale value of the y axis
	 * @param z Scale value of the z axis
	 */
	MYLIB_SPEC void setScale(float x, float y, float z);
	/***
	 * @brief Sets the scale values of this matrix
	 * 
	 * @param s Scale to set the values to
	 */
	MYLIB_SPEC void setScale(Vector3 s);

	/***
	 * @brief Swaps the rows and columns of this matrix and keeps that
	 * 
	 * @return A reference to this matrix which was changed
	 */
	MYLIB_SPEC Matrix4& transpose();
	/***
	 * @brief Returns a new matrix with this matrix's rows and columns swapped
	 * 
	 * @return A new matrix containing the swapped values
	 */
	MYLIB_SPEC Matrix4 transposed();

	/***
	 * @brief Returns the inverse of this matrix
	 * 
	 * @return A new matrix which is the inverse of this matrix
	 */
	MYLIB_SPEC Matrix4 inverse();

	/***
	 * @brief Allows indexing this matrix to get a Vector containing values of
	 *			the specified column
	 *			
	 * @param index Column of matrix to get as a Vector
	 * @return A reference to the Vector containing this column
	 */
	MYLIB_SPEC Vector4& operator[](int index);

	/***
	 * @brief Gets the position values as a vector
	 * 
	 * @return Vector2 of the position values
	 */
	MYLIB_SPEC Vector3 getPosition();
	/***
	 * @brief Gets a direction vector of this matrix's "forward"
	 * 
	 * @return Direction vector pointing forward 
	 */
	MYLIB_SPEC Vector3 getForward();
	/***
	 * @brief Gets a direction vector of this matrix's "up"
	 * 
	 * @return Direction vector pointing up
	 */
	MYLIB_SPEC Vector3 getUp();
	/***
	 * @brief Gets a direction vector of this matrix's "right"
	 * 
	 * @return Direction vector pointing right 
	 */
	MYLIB_SPEC Vector3 getRight();

	/***
	 * @brief Extracts euler angles from the matrix
	 * 
	 * @return Vector containing euler angles
	 */
	MYLIB_SPEC Vector3 getAngles();

	/***
	 * @brief Multiplies this matrix by a column vector
	 *
	 * @param v Vector to multiply by
	 * @return A vector containing the result 
	 */
	MYLIB_SPEC Vector4 operator*(Vector4 const& v);
	/***
	 * @brief Multiplies this matrix by another matrix
	 * 
	 * @param other Matrix to multiply it by
	 * @return A new matrix containing the result
	 */
	MYLIB_SPEC Matrix4 operator*(Matrix4 const& other);
	/***
	 * @brief Multiplies each value of this matrix by a float
	 * 
	 * @param f Value to multiply this matrix by
	 * @return A new matrix with each element multiplied by f
	 */
	MYLIB_SPEC Matrix4 operator*(float f);
	/***
	 * @brief Allows casting to a float pointer to use with the bootstrap
	 */
	MYLIB_SPEC explicit operator float*();

	/***
	 * @brief Creates a view matrix at the start position, facing dest
	 *			Equivalent to glm's lookAt function
	 * 
	 * @param start Position to look from
	 * @param dest Position to look towards
	 * @param up Up vector, usually (0, 1, 0)
	 * @return Resulting view matrix
	 */
	MYLIB_SPEC static Matrix4 lookAt(Vector3 start, Vector3 dest, 
		Vector3 up = {0, 1, 0});

	union
	{
		// one-dimensional array with all the values
		float m[16];
		// two-dimensional array with all the values
		float m2d[4][4];
		// four Vector4s containing the values
		Vector4 mvec[4];
	};
};
