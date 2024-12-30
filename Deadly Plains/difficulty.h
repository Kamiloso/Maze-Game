#pragma once

#include <string>
#include <vector>
#include <random>

#include "common.h"

using namespace std;

struct ColoredString
{
	string str = "";
	unsigned char color = COLOR::LIGHT_GRAY;
};

struct SpawnRule
{
	char type = C::ANIMAL;
	int weight = 1;
	bool magical = false;
};

class Difficulty
{
public:
	Difficulty(int _id = 0);

	void set_name(ColoredString _name);
	void configure_accessibility(int _score_min, int _score_max);
	void configure_spawning(int _max_entities, int _spawn_min, int _spawn_max, vector<SpawnRule> _spawn_rules);
	void configure_degradation(int _max_blocks = -1, int _terrain_deg_num = -1);

	int get_id();
	string get_id_str();
	ColoredString get_name();
	bool is_active(int score);
	int get_score_min();
	int get_score_max();
	int get_next_score();
	vector<SpawnRule> pick_entities(int current_entities, mt19937& ms_twister);
	int get_max_blocks();
	int get_terrain_deg_num();

	static string score_to_str(int score);

private:
	
	int id = 00;
	ColoredString name = { "TEMPLATE" };
	int score_min = 1;
	int score_max = 0;
	int max_entities = 0;
	int spawn_min = 1;
	int spawn_max = 1;
	vector<SpawnRule> spawn_rules = {};
	int max_blocks = -1;
	int terrain_deg_num = -1;
};
