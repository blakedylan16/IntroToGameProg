//
//  Platform.cpp
//  SDLProject
//
//  Created by Dylan Blake on 1/8/25.
//  Copyright © 2025 ctg. All rights reserved.
//

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define LOG(argument) std::cout << argument << '\n'

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "Platform.hpp"
#include <vector>
#include <ctime>
#include "cmath"

Platform::Platform(vec3 pos, vec3 scale) :
Entity(PLATFORM, pos) {
    m_scale = scale;
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, m_scale);
}

void Platform::update() {
    m_model_matrix = mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, m_scale);
}

//void Platform::render(ShaderProgram *program) {
//    program->SetModelMatrix(m_model_matrix);
//    draw_object(program);
//}
