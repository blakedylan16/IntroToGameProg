#ifndef LEVEL2_H
#define LEVEL2_H

#include "Scene.hpp"

class Level2 : public Scene {
public:
    Level2() { m_number_of_enemies = ENEMY_COUNT; }
    
    int ENEMY_COUNT = 1;
    
    ~Level2();
    
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};

#endif // LEVEL2_H
