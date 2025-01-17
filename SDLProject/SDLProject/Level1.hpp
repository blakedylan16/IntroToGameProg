#ifndef LEVEL1_H
#define LEVEL1_H

#include "Scene.hpp"

class Level1 : public Scene {
public:
    Level1() { m_number_of_enemies = ENEMY_COUNT; }
    
    int ENEMY_COUNT = 1;
    
    ~Level1();
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};

#endif // LEVEL1_H
