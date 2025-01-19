// Scene.c++
#include "Scene.hpp"

Scene::Scene() {
    g_map_texture_id = Utility::load_texture(MAP_TILESET_FILEPATH);
    g_font_texture_id = Utility::load_texture(FONTSHEET_FILEPATH);
    g_sprite_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);
}
