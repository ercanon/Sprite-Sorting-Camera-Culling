#include "Exterior.h"

#include "SceneManager.h"
#include "Map.h"

#include "Defs.h"
#include "Log.h"



// Constructor
Exterior::Exterior(SceneManager* sceneManager) : Scene()
{
	this->sceneManager = sceneManager;

	map = new Map(sceneManager->tex);

	if (map->Load("world_research.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;

		RELEASE_ARRAY(data);
	}

	name.Create("exterior");
}
// Destructor
Exterior::~Exterior()
{}

bool Exterior::Load() /*EntityManager entityManager)*/
{
	sceneManager->entityManager->CreateEntity(EntityType::PLAYER)->position = iPoint (100, 100);
	sceneManager->entityManager->CreateEntity(EntityType::ENEMY)->position = iPoint(1408, 128);
	sceneManager->entityManager->CreateEntity(EntityType::ENEMY)->position = iPoint(1600, 448);
	sceneManager->entityManager->CreateEntity(EntityType::ENEMY)->position = iPoint(1856, 192);

	sceneManager->render->camera.w = sceneManager->win->screenSurface->w;
	sceneManager->render->camera.h = sceneManager->win->screenSurface->h;

	return false;
}

// Called each loop iteration
inline bool CheckCollision(SDL_Rect rec1, SDL_Rect rec2)
{
	if ((rec1.x < (rec2.x + rec2.w) && (rec1.x + rec1.w) > rec2.x) &&
		(rec1.y < (rec2.y + rec2.h) && (rec1.y + rec1.h) > rec2.y)) return true;
	else return false;
}

bool Exterior::Update(float dt)
{
	sceneManager->collision->CheckCollision(map);

	if (sceneManager->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) map->drawColliders = !map->drawColliders;

	sceneManager->render->camera.y = 8;

	// Camera moves with player when it is at the middle of the screen
	sceneManager->render->camera.x = -sceneManager->entityManager->CreateEntity(EntityType::PLAYER)->position.x + sceneManager->render->camera.w / 2;

	// Camera stops at limits
	if (sceneManager->render->camera.x > LEFT_CAMERA_LIMIT) sceneManager->render->camera.x = LEFT_CAMERA_LIMIT;
	else if (sceneManager->render->camera.x < RIGHT_CAMERA_LIMIT) sceneManager->render->camera.x = RIGHT_CAMERA_LIMIT;

	return true;
}

bool Exterior::Draw()
{
	// Draw map
	map->Draw(sceneManager->render);
	sceneManager->entityManager->Draw();

	return false;
}

bool Exterior::Unload()
{
	// TODO: Unload all resources
	map->Unload();
	delete map;
	map = nullptr;

	return true;
}