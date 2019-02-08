/* =================================
 *  Camera
 *  Just a wrapper for position/rotation and turning that into a matrix
 * ================================= */
#include "camera.h"

Camera::Camera() { } 
Camera::~Camera() {}

Vector3 Camera::getPosition()
{
	return m_position;
}

void Camera::setPosition(Vector3 const& pos)
{
	m_position = pos;
}

Vector3 Camera::getRotation()
{
	return m_rotation;
}

void Camera::setRotation(Vector3 const& rot)
{
	m_rotation = rot;
}

void Camera::setYaw(float r) { m_rotation.y = r; } 
void Camera::setRoll(float r) { m_rotation.z = r; }
void Camera::setPitch(float r) { m_rotation.x = r; }

Matrix4 Camera::getViewMatrix()
{
	// make position matrix
	Matrix4 posMatrix;
	posMatrix.setPosition(m_position);

	// make matrices for each rotation axis
	Matrix4 xRot, yRot, zRot;

	xRot.setRotateX(m_rotation.x);
	yRot.setRotateY(m_rotation.y);
	zRot.setRotateZ(m_rotation.z);

	// combine those rotation matrices
	Matrix4 rotMatrix = yRot * zRot * xRot;

	// final transform matrix
	Matrix4 transform = (posMatrix * rotMatrix) * m_parentMatrix;

	// an inverse of that is the view matrix
	return transform.inverse();
}

void Camera::setParentMatrix(Matrix4 m)
{
	m_parentMatrix = m;
}
