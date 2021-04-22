# Sprite Sorting & Camera Culling
## Sprite Sorting
The objective of implementing sprite sorting is to make a 2D world look like a 3D world.
To achieve this effect, we can use differents methods:
### Cut Sprites
The sprite or tile used is divided in two. One part is rendered below the entities and the other above.  
It is the easiest and fastest way to implement the sorting of sprites.
### Sorting Layers
#### By position
Consist in sorting the entities depending of their Y position.
#### By triggers
Using colliders to detect what entity must be rendered first.
#### 3D

## Camera Culling
The camera culling is used to optimize the game by not rendering what is outside the camera.

## Implementation

### TODO 1
#### Add attributes to the tiles in the tileset.   
I am using [Tiled](https://www.mapeditor.org/) to create the maps used in this template.  
When you have all setted up to work, Tiled should have opened a tileset window. If not, clic in the tileset options button, at the bottom of your active tilesets.  
![tileset](https://github.com/ercanon/Sprite-Sorting-Camera-Culling/blob/main/docs/images/tileset.png)  
Once you are in the tileset window, add attributes to each tile using right click on the mouse in "Custom Attributes" we see on the left part of this window. I used an int named "detectAssamble", and used a number for each tipe of tile. For example, if a tree is two tiles tall, use the same number in both tiles.    
![detectAssamble_1](https://github.com/ercanon/Sprite-Sorting-Camera-Culling/blob/main/docs/images/detectAssamble_1.png)  
![detectAssamble_2](https://github.com/ercanon/Sprite-Sorting-Camera-Culling/blob/main/docs/images/detectAssamble_2.png)

### TODO 2
#### Read the information of the tileset in your game
You will need to store the information about the attributes implemented; in other words: how many tiles have a property, the ID of the tile, which property, etc. \
  I did it like this:
```
int tileCount;
struct TileProperty
{
  int tileId;
  Properties properties;
} tileProperty[MAX_TILES_WITH_PROPERTIES];
```

`Watch out because you need to declare first the struct Properties to store them.` 
  
Then, in the .cpp, read and store the variables. Something like this:
```
set->tileCount = 0;
pugi::xml_node actualNode = tileset_node.child("tile");
while (actualNode.child("properties"))
{
  set->tileProperty[set->tileCount].tileId = actualNode.attribute("id").as_int();
  LoadProperties(actualNode, set->tileProperty[set->tileCount].properties);
  
  actualNode = actualNode.next_sibling();
  set->tileCount++;
}
```

### TODO 3
#### The function to draw everything
We need in the entity manager a function to draw entities and maps.  
So, for the moment, adapt how the map and the entities are drawed.

### TODO 3.5
#### Detect the tiles with attributes
When rendering each tile, detect if the tile is one with an attribute.
```
for (int t = 0; t < tileset->tileCount; t++)
{
  if (tileId - 1 == tileset->tileProperty[t].tileId) 
  {}
}
```

### TODO 4
#### Avengers, Assemble!
This class will store all the information about the generated assembles, that are tiles united to form like entities.  
It should have enough variables for the sorting and the render.   
This are the variables I used:
```
class Assemble
{
public:

  int tilesAssemble = 0;
  struct TileInfo
  {
    iPoint tileMapPosition;
    iPoint tileWorldPosition;
    TileSet* tileset;
    SDL_Rect rectangle;
    
  } tileInfo[MAX_TILES_ASSEMBLED];
```

### TODO 5
#### Check the previous tiles
To generate assembles we should check previous tiles in the X and Y axis.  
```
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

for (int t = 0; t < tileset->tileCount; t++)
{
  if (tileId - 1 == tileset->tileProperty[t].tileId)
  {
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
        
     if (tileset->tileProperty[t].properties.GetProperty("detectAssamble", 0) == propPrevX) 
     {}
     else if (tileset->tileProperty[t].properties.GetProperty("detectAssamble", 0) == propPrevY) 
     {}
     else
     {}
     break;
  }
}
```

### TODO 6
#### Store the Assemble information
Store information about the assembles that are going to be created. If the actual tile and the previous have the same, the actual tile will count as the assemble of the previous tile. If not, it will generate a new assemble.
```
if (tileset->tileProperty[t].properties.GetProperty("detectAssamble", 0) == propPrevX)
{
    for (int a = 0; a < assembledList.Count(); a++)
    {
        bool done = false;
        for (int i = 0; i < assembledList.At(a)->data->tilesAssemble; i++)
	{
	    if (assembledList.At(a)->data->tileInfo[i].tileMapPosition.x == x - 1 && assembledList.At(a)->data->tileInfo[i].tileMapPosition.y == y)
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
	    if (assembledList.At(a)->data->tileInfo[i].tileMapPosition.x == x && assembledList.At(a)->data->tileInfo[i].tileMapPosition.y == y - 1)
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
```

### TODO 6.5
#### Do not render the assembled tiles
The tiles that have been assembled are going to be rendered later. A boolean will do the work.

### TODO 7
#### The sorting starts
First, organize the entities by their position in the Y axis so, when drawing them, the entities will render in the correct order.
```
ListItem<Entity*>* list = entities.start;
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
```

### TODO 7.5
#### The copy of the sorting
Create a new `ListItem<Entity*>*` to copy the previous list sorted.  
`The list sorted is inverted, so when the copy is used it must be rendered backwards (x->prev)`

### TODO 8
#### Get the dimensions of the assemble
To do it, we go for each assemble created getting its dimensions.
```
for (int l = 0; l < assembledList.Count(); l++)
{
    int maxX = assembledList.At(l)->data->tileInfo[0].tileWorldPosition.x;
    int minX = assembledList.At(l)->data->tileInfo[0].tileWorldPosition.x;
    int maxY = assembledList.At(l)->data->tileInfo[0].tileWorldPosition.y;
    int minY = assembledList.At(l)->data->tileInfo[0].tileWorldPosition.y;
    
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
}
```

### TODO 9
#### Draw entities below the assemble
Using the previous TODO generate all entities that are under an assemble.
```
sorted = list;
while (sorted != NULL)
{
   if (sorted->data->renderable == true)
   {
   	sorted->data->Draw();
	sorted->data->renderable = false;
   }
   sorted = sorted->prev;
}
```

### TODO 10
#### Draw all assembles created
Draw every assemble created, then delete them and clear the list to avoid memory leaks.
```
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
```
### TODO 11
#### Draw entities on top of the assembles
Draw the rest of the entities that have not been rendered.
```
sorted = list;
while (sorted != NULL)
{
    if (sorted->data->renderable == true)
	    sorted->data->Draw();
	    
    sorted->data->renderable = true;
    sorted = sorted->prev;
}
```

### TODO 12
#### Camera Culling
Implement the camera culling. If any tile is not in the camera, will not be calculated and drawed.  
`If the assembles not render correctly, add a bit more in the dimensions of the camera (-render->camera.x - render->camera.w/4)`
```
for (int y = 0; y < map->data.height; ++y)
{
    for (int x = 0; x < map->data.width; ++x)
    {
        iPoint pos = map->MapToWorld(x, y);
	
	if (pos.x + map->data.tileWidth > -render->camera.x && pos.x < -render->camera.x + render->camera.w &&
	    pos.y + map->data.tileWidth > -render->camera.y && pos.y < -render->camera.y + render->camera.h)
	{}
    }
}
```

```
if (sorted->data->position.x + map->data.tileWidth > -render->camera.x && 
    sorted->data->position.x < -render->camera.x + render->camera.w &&
    sorted->data->position.y + map->data.tileWidth > -render->camera.y && 
    sorted->data->position.y < -render->camera.y + render->camera.h &&
    sorted->data->renderable == true)
        sorted->data->Draw();
```
### EXTRAS
Personalize and adapt the code as your needs demand. You can add elements to improve the optimitation, or to draw entities once when rendering layers.
