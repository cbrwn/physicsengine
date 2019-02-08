/* =================================
 *  Shapes
 *  Additional functions to add shapes to the Gizmos system
 * ================================= */
#pragma once

#include <vector3.h>
#include <vector4.h>
#include <matrix4.h>

/***
 *  @brief Adds a cuboid to be drawn using Gizmos
 *
 *  @param center Center point of the box
 *  @param size Extents of the box
 *  @param fill Colour of the box's faces
 *  @param transform Transformation matrix to apply to the box,
 *						nullptr for no transform
 *  @param drawLines Whether or not to draw lines on the edges of the box
 *  @param outline Colour of the edge lines
 */
void drawBox(Vector3 const& center, Vector3 const& size, Vector4 const& fill,
	Matrix4* transform = nullptr, bool drawLines = true,
	Vector4 const& outline = { 0,0,0,1 });

/***
 *  @brief Adds a sphere to be drawn using Gizmos
 *
 *  @param center Center point of the sphere
 *  @param radius Radius of the sphere
 *  @param fill Colour of the box's faces
 *  @param transform Transformation matrix to apply to the box,
 *						nullptr for no transform
 *  @param rows Vertical segments of the sphere
 *  @param columns Horizontal segments of the sphere
 *  @param drawOutline Whether or not to draw the edges of the sphere
 *  @param outline Colour of the edge lines
 */
void drawSphere(Vector3 const& center, float radius, Vector4 const& fill,
	Matrix4* transform = nullptr, int rows = 8, int columns = 8,
	bool drawOutline = true, Vector4 const& outline = { 0,0,0,1 });

/***
 *  @brief Adds an inside-out sphere to be drawn using Gizmos
 *			Useful to draw an "outline" around a sphere
 *
 *  @param center Center point of the sphere
 *  @param radius Radius of the sphere
 *  @param fill Colour of the box's faces
 *  @param transform Transformation matrix to apply to the box,
 *						nullptr for no transform
 *  @param rows Vertical segments of the sphere
 *  @param columns Horizontal segments of the sphere
 */
void drawReverseSphere(Vector3 const& center, float radius,
	Vector4 const& fill, Matrix4* transform = nullptr, int rows = 8,
	int columns = 8);

/***
 *  @brief Adds a cylinder to be drawn using Gizmos
 *
 *  @param center Center point of the cylinder
 *  @param radius Radius of the top/bottom of the cylinder
 *  @param height Vertical size of the cylinder
 *  @param segments Segments around the cylinder
 *  @param fill Colour of the box's faces
 *  @param transform Transformation matrix to apply to the box,
 *						nullptr for no transform
 *  @param drawOutline Whether or not to draw the edges of the cylinder
 *  @param outline Colour of the edge lines
 */
void drawCylinder(Vector3 const& center, float radius, float height,
	int segments, Vector4 const& fill, Matrix4* transform = nullptr,
	bool drawOutline = true, Vector4 const& outline = { 0,0,0,1 });

/***
 *  @brief Adds a cone to be drawn using Gizmos
 *
 *  @param center Center point of the cone
 *  @param topRadius Radius of the top circle - 0 for a pointed cone
 *  @param bottomRadius Radius of the base of the cone
 *  @param height Vertical size of the cylinder
 *  @param segments Segments around the cylinder
 *  @param fill Colour of the box's faces
 *  @param transform Transformation matrix to apply to the box,
 *						nullptr for no transform
 *  @param drawOutline Whether or not to draw the edges of the cylinder
 *  @param outline Colour of the edge lines
 */
void drawCone(Vector3 const& center, float topRadius, float bottomRadius,
	float height, int segments, Vector4 const& fill,
	Matrix4* transform = nullptr, bool drawOutline = true,
	Vector4 const& outline = { 0,0,0,1 });

/***
 *  @brief Adds a plane to be drawn using Gizmos
 *			Clockwise - only draws from the front
 *
 *  @param topLeft Top-left position of the plane
 *  @param topRight Top-right position of the plane
 *  @param bottomRight Bottom-right position of the plane
 *  @param bottomLeft Bottom-left position of the plane
 *  @param fill Colour of the box's faces
 *  @param drawOutline Whether or not to draw the edges of the cylinder
 *  @param outline Colour of the edge lines
 */
void drawPlane(Vector3 const& topLeft, Vector3 const& topRight,
	Vector3 const& bottomRight, Vector3 const& bottomLeft,
	Vector4 const& fill, bool drawOutline = true,
	Vector4 const& outline = { 0,0,0,1 });