#pragma once

#include <vector>
#include <random>

#include "common.h"

class Map;

using namespace std;

struct EntitySmell {
    char id = '\0';
    int smell = 15;
};

struct TileList {
    Coords position = { 0, 0 };
    int smell = -1;
};

class Pathfinding {
private:

    // Private properties
    int simulation_space[5][PATHFIND_TABLE_SIZE][PATHFIND_TABLE_SIZE];

public:

    // Public methods
    void draw_pathmap(Map* map, Coords player, vector<EntitySmell> smells, string mode);
    Coords pathfind(Coords player, Coords entity, mt19937& ms_twister, string mode) const;
    Coords get_sniper_direction(Coords player, Coords entity, mt19937& ms_twister, string mode) const;
    Coords pathfind_distance(Coords player, Coords entity, mt19937& ms_twister, string mode) const;
    int sniper_weight(Coords player, Coords point, int dx, int dy, string mode) const;
    vector<TileList> detect_tiles(Coords player, mt19937& ms_twister, string mode, int max_smell, bool randomize) const;
};
