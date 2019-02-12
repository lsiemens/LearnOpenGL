/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "resource_manager.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <stb_image/stb_image.h>

std::map<std::string, Shader> ResourceManager::Shaders;
std::map<std::string, Texture2D> ResourceManager::Textures;

Shader ResourceManager::LoadShader(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile, std::string name) {
    Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return Shaders[name];
}

Shader ResourceManager::GetShader(std::string name) {
    return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const GLchar* file, std::string name) {
    Textures[name] = loadTextureFromFile(file);
    return Textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name) {
    return Textures[name];
}

void ResourceManager::Clear() {
    // free assets
    for (auto iter : Shaders)
        glDeleteProgram(iter.second.ID);
    for (auto iter : Textures)
        glDeleteTextures(1, &iter.second.ID);
}

Shader ResourceManager::loadShaderFromFile(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile) {
    std::string vShaderCode = loadSourceCode(vShaderFile).c_str();
    std::string fShaderCode = loadSourceCode(fShaderFile).c_str();
    std::string gShaderCode = "";
    if (gShaderFile != nullptr) {
        std::string gShaderCode = loadSourceCode(gShaderFile).c_str();
    }

    // compile and link shader code
    Shader shader;
    shader.Compile(vShaderCode.c_str(), fShaderCode.c_str(), (gShaderFile != nullptr) ? gShaderCode.c_str() : nullptr);
    return shader;
}

std::string ResourceManager::loadSourceCode(const GLchar* sourcePath) {
    // get sourceCode
    std::string shaderSourceCode;
    std::ifstream shaderFile;

    // enable exception checking
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        shaderFile.open(sourcePath);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        shaderSourceCode = shaderStream.str();
    } catch (std::exception e) {
        std::cout << "ERROR::SHADER: Failed to read shader file: " << sourcePath << std::endl;
    }
    return shaderSourceCode;
}

Texture2D ResourceManager::loadTextureFromFile(const GLchar* file) {
    Texture2D texture;

    // load image
    int width, height, numChannels;
    unsigned char* image = stbi_load(file, &width, &height, &numChannels, 0);

    // check if alpha channel exists
    if (numChannels==3) {
        texture.Internal_Format = GL_RGB;
        texture.Image_Format = GL_RGB;
    } else if (numChannels==4) {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    } else {
        std::cout << "ERROR::TEXTURE: Unsupported number of channels ( " << numChannels << " ) in file: " << file << std::endl;
    }

    //  if data exists create texture
    if (image) {
       texture.Generate(width, height, image);
    } else {
        std::cout << "ERROR::TEXTURE: Failed to read texture file: " << file << std::endl;
    }
    stbi_image_free(image);
    return texture;
}

