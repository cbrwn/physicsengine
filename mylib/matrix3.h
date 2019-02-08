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

#include "vector2.h"
#include "vector3.h"

struct Matrix3
{
	/***
	 * @brief Makes a 3D identity matrix
	 */
	MYLIB_SPEC Matrix3();
	/***
	 * @brief Makes a 3D matrix with given values
	 * 
	 * @param m00 Value to be put in row 0, column 0
	 * @param m01 Value to be put in row 1, column 0
	 * @param m02 Value to be put in row 2, column 0
	 * @param m10 Value to be put in row 0, column 1
	 * @param m11 Value to be put in row 1, column 1
	 * @param m12 Value to be put in row 2, column 1
	 * @param m20 Value to be put in row 0, column 2
	 * @param m21 Value to be put in row 1, column 2
	 * @param m22 Value to be put in row 2, column 2
	 */
	MYLIB_SPEC Matrix3(float m00, float m01, float m02, float m10, float m11, 
		float m12, float m20, float m21, float m22);
	/***
	 * @brief Makes a 3D matrix from an array of values
	 * 
	 * @param values Array of floats, hopefully at least 9
	 */
	MYLIB_SPEC explicit Matrix3(float* values);

	// copy constructors
	MYLIB_SPEC Matrix3(Matrix3 const& other);
	MYLIB_SPEC Matrix3& operator=(Matrix3 const& other);

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
	 */
	MYLIB_SPEC void setPosition(float x, float y);
	/***
	 * @brief Sets the position values of this matrix
	 * 
	 * @param p Position to set the values to
	 */
	MYLIB_SPEC void setPosition(Vector2 p);

	/***
	 * @brief Sets the scale values of this matrix
	 * 
	 * @param x Scale value of the x axis
	 * @param y Scale value of the y axis
	 */
	MYLIB_SPEC void setScale(float x, float y);
	/***
	 * @brief Sets the scale values of this matrix
	 * 
	 * @param s Scale to set the values to
	 */
	MYLIB_SPEC void setScale(Vector2 s);

	/***
	 * @brief Swaps the rows and columns of this matrix and keeps that
	 * 
	 * @return A reference to this matrix which was changed
	 */
	MYLIB_SPEC Matrix3& transpose();
	/***
	 * @brief Returns a new matrix with this matrix's rows and columns swapped
	 * 
	 * @return A new matrix containing the swapped values
	 */
	MYLIB_SPEC Matrix3 transposed();

	/***
	 * @brief Gets the determinant used in getting the inverse
	 * 
	 * @return Determinant of this matrix
	 */
	MYLIB_SPEC float getDeterminant();
	/***
	 * @brief Returns the inverse of this matrix
	 * 
	 * @return A new matrix which is the inverse of this matrix
	 */
	MYLIB_SPEC Matrix3 inverse();

	/***
	 * @brief Allows indexing this matrix to get a Vector containing values of
	 *			the specified column
	 *			
	 * @param index Column of matrix to get as a Vector
	 * @return A reference to the Vector containing this column
	 */
	MYLIB_SPEC Vector3& operator[](int index);

	/***
	 * @brief Gets the position values as a vector
	 * 
	 * @return Vector2 of the position values
	 */
	MYLIB_SPEC Vector2 getPosition();
	/***
	 * @brief Gets a direction vector of this matrix's "up"
	 * 
	 * @return Direction vector pointing up
	 */
	MYLIB_SPEC Vector2 getUp();
	/***
	 * @brief Gets a direction vector of this matrix's "right"
	 * 
	 * @return Direction vector pointing right 
	 */
	MYLIB_SPEC Vector2 getRight();

	/***
	 * @brief Multiplies this matrix by a column vector
	 *
	 * @param v Vector to multiply by
	 * @return A vector containing the result 
	 */
	MYLIB_SPEC Vector3 operator*(Vector3 const& v);
	/***
	 * @brief Multiplies this matrix by another matrix
	 * 
	 * @param other Matrix to multiply it by
	 * @return A new matrix containing the result
	 */
	MYLIB_SPEC Matrix3 operator*(Matrix3 const& other);
	/***
	 * @brief Multiplies each value of this matrix by a float
	 * 
	 * @param f Value to multiply this matrix by
	 * @return A new matrix with each element multiplied by f
	 */
	MYLIB_SPEC Matrix3 operator*(float f);
	/***
	 * @brief Allows casting to a float pointer to use with the bootstrap
	 */
	MYLIB_SPEC explicit operator float*();

	union
	{
		// one-dimensional array with all the values
		float m[9];
		// two-dimensional array with all the values
		float m2d[3][3];
		// three Vector3s containing the values
		Vector3 mvec[3];
	};
};
