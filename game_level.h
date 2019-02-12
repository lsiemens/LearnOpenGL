/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef GAME_LEVEL_H
#define GAME_LEVEL_H

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "game_object.h"
#include "sprite_renderer.h"
#include "resource_manager.h"


// class to contain aload levels
class GameLevel {
public:
    // state variables
    std::vector<GameObject> Bricks;
    // Constructor
    GameLevel() { }

    void Load(const GLchar* file, GLuint levelWidth, GLuint levelHeight);
    void Draw(SpriteRenderer &renderer);

    // Check if all non-solid tiles are destroyed
    GLboolean IsCompleted();

private:
    // Initialize level
    void init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight);
};

#endif
