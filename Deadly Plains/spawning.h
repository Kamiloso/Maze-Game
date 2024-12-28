#pragma once

#include <string>
#include <random>

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
	Difficulty get_difficulty(int score);

private:
	Map* map;
	Pathfinding* pathf_ptr;
	mt19937* ms_twister_ptr;
};

int get_next_score(std::string dif_num);
