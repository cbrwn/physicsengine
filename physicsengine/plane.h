#pragma once

#include "actor.h"

class Plane : public Actor
{
public:
	Plane(Vector3 topLeft, Vector3 topRight, Vector3 bottomRight,
		Vector3 bottomLeft, Vector4 col);

	void update(float delta) override;
	void draw() override;
private:
	Vector3 m_points[4];
	Vector4 m_colour;
};