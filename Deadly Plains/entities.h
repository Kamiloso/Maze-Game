#pragma once

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
    bool dmg_show_from_dmg = false;
    bool should_reroll = true;

    // Bullet properties
    Coords movement = { 0,0 };
    bool shot_by_player = false;
};

class Tile2 {
private:

    // Constructors
    Tile2(char _id = ' ', Coords bul_vect = { 0,0 }, bool by_player = false);
    Tile2(char _id, char hard_set_health);

    // Private properties
    char id = ' ';
    char health = -1;
    char reward_score = 0;
    char action_cooldown = 0;
    char dmg_show_time = 0;
    bool dmg_show_from_dmg = false;
    bool initialization_flag = true;
    char bullet_movement = '\0';
    bool shot_by_player = false;

    // Private methods
    void initialize_tile_values(bool dry);

public:

    // Property methods
    char get_health() const;
    char get_score() const;
    bool heal_by_one(char max_health);
    bool damage_by_one();
    void mark_as_acted(char cooldown);
    bool can_act_now() const;
    void action_decrement();
    void dmg_show_decrement();
    bool check_initialization();
    Coords get_bullet_movement() const;

    // Behaviour methods
    // entity_move();
    // spawn_reinforcements();

};
