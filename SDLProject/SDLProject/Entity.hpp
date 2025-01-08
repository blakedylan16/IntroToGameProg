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
    static constexpr GLint  NUMBER_OF_TEXTURES = 1,
                            LEVEL_OF_DETAIL    = 0,
                            TEXTURE_BORDER     = 0;
    
    bool m_is_active;
    
    EntityType m_type;
    
    /* ----- PHYSICS ------ */
    vec3    m_position;
    
    /* ----- TRANSFORMATIONS ----- */
    vec3    m_scale;
    mat4    m_model_matrix;
    
    GLuint  m_texture_id;
    
public:
    
//    bool m_collided_top      = false,
//         m_collided_bottom   = false,
//         m_collided_left     = false,
//         m_collided_right    = false;
//    
//    bool    m_platform_collision = false,
//            m_enemy_collision    = false;
    
    // ————— METHODS ————— //
    Entity(EntityType type, vec3 pos = vec3(0.0f));
//    virtual ~Entity();

    bool const check_collision(const Entity* other) const;
    void check_collisonX(const Entity* collidables, int collidablesCount);
    void check_collisonY(const Entity* collidables, int collidablesCount);
    
    void activate();
    void deactivate();
    
    bool const get_active_state();
    
    void draw_object(ShaderProgram* program);
    
    GLuint load_texture(const char* filepath);
    
    /* ————— GETTERS ————— */
    EntityType get_type() const;
    
    vec3 const get_pos() const;
    vec3 const get_scale() const;
    
    
    
//    bool const getPlatformCollision()   { return m_platformCollision; };
//    bool const getEnemyCollison()       { return m_enemyCollision; };
    
    /* ————— SETTERS ————— */
    void set_position(vec3 pos);
    void set_scale(vec3 scale);
    
//    void const setHeight(float newHeight)   { m_height = newHeight; };
//    void const setWidth(float newWidth)     { m_width = newWidth; };
    
//    void const setPlatformCollision(bool col)   { m_platformCollision = col; };
//    void const setEnemyCollision(bool col)      { m_enemyCollision = col; };
    
};

#endif // ENTITY_H
