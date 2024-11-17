#pragma once

#include <vector>
#include <random>

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
    bool should_reroll = true;

    // Bullet properties
    Coords movement = { 0,0 };
    bool shot_by_player = false;
};

struct EntitySmell {
    char id = '\0';
    int smell = 15;
};

void draw_pathmap(Map* map, Coords player, vector<EntitySmell> smells, string mode);
Coords pathfind(Map* map, Coords player, Coords entity, mt19937& ms_twister, string mode);
