#include "Level1.hpp"
#include "Utility.hpp"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

constexpr char  SPRITESHEET_FILEPATH[]  = "tilemap-characters_packed.png",
                FONTSHEET_FILEPATH[]    = "font1.png",
                MAP_TILESET_FILEPATH[]  = "tilemap_packed.png",
                BGM_FILEPATH[]          = "Sergio_music.mp3",
                JUMP_SFX_FILEPATH[]     = "jump.wav";

unsigned int LEVEL_DATA[] = {
    121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    121, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    121, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
    121, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

Level1::~Level1() {
    for (Entity* enemy : m_game_state.enemies)
        delete enemy;
    m_game_state.enemies.clear();
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void Level1::initialise()
{
    GLuint map_texture_id = Utility::load_texture(MAP_TILESET_FILEPATH);
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA, map_texture_id, 1.0f, 4, 1);
    
    GLuint sprite_tex_id = Utility::load_texture(SPRITESHEET_FILEPATH);

    int player_sprite_index = 0; // which sprite I'm using. specific to tilemap
    std::vector<int> player_walking_anim = { player_sprite_index, player_sprite_index + 1 };

    glm::vec3 acceleration = glm::vec3(0.0f,-4.905f, 0.0f);

    m_game_state.player = new Entity(sprite_tex_id,
                                     4.0f,       // speed
                                     GRAVITY,    // acceleration
                                     7.5f,       // jumping power
                                     player_walking_anim,
                                     .5f,       // size
                                     PLAYER);
    m_game_state.player->update(m_game_state.map, 0.0f);
    m_game_state.player->set_pos(glm::vec3(5.0f, 0.0f, 0.0f));
    
    int enemy_sprite_index = 21;
    std::vector<int> enemy_walking_anim = { enemy_sprite_index, enemy_sprite_index + 1 };
    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_game_state.enemies.push_back(new Entity(sprite_tex_id,
                                                  1.0f,       // speed
                                                  GRAVITY,    // acceleration
                                                  3.0f,       // jumping power
                                                  enemy_walking_anim,
                                                  .75,        // size
                                                  ENEMY, WALKER, IDLE));
        m_game_state.enemies[i]->update(m_game_state.map);
    }
    m_game_state.enemies[0]->set_pos(glm::vec3(8.0f, 0.0f, 0.0f));
    m_game_state.enemies[0]->update(m_game_state.map);


    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_game_state.bgm = Mix_LoadMUS(BGM_FILEPATH);
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(0.0f);
    
    m_game_state.jump_sfx = Mix_LoadWAV(JUMP_SFX_FILEPATH);
}

void Level1::update(float delta_time)
{
    m_game_state.player->update(m_game_state.map, delta_time, nullptr,
                                m_game_state.enemies, ENEMY_COUNT);
    
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (m_game_state.enemies[i]->get_active_state())
            m_game_state.enemies[i]->update(m_game_state.map, delta_time, m_game_state.player);
    }
}


void Level1::render(ShaderProgram *g_shader_program)
{
    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
    for (int i = 0; i < m_number_of_enemies; i++)
            m_game_state.enemies[i]->render(g_shader_program);
}
