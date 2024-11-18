#pragma once

#include <iostream>
#include <vector>
#include <random>

#include "common.h"

struct DisplayData;
struct Tile;

using namespace std;

class Map {
private:

    // Private objects / pointers
    Tile** tiles;
    vector<Coords> entities;
    std::mt19937 ms_twister;

    // Private properties
    unsigned int public_seed = 0;
    int frame = 0;
    int score = 0;

    // Private methods
    void spawn(int x, int y, char type, bool score_rich = true);
    void spawn_around(int x, int y, char type, const int SIDE_CHANCE, bool score_rich = false);
    void spawn_bullet(int x, int y, int dx, int dy, bool by_player = false);
    void remove(int x, int y);
    bool damage(int x, int y, bool dmg_by_player = false); // returns true if tile was killed
    void entity_move(int x, int y);
    bool try_move(int x, int y, int dx, int dy, string mode = "");
    void passive_movement(int x, int y);

public:

    // Constructor & destructor
    Map(unsigned int seed = 0);
    ~Map();

    // Public methods
    void frame_update();
    int end();

    // Read-only public methods
    unsigned int get_seed();
    char get_tile(int x, int y);
    char get_tile_display(int x, int y);
    char get_tile_color_modifier(int x, int y);

};
