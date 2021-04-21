# Sprite Sorting & Camera Culling
## Sprite Sorting
The objective of implementing sprite sorting is to make a 2D world look like a 3D world.
To achieve this effect, the developers use different methods:
### Cut Sprites
The sprite or tile used is divided in two. One part is rendered below the entities and the other above.  
It is the easiest and fastest way to implement the sorting.
### Sorting Layers
#### By position
Consist in sorting the entities depending of their Y position.
#### By triggers
Using colliders to detect what entity must be rendered first.

## Camera Culling
The camera culling is used to optimize the game by not rendering what is outside the camera.

## Implementation
