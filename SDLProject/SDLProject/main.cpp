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

#include "Map.hpp"
#include "Entity.hpp"
#include "Utility.hpp"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 5
#define FIXED_TIMESTEP 0.0166666f
#define ENEMY_COUNT 3



using namespace glm;

/* ----- GAME STATE ----- */
enum AppStatus { RUNNING, PAUSED, WON, LOST, TERMINATED };

struct GameState {
    Entity *player;
    std::vector<Entity*> enemies;
        
    Map *map;
        
    Mix_Music *bgm;
    Mix_Chunk *jump_sfx;
};


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

const vec3 GRAVITY = vec3(0.0f, -10.0f, 0.0f);

unsigned int LEVEL_DATA[] = {
    20,   0,   0,   0,   0, 0, 0,   0,   0,   0,   0,   0,   0,   0,
    120,   0,   0,   0,   0, 0, 0,   0,   0,   0,   0,   0,   0,   0,
    121,  23,  0,   0,   0, 0, 0,   0,   0,   21,  22,  22,  22,  23,
    121, 122, 22,  23,  0, 0, 21,  22,  22,  122, 122, 122, 122, 123,
    121, 122, 122, 123, 0, 0, 121, 122, 122, 122, 122, 122, 122, 123
};


/* ----- VARIABLES ----- */

GameState g_state;

SDL_Window* g_display_window = nullptr;
AppStatus g_app_status = PAUSED;

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

void draw_object(mat4 &object_model_matrix, GLuint &object_texture_id);

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

void initialise() {
    /* ----- GENERAL SET-UP ----- */
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
    
    
    /* ----- VIDEO SET-UP ----- */
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.Load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix       = mat4(1.0f);
    g_projection_matrix = ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.SetProjectionMatrix(g_projection_matrix);
    g_shader_program.SetViewMatrix(g_view_matrix);

    glUseProgram(g_shader_program.programID);
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    
    /* ----- AUDIO SET-UP ----- */
    // Start Audio
    Mix_OpenAudio (
        CD_QUAL_FREQ,        // the frequency to playback audio at (in Hz)
        MIX_DEFAULT_FORMAT,  // audio format
        AUDIO_CHAN_AMT,      // number of channels (1 is mono, 2 is stereo, etc).
        AUDIO_BUFF_SIZE      // audio buffer size in sample FRAMES (total samples divided by channel count)
    );
    
    // Similar to our custom function load_texture
    g_state.bgm = Mix_LoadMUS(BGM_FILEPATH);

    // This will schedule the music object to begin mixing for playback.
    // The first parameter is the pointer to the mp3 we loaded
    // and second parameter is the number of times to loop.
    Mix_PlayMusic(g_state.bgm, LOOP_FOREVER);

    // Set the music to half volume
    // MIX_MAX_VOLUME is a pre-defined constant
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4.0f);
    
    g_state.jump_sfx = Mix_LoadWAV(JUMP_SFX_FILEPATH);
    
    
//    Mix_Volume(ALL_SFX_CHN, MIX_MAX_VOLUME / 4);
    
    
    /* ----- MAP SET-UP ----- */
    GLuint map_texture_id = Utility::load_texture(MAP_TILESET_FILEPATH);
    g_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA,
                          map_texture_id, 1.0f, 20, 9);
    
    /* ----- PLAYER SET-UP ----- */
    GLuint sprite_tex_id = Utility::load_texture(SPRITESHEET_FILEPATH);
    
    int player_sprite_index = 0; // which sprite I'm using. specific to tilemap
    std::vector<int> player_walking_anim = { player_sprite_index, player_sprite_index + 1 };
    LOG("player walking index at init: " << player_walking_anim[0]);
    g_state.player = new Entity(sprite_tex_id,
                                4.0f,       // speed
                                GRAVITY,    // acceleration
                                7.5f,       // jumping power
                                player_walking_anim,
                                .5f,       // size
                                PLAYER);
    g_state.player->update(g_state.map, 0.0f);
    g_state.player->set_pos(vec3(1.0f, 0.0f, 0.0f));
    
    /* ----- ENEMY SET-UP ----- */
    int enemy_sprite_index = 21;
    std::vector<int> enemy_walking_anim = { enemy_sprite_index, enemy_sprite_index + 1 };
    LOG("enemy walking index at init: " << enemy_walking_anim[0]);
    std::vector<AIType> ai_types = { GUARD, WALKER, JUMPER };
    for (int i = 0; i < ENEMY_COUNT; i++) {
        g_state.enemies.push_back(new Entity(sprite_tex_id,
                                    1.0f,       // speed
                                    GRAVITY,    // acceleration
                                    3.0f,       // jumping power
                                    enemy_walking_anim,
                                    .75,        // size
                                    ENEMY, ai_types[i], IDLE));
        g_state.enemies[i]->set_pos(vec3((i + 1) * 3.0f, 0.0f, 0.0f));
        g_state.enemies[i]->update(g_state.map);
    }
    
    g_font_texture_id = Utility::load_texture(FONTSHEET_FILEPATH);

    /* ----- BLENDING ----- */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input() {
    
    g_state.player->set_mov(vec3(0.0f));
    
//    for (int i = 0; i < ENEMY_COUNT; i++)
//        g_state.enemies[i].set_mov(vec3(0.0f));
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE: g_app_status = TERMINATED; break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q: g_app_status = TERMINATED; break;
                        
                    case SDLK_SPACE:
                        if (g_app_status == PAUSED) g_app_status = RUNNING;
                        else if (g_app_status == RUNNING) g_app_status = PAUSED;
                        break;
                    case SDLK_w:
                        if (g_app_status == RUNNING) {
                            g_state.player->jump();
                            Mix_PlayChannel(NEXT_CHNL, g_state.jump_sfx, PLAY_ONCE);
                        }
                    default: break;
                }
            default: break;
        }
    }
    
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    
    if (key_state[SDL_SCANCODE_A])          g_state.player->move_left();
    else if (key_state[SDL_SCANCODE_D])     g_state.player->move_right();
    
    if (length(g_state.player->get_mov()) > 1.0f) g_state.player->normalize_movement();
    
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
            if (not g_state.player->get_active_state()) {
                g_app_status = LOST;
                return;
            }
            if (g_state.enemies.size() <= 0) {
                g_app_status = WON;
                return;
            }
            g_state.player->update(g_state.map, FIXED_TIMESTEP, nullptr,
                                   g_state.enemies, ENEMY_COUNT);
            for (int i = 0; i < g_state.enemies.size(); i++) {
                if (g_state.enemies[i]->get_active_state())
                    g_state.enemies[i]->update(g_state.map, FIXED_TIMESTEP, g_state.player);
            }
                
        }
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_time_accumulator = delta_time;
    
    g_view_matrix = mat4(1.0f);
    g_view_matrix = translate(g_view_matrix,
                              vec3(-g_state.player->get_pos().x, 0.0f, 0.0f));
    
    int dead_enemies = 0;
    for (int i = 0; i < g_state.enemies.size(); i++)
        if (not g_state.enemies[i]->get_active_state())
            dead_enemies++;
    if (dead_enemies == ENEMY_COUNT) g_app_status = WON;
    
}

void render() {
    g_shader_program.SetViewMatrix(g_view_matrix);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    g_state.map->render(&g_shader_program);
        
    g_state.player->render(&g_shader_program);
    
    for (int i = 0; i < ENEMY_COUNT; i++)
        g_state.enemies[i]->render(&g_shader_program);
    float curr_pos_x = g_state.player->get_pos().x;

    if (g_app_status == PAUSED) {
        Utility::draw_text(&g_shader_program, g_font_texture_id, "Hop on the enemies to win!",
                  0.3f, 0.03f, vec3(curr_pos_x - 2, 2.0f, 0.0f));
        Utility::draw_text(&g_shader_program, g_font_texture_id, "Hit Space to Start!",
                  0.3f, 0.03f, vec3(curr_pos_x - 1.5, 1.6f, 0.0f));
    } else if (g_app_status == WON) {
        Utility::draw_text(&g_shader_program, g_font_texture_id, "You won!", 0.3f, 0.03f,
                  vec3(curr_pos_x - 2, 2.0f, 0.0f));
    } else if (g_app_status == LOST) {
        Utility::draw_text(&g_shader_program, g_font_texture_id, "You lost :(", 0.3f, 0.03f,
                  vec3(curr_pos_x - 2, 2.0f, 0.0f));
    }

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() {
    
    SDL_Quit();
    
    for (Entity* enemy : g_state.enemies)
        delete enemy;
    g_state.enemies.clear();

    delete    g_state.player;
    delete    g_state.map;
    
    Mix_FreeChunk(g_state.jump_sfx);
    Mix_FreeMusic(g_state.bgm);
}
