/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/

#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"

GameObject* Player;
SpriteRenderer *Renderer;

Game::Game(GLuint width, GLuint height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height) {
}

Game::~Game() {
    delete Renderer;
    delete Player;
}

void Game::Init() {
    // Load shaders
    ResourceManager::LoadShader("shaders/sprite.vert", "shaders/sprite.frag", nullptr, "sprite");

    // Load Textures
    ResourceManager::LoadTexture("textures/background.jpg", "background");
    ResourceManager::LoadTexture("textures/paddle.png", "paddle");
    ResourceManager::LoadTexture("textures/block.png", "block");
    ResourceManager::LoadTexture("textures/block_solid.png", "block_solid");

    // pass data to GPU glUniform
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

    // initalize Levels
    GameLevel one, two, three, four;
    one.Load("levels/one.lvl", this->Width, this->Height*0.5f);
    two.Load("levels/two.lvl", this->Width, this->Height*0.5f);
    three.Load("levels/three.lvl", this->Width, this->Height*0.5f);
    four.Load("levels/four.lvl", this->Width, this->Height*0.5f);

    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);

    // set current level
    this->Level = 0;

    // initalize player
    glm::vec2 playerPos = glm::vec2((this->Width - PLAYER_SIZE.x)/2.0f, this->Height - PLAYER_SIZE.y);
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
}

void Game::Update(GLfloat dt) {
}

void Game::ProcessInput(GLfloat dt) {
    if (this->State == GAME_ACTIVE) {
        GLfloat velocity = PLAYER_VELOCITY*dt;

        // handel key events
        if (this->Keys[GLFW_KEY_A]) {
            if (Player->Position.x >= 0)
                Player->Position.x -= velocity;
        }

        if (this->Keys[GLFW_KEY_D]) {
            if (Player->Position.x <= this->Width - Player->Size.x)
                Player->Position.x += velocity;
        }
    }
}

void Game::Render() {
    if(this->State == GAME_ACTIVE) {
        Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f);
        this->Levels[this->Level].Draw(*Renderer);
        Player->Draw(*Renderer);
    }
}
