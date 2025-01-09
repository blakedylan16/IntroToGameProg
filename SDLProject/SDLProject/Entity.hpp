//
//  Entity.hpp
//  SDLProject
//
//  Created by Dylan Blake on 1/6/25.
//  Copyright © 2025 ctg. All rights reserved.
//

#ifndef ENTITY_H
#define ENTITY_H

using namespace glm;

enum EntityType { PLATFORM, BIRD, ENEMY, FONT };

class Entity {
protected:
    EntityType m_type;
    
    //    bool m_is_active;
    
    /* ----- TRANSFORMATIONS ----- */
    vec3    m_movement,
            m_position,
            m_scale;
    
    mat4    m_model_matrix;
    

    
    /* ----- ANIMATION ----- */
    GLuint  m_texture_id;

    int m_anim_frames,
        m_anim_rows,
        m_anim_cols,
        m_anim_index;
    
    int *m_anim_indices = nullptr;
    float m_anim_time = 0.0f;
    
public:
    
    // ————— METHODS ————— //
    Entity(EntityType type, vec3 pos = vec3(0.0f));
//    virtual ~Entity();
    
    void draw_sprite_from_texture_atlas(ShaderProgram *program, int index);
    
    void update(float delta_time);
    void render(ShaderProgram* program);
    
    void normalize_movement() { m_movement = normalize(m_movement); };
    
    
//    void activate();
//    void deactivate();
//    bool const get_active_state();
    
    /* ————— GETTERS ————— */
    EntityType get_type() const { return m_type; };
    
    vec3 const get_pos()    const { return m_position; };
    vec3 const get_mov()    const { return m_movement; };
    vec3 const get_scale()  const { return m_scale; };
    
    int const get_anim_cols()   const { return m_anim_cols; };
    int const get_anim_rows()   const { return m_anim_rows; };
    int const get_anim_frames() const { return m_anim_frames; };
    int const get_anim_index()  const { return m_anim_index; };
    float const get_anim_time() const { return m_anim_time; };
    
    /* ————— SETTERS ————— */
    void set_pos(vec3 pos)      { m_position = pos; };
    void set_mov(vec3 mov)      { m_movement = mov; };
    void set_scale(vec3 scale)  { m_scale = scale; };
    void set_texture(GLuint id) {m_texture_id = id; };
    
    void set_anim_cols(int cols)        { m_anim_cols = cols; };
    void set_anim_rows(int rows)        { m_anim_rows = rows; };
    void set_anim_frames(int frames)    { m_anim_frames = frames; };
    void set_anim_index(int index)      { m_anim_index = index; };
    void set_anim_time(int time)        { m_anim_time = time; };
    
//    void const setHeight(float newHeight)   { m_height = newHeight; };
//    void const setWidth(float newWidth)     { m_width = newWidth; };
    
//    void const setPlatformCollision(bool col)   { m_platformCollision = col; };
//    void const setEnemyCollision(bool col)      { m_enemyCollision = col; };
    
};

#endif // ENTITY_H
