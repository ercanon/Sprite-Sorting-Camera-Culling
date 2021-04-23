#ifndef __SCENE_H__
#define __SCENE_H__

#include "SString.h"

class GuiControl;

enum class SceneType
{
    EXTERIOR,
};

class Scene
{
public:

    Scene() : active(true), loaded(false) {}

    virtual bool Load()
    {
        return true;
    }

    virtual bool Update(float dt)
    {
        return true;
    }

    virtual bool Draw()
    {
        return true;
    }

    virtual bool Unload()
    {
        return true;
    }

public:

    bool active = true;
    SString name;         // Scene name identifier?

    // Possible properties
    bool loaded = false;
};

#endif // __SCENE_H__