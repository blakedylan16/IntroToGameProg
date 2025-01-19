#ifndef LEVEL3_H
#define LEVEL3_H

#include "Scene.hpp"

class Level3 : public Scene {
public:
    Level3() { m_number_of_enemies = ENEMY_COUNT; }
    
    int ENEMY_COUNT = 1;
    
    ~Level3();
    
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};

#endif // LEVEL3_H
