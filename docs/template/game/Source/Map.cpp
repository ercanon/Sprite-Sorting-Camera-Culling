#include "Map.h"

#include "Defs.h"
#include "Log.h"

#include <math.h>

Map::Map(Textures* texture) : Entity(EntityType::MAP)
{
	mapLoaded = false;
	folder.Create("Assets/maps/");

	tex = texture;
	scale = 1;
}

// Destructor
Map::~Map()
{}


int Properties::GetProperty(const char* value, int defaultValue) const
{
	ListItem<Property*>* item = list.start;

	while (item)
	{
		if (item->data->name == value)
			return item->data->value;
		item = item->next;
	}

	return defaultValue;
}

// Called before render is available
bool Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	return ret;
}

bool Map::LoadObjLayer(pugi::xml_node& node, ObjectLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();

	pugi::xml_node object;
	for (object = node.child("object"); object; object = object.next_sibling("object"))
	{
		ObjectData* objData = new ObjectData();

		objData->name = object.attribute("name").as_string();
		objData->x = object.attribute("x").as_int();
		objData->y = object.attribute("y").as_int();
		objData->w = object.attribute("width").as_int();
		objData->h = object.attribute("height").as_int();

		objData->rect = { objData->x, objData->y, objData->w, objData->h };

		pugi::xml_node data = object.child("properties");

		if (data != NULL)
		{
			pugi::xml_node prop;

			for (prop = data.child("property"); prop; prop = prop.next_sibling("property"))
			{
				Properties::Property* p = new Properties::Property();

				p->name = prop.attribute("name").as_string();
				p->valueString = prop.attribute("value").as_string();

				objData->properties.list.Add(p);
			}
		}

		layer->data.Add(objData);

	}

	return ret;
}

// Draw the map (all requried layers)
void Map::Draw(Render* render)
{
	if (mapLoaded == false) return;

	for (int i = 0; i < data.layers.Count(); i++)
	{
		if ((data.layers[i]->properties.GetProperty("drawable", 1) != 0) || drawColliders) DrawLayer(render, i);
	}
}

void Map::DrawLayer(Render* render, int num)
{
	if (num < data.layers.Count())
	{
		MapLayer* layer = data.layers[num];

		render->scale = scale;

		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				int tileId = layer->Get(x, y);

				if (tileId > 0)
				{
					TileSet* tileset = GetTilesetFromTileId(tileId);

					SDL_Rect rec = tileset->GetTileRect(tileId);
					iPoint pos = MapToWorld(x, y);

					render->DrawTexture(tileset->texture, pos.x + tileset->offsetX, pos.y + tileset->offsetY, &rec);
				}
			}
		}

		render->scale = 1;
	}
}

iPoint Map::MapToWorld(int x, int y) const
{
	iPoint ret;

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tileWidth;
		ret.y = y * data.tileHeight;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{
		ret.x = (x - y) * (data.tileWidth / 2);
		ret.y = (x + y) * (data.tileHeight / 2);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

iPoint Map::WorldToMap(int x, int y) const
{
	iPoint ret(0, 0);

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tileWidth;
		ret.y = y / data.tileHeight;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{

		float halfWidth = data.tileWidth * 0.5f;
		float halfHeight = data.tileHeight * 0.5f;
		ret.x = int((x / halfWidth + y / halfHeight) / 2);
		ret.y = int((y / halfHeight - (x / halfWidth)) / 2);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

SDL_Rect Map::GetTilemapRec(int x, int y) const
{
	iPoint pos = MapToWorld(x, y);
	SDL_Rect rec = { pos.x * scale /*+ camOffset.x*/, pos.y * scale /*+ camOffset.y*/,
					 data.tileWidth * scale, data.tileHeight * scale };

	return rec;
}

TileSet* Map::GetTilesetFromTileId(int id) const
{
	ListItem<TileSet*>* item = data.tilesets.start;
	TileSet* set = item->data;

	while (item)
	{
		if (id < item->data->firstgid)
		{
			set = item->prev->data;
			break;
		}
		set = item->data;
		item = item->next;
	}

	return set;
}

// Get relative Tile rectangle
SDL_Rect TileSet::GetTileRect(int id) const
{
	SDL_Rect rect = { 0 };

	int relativeId = id - firstgid;
	rect.w = tileWidth;
	rect.h = tileHeight;
	rect.x = margin + ((rect.w + spacing) * (relativeId % numTilesWidth));
	rect.y = margin + ((rect.h + spacing) * (relativeId / numTilesWidth));

	return rect;
}

// Called before quitting
bool Map::CleanUp()
{
	LOG("Unloading map");

	ListItem<TileSet*>* item;
	item = data.tilesets.start;

	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	data.tilesets.Clear();

	// Remove all layers
	ListItem<MapLayer*>* item2;
	item2 = data.layers.start;

	while (item2 != NULL)
	{
		RELEASE(item2->data);
		item2 = item2->next;
	}
	data.layers.Clear();

	// Clean up the pugui tree
	mapFile.reset();

	return true;
}

// Load new map
bool Map::Load(const char* filename)
{
	bool ret = true;
	SString tmp("%s%s", folder.GetString(), filename);

	pugi::xml_parse_result result = mapFile.load_file(tmp.GetString());

	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", filename, result.description());
		ret = false;
	}

	// Load general info
	if (ret == true) ret = LoadMap();

	pugi::xml_node tileset;
	for (tileset = mapFile.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if (ret == true) ret = LoadTilesetDetails(tileset, set);

		if (ret == true) ret = LoadTilesetImage(tileset, set);

		data.tilesets.Add(set);
	}

	// Load layer info
	pugi::xml_node layer;
	for (layer = mapFile.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = new MapLayer();

		ret = LoadLayer(layer, lay);

		if (ret == true) data.layers.Add(lay);
	}

	pugi::xml_node object;
	for (object = mapFile.child("map").child("objectgroup"); object && ret; object = object.next_sibling("objectgroup"))
	{
		if (object.child("properties").child("property").attribute("name").as_string("interaction") == "interaction")
		{
			ObjectLayer* set = new ObjectLayer;

			if (ret == true) ret = LoadObjLayer(object, set);

			data.objLayers.Add(set);
		}
	}

	if (ret == true)
	{
		LOG("Successfully parsed map XML file: %s", filename);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tileWidth, data.tileHeight);

		ListItem<TileSet*>* item = data.tilesets.start;
		while (item != NULL)
		{
			TileSet* s = item->data;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.GetString(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tileWidth, s->tileHeight);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item = item->next;
		}

		ListItem<MapLayer*>* itemLayer = data.layers.start;
		while (itemLayer != NULL)
		{
			MapLayer* l = itemLayer->data;
			LOG("Layer ----");
			LOG("name: %s", l->name.GetString());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			itemLayer = itemLayer->next;
		}
	}

	mapLoaded = ret;

	return ret;
}

bool Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = mapFile.child("map");

	if (map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tileWidth = map.attribute("tilewidth").as_int();
		data.tileHeight = map.attribute("tileheight").as_int();
		SString bg_color(map.attribute("backgroundcolor").as_string());

		data.backgroundColor.r = 0;
		data.backgroundColor.g = 0;
		data.backgroundColor.b = 0;
		data.backgroundColor.a = 0;

		LoadProperties(map, this->properties);
		/*
		if (bg_color.Length() > 0)
		{
			SString red, green, blue;
			bg_color.SubString(1, 2, red);
			bg_color.SubString(3, 4, green);
			bg_color.SubString(5, 6, blue);

			int v = 0;

			sscanf_s(red.GetString(), "%x", &v);
			if (v >= 0 && v <= 255) data.backgroundColor.r = v;

			sscanf_s(green.GetString(), "%x", &v);
			if (v >= 0 && v <= 255) data.backgroundColor.g = v;

			sscanf_s(blue.GetString(), "%x", &v);
			if (v >= 0 && v <= 255) data.backgroundColor.b = v;
		}
		*/

		SString orientation(map.attribute("orientation").as_string());

		if (orientation == "orthogonal") data.type = MAPTYPE_ORTHOGONAL;
		else if (orientation == "isometric") data.type = MAPTYPE_ISOMETRIC;
		else if (orientation == "staggered") data.type = MAPTYPE_STAGGERED;
		else data.type = MAPTYPE_UNKNOWN;
	}

	return ret;
}

bool Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;

	set->name.Create(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tileWidth = tileset_node.attribute("tilewidth").as_int();
	set->tileHeight = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if (offset != NULL)
	{
		set->offsetX = offset.attribute("x").as_int();
		set->offsetY = offset.attribute("y").as_int();
	}
	else
	{
		set->offsetX = 0;
		set->offsetY = 0;
	}

	return ret;
}

bool Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node source = tileset_node.child("image");

	if (source == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = tex->Load(PATH(folder.GetString(), source.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->texWidth = source.attribute("width").as_int();

		if (set->texWidth <= 0)
		{
			set->texWidth = w;
		}

		set->texHeight = source.attribute("height").as_int();

		if (set->texHeight <= 0)
		{
			set->texHeight = h;
		}

		set->numTilesWidth = set->texWidth / set->tileWidth;
		set->numTilesHeight = set->texHeight / set->tileHeight;
	}

	return ret;
}

bool Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_int();
	layer->height = node.attribute("height").as_int();
	pugi::xml_node layerData = node.child("data");

	if (layerData == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		layer->data = new uint[layer->width * layer->height];
		memset(layer->data, 0, layer->width * layer->height);

		int i = 0;
		for (pugi::xml_node tile = layerData.child("tile"); tile; tile = tile.next_sibling("tile"))
		{
			layer->data[i++] = tile.attribute("gid").as_int(0);
		}
	}

	// Load layer properties
	LoadProperties(node, layer->properties);

	return ret;
}

bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
	bool ret = false;

	pugi::xml_node data = node.child("properties");

	if (data != NULL)
	{
		pugi::xml_node prop;

		for (prop = data.child("property"); prop; prop = prop.next_sibling("property"))
		{
			Properties::Property* p = new Properties::Property();

			p->name = prop.attribute("name").as_string();
			p->value = prop.attribute("value").as_int();

			properties.list.Add(p);
		}
	}

	return ret;
}

bool Map::Unload()
{
	bool ret = true;

	ListItem<TileSet*>* item;
	item = data.tilesets.start;

	for (item; item != NULL; item = item->next)
	{
		tex->UnLoad(item->data->texture);
	}

	data.tilesets.Clear();
	data.layers.Clear();
	data.objLayers.Clear();

	mapLoaded = false;

	return ret;
}

bool Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{
	bool ret = false;
	ListItem<MapLayer*>* item;
	item = data.layers.start;

	for (item; item != NULL; item = item->next)
	{
		MapLayer* layer = item->data;

		if (layer->properties.GetProperty("Navigation", 0) == 0)
			continue;

		uchar* map = new uchar[layer->width * layer->height];
		memset(map, 1, layer->width * layer->height);

		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				int i = (y * layer->width) + x;

				int tileId = layer->Get(x, y);
				TileSet* tileset = (tileId > 0) ? GetTilesetFromTileId(tileId) : NULL;

				if (tileset != NULL)
				{
					map[i] = (tileId - tileset->firstgid) > 0 ? 0 : 1;
				}
			}
		}

		*buffer = map;
		width = data.width;
		height = data.height;

		ret = true;

		break;
	}

	return ret;
}