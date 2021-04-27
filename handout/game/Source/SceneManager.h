#ifndef __SCENEMANAGER_H__
#define __SCENEMANAGER_H__

#include "Module.h"
#include "Scene.h"

class GuiButton;

#include "Input.h"
#include "Render.h"
#include "Textures.h"
#include "Window.h"
#include "EntityManager.h"
#include "Collision.h"

class Font;

class SceneManager : public Module
{
public:

	// Constructor
	SceneManager(Input* input, Render* render, Textures* tex, Window* win, EntityManager* entityManager);
	// Destructor
	virtual ~SceneManager();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

public:

	Window* win;
	Input* input;
	Render* render;
	Textures* tex;
	EntityManager* entityManager;

	Collision* collision;

	Scene* current;
};

#endif // __SCENEMANAGER_H__