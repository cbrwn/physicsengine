/* =================================
 *  Shapes
 *  Additional functions to add shapes to the Gizmos system
 * ================================= */
#include "shapes.h"

#include <gmath.h>
#include <Gizmos.h>
#include <glm/ext.hpp>

void drawBox(glm::vec3 const& center, glm::vec3 const& size, glm::vec4 const& fill,
	glm::mat4* transform, bool drawLines, glm::vec4 const& outline)
{
	// copied from Gizmos::addAABBFilled

	using namespace aie;
	using namespace glm;

	vec3 vVerts[8];
	vec3 tempCenter = center;
	vec3 rvExtents = size;
	vec3 vX(rvExtents.x, 0, 0);
	vec3 vY(0, rvExtents.y, 0);
	vec3 vZ(0, 0, rvExtents.z);

	if (transform)
	{
		mat4 trans = *transform;

		vX = vec3((trans * vec4(vX, 0)));
		vY = vec3((trans * vec4(vY, 0)));
		vZ = vec3((trans * vec4(vZ, 0)));
		tempCenter = vec3((trans)[3]) + tempCenter;
	}

	// top verts
	vVerts[0] = tempCenter - vX - vZ - vY;
	vVerts[1] = tempCenter - vX + vZ - vY;
	vVerts[2] = tempCenter + vX + vZ - vY;
	vVerts[3] = tempCenter + vX - vZ - vY;

	// bottom verts
	vVerts[4] = tempCenter - vX - vZ + vY;
	vVerts[5] = tempCenter - vX + vZ + vY;
	vVerts[6] = tempCenter + vX + vZ + vY;
	vVerts[7] = tempCenter + vX - vZ + vY;

	vec4 lineCol = outline;
	vec4 fillCol = fill;

	if (drawLines)
	{
		Gizmos::addLine(vVerts[0], vVerts[1], lineCol, lineCol);
		Gizmos::addLine(vVerts[1], vVerts[2], lineCol, lineCol);
		Gizmos::addLine(vVerts[2], vVerts[3], lineCol, lineCol);
		Gizmos::addLine(vVerts[3], vVerts[0], lineCol, lineCol);

		Gizmos::addLine(vVerts[4], vVerts[5], lineCol, lineCol);
		Gizmos::addLine(vVerts[5], vVerts[6], lineCol, lineCol);
		Gizmos::addLine(vVerts[6], vVerts[7], lineCol, lineCol);
		Gizmos::addLine(vVerts[7], vVerts[4], lineCol, lineCol);

		Gizmos::addLine(vVerts[0], vVerts[4], lineCol, lineCol);
		Gizmos::addLine(vVerts[1], vVerts[5], lineCol, lineCol);
		Gizmos::addLine(vVerts[2], vVerts[6], lineCol, lineCol);
		Gizmos::addLine(vVerts[3], vVerts[7], lineCol, lineCol);
	}

	// top
	Gizmos::addTri(vVerts[2], vVerts[1], vVerts[0], fillCol);
	Gizmos::addTri(vVerts[3], vVerts[2], vVerts[0], fillCol);

	// bottom
	Gizmos::addTri(vVerts[5], vVerts[6], vVerts[4], fillCol);
	Gizmos::addTri(vVerts[6], vVerts[7], vVerts[4], fillCol);

	// front
	Gizmos::addTri(vVerts[4], vVerts[3], vVerts[0], fillCol);
	Gizmos::addTri(vVerts[7], vVerts[3], vVerts[4], fillCol);

	// back
	Gizmos::addTri(vVerts[1], vVerts[2], vVerts[5], fillCol);
	Gizmos::addTri(vVerts[2], vVerts[6], vVerts[5], fillCol);

	// left
	Gizmos::addTri(vVerts[0], vVerts[1], vVerts[4], fillCol);
	Gizmos::addTri(vVerts[1], vVerts[5], vVerts[4], fillCol);

	// right
	Gizmos::addTri(vVerts[2], vVerts[3], vVerts[7], fillCol);
	Gizmos::addTri(vVerts[6], vVerts[2], vVerts[7], fillCol);
}

void drawSphere(glm::vec3 const& center, float radius, glm::vec4 const& fill,
	glm::mat4* transform, int rows, int columns, bool drawOutline,
	glm::vec4 const& outline)
{
	// copied from Gizmos::addsphere

	using namespace aie;
	using namespace glm;

	const float longMin = 0.0f;
	const float longMax = 360.0f;
	const float latMin = -90.0f;
	const float latMax = 90;

	const float DEG2RAD = PI / 180.0f;

	//Lets put everything in radians first
	const float latitiudinalRange = (latMax - latMin) * DEG2RAD;
	const float longitudinalRange = (longMax - longMin) * DEG2RAD;

	float inverseRadius = 1 / radius;

	// invert these first as the multiply is slightly quicker
	float invColumns = 1.0f / columns;
	float invRows = 1.0f / rows;


	vec3 tempCenter = center;
	if (transform)
		tempCenter = glm::vec3((*transform)[3]) + tempCenter;


	// for each row of the mesh
	vec3* v4Array = new vec3[rows*columns + columns];

	// convert my stuff into glm stuff
	mat4 transMat4;
	if (transform)
		transMat4 = *transform;
	vec4 fillColour = fill;
	vec4 lineColour = outline;

	for (int row = 0; row <= rows; ++row)
	{
		// y ordinates this may be a little confusing but here we are navigating around the xAxis in GL
		float ratioAroundXAxis = float(row) * invRows;
		float radiansAboutXAxis = ratioAroundXAxis * latitiudinalRange +
			(latMin * DEG2RAD);
		float y = radius * sin(radiansAboutXAxis);
		float z = radius * cos(radiansAboutXAxis);

		for (int col = 0; col <= columns; ++col)
		{
			float ratioAroundYAxis = float(col) * invColumns;
			float theta = ratioAroundYAxis * longitudinalRange +
				(longMin * DEG2RAD);
			vec3 v4Point(-z * sinf(theta), y, -z * cosf(theta));
			vec3 v4Normal(inverseRadius * v4Point.x,
				inverseRadius * v4Point.y, inverseRadius * v4Point.z);

			if (transform != nullptr)
			{
				v4Point = vec3((transMat4 * vec4(v4Point, 0)));
				v4Normal = vec3((transMat4 * vec4(v4Normal, 0)));
			}

			int index = row * columns + (col % columns);
			v4Array[index] = v4Point;
		}
	}

	for (int face = 0; face < (rows)*(columns); ++face)
	{
		int iNextFace = face + 1;

		if (iNextFace % columns == 0)
			iNextFace = iNextFace - (columns);

		if (drawOutline)
			Gizmos::addLine(tempCenter + v4Array[face],
				tempCenter + v4Array[face + columns], lineColour, lineColour);

		if (face % columns == 0 && longitudinalRange < (pi<float>() * 2))
			continue;

		if (drawOutline)
			Gizmos::addLine(tempCenter + v4Array[iNextFace + columns],
				tempCenter + v4Array[face + columns], lineColour, lineColour);

		Gizmos::addTri(tempCenter + v4Array[iNextFace + columns],
			tempCenter + v4Array[face], tempCenter + v4Array[iNextFace], fillColour);
		Gizmos::addTri(tempCenter + v4Array[iNextFace + columns],
			tempCenter + v4Array[face + columns], tempCenter + v4Array[face], fillColour);
	}

	delete[] v4Array;
}

void drawReverseSphere(glm::vec3 const& center, float radius,
	glm::vec4 const& fill, glm::mat4* transform, int rows, int columns)
{
	using namespace aie;
	using namespace glm;

	const float longMin = 0.0f;
	const float longMax = 360.0f;
	const float latMin = -90.0f;
	const float latMax = 90;

	float inverseRadius = 1 / radius;

	// invert these first as the multiply is slightly quicker
	float invColumns = 1.0f / columns;
	float invRows = 1.0f / rows;

	float DEG2RAD = pi<float>() / 180;

	vec3 tempCenter = center;
	if (transform)
		tempCenter = glm::vec3((*transform)[3]) + tempCenter;

	//Lets put everything in radians first
	float latitiudinalRange = (latMax - latMin) * DEG2RAD;
	float longitudinalRange = (longMax - longMin) * DEG2RAD;

	// for each row of the mesh
	vec3* v4Array = new vec3[rows*columns + columns];

	// convert my stuff into glm stuff
	mat4 transMat4;
	if (transform)
		transMat4 = *transform;
	vec4 fillColour = fill;

	for (int row = 0; row <= rows; ++row)
	{
		// y ordinates this may be a little confusing but here we are navigating around the xAxis in GL
		float ratioAroundXAxis = float(row) * invRows;
		float radiansAboutXAxis = ratioAroundXAxis * latitiudinalRange +
			(latMin * DEG2RAD);
		float y = radius * sin(radiansAboutXAxis);
		float z = radius * cos(radiansAboutXAxis);

		for (int col = 0; col <= columns; ++col)
		{
			float ratioAroundYAxis = float(col) * invColumns;
			float theta = ratioAroundYAxis * longitudinalRange +
				(longMin * DEG2RAD);
			vec3 v4Point(-z * sinf(theta), y, -z * cosf(theta));
			vec3 v4Normal(inverseRadius * v4Point.x,
				inverseRadius * v4Point.y, inverseRadius * v4Point.z);

			if (transform != nullptr)
			{
				v4Point = vec3((transMat4 * vec4(v4Point, 0)));
				v4Normal = vec3((transMat4 * vec4(v4Normal, 0)));
			}

			int index = row * columns + (col % columns);
			v4Array[index] = v4Point;
		}
	}

	for (int face = 0; face < (rows)*(columns); ++face)
	{
		int iNextFace = face + 1;

		if (iNextFace % columns == 0)
			iNextFace = iNextFace - (columns);

		if (face % columns == 0 && longitudinalRange < (pi<float>() * 2))
			continue;

		Gizmos::addTri(
			tempCenter + v4Array[iNextFace],
			tempCenter + v4Array[face],
			tempCenter + v4Array[iNextFace + columns],
			fillColour);
		Gizmos::addTri(
			tempCenter + v4Array[face],
			tempCenter + v4Array[face + columns],
			tempCenter + v4Array[iNextFace + columns],
			fillColour);
	}

	delete[] v4Array;
}

void drawCylinder(glm::vec3 const& center, float radius, float height,
	int segments, glm::vec4 const& fill, glm::mat4* transform, bool drawOutline,
	glm::vec4 const& outline)
{
	using namespace aie;
	using namespace glm;

	vec4 white(1, 1, 1, 1);

	glm::vec3 tempCenter = transform != nullptr ?
		((glm::vec3)(*transform)[3]) + center : center;

	mat4 glmTransform;
	if (transform)
		glmTransform = *transform;
	vec3 glmCenter = tempCenter;

	vec4 glmFill = fill;
	vec4 glmOutline = outline;

	float segmentSize = (2 * pi<float>()) / segments;

	float halfLength = height / 2.0f;

	for (int i = 0; i < segments; ++i)
	{
		vec3 v0top(0, halfLength, 0);
		vec3 v1top(sinf(i * segmentSize) * radius, halfLength, cosf(i * segmentSize) * radius);
		vec3 v2top(sinf((i + 1) * segmentSize) * radius, halfLength, cosf((i + 1) * segmentSize) * radius);
		vec3 v0bottom(0, -halfLength, 0);
		vec3 v1bottom(sinf(i * segmentSize) * radius, -halfLength, cosf(i * segmentSize) * radius);
		vec3 v2bottom(sinf((i + 1) * segmentSize) * radius, -halfLength, cosf((i + 1) * segmentSize) * radius);

		if (transform != nullptr)
		{
			v0top = vec3((glmTransform * vec4(v0top, 0)));
			v1top = vec3((glmTransform * vec4(v1top, 0)));
			v2top = vec3((glmTransform * vec4(v2top, 0)));
			v0bottom = vec3((glmTransform * vec4(v0bottom, 0)));
			v1bottom = vec3((glmTransform * vec4(v1bottom, 0)));
			v2bottom = vec3((glmTransform * vec4(v2bottom, 0)));
		}

		// triangles
		Gizmos::addTri(glmCenter + v0top, glmCenter + v1top, glmCenter + v2top, glmFill);
		Gizmos::addTri(glmCenter + v0bottom, glmCenter + v2bottom, glmCenter + v1bottom, glmFill);
		Gizmos::addTri(glmCenter + v2top, glmCenter + v1top, glmCenter + v1bottom, glmFill);
		Gizmos::addTri(glmCenter + v1bottom, glmCenter + v2bottom, glmCenter + v2top, glmFill);

		// lines
		if (drawOutline)
		{
			Gizmos::addLine(glmCenter + v1top, glmCenter + v2top, glmOutline, glmOutline);
			Gizmos::addLine(glmCenter + v1top, glmCenter + v1bottom, glmOutline, glmOutline);
			Gizmos::addLine(glmCenter + v1bottom, glmCenter + v2bottom, glmOutline, glmOutline);
		}
	}
}

void drawCone(glm::vec3 const& center, float topRadius, float bottomRadius,
	float height, int segments, glm::vec4 const& fill, glm::mat4* transform,
	bool drawOutline, glm::vec4 const& outline)
{
	using namespace aie;
	using namespace glm;

	vec4 white(1, 1, 1, 1);

	glm::vec3 tempCenter = transform != nullptr ?
		((glm::vec3)(*transform)[3]) + center : center;

	mat4 glmTransform;
	if (transform)
		glmTransform = *transform;
	vec3 glmCenter = tempCenter;

	vec4 glmFill = fill;
	vec4 glmOutline = outline;

	float segmentSize = (2 * pi<float>()) / segments;

	float halfLength = height / 2.0f;

	for (int i = 0; i < segments; ++i)
	{
		vec3 v0top(0, halfLength, 0);
		vec3 v1top(sinf(i * segmentSize) * topRadius, halfLength, cosf(i * segmentSize) * topRadius);
		vec3 v2top(sinf((i + 1) * segmentSize) * topRadius, halfLength, cosf((i + 1) * segmentSize) * topRadius);
		vec3 v0bottom(0, -halfLength, 0);
		vec3 v1bottom(sinf(i * segmentSize) * bottomRadius, -halfLength, cosf(i * segmentSize) * bottomRadius);
		vec3 v2bottom(sinf((i + 1) * segmentSize) * bottomRadius, -halfLength, cosf((i + 1) * segmentSize) * bottomRadius);

		if (transform != nullptr)
		{
			v0top = vec3((glmTransform * vec4(v0top, 0)));
			v1top = vec3((glmTransform * vec4(v1top, 0)));
			v2top = vec3((glmTransform * vec4(v2top, 0)));
			v0bottom = vec3((glmTransform * vec4(v0bottom, 0)));
			v1bottom = vec3((glmTransform * vec4(v1bottom, 0)));
			v2bottom = vec3((glmTransform * vec4(v2bottom, 0)));
		}

		// triangles
		Gizmos::addTri(glmCenter + v0top, glmCenter + v1top, glmCenter + v2top, glmFill);
		Gizmos::addTri(glmCenter + v0bottom, glmCenter + v2bottom, glmCenter + v1bottom, glmFill);
		Gizmos::addTri(glmCenter + v2top, glmCenter + v1top, glmCenter + v1bottom, glmFill);
		Gizmos::addTri(glmCenter + v1bottom, glmCenter + v2bottom, glmCenter + v2top, glmFill);

		// lines
		if (drawOutline)
		{
			Gizmos::addLine(glmCenter + v1top, glmCenter + v2top, glmOutline, glmOutline);
			Gizmos::addLine(glmCenter + v1top, glmCenter + v1bottom, glmOutline, glmOutline);
		}
		Gizmos::addLine(glmCenter + v1bottom, glmCenter + v2bottom, glmOutline, glmOutline);
	}
}

void drawPlane(glm::vec3 const& topLeft, glm::vec3 const& topRight, 
	glm::vec3 const& bottomRight, glm::vec3 const& bottomLeft, 
	glm::vec4 const& fill, bool drawOutline, glm::vec4 const& outline)
{
	using namespace aie;
	using namespace glm;

	// turn our glm::vec3s into glm::vec3s
	vec3 tl = topLeft;
	vec3 tr = topRight;
	vec3 br = bottomRight;
	vec3 bl = bottomLeft;

	// and the colour
	vec4 fc = fill;

	/*
	 *  O-------O
	 *  |      /
	 *  |    /
	 *  |  /
	 *  |/
	 *  O
	 */
	Gizmos::addTri(tl, tr, bl, fc);
	/*
	 *          O
	 *         /|
	 *       /  |
	 *     /    |
	 *   /      |
	 *  O-------O
	 */
	Gizmos::addTri(tr, br, bl, fc);

	if (!drawOutline)
		return;

	// let's draw the outline!

	// turn outline colour into vec4
	vec4 lc = outline;

	// top-left to top-right
	Gizmos::addLine(tl, tr, lc);
	// top-right to bottom-right
	Gizmos::addLine(tr, br, lc);
	// bottom-right to bottom-left
	Gizmos::addLine(br, bl, lc);
	// bottom-left to top-left
	Gizmos::addLine(bl, tl, lc);
}
