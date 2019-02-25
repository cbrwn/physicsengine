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

#include <glm/glm.hpp>

/***
 *  @brief Converts RGB values from 0..1 into an unsigned integer
 * 
 *  @param r Red value from 0..1
 *  @param g Green value from 0..1
 *  @param b Blue value from 0..1
 *  @param a Alpha value from 0..1
 *  @return Integer with the format 0xRRGGBBAA
 */
MYLIB_SPEC unsigned int rgbToInt(float r, float g, float b, float a = 1.0f);
/***
 *  @brief Converts RGB values from 0..255 into an unsigned integer 
 * 
 *  @param r Red value from 0..255
 *  @param g Green value from 0..255
 *  @param b Blue value from 0..255
 *  @param a Alpha value from 0..255
 *  @return Integer with the format 0xRRGGBBAA
 */
MYLIB_SPEC unsigned int rgbToInt(int r, int g, int b, int a = 255);
/***
 *  @brief Converts HSB values from 0..1 into an RGB unsigned integer 
 * 
 *  @param hue Hue value from 0..1
 *  @param saturation Saturation value from 0..1
 *  @param brightness Brightness value from 0..1
 *  @return Integer with the format 0xRRGGBBAA
 */
MYLIB_SPEC unsigned int hsb(float hue, float saturation, float brightness);

/***
 *  @brief Converts HSB values from 0..1 into Vector4 with RGBA values 
 *			ranging from 0..1
 * 
 *  @param hue Hue value from 0..1
 *  @param saturation Saturation value from 0..1
 *  @param brightness Brightness value from 0..1
 *  @return Vector4 containing RGBA values from 0..1
 */
MYLIB_SPEC glm::vec4 hsbVec(float hue, float saturation, float brightness);

/***
 *  @brief Converts an RGB unsigned integer into a Vector4 with RGBA values
 *			ranging from 0..1
 * 
 *  @param color Unsigned integer with the format 0xRRGGBBAA
 *  @return Vector4 containing RGBA values from 0..1
 */
MYLIB_SPEC glm::vec4 intToVector(unsigned int color);