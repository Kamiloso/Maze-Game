#include <vector>

#include "spawning.h"
#include "entities.h"
#include "pathfinding.h"
#include "map.h"

using namespace std;

// ---------------------------------- //
// -------- Difficulty CLASS -------- //
// ---------------------------------- //

// Constructor
Difficulty::Difficulty(string _name, int _max_entities, int _spawn_min, int _spawn_max)
{
	name = _name;
	max_entities = _max_entities;
	spawn_min = _spawn_min;
	spawn_max = _spawn_max;
}

// Difficulty name getter
string Difficulty::get_name()
{
	return name;
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

// -------------------------------- //
// -------- Spawning CLASS -------- //
// -------------------------------- //

static int count_egg_entities(Coords player, Map* map)
{
	int x1 = player.x - SIMULATION_DISTANCE;
	int y1 = player.y - SIMULATION_DISTANCE;
	int x2 = player.x + SIMULATION_DISTANCE;
	int y2 = player.y + SIMULATION_DISTANCE;

	int sum = 0;
	for (int x = x1; x <= x2; x++)
		for (int y = y1; y <= y2; y++)
		{
			if (map->get_tile(x, y) == '=') continue;

			Tile& tile = map->get_tile_ref(x, y);
			if (tile.get_id() == C::EGG || tile.was_egg_spawned())
				sum++;
		}

	return sum;
}

// Constructor
Spawning::Spawning(Map* _map, Pathfinding* _pathf_ptr, mt19937* _ms_twister_ptr)
{
	map = _map;
	pathf_ptr = _pathf_ptr;
	ms_twister_ptr = _ms_twister_ptr;
}

// Spawns entities around the player based on difficulty chosen from score
void Spawning::frame_spawn(Coords player, Difficulty difficulty)
{
	// Find spawnable tiles
	pathf_ptr->draw_pathmap(map, player, { {C::PLAYER, SPAWNING_SMELL_INIT} }, "SPAWNING");
	auto tile_list = pathf_ptr->detect_tiles(player, *ms_twister_ptr, "SPAWNING", SPAWNING_SMELL_MID, true);

	// Get spawn list
	auto spawn_list = difficulty.pick_entities(count_egg_entities(player, map), *ms_twister_ptr);

	// Pick tiles and spawn entities on them
	int spawned = 0;
	int target_spawned = spawn_list.size();
	int lngt = tile_list.size();

	for (int i = 0; i < lngt; i++)
	{
		if (spawned >= target_spawned) break;

		int tx = tile_list[i].position.x;
		int ty = tile_list[i].position.y;

		if (map->get_tile(tx, ty) != ' ') continue;

		map->spawn(tx, ty, C::EGG, true, spawn_list[spawned].magical).egg_initialize(spawn_list[spawned].type, EGG_HATCH_TIME);

		spawned++;
	}
}

// Returns difficulty based on current score
Difficulty Spawning::get_difficulty(int score)
{
	if (score < 10)
	{
		auto dif = Difficulty("PEACE", 12, 1, 3);
		dif.add_spawn_rule({ C::ANIMAL, 1 });
		return dif;
	}
	else if (score < 60)
	{
		auto dif = Difficulty("EASY MONSTERS", 12, 1, 3);
		dif.add_spawn_rule({ C::ANIMAL, 1 });
		dif.add_spawn_rule({ C::MONSTER, 1 });
		return dif;
	}
	else if (score < 150)
	{
		auto dif = Difficulty("MELEE CHASE", 12, 1, 3);
		dif.add_spawn_rule({ C::ANIMAL, 1 });
		dif.add_spawn_rule({ C::MONSTER, 1 });
		dif.add_spawn_rule({ C::INSECT, 2 });
		return dif;
	}
	else if (score < 300)
	{
		auto dif = Difficulty("SHOOTING DANCE", 12, 1, 3);
		dif.add_spawn_rule({ C::MONSTER, 1 });
		dif.add_spawn_rule({ C::INSECT, 3 });
		dif.add_spawn_rule({ C::SNIPER, 1 });
		return dif;
	}
	else if (score < 400)
	{
		auto dif = Difficulty("BULLET HORROR", 12, 1, 3);
		dif.add_spawn_rule({ C::INSECT, 1 });
		dif.add_spawn_rule({ C::SNIPER, 1 });
		return dif;
	}
	else if (score < 600)
	{
		auto dif = Difficulty("SUMMONERS", 12, 1, 3);
		dif.add_spawn_rule({ C::MONSTER, 4 });
		dif.add_spawn_rule({ C::INSECT, 5 });
		dif.add_spawn_rule({ C::INSECTOR, 1 });
		return dif;
	}
	else
	{
		auto dif = Difficulty("ARMAGEDDON", 25, 3, 6);
		dif.add_spawn_rule({ C::MONSTER, 3 });
		dif.add_spawn_rule({ C::SNIPER, 3 });
		dif.add_spawn_rule({ C::INSECT, 3 });
		dif.add_spawn_rule({ C::INSECTOR, 1 });
		return dif;
	}
}
