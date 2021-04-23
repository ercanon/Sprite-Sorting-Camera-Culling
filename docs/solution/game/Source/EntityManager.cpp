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
		ListItem<Entity*>* list = NULL;
		list = entities.start;

		while (list != NULL)
		{
			//TODO 12: Camera Culling
			if (list->data->position.x + list->data->width > -render->camera.x && list->data->position.x < -render->camera.x + render->camera.w &&
				list->data->position.y + list->data->height > -render->camera.y && list->data->position.y < -render->camera.y + render->camera.h)
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
void EntityManager::DrawSorted(Map* map)
{
	//Draw Map
	if (map->mapLoaded == false) return;

	bool entitiesDrawn = false;

	for (int i = 0; i < map->data.layers.Count(); i++)
	{
		//Draw Tiles
		MapLayer* layer = map->data.layers[i];
		bool sortingLayer = false;

		render->scale = map->scale;

		Assemble* assemble = nullptr;

		if ((layer->properties.GetProperty("drawable", 1) != 0) || map->drawColliders)
		{
			for (int y = 0; y < map->data.height; ++y)
			{
				for (int x = 0; x < map->data.width; ++x)
				{
					iPoint pos = map->MapToWorld(x, y);

					//TODO 12: Camera Culling
					if (pos.x + map->data.tileWidth > -render->camera.x - render->camera.w/4 && pos.x < -render->camera.x + render->camera.w &&
						pos.y + map->data.tileWidth > -render->camera.y && pos.y < -render->camera.y + render->camera.h)
					{
						int tileId = layer->Get(x, y);

						if (tileId > 0)
						{
							TileSet* tileset = map->GetTilesetFromTileId(tileId);
							SDL_Rect rec = tileset->GetTileRect(tileId);

							int tileIdPrevX = layer->Get(x - 1, y);
							int tileIdPrevY = layer->Get(x, y - 1);
							TileSet* tilesetPrevX = nullptr;
							TileSet* tilesetPrevY = nullptr;

							if (tileIdPrevX > 0)
								tilesetPrevX = map->GetTilesetFromTileId(tileIdPrevX);
							if (tileIdPrevY > 0)
								tilesetPrevY = map->GetTilesetFromTileId(tileIdPrevY);
							int propPrevX = -1;
							int propPrevY = -1;

							bool draw = true;

							//TODO 3.5: Detect the tiles with attributes
							for (int t = 0; t < tileset->tileCount; t++)
							{
								if (tileId - 1 == tileset->tileProperty[t].tileId)
								{
									sortingLayer = true;
									draw = false;

									//TODO 5: Check the previous tiles
									if (tileIdPrevX > 0)
										for (int tp = 0; tp < tileset->tileCount; tp++)
											if (tileIdPrevX - 1 == tilesetPrevX->tileProperty[tp].tileId)
											{
												propPrevX = tilesetPrevX->tileProperty[tp].properties.GetProperty("detectAssamble", 0);
												break;
											}
									if (tileIdPrevY > 0)
										for (int tp = 0; tp < tileset->tileCount; tp++)
											if (tileIdPrevY - 1 == tilesetPrevY->tileProperty[tp].tileId)
											{
												propPrevY = tilesetPrevY->tileProperty[tp].properties.GetProperty("detectAssamble", 0);
												break;
											}
									
									//TODO 6: Store the Assemble information
									if (tileset->tileProperty[t].properties.GetProperty("detectAssamble", 0) == propPrevX)
									{
										for (int a = 0; a < assembledList.Count(); a++)
										{
											bool done = false;
											for (int i = 0; i < assembledList.At(a)->data->tilesAssemble; i++)
											{
												if (assembledList.At(a)->data->tileInfo[i].tileMapPosition.x == x - 1 &&
													assembledList.At(a)->data->tileInfo[i].tileMapPosition.y == y)
												{
													assembledList.At(a)->data->tileInfo[assembledList.At(a)->data->tilesAssemble].tileMapPosition = { x, y };
													assembledList.At(a)->data->tileInfo[assembledList.At(a)->data->tilesAssemble].tileWorldPosition = pos;
													assembledList.At(a)->data->tileInfo[assembledList.At(a)->data->tilesAssemble].rectangle = rec;
													assembledList.At(a)->data->tileInfo[assembledList.At(a)->data->tilesAssemble].tileset = tileset;
													assembledList.At(a)->data->tilesAssemble++;
													done = true;
													break;
												}
											}
											if (done) break;
										}
									}
									else if (tileset->tileProperty[t].properties.GetProperty("detectAssamble", 0) == propPrevY)
									{
										for (int a = 0; a < assembledList.Count(); a++)
										{
											bool done = false;
											for (int i = 0; i < assembledList.At(a)->data->tilesAssemble; i++)
											{
												if (assembledList.At(a)->data->tileInfo[i].tileMapPosition.x == x &&
													assembledList.At(a)->data->tileInfo[i].tileMapPosition.y == y - 1)
												{
													assembledList.At(a)->data->tileInfo[assembledList.At(a)->data->tilesAssemble].tileMapPosition = { x, y };
													assembledList.At(a)->data->tileInfo[assembledList.At(a)->data->tilesAssemble].tileWorldPosition = pos;
													assembledList.At(a)->data->tileInfo[assembledList.At(a)->data->tilesAssemble].rectangle = rec;
													assembledList.At(a)->data->tileInfo[assembledList.At(a)->data->tilesAssemble].tileset = tileset;
													assembledList.At(a)->data->tilesAssemble++;
													done = true;
													break;
												}
											}
											if (done) break;
										}
									}
									else
									{
										assemble = new Assemble;
										assemble->tileInfo[0].tileMapPosition = { x, y };
										assemble->tileInfo[0].tileWorldPosition = pos;
										assemble->tileInfo[0].rectangle = rec;
										assemble->tileInfo[0].tileset = tileset;
										assemble->tilesAssemble++;
										assembledList.Add(assemble);
									}
									break;
								}
							}
							//TODO 6.5: Do not render the assembled tiles
							if (draw == true) render->DrawTexture(tileset->texture, pos.x + tileset->offsetX, pos.y + tileset->offsetY, &rec);
						}
					}
				}
			}
		}

		if (sortingLayer == true)
		{
			//TODO 7: Sort the entities
			ListItem<Entity*>* list = entities.start;
			//TODO 7.5: Copy the sorted list
			ListItem<Entity*>* sorted = NULL;
			if (entitiesDrawn == false)
			{
				//Organize entities
				bool swapped = true;
				while (swapped)
				{
					swapped = false;

					while (list != NULL && list->next != NULL)
					{
						if (list->data->position.y < list->next->data->position.y)
						{
							SWAP(list->data, list->next->data);
							swapped = true;
						}

						list = list->next;
					}
				}

				//TODO 8: Get the dimensions of the assemble
				//Render Assemble by Assemble
				for (int l = 0; l < assembledList.Count(); l++)
				{
					int maxX = assembledList.At(l)->data->tileInfo[0].tileWorldPosition.x;
					int minX = assembledList.At(l)->data->tileInfo[0].tileWorldPosition.x;
					int maxY = assembledList.At(l)->data->tileInfo[0].tileWorldPosition.y;
					int minY = assembledList.At(l)->data->tileInfo[0].tileWorldPosition.y;

					//Calculate max & min X & Y
					for (int a = 0; a < assembledList.At(l)->data->tilesAssemble; a++)
					{
						if (maxX < assembledList.At(l)->data->tileInfo[a].tileWorldPosition.x + map->data.tileWidth)
							maxX = assembledList.At(l)->data->tileInfo[a].tileWorldPosition.x + map->data.tileWidth;
						if (minX > assembledList.At(l)->data->tileInfo[a].tileWorldPosition.x)
							minX = assembledList.At(l)->data->tileInfo[a].tileWorldPosition.x;
						if (maxY < assembledList.At(l)->data->tileInfo[a].tileWorldPosition.y + map->data.tileHeight)
							maxY = assembledList.At(l)->data->tileInfo[a].tileWorldPosition.y + map->data.tileHeight;
						if (minY > assembledList.At(l)->data->tileInfo[a].tileWorldPosition.y)
							minY = assembledList.At(l)->data->tileInfo[a].tileWorldPosition.y;
					}

					//TODO 9: Draw entities below the assemble
					//If there is any entity near the assamble, wait to render
					sorted = list;
					while (sorted != NULL)
					{
						if (sorted->data->position.x + sorted->data->width >= minX && sorted->data->position.x <= maxX &&
							sorted->data->position.y + sorted->data->height >= minY && sorted->data->position.y <= maxY - 13 &&
							sorted->data->renderable == true)
						{
							sorted->data->Draw();
							sorted->data->renderable = false;
						}
						sorted = sorted->prev;
					}
				}
			}

			//TODO 10: Draw assembles
			//Render assembles
			for (int l = 0; l < assembledList.Count(); l++)
			{
				for (int a = 0; a < assembledList.At(l)->data->tilesAssemble; a++)
				{
					render->DrawTexture(assembledList.At(l)->data->tileInfo[a].tileset->texture,
						assembledList.At(l)->data->tileInfo[a].tileWorldPosition.x + assembledList.At(l)->data->tileInfo[a].tileset->offsetX,
						assembledList.At(l)->data->tileInfo[a].tileWorldPosition.y + assembledList.At(l)->data->tileInfo[a].tileset->offsetY,
						&assembledList.At(l)->data->tileInfo[a].rectangle);
				}
				delete assembledList.At(l)->data;
				assembledList.At(l)->data = nullptr;
			}
			assembledList.Clear();

			//TODO 11: Draw the other entities
			if (entitiesDrawn == false)
			{
				//Render Entities
				sorted = list;
				while (sorted != NULL)
				{
					//TODO 12: Camera Culling
					if (sorted->data->position.x + sorted->data->width > -render->camera.x && sorted->data->position.x < -render->camera.x + render->camera.w &&
						sorted->data->position.y + sorted->data->height > -render->camera.y && sorted->data->position.y < -render->camera.y + render->camera.h &&
						sorted->data->renderable == true)
					{
						sorted->data->Draw();
					}

					sorted->data->renderable = true;
					sorted = sorted->prev;
				}
				entitiesDrawn = true;
			}
		}

		render->scale = 1;		
	}
}