#pragma once

#include <random>

#include "common.h"

class Map;

using namespace std;

class Tile {
private:

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
    void initialize_tile_values();

public:

    // Constructors
    Tile(char _id = ' ', Coords bul_vect = { 0,0 }, bool by_player = false);

    // Getters and setters
    void set_health(char value);
    void set_score(char value);
    char get_id() const;
    char get_health() const;
    char get_score() const;
    bool was_shot_by_player() const;

    // Property methods
    bool heal_by_one(char max_health);
    bool damage_by_one();
    void mark_as_acted(char cooldown);
    bool can_act_now() const;
    unsigned char is_dmg_visible() const; // 0:invisible, 1:red, 2:green
    void action_decrement();
    void dmg_show_decrement();
    bool check_initialization();
    Coords get_bullet_movement() const;
    void execute_behaviour();

};
