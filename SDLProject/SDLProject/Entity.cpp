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
#include <vector>

#include "Entity.hpp"

using namespace glm;

// Default constructor
Entity::Entity() :
m_position(0.0f), m_movement(0.0f),  m_velocity(0.0f), m_acceleration(0.0f),
m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f), m_speed(0.0f), m_animation_cols(0),
m_animation_rows(0), m_animation_time(0.0f), m_texture_id(0), m_size(0.0f),
m_animation_indices(nullptr) { }
// Parameterized constructor
Entity::Entity(GLuint tex_id, float speed, vec3 accel, float jump_pow,
               float anim_time, int anim_index, int anim_cols, int anim_rows,
               float size, EntityType type) :
m_position(0.0f), m_movement(0.0f), m_model_matrix(1.0f), m_velocity(0.0f),
m_texture_id(tex_id), m_speed(speed), m_acceleration(accel), m_jumping_power(jump_pow),
m_animation_time(anim_time), m_animation_index(anim_index), m_animation_cols(anim_cols),
m_animation_rows(anim_rows), m_size(size), m_entity_type(type) {
    m_is_facing_right = true;
    set_scale(vec3(m_size, m_size, 0.0f));
}
// Simpler constructor for partial initializaiton
Entity::Entity(GLuint tex_id, float speed, vec3 accel, float jump_pow,
               std::vector<int>& walk_anim, float size, EntityType type) :
m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
m_velocity(0.0f), m_texture_id(tex_id), m_speed(speed), m_jumping_power(jump_pow),
m_walk_animation(walk_anim), m_acceleration(accel), m_size(size), m_entity_type(type) {
    init_anim();
    activate();
    m_is_facing_right = true;
    set_scale(vec3(m_size, m_size, 0.0f));
    m_hitbox_size = m_size * .8;
}
// AI constructor
Entity::Entity(GLuint tex_id, float speed, vec3 accel, float jump_pow,
               std::vector<int>& walk_anim, float size, EntityType entity_type,
               AIType ai_type, AIState ai_state) :
m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
m_velocity(0.0f), m_texture_id(tex_id), m_speed(speed), m_jumping_power(jump_pow),
m_walk_animation(walk_anim), m_acceleration(accel), m_size(size), m_entity_type(entity_type),
m_ai_type(ai_type), m_ai_state(ai_state) {
    init_anim();
    activate();
    m_is_facing_right = false;
    set_scale(vec3(m_size, m_size, 0.0f));
    m_hitbox_size = m_size * .8;
}

Entity::~Entity() { }
// might split into ai update and player update
void Entity::update(Map* map, float delta_time, Entity* player,
                    std::vector<Entity*> objects, int object_count) {
    
    if (not m_is_active) return;
    
    m_collided_top = false;
    m_collided_bottom = false;
    m_collided_right = false;
    m_collided_left = false;
    m_gap_bottom_left = false;
    m_gap_bottom_right = false;
    
    
//    if (m_entity_type == ENEMY) {
//        check_collision_x(map);
//        check_collision_y(map);
//        if (player) ai_activate(player);
//    }
    
    if (m_animation_indices != NULL) {
        if (length(m_movement) != 0) {
            m_animation_time += delta_time;
            float seconds_per_frame = (float) 1 / FRAMES_PER_SECOND;
            
            if (m_animation_time >= seconds_per_frame) {
                m_animation_time = 0.0f;
                m_animation_index++;
                
                if (m_animation_index >= m_animation_frames) m_animation_index = 0;
            }
        }
    }
    
    m_velocity.x = m_movement.x * m_speed;
    m_velocity += m_acceleration * delta_time;
    
    m_position.x += m_velocity.x * delta_time;
    check_collision_x(objects, object_count);
    check_collision_x(map);
    
    m_position.y += m_velocity.y * delta_time;
    check_collision_y(objects, object_count);
    check_collision_y(map);
    
    
    
    if (m_entity_type == ENEMY) {
        if (player) ai_activate(player);
        
        check_platform_x(map, m_velocity.x * delta_time);
        
        if (m_ai_type == WALKER) {
            if (m_collided_right or m_gap_bottom_right)
                move_left();
            else if (m_collided_left or m_gap_bottom_left)
                move_right();
        }
    }
    
    if (m_is_jumping) {
        m_is_jumping = false;
        m_velocity.y += m_jumping_power;
    }
    
    m_model_matrix = mat4(1.0f);
    m_model_matrix = translate(m_model_matrix, m_position);
    if (m_is_facing_right)
        m_model_matrix = scale(m_model_matrix, vec3(-1 * m_scale.x, m_scale.y, m_scale.z));
    else m_model_matrix = scale(m_model_matrix, m_scale);
}

void Entity::render(ShaderProgram* program) {
    program->SetModelMatrix(m_model_matrix);
    
    if (not m_is_active) return;
    
    if (m_animation_indices != nullptr) {
        draw_sprite_from_texture_atlas(program, m_animation_indices[0][m_animation_index]);
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

bool const Entity::check_collision(Entity *other) const {
    float x_dist = fabs(m_position.x - other->m_position.x);
    x_dist -= ((m_hitbox_size + other->m_hitbox_size) / 2.0f);
    float y_dist = fabs(m_position.y - other->m_position.y);
    y_dist -= ((m_size + other->m_size) / 2.0f);
    return x_dist < 0.0f and y_dist < 0.0f;
}

void Entity::check_collision_y(std::vector<Entity*> objects, int object_count) {
    for (int i = 0; i < object_count; i++) {
        Entity *object = objects[i];
        
        if (check_collision(object)) {
            float y_dist = fabs(m_position.y - object->m_position.y);
            float y_overlap = fabs(y_dist - (m_size / 2.0f) - (object->m_size / 2.0f));
            if (m_velocity.y > 0) {
                m_position.y   -= y_overlap;
                m_velocity.y    = 0;

                m_collided_top  = true; // Collision!
            } else if (m_velocity.y < 0) {
                m_position.y      += y_overlap;
                m_velocity.y       = 0;

                m_collided_bottom  = true; // Collision!
                object->kill_off();
            }
        }
    }
}

void Entity::check_collision_x(std::vector<Entity*> objects, int object_count) {
    for (int i = 0; i < object_count; i++) {
        Entity *object = objects[i];
        
        if (check_collision(object)) {
            LOG("collision in the x");
            float x_dist = fabs(m_position.x - object->m_position.x);
            float x_overlap = fabs(x_dist - (m_size / 2.0f) - (object->m_size / 2.0f));
            if (m_velocity.x > 0) {
                m_position.x   -= x_overlap;
//                m_velocity.x    = 0;

                m_collided_right  = true; // Collision!
//                kill_off();a
            } else if (m_velocity.x < 0) {
                m_position.x      += x_overlap;
//                m_velocity.x       = 0;

                m_collided_left  = true; // Collision!
//                kill_off();
            }
            kill_off();
        }
//        if (m_collided_left or m_collided_right) kill_off();
    }
}

void Entity::check_collision_y(Map *map) {
    // Probes for tiles above
    vec3 top = vec3(m_position.x, m_position.y + (m_size / 2), m_position.z);
    vec3 top_left = vec3(m_position.x - (m_size / 2), m_position.y + (m_size / 2), m_position.z);
    vec3 top_right = vec3(m_position.x + (m_size / 2), m_position.y + (m_size / 2), m_position.z);
    
    // Probes for tiles below
    vec3 bottom = vec3(m_position.x, m_position.y - (m_size / 2), m_position.z);
    vec3 bottom_left = vec3(m_position.x - (m_size / 2), m_position.y - (m_size / 2), m_position.z);
    vec3 bottom_right = vec3(m_position.x + (m_size / 2), m_position.y - (m_size / 2), m_position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    // If the map is solid, check the top three points
    if (map->is_solid(top, &penetration_x, &penetration_y)
        and m_velocity.y > 0) {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    } else if (map->is_solid(top_left, &penetration_x, &penetration_y)
               and m_velocity.y > 0) {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    } else if (map->is_solid(top_right, &penetration_x, &penetration_y)
               and m_velocity.y > 0) {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    
    // And the bottom three points
    if (map->is_solid(bottom, &penetration_x, &penetration_y)
        and m_velocity.y < 0) {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    } else if (map->is_solid(bottom_left, &penetration_x, &penetration_y)
               and m_velocity.y < 0) {
            m_position.y += penetration_y;
            m_velocity.y = 0;
            m_collided_bottom = true;
    } else if (map->is_solid(bottom_right, &penetration_x, &penetration_y)
               and m_velocity.y < 0) {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
}

void Entity::check_collision_x(Map *map) {
    // Probes for tiles; the x-checking is much simpler
    vec3 left   = vec3(m_position.x - (m_size / 2), m_position.y, m_position.z);
    vec3 right  = vec3(m_position.x + (m_size / 2), m_position.y, m_position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    if (map->is_solid(left, &penetration_x, &penetration_y)
        and m_velocity.x < 0) {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
    }
    if (map->is_solid(right, &penetration_x, &penetration_y)
        and m_velocity.x > 0) {
        m_position.x -= penetration_x;
        m_velocity.x = 0;
        m_collided_right = true;
    }
}

void Entity::check_platform_x(Map *map, float delta_x) {
    // Probe for tiles at the bottom-left and bottom-right corners of the entity
    vec3 bottom_left    = vec3(m_position.x - (m_size / 2),
                               m_position.y - (m_size / 2),
                               m_position.z);
    vec3 bottom_right   = vec3(m_position.x + (m_size / 2),
                               m_position.y - (m_size / 2),
                               m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    bool solid_bottom_left = map->is_solid(bottom_left, &penetration_x, &penetration_y);
    bool solid_bottom_right = map->is_solid(bottom_right, &penetration_x, &penetration_y);

    if (not solid_bottom_left and m_velocity.x < 0) {
        m_position.x += delta_x;
        m_velocity.x = 0;  // Stop horizontal movement
        m_gap_bottom_left = true;  // Set collision flag
    }
    if (not solid_bottom_right and m_velocity.x > 0) {
        m_position.x -= delta_x;
        m_velocity.x = 0;
        m_gap_bottom_right = true;
    }
}

void Entity::draw_sprite_from_texture_atlas(ShaderProgram *program, int index) const {
    // Step 1: Calculate the UV location of the indexed frame
    float u_coord = (float) (index % m_animation_cols) / (float) m_animation_cols;
    float v_coord = (float) (index / m_animation_cols) / (float) m_animation_rows;

    // Step 2: Calculate its UV size
    float width = 1.0f / (float) m_animation_cols;
    float height = 1.0f / (float) m_animation_rows;

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
// specifc to tilemap
void Entity::init_anim() {
    m_animation_rows = 3;
    m_animation_cols = 9;
    m_animation_frames = 2;
    m_animation_time = 0.0f;
    m_animation_index = 0; // might neeed to change
    m_animation_indices = &m_walk_animation;
}

void Entity::ai_activate(Entity *player) {
    switch (m_ai_type) {
        case WALKER:
            ai_walk();
            break;
        case GUARD: ai_guard(player); break;
        case JUMPER: ai_jump(); break;
        default: break;
    }
}

void Entity::ai_walk() {
    float mov = 0.0f;
    if (m_collided_bottom) {
        if (m_is_facing_right) mov = 1.0f;
        else mov = -1.0f;
    }
    m_movement = vec3(mov, 0.0f, 0.0f);
}

void Entity::ai_jump() {
    jump();
}

void Entity::ai_guard(Entity *player) {
    switch (m_ai_state) {
        case IDLE:
            if (distance(m_position, player->get_pos()) < 3.0f)
                m_ai_state = WALKING;
            break;
        case WALKING:
            if (m_position.x > player->get_pos().x)
                m_movement = vec3(-1.0f, 0.0f, 0.0f);
            else m_movement = vec3(1.0f, 0.0f, 0.0f);
            break;
//        case ATTACKING: break;
        default:
            break;
    }
}

void Entity::kill_off() {
    m_is_active = false;

    m_position = vec3(0.0f, -10.0f, 0.0f);

    m_velocity = vec3(0.0f, 0.0f, 0.0f);

    m_model_matrix = mat4(1.0f);

    LOG("Enemy has been killed off.");
}

void Entity::reset(Map *map, vec3 pos) {
    activate();
    m_is_facing_right = true;
    m_position = pos;
    m_movement = vec3(0.0f);
    update(map);
}

