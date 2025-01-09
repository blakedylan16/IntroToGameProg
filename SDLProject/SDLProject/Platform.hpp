//
//  Platform.hpp
//  SDLProject
//
//  Created by Dylan Blake on 1/8/25.
//  Copyright © 2025 ctg. All rights reserved.
//

#ifndef PLATFORM_H
#define PLATFORM_H

#include "Entity.hpp"

using  namespace glm;

class Platform : public Entity {
private:
    
    vec3    m_scale;
public:
    Platform(vec3 pos, vec3 scale = vec3(1.0f, 1.0f, 0.0f));
    
    void update();
//    void render(ShaderProgram* program);
};

#endif // PLATFORM_H
