#include <sstream>

#include "difficulty.h"

// Constructor
Difficulty::Difficulty(int _id)
{
	id = _id;
}

// Sets difficulty name
void Difficulty::set_name(ColoredString _name)
{
	name = _name;
}

// Sets minimum and maximum score
void Difficulty::configure_accessibility(int _score_min, int _score_max)
{
	score_min = _score_min;
	score_max = _score_max;
}

// Configures all spawning rules
void Difficulty::configure_spawning(int _max_entities, int _spawn_min, int _spawn_max, vector<SpawnRule> _spawn_rules)
{
	max_entities = _max_entities;
	spawn_min = _spawn_min;
	spawn_max = _spawn_max;
	spawn_rules = _spawn_rules;
}

// Configures terrain degradation rules (if present in difficulty)
void Difficulty::configure_degradation(int _max_blocks, int _terrain_deg_num)
{
	max_blocks = _max_blocks;
	terrain_deg_num = _terrain_deg_num;
}

// ID getter
int Difficulty::get_id()
{
	return id;
}

// ID getter in string form
string Difficulty::get_id_str()
{
	stringstream ss;
	if (id < 10) ss << "0" << id;
	else ss << id;
	return ss.str();
}

// Name getter
ColoredString Difficulty::get_name()
{
	return name;
}

// Checks if difficulty is active with the given score
bool Difficulty::is_active(int score)
{
	return (score >= score_min && (score <= score_max || score_to_str(score_max) == "INF"));
}

// Score min getter
int Difficulty::get_score_min()
{
	return score_min;
}

// Score max getter
int Difficulty::get_score_max()
{
	return score_max;
}

// Deducts score_min of the next difficulty based on score_max of the current
int Difficulty::get_next_score()
{
	if (score_to_str(score_max) != "INF")
		return score_max + 1;
	else
		return -1;
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

// --- Static methods ---

// Converts score into a string form (converts -1 into INF)
string Difficulty::score_to_str(int score)
{
	if (score == -1)
		return "INF";

	stringstream ss;
	ss << score;
	return ss.str();
}
