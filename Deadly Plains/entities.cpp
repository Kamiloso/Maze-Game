#include <vector>
#include <random>

#include "map.h"
#include "entities.h"
#include "common.h"

using namespace std;

Tile::Tile(char _id)
{
	id = _id;

	// Player
	if (id == C::PLAYER) {
		health = MAX_PLAYER_HEALTH;
		has_score = 0;
	}

	// Animal
	if (id == C::ANIMAL) {
		health = 1;
		has_score = 2;
	}

	// Monster
	if (id == C::MONSTER) {
		health = 5;
		has_score = 6;
	}

	// Sniper
	if (id == C::SNIPER) {
		health = 3;
		has_score = 8;
	}

	// Insector
	if (id == C::INSECTOR) {
		health = 10;
		has_score = 20;
	}

	// Insect
	if (id == C::INSECT) {
		health = 1;
		has_score = 3;
	}

	// Core
	if (id == C::SPAWNER) {
		
	}

	// Bullet
	if (id == C::BULLET) {
		health = 1;
		has_score = 0;
	}

	// Fruit
	if (id == C::FRUIT) {
		health = 1;
		has_score = 0;
	}
}

static Coords directions[4] = {
	{0,1}, {1,0}, {0,-1}, {-1,0}
};

static int simulation_space[3][PATHFIND_TABLE_SIZE][PATHFIND_TABLE_SIZE];

static int id_from_mode(string mode)
{
	if (mode == "melee") return 0;		// Normal melee attacker
	if (mode == "predator") return 1;	// Melee attacker, which hunts for animals too
	if (mode == "distance") return 2;	// Distance killer

	return 0; // default is melee
}

static Coords map_to_pathfind_convert(Coords player, Coords entity)
{
	return {
		entity.x - player.x + SIMULATION_DISTANCE,
		entity.y - player.y + SIMULATION_DISTANCE
	};
}

static Coords pathfind_to_map_convert(Coords player, Coords entity)
{
	return {
		entity.x + player.x - SIMULATION_DISTANCE,
		entity.y + player.y - SIMULATION_DISTANCE
	};
}

static int find_smell(vector<EntitySmell> smells, char tile)
{
	if (tile == ' ' || tile == C::BULLET)
		return 0;

	int lngt = smells.size();
	for (int i = 0; i < lngt; i++)
	{
		if (smells[i].id == tile)
			return smells[i].smell;
	}

	return -1;
}

static void propagate_smells(int space[PATHFIND_TABLE_SIZE][PATHFIND_TABLE_SIZE], Coords begin_at)
{
	for (int i = 0; i < 4; i++)
	{
		Coords expand_to = {
			begin_at.x + directions[i].x,
			begin_at.y + directions[i].y
		};
		if (expand_to.x >= 0 && expand_to.x < PATHFIND_TABLE_SIZE && expand_to.y >= 0 && expand_to.y < PATHFIND_TABLE_SIZE)
		{
			if (space[expand_to.x][expand_to.y] != -1 && space[begin_at.x][begin_at.y] - 1 > space[expand_to.x][expand_to.y])
			{
				space[expand_to.x][expand_to.y] = space[begin_at.x][begin_at.y] - 1;
				propagate_smells(space, expand_to);
			}
		}
	}
}

void draw_pathmap(Map* map, Coords player, vector<EntitySmell> smells, string mode)
{
	int m = id_from_mode(mode);

	// Construct table
	for (int x = 0; x < PATHFIND_TABLE_SIZE; x++)
		for (int y = 0; y < PATHFIND_TABLE_SIZE; y++)
		{
			Coords map_c = pathfind_to_map_convert(player, { x, y });
			char tile = map->get_tile(map_c.x, map_c.y);
			simulation_space[m][x][y] = find_smell(smells, tile);
		}

	// Smell calculations
	for (int x = 0; x < PATHFIND_TABLE_SIZE; x++)
		for (int y = 0; y < PATHFIND_TABLE_SIZE; y++)
			propagate_smells(simulation_space[m], {x,y});
}

// needs both coordinates to be in "simulation space"
Coords pathfind(Map* map, Coords player, Coords entity, mt19937& ms_twister, string mode)
{
	int m = id_from_mode(mode);

	// Adjust entity coordinates to fit table
	entity = map_to_pathfind_convert(player, entity);

	// Decide where to go
	int best_destination_found = 1;
	vector<Coords> potential_destinations;
	for (int i = 0; i < 4; i++)
	{
		Coords check_here = {
			entity.x + directions[i].x,
			entity.y + directions[i].y
		};
		if (check_here.x >= 0 && check_here.x < PATHFIND_TABLE_SIZE && check_here.y >= 0 && check_here.y < PATHFIND_TABLE_SIZE)
		{
			int destination_found = simulation_space[m][check_here.x][check_here.y];
			if (destination_found > best_destination_found)
			{
				best_destination_found = destination_found;
				potential_destinations.clear();
				potential_destinations.push_back(directions[i]);
			}
			else if (destination_found == best_destination_found)
			{
				potential_destinations.push_back(directions[i]);
			}
		}
	}

	// Return going will
	if (potential_destinations.size() > 0)
		return potential_destinations[ms_twister() % potential_destinations.size()];
	else
		return { 0,0 };
}
