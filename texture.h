/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

// Arbitray shader class
class Texture2D {
public:

    // State variables
    GLuint ID;
    // texture dimensions
    GLuint Width, Height;
    GLuint Internal_Format;
    GLuint Image_Format;

    //OpenGL texture setting
    GLuint Wrap_S;
    GLuint Wrap_T;
    GLuint Filter_Min;
    GLuint Filter_Max;

    // Constructor
    Texture2D();

    // Load texture from image
    void Generate(GLuint widht, GLuint height, unsigned char* data);

    // set as current texture
    void Bind() const;
};

#endif
