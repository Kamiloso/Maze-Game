#include <string>
#include <vector>

#include "spawning.h"
#include "entities.h"
#include "pathfinding.h"
#include "difficulty.h"
#include "map.h"

using namespace std;

// Returns amount of egg entities around the player (in simulation space)
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

// Returns all difficulties
vector<Difficulty> get_difficulties()
{
	vector<Difficulty> arr(16);
	for (int i = 0; i < 16; i++)
		arr[i] = Difficulty(i);

	Difficulty* D = &arr[00];

	D = &arr[01];
	D->set_name({ "ANIMAL PARADISE", COLOR::YELLOW });
	D->configure_accessibility(0, 9);
	D->configure_spawning(6, 1, 3, {
		{ C::ANIMAL, 1 }
		});

	D = &arr[02];
	D->set_name({ "SECOND PHASE", COLOR::RED });
	D->configure_accessibility(10, -1);
	D->configure_spawning(6, 1, 3, {
		{ C::MONSTER, 1 }
		});

	return arr;
}

// Returns difficulty based on its ID
Difficulty get_difficulty_by_id(int id)
{
	vector<Difficulty> arr = get_difficulties();

	if (id >= 1 && id <= 15)
		return arr[id];
	else
		return arr[0];
}

// Returns difficulty based on current score
Difficulty get_difficulty(int score)
{
	vector<Difficulty> arr = get_difficulties();
	for (int i = 1; i <= 15; i++)
	{
		if (arr[i].is_active(score))
			return arr[i];
	}
	return arr[0];
}
