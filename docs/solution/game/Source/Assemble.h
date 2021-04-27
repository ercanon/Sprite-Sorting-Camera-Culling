#ifndef __Assemble_H__
#define __Assemble_H__

#include "Entity.h"

#include "Point.h"
#include "SString.h"
#include "Animation.h"

#include "SDL/include/SDL.h"


class Input;
class Render;
class Textures;

class Assemble : public Entity
{
public:

    // Get unique instance of the class
    static Assemble* GetInstance(Input* input, Render* render, Textures* tex);
    // Delete the instance
    static void ResetInstance();


    bool Update(float dt);

    bool Draw();

    void SetTexture(SDL_Texture *tex);

    SDL_Rect GetBounds();

private:

    // ----- SINGLETON METHODS ----- //
    // Singleton instance
    static Assemble* instance;
    // Private Constructor
    Assemble(Input* input, Render* render, Textures* tex);
    // Private Destructor
    virtual ~Assemble();
    // Declare the copy constructor and the assignment operator
    // as private (or delete them explicitly) to prevent cloning your object
    Assemble(const Assemble&);
    Assemble& operator=(const Assemble&);
    // ----------------------------- //

private:

    Input* input;
    Render* render;
    Textures* tex;
};

#endif // __Assemble_H__
