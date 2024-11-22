#pragma once

#include "common.h"

class Map;

using namespace std;

struct Tile {
    char id = ' ';
    Tile(char _id = ' ');

    // Tile properties
    int health = -1;
    int has_score = 0;
    int action_cooldown = 0;
    int dmg_show_time = 0;
    bool dmg_show_from_dmg = false;
    bool should_reroll = true;

    // Bullet properties
    Coords movement = { 0,0 };
    bool shot_by_player = false;
};
