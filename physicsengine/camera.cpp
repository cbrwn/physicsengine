/* =================================
 *  Camera
 *  Just a wrapper for position/rotation and turning that into a matrix
 * ================================= */
#include "camera.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

Camera::Camera() { } 
Camera::~Camera() {}

glm::vec3 Camera::getPosition()
{
	return m_position;
}

void Camera::setPosition(glm::vec3 const& pos)
{
	m_position = pos;
}

glm::vec3 Camera::getRotation()
{
	return m_rotation;
}

void Camera::setRotation(glm::vec3 const& rot)
{
	m_rotation = rot;
}

void Camera::setYaw(float r) { m_rotation.y = r; } 
void Camera::setRoll(float r) { m_rotation.z = r; }
void Camera::setPitch(float r) { m_rotation.x = r; }

glm::mat4 Camera::getViewMatrix()
{
	// make position matrix
	glm::mat4 posMatrix;
	posMatrix[3] = glm::vec4(m_position, posMatrix[3].w);

	// make matrices for each rotation axis
	glm::mat4 xRot, yRot, zRot;

	xRot = glm::rotate(xRot, m_rotation.x, glm::vec3(1, 0, 0));
	yRot = glm::rotate(yRot, m_rotation.y, glm::vec3(0, 1, 0));
	zRot = glm::rotate(zRot, m_rotation.z, glm::vec3(0, 0, 1));

	// combine those rotation matrices
	glm::mat4 rotMatrix = yRot * zRot * xRot;

	// final transform matrix
	glm::mat4 transform = (posMatrix * rotMatrix) * m_parentMatrix;

	// an inverse of that is the view matrix
	return glm::inverse(transform);
}

void Camera::setParentMatrix(glm::mat4 m)
{
	m_parentMatrix = m;
}
