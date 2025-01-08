//
//  Bird.cpp
//  SDLProject
//
//  Created by Dylan Blake on 1/3/25.
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
#include "Bird.hpp"
#include "Entity.hpp"
#include <vector>
#include <ctime>
#include "cmath"

// will need hitbox
// will change all physics methods to fix hitbox

//constexpr int   NUMBER_OF_TEXTURES  = 1;
//constexpr GLint LEVEL_OF_DETAIL = 0;
//constexpr GLint TEXTURE_BORDER = 0;

Bird::Bird(vec3 initial_pos, vec3 hitbox_size) :
Entity(BIRD, initial_pos), m_speed(0.8f), m_movement(vec3(0.0f)),
m_acceleration(GRAVITY), m_flight_power(.4f), m_rotation_angle(0) {
    translate(m_position);
    
    m_model_matrix = rotate(m_model_matrix, radians(m_rotation_angle),
                                 vec3(0.0f, 0.0f, 1.0f));
    
    m_scale = vec3(.5f, .5f, 0.0f);
    m_model_matrix = scale(m_model_matrix, m_scale);
    
    m_hitbox_min = initial_pos - hitbox_size / 2.0f;
    m_hitbox_max = initial_pos + hitbox_size / 2.0f;
}

void Bird::translate(vec3 pos) {
    m_position = pos;
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    update_hitbox(pos);
}

void Bird::tilt(int angle) { m_rotation_angle += angle * m_speed; }

void Bird::update_hitbox(vec3 pos) {
    m_hitbox_max += pos;
    m_hitbox_min += pos;
}

void Bird::update(float delta_time) {
    
    if (not m_is_active) return;
    
    m_model_matrix = mat4(1.0f);
    
    m_velocity += m_acceleration * delta_time;
    
    translate(m_position + m_velocity * delta_time);
    
    m_model_matrix = rotate(m_model_matrix, radians(m_rotation_angle),
                                 vec3(0.0f, 0.0f, 1.0f)); // make prettier
    
    m_model_matrix = scale(m_model_matrix, m_scale); // make prettier
    
    m_acceleration = GRAVITY;
}

void Bird::reset_flapping()     { flapping = false; }
void Bird::process_flapping()   { flapping = true; }

bool Bird::flap() {
    if (flapping) {
        m_acceleration = m_flight_power * vec3(cos(radians(m_rotation_angle + 90)),
                                               sin(radians(m_rotation_angle + 90)),
                                               0.0f);
    }
    return flapping;
}



void Bird::render(ShaderProgram* program, int animation_index) {
    program->SetModelMatrix(m_model_matrix);
    draw_sprite_from_texture_atlas(program, animation_index);
}

void Bird::init_animation(int rows, int cols) {
    m_rows = rows;
    m_cols = cols;
    m_animation_frames = rows * cols;
    
    m_flapping_animation.resize(rows, std::vector<int>(cols));
    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            m_flapping_animation[i][j] = i * rows  + j;
        }
    }
}

void Bird::draw_sprite_from_texture_atlas(ShaderProgram *program,
                                          int index) {
    // Step 1: Calculate the UV location of the indexed frame
    float u_coord = (float) (index % m_cols) / (float) m_cols;
    float v_coord = (float) (index / m_cols) / (float) m_rows;

    // Step 2: Calculate its UV size
    float width = 1.0f / (float) m_cols;
    float height = 1.0f / (float) m_rows;

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

GLuint Bird::load_texture(const char* filepath, int rows, int cols) {
    
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
    
    init_animation(rows, cols);

    return textureID;
}

