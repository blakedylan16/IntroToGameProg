//
//  Entity.hpp
//  SDLProject
//
//  Created by Dylan Blake on 1/6/25.
//  Copyright © 2025 ctg. All rights reserved.
//

#ifndef ENTITY_H
#define ENTITY_H

#include "Map.hpp"

using namespace glm;

enum EntityType { PLATFORM, PLAYER, ENEMY };
enum AIType     { WALKER, GUARD, JUMPER };
enum AIState    { WALKING, IDLE, ATTACKING };

//enum AnimationDirection { LEFT, RIGHT };

class Entity {
private:
    EntityType m_entity_type;
    AIType m_ai_type;
    AIState m_ai_state;
    
    bool m_is_active = true;
    
    std::vector<int> m_walk_animation;
    
    /* ----- TRANSFORMATIONS ----- */
//    const vec3 GRAVITY;
    
    vec3    m_movement,
            m_position,
            m_scale,
            m_velocity,
            m_acceleration;
    
    mat4    m_model_matrix;
    
    float   m_speed,
            m_jumping_power;
    
    bool    m_is_jumping;
    bool    m_is_facing_right;
    
    /* ----- ANIMATION/TEXTURES ----- */
    GLuint  m_texture_id;

    int m_animation_frames,
        m_animation_rows,
        m_animation_cols,
        m_animation_index;
    
    std::vector<int> *m_animation_indices = nullptr;
    float m_animation_time;
    
//    float   m_width     = 1,
//            m_height    = 1;
    float m_size = 1;
    float m_hitbox_size;
    
    /* ----- COLLISION ----- */
    bool    m_collided_top          = false,
            m_collided_bottom       = false,
            m_collided_right        = false,
            m_collided_left         = false;
    bool    m_gap_bottom_left  = false,
            m_gap_bottom_right = false;
    
public:
    /* ----- STATIC VARIABLES ----- */
    static constexpr int FRAMES_PER_SECOND = 4;
    
    /* ----- METHODS ----- */
    Entity();
    Entity(GLuint tex_id, float speed, vec3 accel, float jump_pow,
           float anim_time, int anim_index, int anim_cols, int anim_rows,
           float size, EntityType type);
    // Simpler constructor - the one I'll be using the most
    Entity(GLuint tex_id, float speed, vec3 accel, float jump_pow,
           std::vector<int>& walk_anim, float size = 1, EntityType type = PLAYER);
    // AI constructor
    Entity(GLuint tex_id, float speed, vec3 accel, float jump_pow, std::vector<int>& walk_anim,
           float size, EntityType entity_type, AIType ai_type, AIState ai_state);
    ~Entity();
    
    void draw_sprite_from_texture_atlas(ShaderProgram *program, int index) const;
    
    bool const check_collision(Entity *other) const;
    
    void check_collision_y(Map *map);
    void check_collision_x(Map *map);
    
    void check_collision_y(std::vector<Entity*> objects, int object_count);
    void check_collision_x(std::vector<Entity*> objects, int object_count);
    
    void check_platform_x(Map *map, float delta_x);
    
    void update(Map *map, float delta_time = 0.0f,  Entity *player = nullptr,
                std::vector<Entity*> objects = std::vector<Entity*>(), int object_count = 0);
    void render(ShaderProgram *program);
    
    void ai_activate(Entity *player);
    void ai_walk();
    void ai_guard(Entity *player);
    void ai_jump();
    
    void normalize_movement() { m_movement = normalize(m_movement); }
    
    void move_left()    { m_movement.x = -1.0f; m_is_facing_right = false; }
    void move_right()   { m_movement.x = 1.0f; m_is_facing_right = true; }
    
    void jump() { m_is_jumping = true; }
    
    void activate()     { m_is_active = true; }
    void deactivate()   { m_is_active = false; }
    void kill_off();
    void reset(Map *map, vec3 pos);
    
    void init_anim();
    void add_anim_time(const float delta_time) { m_animation_time += delta_time; };
    void anim_iterate() { m_animation_index++; };
    
    /* ————— GETTERS ————— */
    EntityType const get_entity_type()  const { return m_entity_type; }
    AIType const get_ai_type()          const { return m_ai_type; }
    AIState const get_ai_state()        const { return m_ai_state; }
    GLuint const get_tex_id()           const { return m_texture_id; }
    vec3 const get_pos()        const { return m_position; }
    vec3 const get_vel()        const { return m_velocity; }
    vec3 const get_accel()      const { return m_acceleration; }
    vec3 const get_mov()        const { return m_movement; }
    vec3 const get_scale()      const { return m_scale; }
    float const get_speed()     const { return m_speed; }
    bool const get_active_state()       const { return m_is_active; }
    bool const get_collided_top()       const { return m_collided_top; }
    bool const get_collided_bottom()    const { return m_collided_bottom; }
    bool const get_collided_right()     const { return m_collided_right; }
    bool const get_collided_left()      const { return m_collided_left; }
    
    /* ————— SETTERS ————— */
    void set_ai_type(AIType type)       { m_ai_type = type; }
    void set_ai_state(AIState state)    { m_ai_state = state; }
    void set_pos(vec3 pos)              { m_position = pos; }
    void set_vel(vec3 vel)              { m_velocity = vel; }
    void set_accel(vec3 accel)          { m_acceleration = accel; }
    void set_mov(vec3 mov)              { m_movement = mov; }
    void set_scale(vec3 scale)          { m_scale = scale; }
    void set_speed(float speed)         { m_speed = speed; }
    void set_jump_pow(float jump_pow)   { m_jumping_power = jump_pow; }
    void set_anim_cols(int cols)        { m_animation_cols = cols; }
    void set_anim_rows(int rows)        { m_animation_rows = rows; }
    void set_anim_frames(int frames)    { m_animation_frames = frames; }
    void set_anim_index(int index)      { m_animation_index = index; }
    void set_anim_time(int time)        { m_animation_time = time; }
    void set_size(float size)           { m_size = size; }
    
    // specific to this tilemap
//    void set_walking(int walking)
    
    
//    void const setPlatformCollision(bool col)   { m_platformCollision = col; };
//    void const setEnemyCollision(bool col)      { m_enemyCollision = col; };
    
};

#endif // ENTITY_H
