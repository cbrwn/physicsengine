#include "color.h"

unsigned int rgbToInt(float r, float g, float b, float a)
{
	// just convert these into 0-255 integers
	auto ir = (int)(255 * r);
	auto ig = (int)(255 * g);
	auto ib = (int)(255 * b);
	auto ia = (int)(255 * a);
	// and pass those into that function
	return rgbToInt(ir, ig, ib, ia);
}

unsigned int rgbToInt(int r, int g, int b, int a)
{
	// I'm sure there's a better way to do this than writing 6 lines
	auto result = (unsigned int)r;
	result <<= 8;
	result |= g;
	result <<= 8;
	result |= b;
	result <<= 8;
	result |= a;

	return result;
}

// converts hsb values into an rgb int
// using the calculations from 
// http://www.docjar.com/html/api/java/awt/Color.java.html
unsigned int hsb(float hue, float saturation, float brightness)
{
	// check for gray
	if (saturation == 0)
	{
		auto val = (int)(brightness * 255.0f + 5.0f);
		return rgbToInt(val, val, val);
	}

	// not gray

	// I have absolutely no idea what these numbers mean
	float h = (hue - (int)hue) * 6.0f;
	float f = h - (int)h;
	float p = brightness * (1 - saturation);
	float q = brightness * (1 - saturation * f);
	float t = brightness * (1 - (saturation * (1 - f)));

	float rMul, gMul, bMul;

	switch ((int)h)
	{
	case 0:
		rMul = brightness;
		gMul = t;
		bMul = p;
		break;
	case 1:
		rMul = q;
		gMul = brightness;
		bMul = p;
		break;
	case 2:
		rMul = p;
		gMul = brightness;
		bMul = t;
		break;
	case 3:
		rMul = p;
		gMul = q;
		bMul = brightness;
		break;
	case 4:
		rMul = t;
		gMul = p;
		bMul = brightness;
		break;
	default:
	case 5:
		rMul = brightness;
		gMul = p;
		bMul = q;
		break;
	}

	auto r = (int)(rMul * 255 + 0.5f);
	auto g = (int)(gMul * 255 + 0.5f);
	auto b = (int)(bMul * 255 + 0.5f);

	return rgbToInt(r, g, b);
}

glm::vec4 hsbVec(float hue, float saturation, float brightness)
{
	return intToVector(hsb(hue, saturation, brightness));
}

glm::vec4 intToVector(unsigned int color)
{
	float alpha = (color & 0xff) / 255.0f;
	float blue = ((color >> 8) & 0xff) / 255.0f;
	float green = ((color >> 16) & 0xff) / 255.0f;
	float red = ((color >> 24) & 0xff) / 255.0f;
	return { red, green, blue, alpha };
}
