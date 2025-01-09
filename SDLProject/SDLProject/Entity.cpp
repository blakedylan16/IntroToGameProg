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
Entity::Entity(EntityType type, vec3 init_pos) : m_type(type), m_position(init_pos),  m_model_matrix(mat4(1.0f)), m_movement(vec3(0.0f)),  m_scale(vec3(1.0f, 1.0f, 0.0f)), m_anim_index(0), m_anim_time(0.0f) { }

void Entity::draw_sprite_from_texture_atlas(ShaderProgram *program,
                                          int index) {
    // Step 1: Calculate the UV location of the indexed frame
    float u_coord = (float) (index % m_anim_cols) / (float) m_anim_cols;
    float v_coord = (float) (index / m_anim_cols) / (float) m_anim_rows;

    // Step 2: Calculate its UV size
    float width = 1.0f / (float) m_anim_cols;
    float height = 1.0f / (float) m_anim_rows;

    // Step 3: Just as we have done before, match the texture coordinates to the vertices
    float tex_coords[] = {
        u_coord, v_coord + height,
        u_coord + width, v_coord + height,
        u_coord + width, v_coord,
        u_coord, v_coord + height,
        u_coord + width, v_coord,
        u_coord, v_coord
    };

    float vertices[] = {
        -0.5, -0.5,
         0.5, -0.5,
         0.5,  0.5,
        -0.5, -0.5,
         0.5,  0.5,
        -0.5,  0.5
    };

    // Step 4: And render
    if (m_texture_id == 0) {
        LOG("ERROR: Invalid texture ID!");
        return;
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    

    glVertexAttribPointer(program->positionAttribute, 2,
                          GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0,
                          tex_coords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}


void Entity::update(float delta_time) {
    m_model_matrix = mat4(1.0f);
    m_model_matrix = translate(m_model_matrix, m_position);
}

void Entity::render(ShaderProgram* program) {
    program->SetModelMatrix(m_model_matrix);
    
    if (m_anim_indices != NULL) {
        draw_sprite_from_texture_atlas(program, m_anim_indices[m_anim_index]);
        return;
    }
    
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

//void Entity::activate()     { m_is_active = true; }
//void Entity::deactivate()   { m_is_active = false; }
//
//bool const Entity::get_active_state()   { return m_is_active; }




