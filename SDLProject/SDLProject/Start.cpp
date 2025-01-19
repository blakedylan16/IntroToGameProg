#include "Start.hpp"
#include "Utility.hpp"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

unsigned int START_LEVEL_DATA[] = {
    121, 122, 142, 142, 142, 142, 142, 142, 142, 142, 142, 142, 122, 123,
    121, 123, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   121, 123,
    121, 123, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   121, 123,
    121, 123, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   121, 123,
    121, 123, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   121, 123,
    121, 123, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   121, 123,
    121, 122,  22,  22,  22,  22,  22,  22,  22,  22, 22,  22,  122, 123,
    121, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 123
};

Start::~Start() {
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void Start::initialise() {
    
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, START_LEVEL_DATA, g_map_texture_id, 1.0f, 20, 9);
    
    int player_sprite_index = 0;
    std::vector<int> player_walking_anim = { player_sprite_index, player_sprite_index + 1 };
    // coded as an enemy for the purpose of Start Screen
    m_game_state.enemies.push_back(new Entity(g_sprite_texture_id,
                                      2.0f,       // speed
                                      GRAVITY,    // acceleration
                                      4.0f,       // jumping power
                                      player_walking_anim,
                                      1.0f,        // size
                                      ENEMY, WALKER, IDLE));
    m_game_state.enemies[0]->update(m_game_state.map, 0.0f);
    m_game_state.enemies[0]->set_pos(glm::vec3(2.0f, 0.0f, 0.0f));
    m_game_state.player = m_game_state.enemies[0];


    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
//    m_game_state.bgm = Mix_LoadMUS(BGM_FILEPATH);
//    Mix_PlayMusic(m_game_state.bgm, -1);
//    Mix_VolumeMusic(0.0f);
    
    m_game_state.jump_sfx = Mix_LoadWAV(JUMP_SFX_FILEPATH);
}

void Start::update(float delta_time) {
    m_game_state.enemies[0]->update(m_game_state.map, delta_time, m_game_state.player);
}


void Start::render(ShaderProgram *g_shader_program) {
    m_game_state.map->render(g_shader_program);
    m_game_state.enemies[0]->render(g_shader_program);
    
    Utility::draw_text(g_shader_program, g_font_texture_id, "Green Alien Game",
                      0.35f, 0.001f, vec3(2.8f, -2.9f, 0.0f));
    Utility::draw_text(g_shader_program, g_font_texture_id, "Hit Enter to Start!",
                      0.35f, 0.001f, vec3(2.6f, -2.4f, 0.0f));
}
