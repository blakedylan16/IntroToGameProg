#ifndef START_H
#define START_H

#include "Scene.hpp"

class Start : public Scene {
public:
    int ENEMY_COUNT = 1;
    
    ~Start();
    
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};

#endif // LEVEL1_H
