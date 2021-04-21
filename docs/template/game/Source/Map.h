#ifndef __MAP_H__
#define __MAP_H__

#include "Entity.h"
#include "Render.h"
#include "Textures.h"

#include "List.h"
#include "Queue.h"
#include "Point.h"
#include "DynArray.h"

#include "SDL/include/SDL.h"
#include "PugiXml/src/pugixml.hpp"

#define COST_MAP_SIZE	100


enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};


struct Properties
{
	struct Property
	{
		SString name;
		int value;
		SString valueString;
	};

	~Properties()
	{
		ListItem<Property*>* item;
		item = list.start;

		while (item != NULL)
		{
			RELEASE(item->data);
			item = item->next;
		}

		list.Clear();
	}

	int GetProperty(const char* name, int default_value = 0) const;

	List<Property*> list;
};

struct TileSet
{
	SString	name;
	int	firstgid;
	int margin;
	int	spacing;
	int	tileWidth;
	int	tileHeight;

	SDL_Texture* texture;
	int	texWidth;
	int	texHeight;
	int	numTilesWidth;
	int	numTilesHeight;
	int	offsetX;
	int	offsetY;

	SDL_Rect GetTileRect(int id) const;
};

struct MapLayer
{
	SString	name;
	int width;
	int height;
	uint* data;

	Properties properties;

	MapLayer() : data(NULL)
	{}

	~MapLayer()
	{
		RELEASE(data);
	}

	inline uint Get(int x, int y) const
	{
		return data[(y * width) + x];
	}
};

struct ObjectData {
	SString name;
	int x;
	int y;
	int w;
	int h;
	SDL_Rect rect;

	Properties properties;
};

struct ObjectLayer {
	SString name;
	List<ObjectData*> data;
};


struct MapData
{
	int width;
	int	height;
	int	tileWidth;
	int	tileHeight;
	SDL_Color backgroundColor;
	MapTypes type;
	List<TileSet*> tilesets;

	List<MapLayer*> layers;

	List<ObjectLayer*> objLayers;
};

class Map : public Entity
{
public:

	Map(Textures* render);

	// Destructor
	virtual ~Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw(Render* render);

	void DrawLayer(Render* render, int num);

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	// Unload current map
	bool Unload();

	iPoint MapToWorld(int x, int y) const;

	iPoint WorldToMap(int x, int y) const;

	TileSet* GetTilesetFromTileId(int id) const;

	SDL_Rect GetTilemapRec(int x, int y) const;

	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer) const;

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadObjLayer(pugi::xml_node& node, ObjectLayer* layer);

	bool LoadProperties(pugi::xml_node& node, Properties& properties);

public:

	MapData data;

	bool drawColliders = false;

	bool mapLoaded;
	uint32 scale;

	Properties properties;

private:

	Textures* tex;

	pugi::xml_document mapFile;
	SString folder;
};

#endif // __MAP_H__