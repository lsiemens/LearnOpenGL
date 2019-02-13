/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include <algorithm>

#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "post_processor.h"
#include "particle_generator.h"
#include "game_object.h"
#include "ball_object.h"

GameObject* Player;
BallObject* Ball;
SpriteRenderer* Renderer;
ParticleGenerator* Particles;
PostProcessor* Effects;
GLfloat ShakeTime = 0.0f;

GLboolean ShouldSpawn(GLuint chance);
void ActivatePowerUp(PowerUp &powerUp);
GLboolean IsOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type);
GLboolean CheckCollision(GameObject &one, GameObject &two);
Collision CheckCollision(BallObject &one, GameObject &two);
Direction VectorDirection(glm::vec2 target);

Game::Game(GLuint width, GLuint height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height) {
}

Game::~Game() {
    delete Renderer;
    delete Ball;
    delete Player;
    delete Particles;
    delete Effects;
}

void Game::Init() {
    // Load shaders
    ResourceManager::LoadShader("shaders/sprite.vert", "shaders/sprite.frag", nullptr, "sprite");
    ResourceManager::LoadShader("shaders/particle.vert", "shaders/particle.frag", nullptr, "particle");
    ResourceManager::LoadShader("shaders/post_processing.vert", "shaders/post_processing.frag", nullptr, "postprocessing");

    // Load Textures
    ResourceManager::LoadTexture("textures/background.jpg", "background");
    ResourceManager::LoadTexture("textures/awesomeface.png", "face");
    ResourceManager::LoadTexture("textures/paddle.png", "paddle");
    ResourceManager::LoadTexture("textures/block.png", "block");
    ResourceManager::LoadTexture("textures/block_solid.png", "block_solid");
    ResourceManager::LoadTexture("textures/particle.png", "particle");
    ResourceManager::LoadTexture("textures/powerup_speed.png", "powerup_speed");
    ResourceManager::LoadTexture("textures/powerup_sticky.png", "powerup_sticky");
    ResourceManager::LoadTexture("textures/powerup_increase.png", "powerup_increase");
    ResourceManager::LoadTexture("textures/powerup_confuse.png", "powerup_confuse");
    ResourceManager::LoadTexture("textures/powerup_chaos.png", "powerup_chaos");
    ResourceManager::LoadTexture("textures/powerup_passthrough.png", "powerup_passthrough");

    // pass data to GPU glUniform
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("particle").SetMatrix4("projection", projection);

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
    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x/2.0f - BALL_RADIUS, -BALL_RADIUS*2.0f);
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));

    Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 500);
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), this->Width, this->Height);

    // Debug Testing options
    //Effects->Shake = GL_TRUE;
    //Effects->Confuse = GL_TRUE;
    //Effects->Chaos = GL_TRUE;
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

        if (this->Keys[GLFW_KEY_SPACE]) {
            Ball->Stuck = false;
        }
    }
}

void Game::Update(GLfloat dt) {
    Ball->Move(dt, this->Width);
    this->DoCollisions();
    Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius/2));
    this->UpdatePowerUps(dt);

    if (ShakeTime > 0.0f) {
        ShakeTime -= dt;
        if (ShakeTime <= 0.0f)
            Effects->Shake = false;
    }

    if (Ball->Position.y >= this->Height) {
        this->ResetLevel();
        this->ResetPlayer();
    }
}

void Game::Render() {
    if(this->State == GAME_ACTIVE) {
        // configure OpenGL to render off screen
        Effects->BeginRender();
            // Background
            Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f);

            // Objects
            Particles->Draw();
            Ball->Draw(*Renderer);
            this->Levels[this->Level].Draw(*Renderer);
            for (PowerUp &powerUp : this->PowerUps) {
                if (!powerUp.Destroyed)
                    powerUp.Draw(*Renderer);
            }
            Player->Draw(*Renderer);
        // save render to texture and return OpenGL configuration to standard render
        Effects->EndRender();
        // render prerenderd scene to screen using postprocessing shaders
        Effects->Render(glfwGetTime());
    }
}

void Game::DoCollisions() {
    for (GameObject &box : this->Levels[this->Level].Bricks) {
        if (!box.Destroyed) {
            Collision collision = CheckCollision(*Ball, box);
            if (std::get<0>(collision)) { // If collision is true
                // Destroy block if not solid
                if (!box.IsSolid) {
                    box.Destroyed = GL_TRUE;
                    this->SpawnPowerUps(box);
                } else {
                    ShakeTime = 0.05f;
                    Effects->Shake = true;
                }
                // Collision resolution
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                if (dir == LEFT || dir == RIGHT) { // Horizontal collision
                    Ball->Velocity.x = -Ball->Velocity.x; // Reverse horizontal velocity
                    // Relocate
                    GLfloat penetration = Ball->Radius - std::abs(diff_vector.x);
                    if (dir == LEFT)
                        Ball->Position.x += penetration; // Move ball to right
                    else
                        Ball->Position.x -= penetration; // Move ball to left;
                } else {
                    Ball->Velocity.y = -Ball->Velocity.y; // Reverse vertical velocity
                    // Relocate
                    GLfloat penetration = Ball->Radius - std::abs(diff_vector.y);
                    if (dir == UP)
                        Ball->Position.y -= penetration; // Move ball back up
                    else
                        Ball->Position.y += penetration; // Move ball back down
                }
            }
        }
    }

    // Also check collisions on PowerUps and if so, activate them
    for (PowerUp &powerUp : this->PowerUps) {
        if (!powerUp.Destroyed) {
            // First check if powerup passed bottom edge, if so: keep as inactive and destroy
            if (powerUp.Position.y >= this->Height)
                powerUp.Destroyed = GL_TRUE;

            // Collided with player, now activate powerup
            if (CheckCollision(*Player, powerUp)) {
                ActivatePowerUp(powerUp);
                powerUp.Destroyed = GL_TRUE;
                powerUp.Activated = GL_TRUE;
            }
        }
    }

    // Also check collisions for player pad (unless stuck)
    Collision result = CheckCollision(*Ball, *Player);
    if (!Ball->Stuck && std::get<0>(result))
    {
        // Check where it hit the board, and change velocity based on where it hit the board
        GLfloat centerBoard = Player->Position.x + Player->Size.x / 2;
        GLfloat distance = (Ball->Position.x + Ball->Radius) - centerBoard;
        GLfloat percentage = distance / (Player->Size.x / 2);
        // Then move accordingly
        GLfloat strength = 2.0f;
        glm::vec2 oldVelocity = Ball->Velocity;
        Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
        //Ball->Velocity.y = -Ball->Velocity.y;
        Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity); // Keep speed consistent over both axes (multiply by length of old velocity, so total strength is not changed)
        // Fix sticky paddle
        Ball->Velocity.y = -1 * abs(Ball->Velocity.y);

        // If Sticky powerup is activated, also stick ball to paddle once new velocity vectors were calculated
        Ball->Stuck = Ball->Sticky;
    }
}

void Game::ResetLevel() {
    if (this->Level == 0) {
        this->Levels[0].Load("levels/one.lvl", this->Width, this->Height * 0.5f);
    } else if (this->Level == 1) {
        this->Levels[1].Load("levels/two.lvl", this->Width, this->Height * 0.5f);
    } else if (this->Level == 2) {
        this->Levels[2].Load("levels/three.lvl", this->Width, this->Height * 0.5f);
    } else if (this->Level == 3) {
        this->Levels[3].Load("levels/four.lvl", this->Width, this->Height * 0.5f);
    }
}

void Game::ResetPlayer() {
    Player->Size = PLAYER_SIZE;
    Player->Position = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
    Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -(BALL_RADIUS * 2)), INITIAL_BALL_VELOCITY);

    Effects->Chaos = GL_FALSE;
    Effects->Confuse = GL_FALSE;
    Ball->PassThrough = GL_FALSE;
    Ball->Sticky = GL_FALSE;
    Player->Color = glm::vec3(1.0f);
    Ball->Color = glm::vec3(1.0f);
}

void Game::SpawnPowerUps(GameObject &block) {
    if (ShouldSpawn(75)) // 1 in 75 chance
        this->PowerUps.push_back(PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position, ResourceManager::GetTexture("powerup_speed")));
    if (ShouldSpawn(75))
        this->PowerUps.push_back(PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position, ResourceManager::GetTexture("powerup_sticky")));
    if (ShouldSpawn(75))
        this->PowerUps.push_back(PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position, ResourceManager::GetTexture("powerup_passthrough")));
    if (ShouldSpawn(75))
        this->PowerUps.push_back(PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.Position, ResourceManager::GetTexture("powerup_increase")));
    if (ShouldSpawn(15)) // Negative powerups should spawn more often
        this->PowerUps.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_confuse")));
    if (ShouldSpawn(15))
        this->PowerUps.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_chaos")));
}

void Game::UpdatePowerUps(GLfloat dt) {
    for (PowerUp &powerUp : this->PowerUps) {
        powerUp.Position += powerUp.Velocity * dt;
        if (powerUp.Activated) {
            powerUp.Duration -= dt;

            if (powerUp.Duration <= 0.0f) {
                // Remove powerup from list (will later be removed)
                powerUp.Activated = GL_FALSE;
                // Deactivate effects
                if (powerUp.Type == "sticky") {
                    // Only reset if no other PowerUp of type sticky is active
                    if (!IsOtherPowerUpActive(this->PowerUps, "sticky")) {
                        Ball->Sticky = GL_FALSE;
                        Player->Color = glm::vec3(1.0f);
                    }
                } else if (powerUp.Type == "pass-through") {
                    // Only reset if no other PowerUp of type pass-through is active
                    if (!IsOtherPowerUpActive(this->PowerUps, "pass-through")) {
                        Ball->PassThrough = GL_FALSE;
                        Ball->Color = glm::vec3(1.0f);
                    }
                } else if (powerUp.Type == "confuse") {
                    // Only reset if no other PowerUp of type confuse is active
                    if (!IsOtherPowerUpActive(this->PowerUps, "confuse")) {
                        Effects->Confuse = GL_FALSE;
                    }
                } else if (powerUp.Type == "chaos") {
                    // Only reset if no other PowerUp of type chaos is active
                    if (!IsOtherPowerUpActive(this->PowerUps, "chaos")) {
                        Effects->Chaos = GL_FALSE;
                    }
                }
            }
        }
    }
    // Remove all PowerUps from vector that are destroyed AND !activated (thus either off the map or finished)
    // Note we use a lambda expression to remove each PowerUp which is destroyed and not activated
    this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
        [](const PowerUp &powerUp) { return powerUp.Destroyed && !powerUp.Activated; }
    ), this->PowerUps.end());
}

GLboolean ShouldSpawn(GLuint chance) {
    GLuint random = rand() % chance;
    return random == 0;
}

void ActivatePowerUp(PowerUp &powerUp) {
    // Initiate a powerup based type of powerup
    if (powerUp.Type == "speed") {
        Ball->Velocity *= 1.2;
    } else if (powerUp.Type == "sticky") {
        Ball->Sticky = GL_TRUE;
        Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
    } else if (powerUp.Type == "pass-through") {
        Ball->PassThrough = GL_TRUE;
        Ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
    } else if (powerUp.Type == "pad-size-increase") {
        Player->Size.x += 50;
    } else if (powerUp.Type == "confuse") {
        if (!Effects->Chaos)
            Effects->Confuse = GL_TRUE; // Only activate if chaos wasn't already active
    } else if (powerUp.Type == "chaos") {
        if (!Effects->Confuse)
            Effects->Chaos = GL_TRUE;
    }
}

GLboolean IsOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type) {
    // Check if another PowerUp of the same type is still active
    // in which case we don't disable its effect (yet)
    for (const PowerUp &powerUp : powerUps) {
        if (powerUp.Activated)
            if (powerUp.Type == type)
                return GL_TRUE;
    }
    return GL_FALSE;
}

GLboolean CheckCollision(GameObject &one, GameObject &two) {
    // Collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    // Collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    // Collision only if on both axes
    return collisionX && collisionY;
}

Collision CheckCollision(BallObject &one, GameObject &two) { // AABB - Circle collision
    // Get center point circle first
    glm::vec2 center(one.Position + one.Radius);
    // Calculate AABB info (center, half-extents)
    glm::vec2 aabb_half_extents(two.Size.x / 2, two.Size.y / 2);
    glm::vec2 aabb_center(two.Position.x + aabb_half_extents.x, two.Position.y + aabb_half_extents.y);
    // Get difference vector between both centers
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // Now that we know the the clamped values, add this to AABB_center and we get the value of box closest to circle
    glm::vec2 closest = aabb_center + clamped;
    // Now retrieve vector between center circle and closest point AABB and check if length < radius
    difference = closest - center;

    if (glm::length(difference) < one.Radius) // not <= since in that case a collision also occurs when object one exactly touches object two, which they are at the end of each collision resolution stage.
        return std::make_tuple(GL_TRUE, VectorDirection(difference), difference);
    else
        return std::make_tuple(GL_FALSE, UP, glm::vec2(0, 0));
}

Direction VectorDirection(glm::vec2 target) {
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),  // up
        glm::vec2(1.0f, 0.0f),  // right
        glm::vec2(0.0f, -1.0f), // down
        glm::vec2(-1.0f, 0.0f)  // left
    };
    GLfloat max = 0.0f;
    GLuint best_match = -1;
    for (GLuint i = 0; i < 4; i++) {
        GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max) {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}
