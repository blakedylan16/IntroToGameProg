//
//  Bird.hpp
//  SDLProject
//
//  Created by Dylan Blake on 1/3/25.
//  Copyright © 2025 ctg. All rights reserved.
//

#ifndef BIRD_H
#define BIRD_H

#include "Entity.hpp"
#include <vector>

using namespace glm;

class Bird : public Entity {
private:
    
    vec3    m_hitbox_min, m_hitbox_max;
    
    /* ----- PHYSICS/TRANSFORMATIONS ----- */
    const vec3 GRAVITY = vec3(0.0f, -.3f, 0.0f);
    
    const float m_speed, m_flight_power;
    
    vec3    m_velocity,
            m_acceleration,
            m_movement;
    float   m_rotation_angle;
    
    
    /* ————— ANIMATION ————— */
    int m_animation_frames;
    std::vector<std::vector<int>> m_flapping_animation;
    
    int m_rows, m_cols;
    
    bool flapping = false;
    
    float   m_width     = 1,
            m_height    = 1;
    
    int *m_animation_flap   = NULL;
    
    float m_animation_time    = 0.0f;
    
public:
    
    // ————— STATIC VARIABLES ————— //
    static const int SECONDS_PER_FRAME = 4;
    static const int LEFT  = 0,
                     RIGHT = 1,
                     UP    = 2,
                     DOWN  = 3;
    
    /* ----- METHODS ----- */
    Bird(vec3 initial_position = vec3(0.0f),
         vec3 hitbox_size = vec3(0.0f));
    
    void translate(const vec3 pos);
    void tilt(int angle);
    void update_hitbox(const vec3 pos);
    void draw_sprite_from_texture_atlas(ShaderProgram *program,
                                        int index);
    GLuint load_texture(const char* filepath, int rows, int cols);
    
    void init_animation(int rows, int cols);
    
    void reset_flapping();
    void process_flapping();
    bool flap();
    
    
    void update(float delta_time);
    void render(ShaderProgram* program, int animation_index);
    
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
    
    void stop_movement() { m_velocity = vec3(0.0f); };
    
};

#endif // BIRD_H
