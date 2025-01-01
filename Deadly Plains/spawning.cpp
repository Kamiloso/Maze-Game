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

	Difficulty* D = &arr[0];

	D = &arr[1];
	D->set_name({ "ANIMAL PARADISE", COLOR::YELLOW });
	D->configure_accessibility(0, 9); // 10
	D->configure_spawning(6, 500, 1, 3, {
		{ C::ANIMAL, 1 },
		});

	D = &arr[2];
	D->set_name({ "MONSTER INC", COLOR::RED });
	D->configure_accessibility(10, 59); // 50
	D->configure_spawning(6, 600, 1, 3, {
		{ C::ANIMAL, 1 },
		{ C::MONSTER, 1 },
		});

	D = &arr[3];
	D->set_name({ "INSECT INFEST", COLOR::RED });
	D->configure_accessibility(60, 149); // 90
	D->configure_spawning(6, 750, 1, 3, {
		{ C::INSECT, 5 },
		{ C::MONSTER, 1 },
		});

	D = &arr[4];
	D->set_name({ "TRAINING AREA", COLOR::RED });
	D->configure_accessibility(150, 274); // 125
	D->configure_spawning(7, 600, 1, 3, {
		{ C::INSECT, 1 },
		{ C::MONSTER, 1 },
		});
	D->configure_degradation(4000);

	D = &arr[5];
	D->set_name({ "SNIPER ATTACK", COLOR::RED });
	D->configure_accessibility(275, 399); // 125
	D->configure_spawning(7, 650, 1, 3, {
		{ C::INSECT, 1 },
		{ C::MONSTER, 1 },
		{ C::SNIPER, 1 },
		});
	D->configure_degradation(3500);

	D = &arr[6];
	D->set_name({ "MAGICAL BURST", COLOR::MAGENTA });
	D->configure_accessibility(400, 549); // 150
	D->configure_spawning(8, 700, 1, 3, {
		{ C::INSECT, 3 },
		{ C::INSECT, 3, true },
		{ C::MONSTER, 4 },
		{ C::SNIPER, 2 },
		});
	D->configure_degradation(3000, 300);

	D = &arr[7];
	D->set_name({ "THE CAPITAL I", COLOR::RED });
	D->configure_accessibility(550, 749); // 200
	D->configure_spawning(8, 700, 1, 3, {
		{ C::INSECT, 4 },
		{ C::MONSTER, 4 },
		{ C::INSECTOR, 1 },
		});
	D->configure_degradation(2500, 220);

	D = &arr[8];
	D->set_name({ "BULLET STORM", COLOR::RED });
	D->configure_accessibility(750, 899); // 150
	D->configure_spawning(9, 800, 2, 3, {
		{ C::INSECT, 2 },
		{ C::SNIPER, 4 },
		});
	D->configure_degradation(2000, 160);

	D = &arr[9];
	D->set_name({ "MONSTER ENERGY", COLOR::MAGENTA });
	D->configure_accessibility(900, 1149); // 250
	D->configure_spawning(9, 700, 1, 3, {
		{ C::INSECT, 2 },
		{ C::INSECT, 1, true },
		{ C::MONSTER, 1 },
		{ C::MONSTER, 1, true },
		{ C::SNIPER, 1 },
		{ C::INSECTOR, 1 },
		});
	D->configure_degradation(1500, 120);

	D = &arr[10];
	D->set_name({ "INSECT FACTORY", COLOR::DARK_RED });
	D->configure_accessibility(1150, 1399); // 250
	D->configure_spawning(10, 900, 1, 3, {
		{ C::INSECT, 1 },
		{ C::MONSTER, 3 },
		{ C::SNIPER, 1 },
		{ C::INSECTOR, 1 },
		});
	D->configure_degradation(1000, 80);

	D = &arr[11];
	D->set_name({ "FRONTLINE", COLOR::DARK_RED });
	D->configure_accessibility(1400, 1699); // 300
	D->configure_spawning(10, 900, 2, 3, {
		{ C::INSECT, 8 },
		{ C::MONSTER, 16 },
		{ C::SNIPER, 8 },
		{ C::INSECTOR, 3 },
		});
	D->configure_degradation(500, 35);

	D = &arr[12];
	D->set_name({ "WALL OF FIRE", COLOR::MAGENTA });
	D->configure_accessibility(1700, 1999); // 300
	D->configure_spawning(11, 1000, 2, 3, {
		{ C::INSECT, 4 },
		{ C::INSECT, 4, true },
		{ C::MONSTER, 8 },
		{ C::MONSTER, 4, true },
		{ C::SNIPER, 4 },
		{ C::SNIPER, 4, true },
		{ C::INSECTOR, 3 },
		});
	D->configure_degradation(200, 15);

	D = &arr[13];
	D->set_name({ "CHERNOBYL", COLOR::MAGENTA });
	D->configure_accessibility(2000, 2399); // 400
	D->configure_spawning(11, 1000, 2, 3, {
		{ C::INSECT, 2 },
		{ C::INSECT, 6, true },
		{ C::MONSTER, 6 },
		{ C::MONSTER, 6, true },
		{ C::SNIPER, 2 },
		{ C::SNIPER, 6, true },
		{ C::INSECTOR, 2 },
		{ C::INSECTOR, 1, true },
		});
	D->configure_degradation(0, 15);

	D = &arr[14];
	D->set_name({ "HELL'S ENTRANCE", COLOR::DARK_MAGENTA });
	D->configure_accessibility(2400, 2999); // 600
	D->configure_spawning(12, 1000, 2, 4, {
		{ C::INSECT, 1 },
		{ C::INSECT, 7, true },
		{ C::MONSTER, 3 },
		{ C::MONSTER, 9, true },
		{ C::SNIPER, 1 },
		{ C::SNIPER, 7, true },
		{ C::INSECTOR, 1 },
		{ C::INSECTOR, 2, true },
		});
	D->configure_degradation(0, 15);

	D = &arr[15];
	D->set_name({ "APOCALYPSE", COLOR::DARK_MAGENTA });
	D->configure_accessibility(3000, -1); // INF
	D->configure_spawning(14, 1000, 3, 4, {
		{ C::INSECT, 8, true },
		{ C::MONSTER, 12, true },
		{ C::SNIPER, 8, true },
		{ C::INSECTOR, 3, true },
		});
	D->configure_degradation(0, 15);

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
