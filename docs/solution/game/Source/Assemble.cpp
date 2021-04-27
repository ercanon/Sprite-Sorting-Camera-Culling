#include "Assemble.h"

#include "Log.h"
#include <time.h>

#include "Input.h"
#include "Render.h"
#include "Textures.h"


Assemble* Assemble::instance = nullptr;
// Instance creator
Assemble* Assemble::GetInstance(Input* input, Render* render, Textures* tex)
{
    instance = new Assemble(input, render, tex);
    //LOG("Returning slime instance");

    return instance;
}
// Instance reseter
void Assemble::ResetInstance()
{
    delete instance;
    instance = nullptr;
}
// Constructor
Assemble::Assemble(Input* input, Render* render, Textures* tex) : Entity(EntityType::ASSEMBLE)
{
    this->input = input;
    this->render = render;
    this->tex = tex;
}
// Destructor
Assemble::~Assemble()
{}



bool Assemble::Update(float dt)
{

    return true;
}


bool Assemble::Draw()
{

    return false;
}

void Assemble::SetTexture(SDL_Texture *tex)
{

}

SDL_Rect Assemble::GetBounds()
{
    return { position.x, position.y, width, height };
}
