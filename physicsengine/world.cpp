#include "world.h"

#include <gmath.h>
#include <Input.h>
#include <Gizmos.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "game.h"
#include "util.h"
#include "actor.h"
#include "camera.h"
#include "objectpool.h"
#include "physicsbody.h"
#include "physicsactor.h"
#include "shapes.h"

World::World(Game* game)
	: m_game(game)
{
	m_camera = new Camera();
	m_pool = new ObjectPool(this);
}

World::~World()
{
	for (int i = 0; i < m_actors.getCount(); ++i)
		delete m_actors[i];

	delete m_camera;
	delete m_pool;
}

void World::addActor(Actor* a)
{
	m_actors.add(a);
	a->setWorld(this);

	if (a->getType() == ACTORTYPE_PHYSICS)
		m_physicsActors.add((PhysicsActor*)a);
}

void World::update(float delta)
{
	for (int i = 0; i < m_actors.getCount(); ++i)
		if (m_actors[i]->isEnabled())
			m_actors[i]->update(delta);
    aie::Gizmos::clear();
}

void World::draw()
{
	using namespace aie;


	for (int i = 0; i < m_actors.getCount(); ++i)
		if (m_actors[i]->isEnabled())
			m_actors[i]->draw();

	//PhysicsManager::getInstance()->drawTree(PhysicsManager::getInstance()->getTree());

	float windowWidth = (float)m_game->getWindowWidth();
	float windowHeight = (float)m_game->getWindowHeight();

	// grab projection matrix
	auto glmProj = glm::perspective(PI*0.25f,
		windowWidth / windowHeight, 0.1f, 1000.f);
	m_projectionMatrix = fromMat4(glmProj);

	auto glmView = toMat4(m_camera->getViewMatrix());

	Gizmos::draw(glmProj * glmView);

}

ObjectPool* World::getPool()
{
	return m_pool;
}

PhysicsActor* World::getActorWithBody(PhysicsBody* body)
{
	for (int i = 0; i < m_physicsActors.getCount(); ++i)
	{
		if (m_physicsActors[i]->getBody() == body)
			return m_physicsActors[i];
	}

	return nullptr;
}

void World::getMouseRay(Vector3* outStart, Vector3* outDir)
{
	float screenWidth = (float)m_game->getWindowWidth();
	float screenHeight = (float)m_game->getWindowHeight();
	int mx, my;
	aie::Input::getInstance()->getMouseXY(&mx, &my);

	Matrix4 view = m_camera->getViewMatrix();

	Vector4 startScreen(((float)mx / screenWidth - 0.5f)*2.0f,
		((float)my / screenHeight - 0.5f) * 2.0f,
		-1.0f,
		1.0f);
	Vector4 endScreen(((float)mx / screenWidth - 0.5f)*2.0f,
		((float)my / screenHeight - 0.5f) * 2.0f,
		0.0f,
		1.0f);

	Matrix4 m = (m_projectionMatrix * view).inverse();

	Vector4 startWorld = m * startScreen;
	startWorld /= startWorld.w;
	Vector4 endWorld = m * endScreen;
	endWorld /= endWorld.w;

	Vector3 dirWorld = (Vector3)(endWorld - startWorld);
	dirWorld.normalise();

	*outStart = (Vector3)startWorld;
	*outDir = dirWorld;
}

Camera* World::getCamera()
{
	return m_camera;
}
