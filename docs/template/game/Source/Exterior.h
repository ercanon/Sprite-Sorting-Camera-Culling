#ifndef __EXTERIOR_H__
#define __EXTERIOR_H__

#define LEFT_CAMERA_LIMIT 0
#define RIGHT_CAMERA_LIMIT -795

#include "Scene.h"
#include "Font.h"

class Map;

class SceneManager;

class Exterior : public Scene
{
public:

    // Constructor
    Exterior(SceneManager* sceneManager);
    // Destructor
    virtual ~Exterior();


    // Load this scene
    bool Load();

    // Update this scene
    bool Update(float dt);

    // Draw this scene
    bool Draw();

    // Unload this scene
    bool Unload();

private:
    Map* map;

    SceneManager* sceneManager;
};

#endif // __EXTERIOR_H__