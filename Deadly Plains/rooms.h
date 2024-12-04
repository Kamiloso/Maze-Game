#pragma once

#include <random>

class Map;

using namespace std;

// Build empty placeholder
void empty_room(Map* map, int X, int Y, mt19937& ms_twister);

// Build rooms
void animal_room(Map* map, int X, int Y, mt19937& ms_twister);
void monster_room(Map* map, int X, int Y, mt19937& ms_twister);
void square_room(Map* map, int X, int Y, mt19937& ms_twister);

// Build corridors
void fruit_corridor(Map* map, int X, int Y, mt19937& ms_twister);
void normal_corridor(Map* map, int X, int Y, mt19937& ms_twister);

// Build temples
void spawner_temple(Map* map, int X, int Y, mt19937& ms_twister);
void block_temple(Map* map, int X, int Y, mt19937& ms_twister);
