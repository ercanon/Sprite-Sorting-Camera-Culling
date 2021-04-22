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
Once you are in the tileset window, add attributes to each tile using right click on the mouse in "Custom Attributes" we see on the left part of this window. I used an int named "detectAssamble", and used a number for each tipe of tile. For example, if a tree is two tiles tall, use the same number in both tiles.

### TODO 2
#### Read the information of the tileset in your game
`hola`

### TODO 3

### TODO 4
