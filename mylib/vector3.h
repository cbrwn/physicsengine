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

struct Vector3
{
	/***
	 * @brief Makes a 3D vector and initialises it to (0,0,0)
	 */
	MYLIB_SPEC Vector3();
	/***
	 * @brief Makes a 3D vector with a set of values
	 */
	MYLIB_SPEC Vector3(float x, float y, float z);
	MYLIB_SPEC ~Vector3() = default;

	// copy constructor
	MYLIB_SPEC Vector3(const Vector3& vec);

	// move constructors aren't needed (I think)
	MYLIB_SPEC Vector3(Vector3&& vec) = default;
	MYLIB_SPEC Vector3& operator=(Vector3&& vec) = default;

	/***
	 * @brief Sets the values of this vector
	 *
	 * @param x New x value
	 * @param y New y value
	 * @param z New z value
	 */
	MYLIB_SPEC inline void set(float x, float y, float z);

	/***
	 * @brief Gets the dot product of this vector and another vector
	 *
	 * @param vec Other vector to get the dot product against
	 * @return Resulting dot product
	 */
	MYLIB_SPEC float dot(Vector3 const& vec) const;
	/***
	 * @brief Gets the cross product of this vector and another vector
	 * 
	 * @param vec Other vector to get the cross product with
	 * @return Resulting vector
	 */
	MYLIB_SPEC Vector3 cross(Vector3 const& vec) const;

	/***
	 * @brief Gets the magnitude/length of this vector
	 *
	 * @return The magnitude/length of this vector
	 */
	MYLIB_SPEC float magnitude() const;
	/***
	 * @brief Gets the squared magnitude of this vector
	 *			Just magnitude() without the sqrt so it's faster
	 *			Useful for comparisons where the actual magnitude isn't needed
	 *
	 * @return The squared magnitude of this vector
	 */
	MYLIB_SPEC float magnitudeSquared() const;

	/***
	 * @brief Normalises this vector and stores that
	 */
	MYLIB_SPEC void normalise();
	/***
	 * @brief Makes a new vector which is this but normalised
	 *
	 * @return A new vector containing this vector's normalised values
	 */
	MYLIB_SPEC Vector3 normalised() const;

	/***
	 * @brief Caps this vector's magnitude at a specific value
	 *
	 * @param mag Magnitude to limit this vector to
	 */
	MYLIB_SPEC void limit(float mag);

	/***
	 * @brief gets the distance between this and another point
	 *
	 * @param vec point to get the distance to
	 * @return distance between these points
	 */
	MYLIB_SPEC float distanceTo(Vector3& vec) const;
	/***
	 * @brief Gets the squared distance between this and another point
	 *			Useful for comparisons when the actual value isn't needed
	 *
	 * @param vec point to get the distance to
	 * @return distance between these points
	 */
	MYLIB_SPEC float distanceToSquared(Vector3& vec) const;

	/***
	 * @brief Gets the dot product between two vectors
	 *
	 * @param a First vector
	 * @param b Second vector
	 * @return Dot product between these vectors
	 */
	MYLIB_SPEC static float dot(Vector3 const& a, Vector3 const& b);
	/***
	 * @brief Gets the cross product from two vectors
	 * 
	 * @param a First vector
	 * @param b Second vector
	 * @return Cross product
	 */
	MYLIB_SPEC static Vector3 cross(Vector3 const& a, Vector3 const& b);
	/***
	 * @brief Linearly interpolates all values between two vectors
	 *
	 * @param a Starting values
	 * @param b Target values
	 * @param t Percentage of the way through the lerp (from 0..1)
	 */
	MYLIB_SPEC static Vector3 lerpBetween(Vector3 const& a, Vector3 const& b, 
		float t);

	/***
	 * @brief Allows rearranging this vector's values using a string
	 *			For example, to turn (2, 1, 0) into (0, 1, 2), 
	 *			layout would be "zyx"
	 *			Any combination of 'x', 'y', 'z', 'r', 'g' and 'b' works
	 *
	 * @param layout String containing the desired layout
	 * @return Resulting swizzled vector
	 */
	MYLIB_SPEC Vector3 swizzle(const char* layout);
	/***
	 * @brief Clamps all values of this vector between a minimum and maximum
	 *
	 * @param min Minimum values
	 * @param max Maximum values
	 */
	MYLIB_SPEC void clamp(Vector3 const& min, Vector3 const& max);

	// operator overloads

	/***
	 * @brief Adds this vector to another vector
	 *
	 * @param vec Vector to add to this
	 * @return Resulting vector
	 */
	MYLIB_SPEC Vector3  operator+ (Vector3 const& vec);
	/***
	 * @brief Subtracts a vector from this vector
	 *
	 * @param vec Vector to subtract from this
	 * @return Resulting vector
	 */
	MYLIB_SPEC Vector3  operator- (Vector3 const& vec);
	/***
	 * @brief Adds a vector to this vector and stores it
	 *
	 * @param vec Vector to add to this
	 * @return A reference to this vector which was changed
	 */
	MYLIB_SPEC Vector3& operator+=(Vector3 const& vec);
	/***
	 * @brief Subtracts a vector from this vector and stores it
	 *
	 * @param vec Vector to subtract from this
	 * @return A reference to this vector which was changed
	 */
	MYLIB_SPEC Vector3& operator-=(Vector3 const& vec);
	/***
	 * @brief Multiplies all values of this vector by a value
	 *
	 * @param mul Value to multiply by
	 * @return Resulting vector
	 */
	MYLIB_SPEC Vector3  operator* (float mul);
	/***
	 * @brief Multiplies all values of this vector by a value and stores it
	 *
	 * @param mul Value to multiply by
	 * @return A reference to this vector which was changed
	 */
	MYLIB_SPEC Vector3& operator*=(float mul);
	/***
	 * @brief Divides all values of this vector by a value
	 *
	 * @param div Value to divide by
	 * @return Resulting vector
	 */
	MYLIB_SPEC Vector3  operator/ (float div);
	/***
	 * @brief Divides all values of this vector by a value and stores it
	 *
	 * @param div Value to divide by
	 * @return A reference to this vector which was changed
	 */
	MYLIB_SPEC Vector3& operator/=(float div);
	/***
	 * @brief Multiplies all values of this vector by another vector's
	 *			corresponding values
	 * 
	 * @param vec Vector to multiply this vector's values by
	 * @return Resulting vector
	 */
	MYLIB_SPEC Vector3  operator* (Vector3 const& vec);
	/***
	 * @brief Multiplies all values of this vector by another vector's
	 *			corresponding values and stores it
	 * 
	 * @param vec Vector to multiply this vector's values by
	 * @return A reference to this vector which was changed
	 */
	MYLIB_SPEC Vector3& operator*=(Vector3 const& vec);
	/***
	 * @brief Copies all values from a vector into this one
	 * 
	 * @param vec Vector to copy values from
	 * @return A reference to this vector which was changed
	 */
	MYLIB_SPEC Vector3& operator= (Vector3 const& vec);
	/***
	 * @brief Checks if this vector is approximately equal to another one
	 * 
	 * @param vec Vector to check equality with
	 * @return Whether or not they are approximately equal
	 */
	MYLIB_SPEC bool		operator==(Vector3 const& vec);
	/***
	 * @brief Checks if all values of this vector are less than another one
	 * 
	 * @param vec Vector to check against
	 * @return Whether or not all values are less
	 */
	MYLIB_SPEC bool		operator< (Vector3 const& vec);
	/***
	 * @brief Checks if all values of this vector are greater than another one
	 * 
	 * @param vec Vector to check against
	 * @return Whether or not all values are greater 
	 */
	MYLIB_SPEC bool		operator> (Vector3 const& vec);
	/***
	 * @brief Allows indexing this vector like an array
	 * 
	 * @param index Index of the value to get (0 for x, 1 for y)
	 * @return A reference to the desired value
	 */
	MYLIB_SPEC float&   operator[](int index);
	MYLIB_SPEC explicit operator float*();

	// values - ability to use xyz or rgb to reference the values
	union { float x, r; };
	union { float y, g; };
	union { float z, b; };
};

// operator for float * Vector3
static Vector3 operator*(float lhs, Vector3 rhs)
{
	return rhs * lhs;
}

// multi-line inline
inline void Vector3::set(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}