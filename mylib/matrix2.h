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

struct Matrix2
{
	/***
	 * @brief Makes a 2D identity matrix
	 */
	MYLIB_SPEC Matrix2();
	/***
	 * @brief Makes a 2D matrix with given values
	 * 
	 * @param m00 Value to be put in row 0, column 0
	 * @param m01 Value to be put in row 1, column 0
	 * @param m10 Value to be put in row 0, column 1
	 * @param m11 Value to be put in row 1, column 1
	 */
	MYLIB_SPEC Matrix2(float m00, float m01, float m10, float m11);
	/***
	 * @brief Makes a 2D matrix from an array of values
	 * 
	 * @param values Array of floats, hopefully at least 4
	 */
	MYLIB_SPEC explicit Matrix2(float* values);

	// copy constructors
	MYLIB_SPEC Matrix2(Matrix2 const& other);
	MYLIB_SPEC Matrix2& operator=(Matrix2 const& other);

	/***
	 * @brief Sets values to turn this into a rotation matrix
	 * 
	 * @param angle Angle to set the rotation matrix to
	 */
	MYLIB_SPEC void setRotate(float angle);

	/***
	 * @brief Swaps the rows and columns of this matrix and keeps that
	 * 
	 * @return A reference to this matrix which was changed
	 */
	MYLIB_SPEC Matrix2& transpose();
	/***
	 * @brief Returns a new matrix with this matrix's rows and columns swapped
	 * 
	 * @return A new matrix containing the swapped values
	 */
	MYLIB_SPEC Matrix2 transposed();

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
	MYLIB_SPEC Matrix2 inverse();

	/***
	 * @brief Allows indexing this matrix to get a Vector containing values of
	 *			the specified column
	 *			
	 * @param index Column of matrix to get as a Vector
	 * @return A reference to the Vector containing this column
	 */
	MYLIB_SPEC Vector2& operator[](int index);

	/***
	 * @brief Multiplies this matrix by a column vector
	 *
	 * @param v Vector to multiply by
	 * @return A vector containing the result 
	 */
	MYLIB_SPEC Vector2 operator*(Vector2 const& v);
	/***
	 * @brief Multiplies this matrix by another matrix
	 * 
	 * @param other Matrix to multiply it by
	 * @return A new matrix containing the result
	 */
	MYLIB_SPEC Matrix2 operator*(Matrix2 const& other);
	/***
	 * @brief Multiplies each value of this matrix by a float
	 * 
	 * @param f Value to multiply this matrix by
	 * @return A new matrix with each element multiplied by f
	 */
	MYLIB_SPEC Matrix2 operator*(float f);
	/***
	 * @brief Allows casting to a float pointer to use with the bootstrap
	 */
	MYLIB_SPEC explicit operator float*();

	union
	{
		// one-dimensional array with all the values
		float m[4];
		// two-dimensional array with all the values
		float m2d[2][2];
		// two Vector2s containing the values
		Vector2 mvec[2];
	};
};