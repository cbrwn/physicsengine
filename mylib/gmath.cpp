#include "gmath.h"

#include <random>
#include <cstdlib>

#include "vector2.h"

unsigned int hash(const char* val, int size)
{
	if (size <= 0)
		return 0;

	unsigned int result = 0;
	for (int i = 0; i < size; ++i)
	{
		// I don't really know what I'm doing here
		// just doing some random things
		result += val[i] ^ (val[i] << 3 * 8);
		result -= val[i] ^ 0xfbc;
		result = result << 8;
		result ^= val[i] * 3;
	}

	result ^= val[size - 1];
	result *= val[size - 1];
	return result;
}

// I think only one of these should be created
// so do it globally (sorry)
std::random_device _gmath_random_device;
std::mt19937 _gmath_random_engine(_gmath_random_device());

int randBetween(int min, int max)
{
	std::uniform_int_distribution<int> dist(min, max);
	return dist(_gmath_random_engine);
}

float randBetween(float min, float max)
{
	std::uniform_real_distribution<float> dist(min, max);
	return dist(_gmath_random_engine);
}

bool circlesIntersect(Vector2& pos1, float rad1, Vector2& pos2, float rad2)
{
	// get the squared distance between them so we don't have to use a sqrt
	float dist = pos1.distanceToSquared(pos2);
	// add the radii - the distance will be less than it if it's intersecting
	float minDist = rad1 + rad2;
	// square it because we got the squared distance
	minDist *= minDist;
	return dist <= minDist;
}

float lerp(float b, float e, float t)
{
	return b + (e - b) * t;
}

float easeOutElastic(float b, float e, float t)
{
	float u = (t - 1.0f) * (t - 1.0f);
	float t2 = 1.0f - u * u * cosf(t * PI * 4.5f);
	return lerp(b, e, t2);
}

float wiggle(float t, float freq)
{
	// use sin for the actual wiggle, and use lerp to 'fade' it out
	return (sinf(t * freq)+1) * lerp(1, 0, t);
}

// https://stackoverflow.com/questions/3120357/get-closest-point-to-a-line
Vector2 closestToLine(Vector2 const &a, Vector2 const& b, Vector2 const& p)
{
	auto ap = (Vector2)p - (Vector2)a;
	auto ab = (Vector2)b - (Vector2)a;

	float abMag = ab.magnitudeSquared();
	float abap = ap.dot(ab);
	float dist = abap / abMag;

	if (dist < 0)
		return a;

	if (dist > 1)
		return b;

	return (Vector2)a + ab * dist;
}
