//
//  Bird.cpp
//  SDLProject
//
//  Created by Dylan Blake on 1/3/25.
//  Copyright © 2025 ctg. All rights reserved.
//


// will need hitbox
// will change all physics methods to fix hitbox

//constexpr int   NUMBER_OF_TEXTURES  = 1;
//constexpr GLint LEVEL_OF_DETAIL = 0;
//constexpr GLint TEXTURE_BORDER = 0;

#include "Bird.hpp"

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
//    update_hitbox(pos);
}

void Bird::tilt(int angle) { m_rotation_angle += angle * m_speed; }

//void Bird::update_hitbox(vec3 pos) {
//    m_hitbox_max += pos;
//    m_hitbox_min += pos;
//}

void Bird::update(float delta_time) {
    
//    if (not m_is_active) return;
    
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




