/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>

#include <glad/glad.h>

#include "texture.h"
#include "shader.h"

// static singleton resource manager class
class ResourceManager {
public:
    // asset storage
    static std::map<std::string, Shader> Shaders;
    static std::map<std::string, Texture2D> Textures;

    // setup shader
    static Shader LoadShader(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile, std::string name);
    static Shader GetShader(std::string name);

    // setup texture
    static Texture2D LoadTexture(const GLchar* file, std::string name);
    static Texture2D GetTexture(std::string name);

    // cleanup assets
    static void Clear();

private:
    ResourceManager() { }
    static Shader loadShaderFromFile(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile=nullptr);
    static std::string loadSourceCode(const GLchar* sourcePath);
    static Texture2D loadTextureFromFile(const GLchar* file);
};

#endif
