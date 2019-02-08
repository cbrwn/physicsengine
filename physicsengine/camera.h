/* =================================
 *  Camera
 *  Just a wrapper for position/rotation and turning that into a matrix
 * ================================= */
#pragma once

#include <matrix4.h>

class Camera
{
public:
	Camera();
	~Camera();

	Vector3 getPosition();
	void setPosition(Vector3 const& pos);

	Vector3 getRotation();
	void setRotation(Vector3 const& rot);

	// setting individual rotation axes
	void setYaw(float r);
	void setRoll(float r);
	void setPitch(float r);

	// turn the position and rotation into a view matrix
	Matrix4 getViewMatrix();

	// set the "parent" matrix - a transform matrix that the view matrix is
	// transformed by
	void setParentMatrix(Matrix4 m);
	Matrix4 getParentMatrix() { return m_parentMatrix; }

private:
	Vector3 m_position;
	Vector3 m_rotation;

	Matrix4 m_parentMatrix;
};