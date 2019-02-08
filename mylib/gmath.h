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

struct Vector2;

#define PI 3.141592653589793238f

/***
 *  @brief Gets the sign (positive or negative) of a value
 * 
 *  @param n Value to get the sign of
 *  @return Sign of the value, -1 for negative and 1 for 0 or positive
 */
MYLIB_SPEC inline int sign(float n) { return n < 0 ? -1 : 1; }
/***
 *  @brief Gets the sign (positive or negative) of a value
 * 
 *  @param n Value to get the sign of
 *  @return Sign of the value, -1 for negative and 1 for 0 or positive
 */
MYLIB_SPEC inline int sign(int n) { return n < 0 ? -1 : 1; }

/***
 *  @brief Turns any number of bytes into a hash
 * 
 *  @param val Array of bytes to hash
 *  @param size Number of bytes in the array
 *  @return Hashed value of the bytes
 */
MYLIB_SPEC unsigned int hash(const char* val, int size);

/***
 *  @brief Gets a random integer within a range
 * 
 *  @param min Minimum value of the random number
 *  @param max Maximum value of the random number
 *  @return Random integer between min and max
 */
MYLIB_SPEC int randBetween(int min, int max);

/***
 *  @brief Gets a random float within a range
 * 
 *  @param min Minimum value of the random number
 *  @param max Maximum value of the random number
 *  @return Random float between min and max
 */
MYLIB_SPEC float randBetween(float min, float max);

/***
 *  @brief Checks if two circles intersect
 * 
 *  @param pos1 Middle position of the first circle
 *  @param rad1 Radius of the first circle
 *  @param pos2 Middle position of the second circle
 *  @param rad2 Radius of the second circle
 *  @return Whether or not the circles intersect
 */
MYLIB_SPEC bool circlesIntersect(Vector2 const& pos1, float rad1, 
	Vector2 const& pos2, float rad2);

/***
 *  @brief Linearly interpolates between two values
 * 
 *  @param b Beginning value
 *  @param e End/target value
 *  @param t Time, or percentage of the way through the lerp
 *  @return A value somewhere between b and e
 */
MYLIB_SPEC float lerp(float b, float e, float t);

/***
 *  @brief Easing function which jiggles a little towards the end
 * 
 *  @param b Beginning value
 *  @param e End/target value
 *  @param t Time, or percentage of the way through the ease 
 *  @return An elastic-y interpolation between b and e
 */
MYLIB_SPEC float easeOutElastic(float b, float e, float t);

/***
 *  @brief A sine wave that fades over time
 *			Useful for a "wiggle" effect
 * 
 *  @param t Time, or percentage of the way through the wiggle
 *  @param freq How many times it should wiggle
 *  @return Wiggle-fied value
 */
MYLIB_SPEC float wiggle(float t, float freq);

/***
 * @brief Gets the point on a line which is closest to a given point
 * 
 * @param a Start point of the line
 * @param b End point of the line
 * @param p Point we want the closest value to
 * @return A point on line AB which is closest to point P
 */
MYLIB_SPEC Vector2 closestToLine(Vector2 const& a, Vector2 const& b, 
	Vector2 const& p);