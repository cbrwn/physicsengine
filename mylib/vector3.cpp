#include "vector3.h"

#include <cmath>
#include <cstring>
#include <cassert>

#include "gmath.h"

Vector3::Vector3()
	: x(0), y(0), z(0) { } 

Vector3::Vector3(float x, float y, float z)
	: x(x), y(y), z(z) { }

Vector3::Vector3(int x, int y, int z)
{
	this->x = (float)x;
	this->y = (float)y;
	this->z = (float)z;
}

Vector3::Vector3(const Vector3& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
}

float Vector3::dot(Vector3 const& vec) const
{
	return Vector3::dot(*this, vec);
}

Vector3 Vector3::cross(Vector3 const& vec) const
{
	return Vector3::cross(*this, vec);
}

float Vector3::magnitude() const
{
	return sqrtf((x*x) + (y*y) + (z*z));
}

// magnitude without the need for sqrt
// useful for comparisons when the actual magnitude isn't required
float Vector3::magnitudeSquared() const
{
	return (x*x) + (y*y) + (z*z);
}

void Vector3::normalise()
{
	float mag = magnitude();
	x /= mag;
	y /= mag;
	z /= mag;
}

Vector3 Vector3::normalised() const
{
	float mag = magnitude();
	return {x / mag, y / mag, z / mag };
}

void Vector3::limit(float mag)
{
	// only limit if we're over the limit
	if (magnitudeSquared() <= mag*mag)
		return;

	Vector3 norm = normalised();
	x = norm.x*mag;
	y = norm.y*mag;
	z = norm.z*mag;
}

float Vector3::dot(Vector3 const& a, Vector3 const& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3 Vector3::cross(Vector3 const& a, Vector3 const& b)
{
	Vector3 result;
	result.x = (a.y * b.z) - (b.y * a.z);
	result.y = (a.z * b.x) - (b.z * a.x);
	result.z = (a.x * b.y) - (b.x * a.y);
	return result;
}

float Vector3::distanceTo(Vector3& vec) const
{
	float distX = x - vec.x;
	float distY = y - vec.x;
	float distZ = z - vec.z;
	return sqrtf((distX*distX) + (distY*distY) + (distZ*distZ));
}

// distance without the need for sqrt
// useful for comparisons, when the actual distance isn't required
float Vector3::distanceToSquared(Vector3& vec) const
{
	float distX = x - vec.x;
	float distY = y - vec.y;
	float distZ = z - vec.z;
	return (distX*distX) + (distY*distY) + (distZ*distZ);
}

Vector3 Vector3::lerpBetween(Vector3 const& a, Vector3 const& b, float t)
{
	Vector3 result;
	result.x = lerp(a.x, b.x, t);
	result.y = lerp(a.y, b.y, t);
	result.z = lerp(a.z, b.z, t);
	return result;
}

Vector3 Vector3::swizzle(const char* layout)
{
	// use the length of the string to choose how many values
	// we want to swizzle
	int length = (int)strlen(layout);
	if (length > 3)
		length = 3;

	Vector3 result;
	for (int i = 0; i < length; ++i)
	{
		float val = x;
		switch (layout[i])
		{
		case 'x':
		case 'r':
			val = x;
			break;
		case 'y':
		case 'g':
			val = y;
			break;
		case 'z':
		case 'b':
			val = z;
			break;
		default:
			val = 0.0f;
		}
		result[i] = val;
	}
	return result;
}

void Vector3::clamp(Vector3 const& min, Vector3 const& max)
{
	if (x < min.x)
		x = min.x;
	if (y < min.y)
		y = min.y;
	if (z < min.z)
		z = min.z;

	if (x > max.x)
		x = max.x;
	if (y > max.y)
		y = max.y;
	if (z > max.z)
		z = max.z;
}

Vector3 Vector3::operator+(Vector3 const& vec)
{
	return {x + vec.x, y + vec.y, z + vec.z };
}

Vector3 Vector3::operator-(Vector3 const& vec)
{
	return {x - vec.x, y - vec.y, z - vec.z };
}

Vector3 Vector3::operator*(float mul)
{
	return {x * mul, y * mul, z * mul };
}

Vector3& Vector3::operator*=(float mul)
{
	x *= mul;
	y *= mul;
	z *= mul;
	return *this;
}

Vector3 Vector3::operator/(float div)
{
	return { x / div, y / div, z / div };
}

Vector3& Vector3::operator/=(float div)
{
	x /= div;
	y /= div;
	z /= div;
	return *this;
}

Vector3 Vector3::operator*(Vector3 const& vec)
{
	return { x * vec.x, y * vec.y, z * vec.z };
}

Vector3& Vector3::operator*=(Vector3 const& vec)
{
	x *= vec.x;
	y *= vec.y;
	z *= vec.z;
	return *this;
}

Vector3& Vector3::operator=(Vector3 const& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
	return *this;
}

Vector3& Vector3::operator+=(Vector3 const& vec)
{
	x += vec.x;
	y += vec.y;
	z += vec.z;
	return *this;
}

Vector3& Vector3::operator-=(Vector3 const& vec)
{
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;
	return *this;
}

// checks for approximate equality by checking if the difference
// is lower than some arbitrary value
bool Vector3::operator==(Vector3 const& vec)
{
	// our arbitrary value
	const float allowance = 0.001f;

	// grab the absolute differences of the components
	float xdif = fabsf(vec.x - x);
	float ydif = fabsf(vec.y - y);
	float zdif = fabsf(vec.z - z);

	// it's approximately equal if all of the components are close
	return xdif < allowance &&
		ydif < allowance &&
		zdif < allowance;
}

bool Vector3::operator<(Vector3 const& vec)
{
	return x < vec.x && y < vec.y && z < vec.z;
}

bool Vector3::operator>(Vector3 const& vec)
{
	return x > vec.x && y > vec.y && z > vec.z;
}

// allows indexing the vector like an array
// dangerous! out of bounds has unknown results
float& Vector3::operator[](int index)
{
	// make sure the index is within range
	assert(index >= 0 && index < 3);
	return (&x)[index];
}

// allows conversion from a vector to an array of floats
// also dangerous
Vector3::operator float*()
{
	return &x;
}
