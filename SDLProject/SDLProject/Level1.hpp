#ifndef LEVEL1_H
#define LEVEL1_H

#include "Scene.hpp"

class Level1 : public Scene {
public:
    int ENEMY_COUNT = 1;
    
    const vec3 GRAVITY = vec3(0.0f, -10.0f, 0.0f);
    
    ~Level1();
    
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};

#endif // LEVEL1_H
