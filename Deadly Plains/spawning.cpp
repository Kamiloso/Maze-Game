#include <string>
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
	if (score < get_next_score("01"))
	{
		auto dif = Difficulty("o01 ANIMAL PARADISE", 6, 1, 3);
		dif.add_spawn_rule({ C::ANIMAL, 1 });
		return dif;
	}
	else if(score < get_next_score("02"))
	{
		auto dif = Difficulty("m02 TRAINING AREA", 6, 1, 3);
		dif.add_spawn_rule({ C::ANIMAL, 5 });
		dif.add_spawn_rule({ C::MONSTER, 5 });
		dif.add_spawn_rule({ C::INSECTOR, 1 });
		return dif;
	}
	else
	{
		auto dif = Difficulty("n03 THE APOCALYPSE", 25, 5, 10, /* B;N */ 0, 8);
		dif.add_spawn_rule({ C::MONSTER, 5 });
		dif.add_spawn_rule({ C::SNIPER, 5 });
		dif.add_spawn_rule({ C::INSECT, 5 });
		dif.add_spawn_rule({ C::INSECTOR, 1 });
		return dif;
	}
}

// Returns score after which next phase starts
int get_next_score(std::string dif_num)
{
	if (dif_num == "01") return 10;
	if (dif_num == "02") return 100;
	return -1;
}
