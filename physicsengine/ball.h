#pragma once

#include "physicsactor.h"

class Ball : public PhysicsActor {

public:
	Ball(glm::vec3 pos, float radius, unsigned int col);

	void update(float delta) override;
	void draw() override;

private:
	float m_radius;
	unsigned int m_color;

};
