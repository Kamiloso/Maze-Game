#pragma once

#include <string>
#include <vector>
#include <random>

#include "difficulty.h"
#include "common.h"

using namespace std;

class Map;
class Pathfinding;
class Difficulty;

class Spawning
{
public:
	Spawning(Map* _map = nullptr, Pathfinding* _pathf_ptr = nullptr, mt19937* _ms_twister_ptr = nullptr);
	void frame_spawn(Coords player, Difficulty difficulty);

private:
	Map* map;
	Pathfinding* pathf_ptr;
	mt19937* ms_twister_ptr;
};

vector<Difficulty> get_difficulties();
Difficulty get_difficulty_by_id(int id);
Difficulty get_difficulty(int score);
