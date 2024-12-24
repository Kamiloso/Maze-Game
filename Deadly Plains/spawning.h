#pragma once

#include <string>
#include <vector>
#include <random>

#include "common.h"

using namespace std;

class Map;
class Pathfinding;

struct SpawnRule
{
	char type = C::ANIMAL;
	int weight = 1;
	bool magical = false;
};

class Difficulty
{
public:
	Difficulty(string _name = "h00 NONE", int _max_entities = 0, int _spawn_min = 0, int _spawn_max = 0, int _max_blocks = -1, int _terrain_deg_num = -1);
	string get_name(bool get_full = false);
	int get_max_blocks();
	int get_terrain_deg_num();
	void add_spawn_rule(SpawnRule rule);
	vector<SpawnRule> pick_entities(int current_entities, mt19937& ms_twister);

private:
	string name;
	int max_entities;
	int spawn_min;
	int spawn_max;
	int max_blocks;
	int terrain_deg_num;
	vector<SpawnRule> spawn_rules;
};

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
