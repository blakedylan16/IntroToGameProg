#ifndef SCENE_H
#define SCENE_H

#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Utility.hpp"
#include "Entity.hpp"
#include "Map.hpp"


struct GameState
{
    Map *map;
    Entity *player;
    std::vector<Entity*> enemies;
    
    Mix_Music *bgm;
    Mix_Chunk *jump_sfx;
    
    int next_scene_id;
};

class Scene {
protected:
    int *g_lives;
public:
    
    Scene();
    
    virtual ~Scene() {}
    
    GameState m_game_state;
    
    GLuint  g_map_texture_id,
            g_font_texture_id,
            g_sprite_texture_id;
    
    static constexpr const char *SPRITESHEET_FILEPATH = "tilemap-characters_packed.png",
                        *FONTSHEET_FILEPATH = "font1.png",
                        *MAP_TILESET_FILEPATH = "tilemap_packed.png",
                        *BGM_FILEPATH = "Sergio_music.mp3",
                        *JUMP_SFX_FILEPATH = "jump.wav";
    
    const glm::vec3 GRAVITY = glm::vec3(0.0f,-6.0f, 0.0f);
    
    int m_number_of_enemies = 1;
    
    void set_lives(int *lives) { g_lives = lives; }
    
    virtual void initialise() = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(ShaderProgram *program) = 0;
    
    GameState const get_state()     const { return m_game_state; }
    int const get_num_of_enemies()  const { return m_number_of_enemies; }
};

#endif // SCENE_H
