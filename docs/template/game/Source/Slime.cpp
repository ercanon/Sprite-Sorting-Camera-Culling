#include "Slime.h"

#include "Log.h"
#include <time.h>

#include "Input.h"
#include "Render.h"
#include "Textures.h"


Slime* Slime::instance = nullptr;
// Instance creator
Slime* Slime::GetInstance(Input* input, Render* render, Textures* tex)
{
    instance = new Slime(input, render, tex);
    //LOG("Returning slime instance");

    return instance;
}
// Instance reseter
void Slime::ResetInstance()
{
    delete instance;
    instance = nullptr;
}
// Constructor
Slime::Slime(Input* input, Render* render, Textures* tex) : Entity(EntityType::ENEMY)
{
    this->input = input;
    this->render = render;
    this->tex = tex;

    slimeTexture = NULL;
    position = iPoint(0, 0);

    width = 24;
    height = 5;

    // Define Player animations
    slimeTexture = this->tex->Load("assets/sprites/slime.png");

    srand(time(NULL));
    switch (rand() % 2)
    {
    case 0 :
        slimeAnim->loop = true;
        slimeAnim->speed = 0.05f;
        for (int i = 0; i < 4; ++i)
            slimeAnim->PushBack({ 24 * i, 0, 24, 20 });
        break;
    case 1:
        slimeAnim->loop = true;
        slimeAnim->speed = 0.05f;
        for (int i = 0; i < 4; ++i)
            slimeAnim->PushBack({ 24 * i, 20, 24, 20 });
        break;
    }
}
// Destructor
Slime::~Slime()
{}



bool Slime::Update(float dt)
{
    slimeAnim->Update();
    return true;
}


bool Slime::Draw()
{
    SDL_Rect rec = slimeAnim->GetCurrentFrame();
    render->DrawTexture(slimeTexture, position.x, position.y - 13, &rec);

    return false;
}

void Slime::SetTexture(SDL_Texture *tex)
{
   slimeTexture = tex;
}

SDL_Rect Slime::GetBounds()
{
    return { position.x, position.y, width, height };
}
