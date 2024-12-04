#include <random>
#include <vector>

#include "map.h"
#include "rooms.h"

using namespace std;

// Returns the array of points where object should be spawned randomly
static vector<Coords> where_spawn(vector<Coords> base, int min, int probability, mt19937& ms_twister)
{
    vector<Coords> return_vec;
    std::shuffle(base.begin(), base.end(), ms_twister);
    const int lngt = base.size();
    for (int i = 0; i < lngt; i++)
    {
        if (i < min || ms_twister() % 1000 < probability)
            return_vec.push_back(base[i]);
    }
    return return_vec;
}

// Creates a customizable entity room (just to avoid code redundance)
static void customizable_entity_room(Map* map, int X, int Y, mt19937& ms_twister, char type, int min, int probability, bool has_ai)
{
    vector<Coords> spawn_spaces;

    // Gather spaces
    for (int dx = -2; dx <= 2; dx++)
        for (int dy = -2; dy <= 2; dy++)
        {
            int x = X + dx;
            int y = Y + dy;

            map->spawn(x, y, ' ', false);
            spawn_spaces.push_back({ x,y });
        }

    // Spawn entities
    spawn_spaces = where_spawn(spawn_spaces, min, probability, ms_twister);
    const int lngt = spawn_spaces.size();
    for (int i = 0; i < lngt; i++)
    {
        Coords& crd = spawn_spaces[i];
        Tile& tile = map->spawn(crd.x, crd.y, type, has_ai);
        if (type == C::NUMBER) tile.set_health(ms_twister() % 3 + 3);
    }
}

// Creates an empty room
void empty_room(Map* map, int X, int Y, mt19937& ms_twister)
{
    for (int dx = -2; dx <= 2; dx++)
        for (int dy = -2; dy <= 2; dy++)
        {
            int x = X + dx;
            int y = Y + dy;

            if(dx != 0 && dy != 0) // to avoid errors allow to not erase anchor tile
                map->spawn(x, y, ' ', false);
        }
}

// Creates an animal room
void animal_room(Map* map, int X, int Y, mt19937& ms_twister)
{
    customizable_entity_room(map, X, Y, ms_twister, C::ANIMAL, 4, 70, true);
}

// Creates a monster room
void monster_room(Map* map, int X, int Y, mt19937& ms_twister)
{
    customizable_entity_room(map, X, Y, ms_twister, C::MONSTER, 3, 60, true);
}

// Creates a room with a random number square
void square_room(Map* map, int X, int Y, mt19937& ms_twister)
{
    for (int dx = -2; dx <= 2; dx++)
        for (int dy = -2; dy <= 2; dy++)
        {
            int x = X + dx;
            int y = Y + dy;

            if (dx == 0 && dy == 0)
                map->spawn(x, y, C::NUMBER, false).set_health(ms_twister() % 10);
            else if (dx >= -1 && dx <= 1 && dy >= -1 && dy <= 1)
                map->spawn(x, y, C::WALL, false);
            else
                map->spawn(x, y, ' ', false);
        }
}

// Creates a corridor with fruits on sides
void fruit_corridor(Map* map, int X, int Y, mt19937& ms_twister)
{
    bool is_vertical = map->get_tile_ref(X, Y).get_score() == 5;
    vector<Coords> spawn_spaces;

    for (int dx = -2; dx <= 2; dx++)
        for (int dy = -2; dy <= 2; dy++)
        {
            int x = X + dx;
            int y = Y + dy;

            if (is_vertical)
            {
                if (dx != -2 && dx != 2)
                    map->spawn(x, y, ' ', false);
                else if (dy % 2 != 0)
                    map->spawn(x, y, C::WALL, false);
                else {
                    map->spawn(x, y, ' ', false);
                    spawn_spaces.push_back({ x,y });
                }
            }
            else
            {
                if (dy != -2 && dy != 2)
                    map->spawn(x, y, ' ', false);
                else if (dx % 2 != 0)
                    map->spawn(x, y, C::WALL, false);
                else {
                    map->spawn(x, y, ' ', false);
                    spawn_spaces.push_back({ x,y });
                }
            }
        }

    // Spawn entities
    spawn_spaces = where_spawn(spawn_spaces, 1, 500, ms_twister);
    const int lngt = spawn_spaces.size();
    for (int i = 0; i < lngt; i++)
    {
        Coords& crd = spawn_spaces[i];
        map->spawn(crd.x, crd.y, C::FRUIT, false);
    }
}

// Creates a decorational corridor
void normal_corridor(Map* map, int X, int Y, mt19937& ms_twister)
{
    bool is_vertical = map->get_tile_ref(X, Y).get_score() == 5;
    int variant = ms_twister() % 1;

    for (int dx = -2; dx <= 2; dx++)
        for (int dy = -2; dy <= 2; dy++)
        {
            int x = X + dx;
            int y = Y + dy;

            char decision = ' ';

            if (is_vertical)
            {
                if (variant == 0) // middle wall
                {
                    if (dx == 0 && dy >= -1 && dy <= 1)
                        decision = C::WALL;
                    else
                        decision = ' ';
                }
            }
            else
            {
                if (variant == 0) // middle wall
                {
                    if (dy == 0 && dx >= -1 && dx <= 1)
                        decision = C::WALL;
                    else
                        decision = ' ';
                }
            }

            map->spawn(x, y, decision, false);
        }
}

// Creates a temple with a magic spawner
void spawner_temple(Map* map, int X, int Y, mt19937& ms_twister)
{
    for (int dx = -2; dx <= 2; dx++)
        for (int dy = -2; dy <= 2; dy++)
        {
            int x = X + dx;
            int y = Y + dy;

            if (
                (dx == -2 && dy == -2) ||
                (dx == 2 && dy == -2) ||
                (dx == -2 && dy == 2) ||
                (dx == 2 && dy == 2)
                )
                map->spawn(x, y, C::BLOCK, false, true);
            else if (dx == 0 && dy == 0)
                map->spawn(x, y, C::SPAWNER, true, true);
            else
                map->spawn(x, y, ' ', false);
        }
}

// Creates a block temple
void block_temple(Map* map, int X, int Y, mt19937& ms_twister)
{
    customizable_entity_room(map, X, Y, ms_twister, C::BLOCK, 6, 200, false);
}
