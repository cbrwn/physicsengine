/* =================================
 *  Camera
 *  Just a wrapper for position/rotation and turning that into a matrix
 * ================================= */
#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	Camera();
	~Camera();

	glm::vec3 getPosition();
	void setPosition(glm::vec3 const& pos);

	glm::vec3 getRotation();
	void setRotation(glm::vec3 const& rot);

	// setting individual rotation axes
	void setYaw(float r);
	void setRoll(float r);
	void setPitch(float r);

	// turn the position and rotation into a view matrix
	glm::mat4 getViewMatrix();

	// set the "parent" matrix - a transform matrix that the view matrix is
	// transformed by
	void setParentMatrix(glm::mat4 m);
	glm::mat4 getParentMatrix() { return m_parentMatrix; }

private:
	glm::vec3 m_position;
	glm::vec3 m_rotation;

	glm::mat4 m_parentMatrix;
};