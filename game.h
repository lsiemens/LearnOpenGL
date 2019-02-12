/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/

#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN,
    GAME_LOSS
};

class Game {
public:
    // State variables
    GameState State;
    GLboolean Keys[1024];
    GLuint Width, Height;

    // class constructor destructor
    Game(GLuint width, GLuint height);
    ~Game();

    // Initalize game and assets
    void Init();

    // Game loop functions
    void ProcessInput(GLfloat dt);
    void Update(GLfloat dt);
    void Render();
};

#endif
