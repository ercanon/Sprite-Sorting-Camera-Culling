#ifndef __SLIME_H__
#define __SLIME_H__

#include "Entity.h"

#include "Point.h"
#include "SString.h"
#include "Animation.h"

#include "SDL/include/SDL.h"


class Input;
class Render;
class Textures;

class Slime : public Entity
{
public:

    // Get unique instance of the class
    static Slime* GetInstance(Input* input, Render* render, Textures* tex);
    // Delete the instance
    static void ResetInstance();


    bool Update(float dt);

    bool Draw();

    void SetTexture(SDL_Texture *tex);

    SDL_Rect GetBounds();

public:

    Animation* slimeAnim = new Animation();
    SDL_Texture* slimeTexture;

private:

    // ----- SINGLETON METHODS ----- //
    // Singleton instance
    static Slime* instance;
    // Private Constructor
    Slime(Input* input, Render* render, Textures* tex);
    // Private Destructor
    virtual ~Slime();
    // Declare the copy constructor and the assignment operator
    // as private (or delete them explicitly) to prevent cloning your object
    Slime(const Slime&);
    Slime& operator=(const Slime&);
    // ----------------------------- //

private:

    Input* input;
    Render* render;
    Textures* tex;
};

#endif // __SLIME_H__
