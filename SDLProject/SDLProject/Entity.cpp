//
//  Entity.cpp
//  SDLProject
//
//  Created by Dylan Blake on 1/6/25.
//  Copyright © 2025 ctg. All rights reserved.
//


#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define LOG(argument) std::cout << argument << '\n'

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "Entity.hpp"
#include <vector>
#include <ctime>
#include "cmath"

using namespace glm;

/* ----- METHODS ----- */
Entity::Entity(EntityType type, vec3 pos) :
m_is_active(true), m_type(type), m_position(pos) {
    m_model_matrix = mat4(1.0f);
}

void Entity::activate()     { m_is_active = true; }
void Entity::deactivate()   { m_is_active = false; }

bool const Entity::get_active_state()   { return m_is_active; }

void Entity::draw_object(ShaderProgram* program) {
    float vertices[] = {
        -0.5, -0.5,
         0.5, -0.5,
         0.5,  0.5,
        -0.5, -0.5,
         0.5,  0.5,
        -0.5,  0.5
    };
    
    float texCoords[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f
    };
        
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
        
        glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program->positionAttribute);
            
        glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(program->texCoordAttribute);
            
        glDrawArrays(GL_TRIANGLES, 0, 6);
            
        glDisableVertexAttribArray(program->positionAttribute);
        glDisableVertexAttribArray(program->texCoordAttribute);
}

GLuint Entity::load_texture(const char *filepath) {
    int width, height, number_of_components;
    LOG("Attempting to load texture: " << filepath);
    // "load" dynamically allocates memory
    unsigned char* image = stbi_load(filepath, &width, &height,
                                     &number_of_components, STBI_rgb_alpha);

    if (image == nullptr) {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }
    
    GLuint textureID;                               // declaration
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);  // assignment
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexImage2D(
        GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA,
        width, height,
        TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE,
        image
    );
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    stbi_image_free(image);
    
    m_texture_id = textureID;

    return textureID;
}
/* ————— GETTERS ————— */
EntityType Entity::get_type() const     { return m_type; }

vec3 const Entity::get_pos() const { return m_position; }
vec3 const Entity::get_scale() const {return m_scale; }


//    bool const getPlatformCollision()   { return m_platformCollision; };
//    bool const getEnemyCollison()       { return m_enemyCollision; };

/* ————— SETTERS ————— */
void Entity::set_position(vec3 pos)   { m_position = pos; }
void Entity::set_scale(vec3 scale)    { m_scale = scale; }

