#include "vector4.h"

#include <cmath>
#include <cstring>
#include <cassert>

#include "gmath.h"

Vector4::Vector4()
	: x(0), y(0), z(0), w(0) { }

Vector4::Vector4(float x, float y, float z, float w)
	: x(x), y(y), z(z), w(w) { }

Vector4::Vector4(const Vector4& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
	w = vec.w;
}

float Vector4::dot(Vector4 const& vec) const
{
	return Vector4::dot(*this, vec);
}

Vector4 Vector4::cross(Vector4 const& vec) const
{
	return Vector4::cross(*this, vec);
}

float Vector4::magnitude() const
{
	return sqrtf((x*x) + (y*y) + (z*z));
}

// magnitude without the need for sqrt
// useful for comparisons when the actual magnitude isn't required
float Vector4::magnitudeSquared() const
{
	return (x*x) + (y*y) + (z*z);
}

void Vector4::normalise()
{
	float mag = magnitude();
	x /= mag;
	y /= mag;
	z /= mag;
	w /= mag;
}

Vector4 Vector4::normalised() const
{
	float mag = magnitude();
	return {x / mag, y / mag, z / mag, w/mag };
}

void Vector4::limit(float mag)
{
	// only limit if we're over the limit
	if (magnitudeSquared() <= mag*mag)
		return;

	Vector4 norm = normalised();
	x = norm.x*mag;
	y = norm.y*mag;
	z = norm.z*mag;
}

float Vector4::dot(Vector4 const& a, Vector4 const& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector4 Vector4::cross(Vector4 const& a, Vector4 const& b)
{
	Vector4 result;
	result.x = (a.y * b.z) - (b.y * a.z);
	result.y = (a.z * b.x) - (b.z * a.x);
	result.z = (a.x * b.y) - (b.x * a.y);
	result.w = 0;
	return result;
}

float Vector4::distanceTo(Vector4& vec) const
{
	float distX = x - vec.x;
	float distY = y - vec.x;
	float distZ = z - vec.z;
	return sqrtf((distX*distX) + (distY*distY) + (distZ*distZ));
}

// distance without the need for sqrt
// useful for comparisons, when the actual distance isn't required
float Vector4::distanceToSquared(Vector4& vec) const
{
	float distX = x - vec.x;
	float distY = y - vec.y;
	float distZ = z - vec.z;
	return (distX*distX) + (distY*distY) + (distZ*distZ);
}

Vector4 Vector4::lerpBetween(Vector4 const& a, Vector4 const& b, float t)
{
	Vector4 result;
	result.x = lerp(a.x, b.x, t);
	result.y = lerp(a.y, b.y, t);
	result.z = lerp(a.z, b.z, t);
	result.w = lerp(a.w, b.w, t);
	return result;
}

Vector4 Vector4::swizzle(const char* layout)
{
	// use the length of the string to choose how many values
	// we want to swizzle
	int length = (int)strlen(layout);
	if (length > 4)
		length = 4;

	Vector4 result;
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
		case 'w':
		case 'a':
			val = w;
			break;
		default:
			val = 0.0f;
		}
		result[i] = val;
	}
	return result;
}

void Vector4::clamp(Vector4 const& min, Vector4 const& max)
{
	if (x < min.x)
		x = min.x;
	if (y < min.y)
		y = min.y;
	if (z < min.z)
		z = min.z;
	if (w < min.w)
		w = min.w;

	if (x > max.x)
		x = max.x;
	if (y > max.y)
		y = max.y;
	if (z > max.z)
		z = max.z;
	if (w > max.w)
		w = max.w;
}

Vector4 Vector4::operator+(Vector4 const& vec)
{
	return {x + vec.x, y + vec.y, z + vec.z, w + vec.w };
}

Vector4 Vector4::operator-(Vector4 const& vec)
{
	return {x - vec.x, y - vec.y, z - vec.z, w - vec.w };
}

Vector4 Vector4::operator*(float mul)
{
	return {x * mul, y * mul, z * mul, w * mul };
}

Vector4& Vector4::operator*=(float mul)
{
	x *= mul;
	y *= mul;
	z *= mul;
	w *= mul;
	return *this;
}

Vector4 Vector4::operator/(float div)
{
	return { x / div, y / div, z / div, w / div };
}

Vector4& Vector4::operator/=(float div)
{
	x /= div;
	y /= div;
	z /= div;
	w /= div;
	return *this;
}

Vector4 Vector4::operator*(Vector4 const& vec)
{
	return { x * vec.x, y * vec.y, z * vec.z, w * vec.w };
}

Vector4& Vector4::operator*=(Vector4 const& vec)
{
	x *= vec.x;
	y *= vec.y;
	z *= vec.z;
	w *= vec.w;
	return *this;
}

Vector4& Vector4::operator=(Vector4 const& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
	w = vec.w;
	return *this;
}

Vector4& Vector4::operator+=(Vector4 const& vec)
{
	x += vec.x;
	y += vec.y;
	z += vec.z;
	w += vec.w;
	return *this;
}

Vector4& Vector4::operator-=(Vector4 const& vec)
{
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;
	w -= vec.w;
	return *this;
}

// checks for approximate equality by checking if the difference
// is lower than some arbitrary value
bool Vector4::operator==(Vector4 const& vec)
{
	// our arbitrary value
	const float allowance = 0.001f;

	// grab the absolute differences of the components
	float xdif = fabsf(vec.x - x);
	float ydif = fabsf(vec.y - y);
	float zdif = fabsf(vec.z - z);
	float wdif = fabsf(vec.w - w);

	// it's approximately equal if all of the components are close
	return xdif < allowance &&
		ydif < allowance &&
		zdif < allowance &&
		wdif < allowance;
}

bool Vector4::operator<(Vector4 const& vec)
{
	return x < vec.x && y < vec.y && z < vec.z && w < vec.w;
}

bool Vector4::operator>(Vector4 const& vec)
{
	return x > vec.x && y > vec.y && z > vec.z && w > vec.w;
}

// allows indexing the vector like an array
// potentially dangerous! out of bounds has unknown results
float& Vector4::operator[](int index)
{
	// make sure the index is within range
	assert(index >= 0 && index < 4);
	return (&x)[index];
}

// allows conversion from a vector to an array of floats
// also potentially dangerous
Vector4::operator float*()
{
	return &x;
}

Vector4::operator Vector3()
{
	return Vector3(x, y, z);
}
