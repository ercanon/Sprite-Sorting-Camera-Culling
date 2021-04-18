#include "Player.h"

#include "Log.h"

#include "Input.h"
#include "Render.h"
#include "Textures.h"

#include "Collision.h"


Player* Player::instance = nullptr;
// Instance creator
Player* Player::GetInstance(Input* input, Render* render, Textures* tex)
{
    if (instance == nullptr) instance = new Player(input, render, tex);
    //else LOG("Returning player instance");

    return instance;
}
// Instance reseter
void Player::ResetInstance()
{
    delete instance;
    instance = nullptr;
}
// Constructor
Player::Player(Input* input, Render* render, Textures* tex) : Entity(EntityType::PLAYER)
{
    this->input = input;
    this->render = render;
    this->tex = tex;

    //
    // Animation pushbacks
    //
    animPlayerWalkLeft->loop = true;
    animPlayerWalkLeft->speed = 0.05f;
    for (int i = 0; i < 4; ++i)
        animPlayerWalkLeft->PushBack({ 34 * i, 0, 34, 32 });

    animPlayerWalkRight->loop = true;
    animPlayerWalkRight->speed = 0.05f;
    for (int i = 0; i < 4; ++i)
        animPlayerWalkRight->PushBack({ 34 * i, 32, 34, 32 });

    animPlayerWalkDown->loop = true;
    animPlayerWalkDown->speed = 0.05f;
    for (int i = 0; i < 4; ++i)
        animPlayerWalkDown->PushBack({ 34 * i, 64, 34, 32 });

    animPlayerWalkUp->loop = true;
    animPlayerWalkUp->speed = 0.05f;
    for (int i = 0; i < 4; ++i)
        animPlayerWalkUp->PushBack({ 34 * i, 96, 34, 32 });

    //
    // Load textures
    //
    playerTexture = this->tex->Load("assets/sprites/eru_movement.png");

    position = iPoint(12 * 16, 27 * 16);

    width = 14;
    height = 8;

    playerCollision[0] = { 0, 0 };
    playerCollision[1] = { width, 0 };
    playerCollision[2] = { 0, height };
    playerCollision[3] = { width, height };

    // Define Player current animation
    currentAnimation = animPlayerWalkDown;
}
// Destructor
Player::~Player()
{}



bool Player::Update(float dt)
{
    GamePad& pad = input->pads[0];

    if (collision->player == nullptr) collision->player = this;

    temPos = position;

    if ((input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) && input->joystickState() == false || (pad.l_x < -0.75f && pad.l_y > -0.75f && pad.l_y < 0.75f) && input->joystickState() == true)
    {
        position.x -= (PLAYER_MOVE_SPEED * dt);
        if (currentAnimation != animPlayerWalkLeft) currentAnimation = animPlayerWalkLeft;
    }
    if ((input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) && input->joystickState() == false || (pad.l_x > 0.75f && pad.l_y > -0.75f && pad.l_y < 0.75f) && input->joystickState() == true)
    {
        position.x += (PLAYER_MOVE_SPEED * dt + 1);
        if (currentAnimation != animPlayerWalkRight) currentAnimation = animPlayerWalkRight;
    }

    if ((input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) && input->joystickState() == false || (pad.l_y < 0.75f && pad.l_x > -0.75f && pad.l_x < 0.75f) && input->joystickState() == true)
    {
        position.y -= (PLAYER_MOVE_SPEED * dt);
        if (currentAnimation != animPlayerWalkUp) currentAnimation = animPlayerWalkUp;
    }
    if ((input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) && input->joystickState() == false || (pad.l_y > -0.75f && pad.l_x > -0.75f && pad.l_x < 0.75f) && input->joystickState() == true)
    {
        position.y += (PLAYER_MOVE_SPEED * dt + 1);
        if (currentAnimation != animPlayerWalkDown) currentAnimation = animPlayerWalkDown;
    }   

    if (position == temPos) currentAnimation->SetCurrentFrame(0);
    else currentAnimation->Update();

    return true;
}


bool Player::Draw()
{
    SDL_Rect rect = currentAnimation->GetCurrentFrame();
    render->DrawTexture(playerTexture, (int)position.x - 10, (int)position.y - 24, &rect);

    render->DrawRectangle({position.x + playerCollision[0].x, position.y + playerCollision[0].y, playerCollision[3].x, playerCollision[3].y }, 0, 255, 0, 255);

    return false;
}


SDL_Rect Player::GetBounds()
{
    return { position.x, position.y, width, height };
}

void Player::SetCollision(Collision* collision, Player* player)
{
    player->collision = collision;
}