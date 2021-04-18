#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "Point.h"
#include "SString.h"

enum class EntityType
{
    PLAYER,
    ENEMY,
    MAP,
    UNKNOWN
};

class Entity
{
public:

    Entity(EntityType type) : type(type), active(true) {}

    virtual bool Update(float dt)
    {
        return true;
    }

    virtual bool Draw()
    {
        return true;
    }

public:

    EntityType type;
    bool active = true;

    iPoint temPos;
    iPoint position;  
    bool renderable = false;
};

#endif // __ENTITY_H__