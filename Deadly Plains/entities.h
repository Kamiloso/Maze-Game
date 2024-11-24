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
    bool magical = false;

    // Private methods
    void initialize_tile_values();
    bool check_initialization();
    void dmg_show_decrement();
    void mark_as_acted(char cooldown);
    bool can_act_now() const;
    void action_decrement();

    // Entity action methods
    void spawn_insects(Map* map, mt19937& ms_twister, bool mag, int x, int y);
    void spawner_activate(Map* map, mt19937& ms_twister, bool mag, int x, int y);

public:

    // Constructors
    Tile(char _id = ' ', Coords bul_vect = { 0,0 }, bool by_player = false);

    // Getters and setters
    void set_health(char value);
    void set_score(char value);
    void make_magical();
    char get_id() const;
    char get_health() const;
    char get_score() const;
    bool was_shot_by_player() const;
    bool is_magical() const;
    Coords get_bullet_movement() const;
    unsigned char is_dmg_visible() const; // 0:invisible, 1:damaged, 2:healed

    // Public methods
    bool heal_by_one(char max_health);
    bool damage_by_one();
    void execute_behaviour(Map* map, mt19937& ms_twister, int x, int y);
    void on_kill(Map* map, mt19937& ms_twister, char pre_ch, bool pre_mag, int x, int y); // BE VERY CAREFUL WITH IT

};
