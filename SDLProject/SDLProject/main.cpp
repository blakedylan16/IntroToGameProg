///*
// Author: Dylan Blake
// Assignment: Platformer
// I pledge that I have completed this assignment without collaborating
// with anyone else, in conformance with the NYU School of Engineering
// Policies and Procedures on Academic Misconduct
//
// sprite from ...
// */


#define LOG(argument) std::cout << argument << '\n'
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
#include <vector>
#include <ctime>
#include "cmath"

#define NUMBER_OF_SCENES 4
#define LEFT_EDGE 5.0f

#include "Utility.hpp"
#include "Scene.hpp"
#include "Level1.hpp"
#include "Level2.hpp"
#include "Level3.hpp"
#include "Start.hpp"

#define FIXED_TIMESTEP 0.0166666f

using namespace glm;

/* ----- GAME STATE ----- */
enum AppStatus { RUNNING, PAUSED, WON, LOST, TERMINATED };

/* ----- CONSTANTS ----- */

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

constexpr float MILLISECONDS_IN_SECOND = 1000.0f;
 
constexpr char  SPRITESHEET_FILEPATH[]  = "tilemap-characters_packed.png",
                FONTSHEET_FILEPATH[]    = "font1.png",
                MAP_TILESET_FILEPATH[]  = "tilemap_packed.png",
                BGM_FILEPATH[]          = "Sergio_music.mp3",
                JUMP_SFX_FILEPATH[]     = "jump.wav";

constexpr int   LOOP_FOREVER    = -1,
                PLAY_ONCE       = 0,
                NEXT_CHNL       = -1,
                MUTE_VOL        =  0,
                ALL_SFX_CHN     = -1;

constexpr GLint NUMBER_OF_TEXTURES = 1,
                LEVEL_OF_DETAIL    = 0,
                TEXTURE_BORDER     = 0;

constexpr int FONTBANK_SIZE = 16;

constexpr int   CD_QUAL_FREQ    = 44100,  // compact disk (CD) quality frequency
                AUDIO_CHAN_AMT  = 2,
                AUDIO_BUFF_SIZE = 4096;

/* ----- VARIABLES ----- */

Scene   *g_current_scene;
Level1  *g_level_1;
Level2  *g_level_2;
Level3  *g_level_3;
Start   *g_start;

Scene *scenes[4];
int scene_index;
bool next_scene;

int *g_lives;

SDL_Window* g_display_window = nullptr;
AppStatus g_app_status = RUNNING;

ShaderProgram g_shader_program = ShaderProgram();

mat4    g_view_matrix,
        g_projection_matrix;

float g_time_accumulator = 0.0f;

GLuint g_font_texture_id;

float g_player_speed = 1.0f;  // move 1 unit per second

int *g_animation_indices;
int g_animation_frames;
int g_animation_index;

float g_animation_time = 0.0f;

float g_previous_ticks = 0.0f;


void initialise();
void process_input();
void update();
void render();
void shutdown();

void switch_to_scene(Scene *scene);

int main(int argc, char* argv[]) {
    initialise();

    while (g_app_status != TERMINATED) {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}

void initialise() {
    /* ----- GENERAL SET-UP ----- */
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    
    g_display_window = SDL_CreateWindow("Platformer",
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
    
    
    /* ----- VIDEO SET-UP ----- */
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.Load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix       = mat4(1.0f);
    g_projection_matrix = ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.SetProjectionMatrix(g_projection_matrix);
    g_shader_program.SetViewMatrix(g_view_matrix);

    glUseProgram(g_shader_program.programID);
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    g_font_texture_id = Utility::load_texture(FONTSHEET_FILEPATH);
    
    /* ----- SCENE SET-UP ----- */
    g_lives = new int;
    *g_lives = 3;
    g_level_1 = new Level1();
    g_level_2 = new Level2();
    g_level_3 = new Level3();
    g_start = new Start();
    scene_index = 0;
    scenes[0] = g_start;
    scenes[1] = g_level_1;
    scenes[2] = g_level_2;
    scenes[3] = g_level_3;
    switch_to_scene(scenes[scene_index]);
    
    /* ----- MUSIC SET-UP ----- */
    g_current_scene->m_game_state.bgm = Mix_LoadMUS(BGM_FILEPATH);
    Mix_PlayMusic(g_current_scene->m_game_state.bgm, LOOP_FOREVER);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4.0f);
    
    /* ----- BLENDING ----- */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input() {
    
    if (g_current_scene != g_start)
        g_current_scene->m_game_state.player->set_mov(vec3(0.0f));
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE: g_app_status = TERMINATED; break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                        g_app_status = TERMINATED;
                        break;
                    case SDLK_SPACE:
                        if (g_app_status == PAUSED) g_app_status = RUNNING;
                        else if (g_app_status == RUNNING) g_app_status = PAUSED;
                        break;
                    case SDLK_RETURN:
                        if (g_current_scene == g_start)
                            next_scene = true;
                    case SDLK_w:
                        if (g_current_scene != g_start){
                            if (g_current_scene->m_game_state.player->get_collided_bottom()) {
                                g_current_scene->m_game_state.player->jump();
                                Mix_PlayChannel(NEXT_CHNL,
                                                g_current_scene->m_game_state.jump_sfx,
                                                PLAY_ONCE);
                            }
                        }
                    default: break;
                }
            default: break;
        }
    }
    
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    
    if (g_current_scene != g_start){
        if (key_state[SDL_SCANCODE_A])      g_current_scene->m_game_state.player->move_left();
        else if (key_state[SDL_SCANCODE_D]) g_current_scene->m_game_state.player->move_right();
        
        if (length(g_current_scene->m_game_state.player->get_mov()) > 1.0f)
                g_current_scene->m_game_state.player->normalize_movement();
    }
    
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
        // Update whole scene
        if (g_app_status == RUNNING)
            g_current_scene->update(FIXED_TIMESTEP);
        
        if (g_current_scene->m_game_state.player->get_pos().y < -10.0f)
            g_current_scene->m_game_state.player->kill_off();
            
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_time_accumulator = delta_time;
    
    int enemy_count = 0;
    for (Entity *enemy : g_current_scene->m_game_state.enemies)
        if (enemy->get_active_state())
            enemy_count++;
    if (*g_lives <= 0) g_app_status = LOST;

    if (enemy_count == 0) next_scene = true;
    
    g_view_matrix = mat4(1.0f);
    if (g_current_scene != g_start and
        g_current_scene->m_game_state.player->get_pos().x > LEFT_EDGE) {
            g_view_matrix = translate(g_view_matrix,
                                      vec3(-g_current_scene->m_game_state.player->get_pos().x,
                                           3.75, 0));
    } else g_view_matrix = translate(g_view_matrix, vec3(-5, 3.75, 0));
    
    if (next_scene) {
        scene_index ++;
        if (scene_index > 3) {
            g_app_status = WON;
            return;
        } else {
            switch_to_scene(scenes[scene_index]);
            next_scene = false;
        }
        
    }
}

void render() {
    g_shader_program.SetViewMatrix(g_view_matrix);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    g_current_scene->render(&g_shader_program);
    
    float curr_pos_x;
    if (g_current_scene->m_game_state.player->get_pos().x > LEFT_EDGE)
        curr_pos_x = g_current_scene->m_game_state.player->get_pos().x;
    else curr_pos_x = 4;
    
    if (g_current_scene != g_start) {
        std::string lives_string = "Lives: " + std::to_string(*g_lives);
        Utility::draw_text(&g_shader_program, g_font_texture_id, lives_string,
                           0.3f, 0.0005f, vec3(1.0f, -.5f, 0.0f));
    }

    if (g_app_status == WON)
        Utility::draw_text(&g_shader_program, g_font_texture_id, "You won! :)", 0.3f, 0.03f,
                           vec3(curr_pos_x - 1.0f, -1.5f, 0.0f));
    else if (g_app_status == LOST)
        Utility::draw_text(&g_shader_program, g_font_texture_id, "You lost! :(", 0.3f, 0.03f,
                           vec3(curr_pos_x - 1.0f, -1.5f, 0.0f));

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() {
    
    SDL_Quit();
    
    delete g_level_1;
    delete g_level_2;
    delete g_level_3;
    delete g_lives;
    delete g_start;
    
    g_current_scene = nullptr;
}

void switch_to_scene(Scene *scene) {
    g_current_scene = scene;
    g_current_scene->initialise();
    g_current_scene->set_lives(g_lives);
}
