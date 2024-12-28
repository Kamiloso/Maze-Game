#include "difficulty.h"

// Constructor
Difficulty::Difficulty(string _name, int _max_entities, int _spawn_min, int _spawn_max, int _max_blocks, int _terrain_deg_num)
{
	name = _name;
	max_entities = _max_entities;
	spawn_min = _spawn_min;
	spawn_max = _spawn_max;
	max_blocks = _max_blocks;
	terrain_deg_num = _terrain_deg_num;
}

// Difficulty name getter
string Difficulty::get_name(bool get_full)
{
	if (get_full)
		return name;
	else
		return name.substr(4);
}

// Max blocks getter
int Difficulty::get_max_blocks()
{
	return max_blocks;
}

// Terrain deg num getter
int Difficulty::get_terrain_deg_num()
{
	return terrain_deg_num;
}

// Adds spawn rule to the rules vector
void Difficulty::add_spawn_rule(SpawnRule rule)
{
	spawn_rules.push_back(rule);
}

// Returns list of entities to spawn
vector<SpawnRule> Difficulty::pick_entities(int current_entities, mt19937& ms_twister)
{
	// Create return vector
	auto return_vector = vector<SpawnRule>();

	// Return nothing if simulation space full
	if (current_entities >= max_entities)
		return return_vector;

	// Calculate weight sum
	int weight_sum = 0;
	for (auto it = spawn_rules.begin(); it != spawn_rules.end(); ++it)
	{
		weight_sum += it->weight;
	}

	// If no ideas, return nothing
	if (weight_sum == 0)
		return return_vector;

	// Calculate how many entities should spawn
	int target_amount = spawn_min + ms_twister() % (spawn_max - spawn_min + 1);

	// Pick the random rule target_amount times
	for (int i = 0; i < target_amount; i++)
	{
		int last_weight_int = -1;
		int rnd = ms_twister() % weight_sum;
		for (auto it = spawn_rules.begin(); it != spawn_rules.end(); ++it)
		{
			last_weight_int += it->weight;
			if (rnd <= last_weight_int)
			{
				return_vector.push_back(*it);
				break;
			}
		}
	}

	return return_vector;
}
