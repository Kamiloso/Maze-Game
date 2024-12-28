#pragma once

#include <iostream>
#include <vector>
#include <random>

#include "common.h"
#include "pathfinding.h"
#include "entities.h"
#include "spawning.h"
#include "difficulty.h"

struct DisplayData;
struct ConsoleChar;

using namespace std;

struct KillDelayed {
    char id = ' ';
    bool mag = false;
    Coords coords = { 0,0 };
};

class Map {
private:

    // Private objects / pointers
    Tile** tiles;
    vector<Coords> entities;
    std::mt19937 ms_twister;
    Pathfinding pathfinding;
    Spawning spawning;
    vector<KillDelayed> delayed_kills;
    Difficulty current_difficulty;

    // Private properties
    unsigned int public_seed = 0;
    int frame = 0;
    int score = 0;

    // Private methods
    void entity_move(int x, int y);
    void frame_display();

public:

    // Constructor & destructor
    Map(unsigned int seed = 0);
    ~Map();

    // Public methods
    void frame_update();
    int get_frame() const;
    unsigned int get_seed() const;
    ConsoleChar get_tile_display(int x, int y) const;
    int end() const;

    // Map tile API
    char get_tile(int x, int y) const;
    int enclosed_by(int x, int y, char by_what) const;
    Tile& get_tile_ref(int x, int y) const;
    Tile& spawn(int x, int y, char type, bool has_ai = true, bool magical = false);
    void spawn_bullet(int x, int y, int dx, int dy, bool by_player, bool magical = false);
    bool damage(int x, int y, bool dmg_by_player = false); // returns true if tile was killed
    void remove(int x, int y, bool despawn_mode = false);
    bool try_move(int x, int y, int dx, int dy, string mode = "");
    void passive_movement(int x, int y);

    // Friend declarations
    friend void Tile::execute_behaviour(Map* map, mt19937& ms_twister, int x, int y);

};
