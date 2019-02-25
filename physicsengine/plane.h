#pragma once

#include "actor.h"

class Plane : public Actor
{
public:
	Plane(glm::vec3 topLeft, glm::vec3 topRight, glm::vec3 bottomRight,
		glm::vec3 bottomLeft, glm::vec4 col);

	void update(float delta) override;
	void draw() override;
private:
	glm::vec3 m_points[4];
	glm::vec4 m_colour;
};