/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "sprite_renderer.h"

// Class for defining objects in breakout
class GameObject {
public:
    // State variables
    glm::vec2 Position, Size, Velocity;
    glm::vec3 Color;
    GLfloat Rotation;
    GLboolean IsSolid, Destroyed;
    Texture2D Sprite;

    // Constructors
    GameObject();
    GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color=glm::vec3(1.0f), glm::vec2 velocity=glm::vec2(0.0f, 0.0f));

    virtual void Draw(SpriteRenderer &renderer);
};

#endif
