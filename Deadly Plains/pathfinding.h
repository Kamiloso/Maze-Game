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

class Pathfinding {
private:

    // Private properties
    int simulation_space[3][PATHFIND_TABLE_SIZE][PATHFIND_TABLE_SIZE];

public:

    // Public methods
    void draw_pathmap(Map* map, Coords player, vector<EntitySmell> smells, string mode);
    Coords pathfind(Coords player, Coords entity, mt19937& ms_twister, string mode) const;
};
