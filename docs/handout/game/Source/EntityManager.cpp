#include "EntityManager.h"

#include "Input.h"
#include "Render.h"
#include "Textures.h"
#include "SceneManager.h"

#include "Player.h"
#include "Slime.h"

#include "Defs.h"
#include "Log.h"



// Constructor
EntityManager::EntityManager(Input* input, Render* render, Textures* tex) : Module()
{
	this->input = input;
	this->render = render;
	this->tex = tex;

	name.Create("entitymanager");
}
// Destructor
EntityManager::~EntityManager()
{}


// Called before render is available
bool EntityManager::Awake(pugi::xml_node& config)
{
	LOG("Loading Entity Manager");
	bool ret = true;

	return ret;
}


// Called each loop iteration
bool EntityManager::Update(float dt)
{
	accumulatedTime += dt;
	if (accumulatedTime >= updateMsCycle) doLogic = true;

	UpdateAll(dt, doLogic);

	if (doLogic == true)
	{
		accumulatedTime = 0.0f;
		doLogic = false;
	}

	return true;
}


// Called before quitting
bool EntityManager::CleanUp()
{
	if (!active) return true;

	return true;
}


Entity* EntityManager::CreateEntity(EntityType type)
{
	Entity* ret = nullptr;

	switch (type)
	{
	case EntityType::PLAYER:
		ret = Player::GetInstance(input, render, tex);

		if (ret != nullptr && (Player*)entities.start == nullptr)
		{
			Player::SetCollision(&collision, (Player*)ret);

			// Created entities are added to the list
			entities.Add(ret);
		}
		break;
	case EntityType::ENEMY:
		ret = Slime::GetInstance(input, render, tex);

		// Created entities are added to the list
		if (ret != nullptr) entities.Add(ret);
		break;
	default: break;
	}

	return ret;
}


bool EntityManager::UpdateAll(float dt, bool doLogic)
{
	if (doLogic)
	{
		// TODO: Update all entities 
		ListItem<Entity*>* list = NULL;
		list = entities.start;

		while (list != NULL)
		{
			//TODO 12: Camera Culling
			list->data->Update(dt);
			list = list->next;
		}
	}

	return true;
}

bool EntityManager::Draw()
{
	ListItem<Entity*>* list = NULL;
	list = entities.start;

	while (list != NULL)
	{
		list->data->Draw();
		list = list->next;
	}

	return true;
}

//TODO 3: The function to draw everything
//TODO 3.5: Detect the tiles with attributes
//TODO 5: Check the previous tiles
//TODO 6: Store the assemble information
//TODO 6.5: Do not render the assembles
//TODO 7: Sort the entities by their position in Y
//TODO 7.5: Copy the list sorted in a new list
//TODO 8: Get the dimensions of the assemble
//TODO 9: Draw the entities that are below the assemble
//TODO 10: Draw assembles
//TODO 11: Draw the rest of the entities
//TODO 12: Camera Culling