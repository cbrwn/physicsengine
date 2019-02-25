#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <Renderer2D.h>

class Actor;
class Camera;
class ObjectPool;
class PhysicsBody;
class PhysicsActor;
class Game;

class World
{
public:
	World(Game* game);
	~World();

	void addActor(Actor* a);

	void update(float delta);
	void draw();

	ObjectPool* getPool();

	PhysicsActor* getActorWithBody(PhysicsBody* body);
	void getMouseRay(glm::vec3* outStart, glm::vec3* outDir);

	Camera* getCamera();

private:
	Game* m_game;

	std::vector<Actor*> m_actors;
	std::vector<PhysicsActor*> m_physicsActors;

	ObjectPool* m_pool;

	Camera* m_camera;
	glm::mat4 m_projectionMatrix;
};