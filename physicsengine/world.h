#pragma once

#include <darray.h>
#include <matrix4.h>
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
	void getMouseRay(Vector3* outStart, Vector3* outDir);

	Camera* getCamera();

private:
	Game* m_game;

	DArray<Actor*> m_actors;
	DArray<PhysicsActor*> m_physicsActors;

	ObjectPool* m_pool;

	Camera* m_camera;
	Matrix4 m_projectionMatrix;
};