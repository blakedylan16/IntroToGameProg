///*
// Author: Dylan Blake
// Assignment: Lunar Lander
// I pledge that I have completed this assignment without collaborating
// with anyone else, in conformance with the NYU School of Engineering
// Policies and Procedures on Academic Misconduct
// 
// sprite from https://www.freepik.com/free-vector/flat-design-animation-frames-element-collection_31630031.htm#fromView=keyword&page=1&position=0&uuid=e59fd0c7-c217-4f01-be6a-80318579dcec
// */


#define LOG(argument) std::cout << argument << '\n'
#define STB_IMAGE_IMPLEMENTATION
#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1

#ifdef _WINDOWS
    #include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include <vector>
#include <ctime>
#include "cmath"

#include "Bird.hpp"
#include "Platform.hpp"

using namespace glm;

enum AppStatus { RUNNING, PAUSED, WON, LOST, TERMINATED };

struct GameState {
    Bird* bird;
    Platform* plat;
};

GameState g_state;

constexpr int   CD_QUAL_FREQ    = 44100,  // compact disk (CD) quality frequency
                AUDIO_CHAN_AMT  = 2,
                AUDIO_BUFF_SIZE = 4096;

constexpr int WINDOW_WIDTH  = 640 * 1.5,
              WINDOW_HEIGHT = 480 * 1.5;

const float ORTHO_WIDTH = 10.0f,
            ORTHO_HEIGHT = 7.5f;

constexpr float BG_RED     = 0.1922f,
                BG_GREEN   = 0.8059f,
                BG_BLUE    = 0.549f,
                BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
              VIEWPORT_Y = 0,
              VIEWPORT_WIDTH  = WINDOW_WIDTH,
              VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
               F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;
 
constexpr char  SPRITESHEET_FILEPATH[]  = "bird_spritesheet.png",
                FONTSHEET_FILEPATH[]    = "font1.png",
                PLATFORM_FILEPATH[]     = "platform_tile.png";

constexpr char BGM_FILEPATH[] = "music.mp3";
constexpr int   LOOP_FOREVER    = -1,
                PLAY_ONCE       = 0,
                NEXT_CHNL       = -1,
                MUTE_VOL        =  0,
                ALL_SFX_CHN     = -1;

Mix_Music *g_music;
Mix_Chunk *g_bouncing_sfx;

constexpr GLint NUMBER_OF_TEXTURES = 1,
                LEVEL_OF_DETAIL    = 0,
                TEXTURE_BORDER     = 0;

constexpr int SPRITESHEET_DIMENSIONS = 6;
constexpr int FRAMES_PER_SECOND = 6;
constexpr int LEFT  = 0,
              RIGHT = 1,
              UP    = 2,
              DOWN  = 3;

constexpr float FIXED_TIMESTEP = 1.0f / 30.0f;
float g_time_accumulator = 0.0f;

constexpr int FONTBANK_SIZE = 16;

GLuint g_font_texture_id;

float g_player_speed = 1.0f;  // move 1 unit per second

int *g_animation_indices; 
int g_animation_frames   = SPRITESHEET_DIMENSIONS;
int g_animation_index    = 0;

float g_animation_time = 0.0f;


SDL_Window* g_display_window = nullptr;
AppStatus g_app_status = PAUSED;
//GameStatus g_game_status = PAUSED;

ShaderProgram g_shader_program = ShaderProgram();

mat4    g_view_matrix,
        g_projection_matrix;

float g_previous_ticks = 0.0f;

//Bird george(vec3(-2.5f, 0.0f, 0.0f));
//Bird george2(vec3(2.0f, 0.0f, 0.0f));
//Platform plat(vec3(3.0f, -1.0f, 0.0f), vec3(1.0f, 0.25f, 0.0f));

void initialise();
void process_input();
void update();
void render();
void shutdown();

GLuint load_texture(const char* filepath);
void draw_object(mat4 &object_model_matrix, GLuint &object_texture_id);

void draw_text(ShaderProgram *shader_program, GLuint font_texture_id, std::string text,
               float font_size, float spacing, vec3 position);

int main(int argc, char* argv[]) {
    initialise();

    while (g_app_status != TERMINATED)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}

void draw_text(ShaderProgram *shader_program, GLuint font_texture_id, std::string text,
               float font_size, float spacing, vec3 position) {
    // Scale the size of the fontbank in the UV-plane
    // We will use this for spacing and positioning
    float width = 1.0f / FONTBANK_SIZE;
    float height = 1.0f / FONTBANK_SIZE;

    // Instead of having a single pair of arrays, we'll have a series of pairs—one for
    // each character. Don't forget to include <vector>!
    std::vector<float> vertices;
    std::vector<float> texture_coordinates;

    // For every character...
    for (int i = 0; i < text.size(); i++) {
        // 1. Get their index in the spritesheet, as well as their offset (i.e. their
        //    position relative to the whole sentence)
        int spritesheet_index = (int) text[i];  // ascii value of character
        float offset = (font_size + spacing) * i;

        // 2. Using the spritesheet index, we can calculate our U- and V-coordinates
        float u_coordinate = (float) (spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
        float v_coordinate = (float) (spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;

        // 3. Inset the current pair in both vectors
        vertices.insert(vertices.end(), {
            offset + (-0.5f * font_size), 0.5f * font_size,
            offset + (-0.5f * font_size), -0.5f * font_size,
            offset + (0.5f * font_size), 0.5f * font_size,
            offset + (0.5f * font_size), -0.5f * font_size,
            offset + (0.5f * font_size), 0.5f * font_size,
            offset + (-0.5f * font_size), -0.5f * font_size,
        });

        texture_coordinates.insert(texture_coordinates.end(), {
            u_coordinate, v_coordinate,
            u_coordinate, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate + width, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate, v_coordinate + height,
        });
    }

    // 4. And render all of them using the pairs
    mat4 model_matrix = mat4(1.0f);
    model_matrix = translate(model_matrix, position);

    shader_program->SetModelMatrix(model_matrix);
    glUseProgram(shader_program->programID);

    glVertexAttribPointer(shader_program->positionAttribute, 2, GL_FLOAT, false, 0,
                          vertices.data());
    glEnableVertexAttribArray(shader_program->positionAttribute);

    glVertexAttribPointer(shader_program->texCoordAttribute, 2, GL_FLOAT,
                          false, 0, texture_coordinates.data());
    glEnableVertexAttribArray(shader_program->texCoordAttribute);

    glBindTexture(GL_TEXTURE_2D, font_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, (int) (text.size() * 6));

    glDisableVertexAttribArray(shader_program->positionAttribute);
    glDisableVertexAttribArray(shader_program->texCoordAttribute);
}


GLuint load_texture(const char* filepath) {
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components,
                                     STBI_rgb_alpha);

    if (not image) {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER,
                 GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(image);

    return textureID;
}


void initialise() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    
    g_display_window = SDL_CreateWindow("Bird Lander",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

    if (g_display_window == nullptr) {
        std::cerr << "Error: SDL window could not be created.\n";
        shutdown();
    }

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.Load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix       = mat4(1.0f);
    g_projection_matrix = ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.SetProjectionMatrix(g_projection_matrix);
    g_shader_program.SetViewMatrix(g_view_matrix);

    glUseProgram(g_shader_program.programID);
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    g_state.bird = new Bird(vec3(-2.5f, 0.0f, 0.0f));
    g_state.plat = new Platform(vec3(3.0f, -1.0f, 0.0f),
                                vec3(1.0f, 0.25f, 0.0f));
    
    g_state.bird->set_texture(load_texture(SPRITESHEET_FILEPATH));
    g_state.bird->init_anim();
    
    g_state.plat->set_texture(load_texture(PLATFORM_FILEPATH));

    g_font_texture_id = load_texture(FONTSHEET_FILEPATH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void process_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE: g_app_status = TERMINATED; break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q: g_app_status = TERMINATED; break;
                    default: break;
                }
            default: break;
        }
    }
    
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    
    if (g_app_status == PAUSED) {
        if(key_state[SDL_SCANCODE_SPACE]) g_app_status = RUNNING;
    }
    else if (g_app_status == RUNNING) {
        g_state.bird->reset_flapping();
        if (key_state[SDL_SCANCODE_SPACE]) g_state.bird->process_flapping();
        
        if (key_state[SDL_SCANCODE_LEFT]) g_state.bird->tilt(1);
        else if (key_state[SDL_SCANCODE_RIGHT]) g_state.bird->tilt(-1);
    }
//    else if (g_app_status == WON) {
//        if(key_state[SDL_SCANCODE_SPACE])
//            
//    }
}


void update() {
    /* DELTA TIME */
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_time_accumulator;
    
    if (delta_time < FIXED_TIMESTEP) {
        g_time_accumulator = delta_time;
        return;
    }
    while (delta_time >= FIXED_TIMESTEP) {
        if (g_app_status == RUNNING) {
            /* ANIMATION */
            if (g_state.bird->flap()){
                g_state.bird->add_anim_time(delta_time);
                float seconds_per_frame = (float) 1 / FRAMES_PER_SECOND;
                
                if (g_state.bird->get_anim_time() >= seconds_per_frame) {
                    g_state.bird->set_anim_time(0.0f);
                    g_state.bird->anim_iterate();
                    
                    if(g_state.bird->get_anim_index() >= g_state.bird->get_anim_frames())
                        g_state.bird->set_anim_index(0);
                }
            }
            
            // check border collisions
            if (ORTHO_HEIGHT / 2 - fabs(g_state.bird->get_pos().y) - g_state.bird->get_scale().y / 2 <= 0) {
                g_state.bird->stop_movement();
                g_app_status = LOST;
            }
            if (ORTHO_WIDTH / 2 - fabs(g_state.bird->get_pos().x) - g_state.bird->get_scale().x / 2 <= 0) {
                g_state.bird->stop_movement();
                g_app_status = LOST;
            }
            
            // check platform collisions
            vec2 scale_buffer = vec2((g_state.bird->get_scale().x + g_state.plat->get_scale().x),
                                     g_state.bird->get_scale().y + g_state.plat->get_scale().y);
            vec2 distance = vec2(fabs(g_state.bird->get_pos().x - g_state.plat->get_pos().x),
                                 fabs(g_state.bird->get_pos().y - g_state.plat->get_pos().y));
            distance -= scale_buffer / 2.0f;
            
            LOG(distance.x << " " << distance.y);
            
            if (distance.x < 0 and distance.y < 0) {
                g_state.bird->stop_movement();
                if (g_state.bird->get_pos().y < g_state.plat->get_pos().y)
                    g_app_status = LOST;
                else g_app_status = WON;
            }
            
            g_state.bird->update(FIXED_TIMESTEP);
        }
        g_state.plat->update();
        delta_time -= FIXED_TIMESTEP;
    }
    g_time_accumulator = delta_time;
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    g_state.bird->render(&g_shader_program);
    g_state.plat->render(&g_shader_program);

    if (g_app_status == PAUSED) {
        draw_text(&g_shader_program, g_font_texture_id, "Don't touch the Border!",
                  0.3f, 0.03f, vec3(-3.5f, 2.0f, 0.0f));
        draw_text(&g_shader_program, g_font_texture_id, "Hit Space to Start!",
                  0.3f, 0.03f, vec3(-3.35f, 1.6f, 0.0f));
    } else if (g_app_status == WON) {
        draw_text(&g_shader_program, g_font_texture_id, "You won!", 0.3f, 0.03f,
                  vec3(-1.75f, 2.0f, 0.0f));
    } else if (g_app_status == LOST) {
        draw_text(&g_shader_program, g_font_texture_id, "You lost :(", 0.3f, 0.03f,
                  vec3(-2.5f, 2.0f, 0.0f));
    }

    SDL_GL_SwapWindow(g_display_window);
}


void shutdown() {
    Mix_FreeChunk(g_bouncing_sfx);
    Mix_FreeMusic(g_music);
    SDL_Quit();
}


