#include "Level2.hpp"
#include "Utility.hpp"

#define LOG(argument) std::cout << argument << '\n'

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

unsigned int Level2_DATA[] = {
    123, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    123, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    123, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    123, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    123, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    121, 23,  0,   0,   0, 0, 0,   0,   0,   21,  22,  22,  22,  23,
    121, 122, 22,  23,  0, 0, 21,  22,  22,  122, 122, 122, 122, 123,
    121, 122, 122, 123, 0, 0, 121, 122, 122, 122, 122, 122, 122, 123
};

Level2::~Level2() {
    for (Entity* enemy : m_game_state.enemies)
        delete enemy;
    m_game_state.enemies.clear();
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void Level2::initialise() {
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, Level2_DATA, g_map_texture_id, 1.0f, 20, 9);

    int player_sprite_index = 0; // which sprite I'm using. specific to tilemap
    std::vector<int> player_walking_anim = { player_sprite_index, player_sprite_index + 1 };

    m_game_state.player = new Entity(g_sprite_texture_id,
                                     4.0f,       // speed
                                     GRAVITY,    // acceleration
                                     4.0f,       // jumping power
                                     player_walking_anim,
                                     .5f,       // size
                                     PLAYER);
    m_game_state.player->update(m_game_state.map, 0.0f);
    m_game_state.player->set_pos(glm::vec3(1.0f, -4.0f, 0.0f));
    
    int enemy_sprite_index = 21;
    std::vector<int> enemy_walking_anim = { enemy_sprite_index, enemy_sprite_index + 1 };
    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_game_state.enemies.push_back(new Entity(g_sprite_texture_id,
                                                  1.0f,       // speed
                                                  GRAVITY,    // acceleration
                                                  3.0f,       // jumping power
                                                  enemy_walking_anim,
                                                  .75,        // size
                                                  ENEMY, WALKER, IDLE));
        m_game_state.enemies[i]->update(m_game_state.map);
    }
    m_game_state.enemies[0]->set_pos(glm::vec3(8.0f, -4.0f, 0.0f));
    m_game_state.enemies[0]->update(m_game_state.map);


    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
//    m_game_state.bgm = Mix_LoadMUS(BGM_FILEPATH);
//    Mix_PlayMusic(m_game_state.bgm, -1);
//    Mix_VolumeMusic(0.0f);
    
    m_game_state.jump_sfx = Mix_LoadWAV(JUMP_SFX_FILEPATH);
}

void Level2::update(float delta_time) {
    
    m_game_state.player->update(m_game_state.map, delta_time, nullptr,
                                m_game_state.enemies, ENEMY_COUNT);
    
    if (not m_game_state.player->get_active_state() and *g_lives > 0) {
        m_game_state.player->reset(m_game_state.map, vec3(1.0f, -4.0f, 0.0f));
        m_game_state.enemies[0]->reset(m_game_state.map, vec3(8.0f, -5.0f, 0.0f));
        (*g_lives) --;
    }
    
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (m_game_state.enemies[i]->get_active_state())
            m_game_state.enemies[i]->update(m_game_state.map, delta_time, m_game_state.player);
    }
    
}


void Level2::render(ShaderProgram *g_shader_program)
{
    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
    for (int i = 0; i < m_number_of_enemies; i++)
            m_game_state.enemies[i]->render(g_shader_program);
    
}
