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
	Difficulty(string _name, int _max_entities, int _spawn_min, int _spawn_max);
	string get_name();
	void add_spawn_rule(SpawnRule rule);
	vector<SpawnRule> pick_entities(int current_entities, mt19937& ms_twister);

private:
	string name = "default";
	int max_entities = 10;
	int spawn_min = 1;
	int spawn_max = 3;
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
