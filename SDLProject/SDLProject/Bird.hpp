//
//  Bird.hpp
//  SDLProject
//
//  Created by Dylan Blake on 1/3/25.
//  Copyright © 2025 ctg. All rights reserved.
//

#ifndef BIRD_H
#define BIRD_H

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
#include "Entity.hpp"
#include <vector>
#include <ctime>
#include "cmath"

#include <vector>

using namespace glm;

class Bird : public Entity {
private:
    
    // special case for lunar lander proj
    int m_flap[6];
    
    
    vec3    m_hitbox_min, m_hitbox_max;
    
    /* ----- PHYSICS/TRANSFORMATIONS ----- */
    const vec3 GRAVITY = vec3(0.0f, -.3f, 0.0f);
    
    const float m_speed, m_flight_power;
    
    vec3    m_velocity,
            m_acceleration,
            m_movement;
    float   m_rotation_angle;
    
    
    /* ————— ANIMATION ————— */
    
    
    std::vector<std::vector<int>> m_flapping_animation;
    
    bool flapping = false;
    
    float   m_width     = 1,
            m_height    = 1;
    
public:
    
    // ————— STATIC VARIABLES ————— //
    static constexpr int SECONDS_PER_FRAME = 4;
    
    /* ----- METHODS ----- */
    Bird(vec3 initial_position = vec3(0.0f),
         vec3 hitbox_size = vec3(0.0f));
    
    
    
    void update(float delta_time);
    
    void translate(const vec3 pos);
    void tilt(int angle);
//    void update_hitbox(const vec3 pos);
    
    
    void init_anim();
    
    void reset_flapping();
    void process_flapping();
    bool flap();
    
    /* ----- GETTERS ----- */
    vec3 const get_velocity()       const { return m_velocity; };
    vec3 const get_acceleration()   const { return m_acceleration; };
    float const get_height() const { return m_height; };
    float const get_width()  const { return m_width; };
    
    float const get_speed()         const { return m_speed; };
    
    /* ----- SETTERS ----- */
    void set_movement(const vec3 mov)           { m_movement = mov; };
    void set_velocity(const vec3 vel)           { m_velocity = vel; };
    void set_acceleration(const vec3 accel)     { m_acceleration = accel; };
    
    void add_anim_time(const float delta_time) { m_anim_time += delta_time; };
    void anim_iterate() { m_anim_index++; };
    
    void stop_movement() { m_velocity = vec3(0.0f); };
    
};

#endif // BIRD_H
