#include <iostream>
#include <vector>
#include <random>

#include "map.h"
#include "pathfinding.h"
#include "common.h"

using namespace std;

// Self explanatory I think
static Coords directions[4] = {
	{0,1}, {1,0}, {0,-1}, {-1,0}
};

// Returns biggest element of the array
static int biggest_element(int* arr, int lngt)
{
	int biggest = arr[0];
	for (int i = 1; i < lngt; i++)
		if (arr[i] > biggest)
			biggest = arr[i];
	return biggest;
}

// Returns pathfind mode ID
static int id_from_mode(string mode)
{
	if (mode == "melee") return 0;		// Normal melee attacker
	if (mode == "predator") return 1;	// Melee attacker, which hunts for animals too
	if (mode == "distance") return 2;	// Distance killer
	if (mode == "spawner") return 3;	// Spawner silent observator
	if (mode == "SPAWNING") return 4;	// SPAWNING special mode

	return 0; // default is melee
}

// Can see through this tile while pathfinding?
static bool can_see_through(char type)
{
	return (
		type == ' ' ||
		type == C::BULLET
	);
}

// Converts map coordinates to pathfind coordinates
static Coords map_to_pathfind_convert(Coords player, Coords entity)
{
	return {
		entity.x - player.x + SIMULATION_DISTANCE,
		entity.y - player.y + SIMULATION_DISTANCE
	};
}

// Converts pathfind coordinates to map coordinates
static Coords pathfind_to_map_convert(Coords player, Coords entity)
{
	return {
		entity.x + player.x - SIMULATION_DISTANCE,
		entity.y + player.y - SIMULATION_DISTANCE
	};
}

// Selects smell of a tile based on its type and smell array
static int find_smell(vector<EntitySmell> smells, char tile)
{
	if (can_see_through(tile))
		return 0;

	int lngt = smells.size();
	for (int i = 0; i < lngt; i++)
	{
		if (smells[i].id == tile)
			return smells[i].smell;
	}

	return -1;
}

// Propagates smells from entity for pathfinding
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

// Draws pathfinfing map of a certain type
void Pathfinding::draw_pathmap(Map* map, Coords player, vector<EntitySmell> smells, string mode)
{
	int id = id_from_mode(mode);

	// Construct table
	for (int x = 0; x < PATHFIND_TABLE_SIZE; x++)
		for (int y = 0; y < PATHFIND_TABLE_SIZE; y++)
		{
			Coords map_c = pathfind_to_map_convert(player, { x, y });
			char tile = map->get_tile(map_c.x, map_c.y);
			simulation_space[id][x][y] = find_smell(smells, tile);
		}

	// Smell calculations
	for (int x = 0; x < PATHFIND_TABLE_SIZE; x++)
		for (int y = 0; y < PATHFIND_TABLE_SIZE; y++)
			propagate_smells(simulation_space[id], { x,y });
}

// Returns pathfinfing vector of entity (both coordinates must be in "simulation space")
Coords Pathfinding::pathfind(Coords player, Coords entity, mt19937& ms_twister, string mode) const
{
	// Get pathfind table id
	int id = id_from_mode(mode);

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
			int destination_found = simulation_space[id][check_here.x][check_here.y];
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

// Returns the direction of shooting if in right position (both coordinates must be in "simulation space")
Coords Pathfinding::get_sniper_direction(Coords player, Coords entity, mt19937& ms_twister, string mode) const
{
	// Get pathfind table id
	int id = id_from_mode(mode);

	// Find weights of all directions
	int D[4] = {
		sniper_weight(player, { entity.x + 0, entity.y + 1 }, 0, 1, mode),
		sniper_weight(player, { entity.x + 0, entity.y - 1 }, 0, -1, mode),
		sniper_weight(player, { entity.x + 1, entity.y + 0 }, 1, 0, mode),
		sniper_weight(player, { entity.x - 1, entity.y + 0 }, -1, 0, mode)
	};

	// Choose the best value
	int value_TOTAL = biggest_element(D, 4);
	if (value_TOTAL < MIN_SNIPER_SMELL)
		return { 0,0 };

	// Pick the random best element
	vector<Coords> potential_destinations;
	if (D[0] == value_TOTAL) potential_destinations.push_back({0,1});
	if (D[1] == value_TOTAL) potential_destinations.push_back({0,-1});
	if (D[2] == value_TOTAL) potential_destinations.push_back({1,0});
	if (D[3] == value_TOTAL) potential_destinations.push_back({-1,0});
	std::shuffle(potential_destinations.begin(), potential_destinations.end(), ms_twister);

	return potential_destinations[0];
}

// Returns distance pathfinfing vector of entity (both coordinates must be in "simulation space")
Coords Pathfinding::pathfind_distance(Coords player, Coords entity, mt19937& ms_twister, string mode) const
{
	// Get pathfind table id
	int id = id_from_mode(mode);

	// Find tile weights of all directions
	int N[3] = {
		sniper_weight(player, { entity.x - 1, entity.y + 0 }, 0, 1, mode),
		sniper_weight(player, { entity.x + 0, entity.y + 1 }, 0, 1, mode),
		sniper_weight(player, { entity.x + 1, entity.y + 0 }, 0, 1, mode)
	};
	int S[3] = {
		sniper_weight(player, { entity.x - 1, entity.y + 0 }, 0, -1, mode),
		sniper_weight(player, { entity.x + 0, entity.y - 1 }, 0, -1, mode),
		sniper_weight(player, { entity.x + 1, entity.y + 0 }, 0, -1, mode)
	};
	int E[3] = {
		sniper_weight(player, { entity.x + 0, entity.y - 1 }, 1, 0, mode),
		sniper_weight(player, { entity.x + 1, entity.y + 0 }, 1, 0, mode),
		sniper_weight(player, { entity.x + 0, entity.y + 1 }, 1, 0, mode)
	};
	int W[3] = {
		sniper_weight(player, { entity.x + 0, entity.y - 1 }, -1, 0, mode),
		sniper_weight(player, { entity.x - 1, entity.y + 0 }, -1, 0, mode),
		sniper_weight(player, { entity.x + 0, entity.y + 1 }, -1, 0, mode)
	};

	// Calculate best position for all possibilities
	int arr_N[] = { N[1], S[1], E[2], W[2] };
	int arr_S[] = { N[1], S[1], E[0], W[0] };
	int arr_E[] = { N[2], S[2], E[1], W[1] };
	int arr_W[] = { N[0], S[0], E[1], W[1] };
	int arr_CURRENT[] = { N[1], S[1], E[1], W[1] };

	int value_N = biggest_element(arr_N, 4);
	int value_S = biggest_element(arr_S, 4);
	int value_E = biggest_element(arr_E, 4);
	int value_W = biggest_element(arr_W, 4);
	int value_CURRENT = biggest_element(arr_CURRENT, 4);

	// Calculate the best movement value
	int arr_TOTAL[] = { value_N, value_S, value_E, value_W };
	int value_TOTAL = biggest_element(arr_TOTAL, 4);
	
	// No decision conditions
	if (value_TOTAL < 0 || value_TOTAL <= value_CURRENT)
		return pathfind(player, entity, ms_twister, mode);

	// Create a list of all potential destinations
	vector<Coords> potential_destinations;
	if (value_N == value_TOTAL) potential_destinations.push_back({ 0,1 });
	if (value_S == value_TOTAL) potential_destinations.push_back({ 0,-1 });
	if (value_E == value_TOTAL) potential_destinations.push_back({ 1,0 });
	if (value_W == value_TOTAL) potential_destinations.push_back({ -1,0 });
	std::shuffle(potential_destinations.begin(), potential_destinations.end(), ms_twister);

	// Return the first element of shuffled vector
	return potential_destinations[0];
}

// Checks how good for shooting is the tile for distance attacker
int Pathfinding::sniper_weight(Coords player, Coords point, int dx, int dy, string mode) const
{
	// Get pathfind table id
	int id = id_from_mode(mode);

	// Adjust entity coordinates to fit table
	point = map_to_pathfind_convert(player, point);

	// Find the best tile
	int best_found = -1;
	while (point.x >= 0 && point.y >= 0 && point.x < PATHFIND_TABLE_SIZE && point.y < PATHFIND_TABLE_SIZE)
	{
		int found_now = simulation_space[id][point.x][point.y];
		if (found_now <= 0) // should work only in smell space
			break;

		if (found_now > best_found)
			best_found = found_now;

		point.x += dx;
		point.y += dy;
	}

	return best_found;
}

// Returns a list of tiles, which were marked in the pathfinding
vector<TileSmell> Pathfinding::detect_tiles(Coords player, mt19937& ms_twister, string mode, int min_smell, bool randomize) const
{
	vector<TileSmell> return_list;

	// Get pathfind table id
	int id = id_from_mode(mode);

	// Detect tiles
	for (int x = 0; x < PATHFIND_TABLE_SIZE; x++)
		for (int y = 0; y < PATHFIND_TABLE_SIZE; y++)
		{
			int tile_smell = simulation_space[id][x][y];
			if (tile_smell > 0 && tile_smell <= min_smell)
			{
				Coords map_c = pathfind_to_map_convert(player, { x, y });
				return_list.push_back({ {map_c.x, map_c.y}, tile_smell });
			}
		}

	if(randomize)
		std::shuffle(return_list.begin(), return_list.end(), ms_twister);

	return return_list;
}
