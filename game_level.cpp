/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "game_level.h"

#include <fstream>
#include <sstream>

void GameLevel::Load(const GLchar* file, GLuint levelWidth, GLuint levelHeight) {
    // reset
    this->Bricks.clear();

    GLuint tileCode;
    std::string line;
    std::ifstream fstream(file);
    std::vector<std::vector<GLuint>> tileData;
    if (fstream) {
        while (std::getline(fstream, line)) {
            std::istringstream sstream(line);
            std::vector<GLuint> row;
            // add each tile to row
            while (sstream >> tileCode)
                row.push_back(tileCode);
            tileData.push_back(row);
        }

        if (tileData.size() > 0)
            this->init(tileData, levelWidth, levelHeight);
    }
}

void GameLevel::Draw(SpriteRenderer &renderer) {
    for (GameObject &tile : this->Bricks) {
        if (!tile.Destroyed)
            tile.Draw(renderer);
    }
}

GLboolean GameLevel::IsCompleted() {
    for (GameObject &tile : this->Bricks) {
        if (!tile.IsSolid && !tile.Destroyed)
            return GL_FALSE;
    }
    return GL_TRUE;
}

void GameLevel::init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight) {
    GLuint height = tileData.size();
    GLuint width = tileData[0].size();
    GLfloat unit_width = levelWidth/static_cast<GLfloat>(width);
    GLfloat unit_height = levelHeight/static_cast<GLfloat>(height);

    // initalize game objects
    for (GLuint y=0; y < height; ++y) {
        for (GLuint x=0; x < width; ++x) {
            // do nothing if (tileData[y][x] == 0)
            if(tileData[y][x] > 0) {
                glm::vec2 pos(unit_width*x, unit_height*y);
                glm::vec2 size(unit_width, unit_height);
                std::string block_texture_name = "block";
                glm::vec3 color;
                GLboolean isSolid = GL_FALSE;

                if (tileData[y][x] == 1) {
                    color = glm::vec3(0.8f, 0.8f, 0.7f);
                    block_texture_name = "block_solid";
                    isSolid = GL_TRUE;
                } else if (tileData[y][x] == 2) {
                    color = glm::vec3(0.2f, 0.6f, 1.0f);
                } else if (tileData[y][x] == 3) {
                    color = glm::vec3(0.0f, 0.7f, 0.0f);
                } else if (tileData[y][x] == 4) {
                    color = glm::vec3(0.8f, 0.8f, 0.4f);
                } else if (tileData[y][x] == 5) {
                    color = glm::vec3(1.0f, 0.5f, 0.0f);
                }

                GameObject obj(pos, size, ResourceManager::GetTexture(block_texture_name), color);
                obj.IsSolid = isSolid;
                this->Bricks.push_back(obj);
            }
        }
    }
}

