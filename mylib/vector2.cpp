#include "vector2.h"

#include <cmath>
#include <cstring>
#include <cassert>

#include "gmath.h"

Vector2::Vector2()
	: x(0), y(0) { }

Vector2::Vector2(float x, float y)
	: x(x), y(y) { }

Vector2::Vector2(const Vector2& vec)
{
	x = vec.x;
	y = vec.y;
}

float Vector2::dot(Vector2 const& vec) const
{
	return Vector2::dot(*this, vec);
}

float Vector2::magnitude() const
{
	return sqrtf((x*x) + (y*y));
}

// magnitude without the need for sqrt
// useful for comparisons when the actual magnitude isn't required
float Vector2::magnitudeSquared() const
{
	return (x*x) + (y*y);
}

void Vector2::normalise()
{
	float mag = magnitude();
	x /= mag;
	y /= mag;
}

Vector2 Vector2::normalised() const
{
	float mag = magnitude();
	return { x / mag, y / mag };
}

void Vector2::limit(float mag)
{
	// only limit if we're over the limit
	if (magnitudeSquared() <= mag * mag)
		return;

	Vector2 norm = normalised();
	x = norm.x*mag;
	y = norm.y*mag;
}

float Vector2::angle() const
{
	return atan2f(y, x);
}

float Vector2::angleTo(Vector2 const& vec)
{
	Vector2 v = *this - vec;
	return v.angle();
}

float Vector2::dot(Vector2 const & a, Vector2 const & b)
{
	return a.x * b.x + a.y * b.y;
}

Vector2 Vector2::fromAngle(float a)
{
	return { cosf(a), sinf(a) };
}

float Vector2::distanceTo(Vector2& vec) const
{
	float distX = x - vec.x;
	float distY = y - vec.x;
	return sqrtf((distX*distX) + (distY*distY));
}

// distance without the need for sqrt
// useful for comparisons, when the actual distance isn't required
float Vector2::distanceToSquared(Vector2& vec) const
{
	float distX = x - vec.x;
	float distY = y - vec.y;
	return (distX*distX) + (distY*distY);
}

Vector2 Vector2::lerpBetween(Vector2 const& a, Vector2 const& b, float t)
{
	Vector2 result;
	result.x = lerp(a.x, b.x, t);
	result.y = lerp(a.y, b.y, t);
	return result;
}

Vector2 Vector2::swizzle(const char* layout)
{
	// use the length of the string to choose how many values
	// we want to swizzle, even though it's kinda useless in vec2D
	int length = (int)strlen(layout);
	if (length > 2)
		length = 2;

	Vector2 result;
	for (int i = 0; i < length; ++i)
	{
		float val = x;
		switch (layout[i])
		{
		case 'x':
			val = x;
			break;
		case 'y':
			val = y;
			break;
		default:
			val = 0.0f;
		}
		result[i] = val;
	}
	return result;
}

void Vector2::clamp(Vector2 const& min, Vector2 const& max)
{
	if (x < min.x)
		x = min.x;
	if (y < min.y)
		y = min.y;

	if (x > max.x)
		x = max.x;
	if (y > max.y)
		y = max.y;
}

Vector2 Vector2::operator+(Vector2 const& vec)
{
	return { x + vec.x, y + vec.y };
}

Vector2 Vector2::operator-(Vector2 const& vec)
{
	return { x - vec.x, y - vec.y };
}

Vector2 Vector2::operator*(float mul)
{
	return { x * mul, y * mul };
}

Vector2& Vector2::operator*=(float mul)
{
	x *= mul;
	y *= mul;
	return *this;
}

Vector2 Vector2::operator/(float div)
{
	return { x / div, y / div };
}

Vector2& Vector2::operator/=(float div)
{
	x /= div;
	y /= div;
	return *this;
}

Vector2 Vector2::operator*(Vector2 const& vec)
{
	return { x * vec.x, y * vec.y };
}

Vector2& Vector2::operator*=(Vector2 const& vec)
{
	x *= vec.x;
	y *= vec.y;
	return *this;
}

Vector2& Vector2::operator=(Vector2 const& vec)
{
	x = vec.x;
	y = vec.y;
	return *this;
}

Vector2& Vector2::operator+=(Vector2 const& vec)
{
	x += vec.x;
	y += vec.y;
	return *this;
}

Vector2& Vector2::operator-=(Vector2 const& vec)
{
	x -= vec.x;
	y -= vec.y;
	return *this;
}

// checks for approximate equality by checking if the difference
// is lower than some arbitrary value
bool Vector2::operator==(Vector2 const& vec)
{
	// our arbitrary value
	const float allowance = 0.001f;

	// grab the absolute differences of the components
	float xdif = fabsf(vec.x - x);
	float ydif = fabsf(vec.y - y);

	// it's approximately equal if BOTH of the components are close
	return xdif < allowance && ydif < allowance;
}

bool Vector2::operator<(Vector2 const& vec)
{
	return x < vec.x && y < vec.y;
}

bool Vector2::operator>(Vector2 const& vec)
{
	return x > vec.x && y > vec.y;
}

// allows indexing the vector like an array
// dangerous! out of bounds has unknown results
float& Vector2::operator[](int index)
{
	// make sure the index is within range
	assert(index >= 0 && index < 2);
	return (&x)[index];
}

// allows conversion from a vector to an array of floats
// also dangerous
Vector2::operator float*()
{
	return &x;
}
