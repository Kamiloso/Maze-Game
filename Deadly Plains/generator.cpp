#include <random>
#include <vector>
#include <iostream>

#include "common.h"
#include "generator.h"
#include "entities.h"
#include "map.h"

using namespace std;

// Resets all marks from the labirynth instructions
static void reset_marked_spaces(string* spaces)
{
    for (int i = 0; i < LAB_ARRAY_SIZE; i++)
        for (int j = 0; j < LAB_ARRAY_SIZE; j++)
        {
            int r_sum = (i % 2) + (j % 2);
            if (r_sum == 0)
                spaces[i][j] = ' ';
        }
}

// Marks all spaces in labirynth instructions, which connect to given coordinates (recursive)
static int mark_spaces(string* spaces, int x, int y)
{
    static const int MIN = 0;
    static const int MAX = LAB_ARRAY_SIZE - 1;

    int ret_count = 1;
    spaces[y][x] = '*'; // marking space

    if (x != MIN) // left
    {
        if (spaces[y][x - 1] == ' ' && spaces[y][x - 2] != '*')
            ret_count += mark_spaces(spaces, x - 2, y);
    }
    if (x != MAX) // right
    {
        if (spaces[y][x + 1] == ' ' && spaces[y][x + 2] != '*')
            ret_count += mark_spaces(spaces, x + 2, y);
    }
    if (y != MIN) // down
    {
        if (spaces[y - 1][x] == ' ' && spaces[y - 2][x] != '*')
            ret_count += mark_spaces(spaces, x, y - 2);
    }
    if (y != MAX) // up
    {
        if (spaces[y + 1][x] == ' ' && spaces[y + 2][x] != '*')
            ret_count += mark_spaces(spaces, x, y + 2);
    }

    return ret_count;
}

// Breaks completely random walls in labirynth instructions based on P probability (promiles)
static void labirynth_walls_break(string* spaces, std::mt19937& ms_twister, int P)
{
    for (int i = 0; i < LAB_ARRAY_SIZE; i++)
    {
        for (int j = 0; j < LAB_ARRAY_SIZE; j++)
        {
            int r_sum = (i % 2) + (j % 2);
            if (r_sum == 1)
            {
                if (ms_twister() % 1000 < P)
                    spaces[i][j] = ' ';
            }
        }
    }
}

// Returns instructions how to create labirynth. WARNING: Array pointer must be removed!
static string* labirynth_instructions(std::mt19937& ms_twister)
{
    system("cls");
    cout << "Generating map..." << endl;

    // Labirynth initialization
    string* spaces = new string[LAB_ARRAY_SIZE];
    for (int i = 0; i < LAB_ARRAY_SIZE; i++)
    {
        spaces[i] = "";
        for (int j = 0; j < LAB_ARRAY_SIZE; j++)
        {
            int r_sum = (i % 2) + (j % 2);
            switch (r_sum)
            {
                case 0: spaces[i] += ' '; break;
                case 1: spaces[i] += '#'; break;
                case 2: spaces[i] += '#'; break;
            }
        }
    }

    // Detecting wall spaces
    vector<Coords> all_wall_spaces;
    for (int i = 0; i < LAB_ARRAY_SIZE; i++)
        for (int j = 0; j < LAB_ARRAY_SIZE; j++)
        {
            int r_sum = (i % 2) + (j % 2);
            if (r_sum == 1)
                all_wall_spaces.push_back({j,i});
        }

    // Connecting labirynth tiles until all of them are connected
    while (true)
    {
        // Mark random tile and check if labirynth is done
        reset_marked_spaces(spaces);
        int rand_x = 2 * (ms_twister() % SECTORS_IN_LINE);
        int rand_y = 2 * (ms_twister() % SECTORS_IN_LINE);
        int connected_sectors = mark_spaces(spaces, rand_x, rand_y);
        if (connected_sectors == TOTAL_SECTORS)
            break; // everything connected, break

        // Reroll all_wall_spaces
        std::shuffle(all_wall_spaces.begin(), all_wall_spaces.end(), ms_twister);

        // Decide which wall to destroy
        for (int i = 0; i < all_wall_spaces.size(); i++)
        {
            int x = all_wall_spaces[i].x;
            int y = all_wall_spaces[i].y;

            if (x % 2 == 1 && spaces[y][x - 1] != spaces[y][x + 1]) // horizontal wall
            {
                spaces[y][x] = ' ';
                break;
            }
            if (y % 2 == 1 && spaces[y - 1][x] != spaces[y + 1][x]) // vertical wall
            {
                spaces[y][x] = ' ';
                break;
            }
        }
    }

    // Breaking some walls to make labirynth more spatial
    labirynth_walls_break(spaces, ms_twister, LABIRYNTH_WB);

    // Corner removing
    for (int i = 0; i < LAB_ARRAY_SIZE; i++)
    {
        for (int j = 0; j < LAB_ARRAY_SIZE; j++)
        {
            int r_sum = (i % 2) + (j % 2);
            if (
                r_sum == 2 &&
                spaces[i + 1][j + 0] == ' ' &&
                spaces[i + 0][j + 1] == ' ' &&
                spaces[i - 1][j + 0] == ' ' &&
                spaces[i + 0][j - 1] == ' '
            ) spaces[i][j] = ' ';
        }
    }

    system("cls");
    return spaces;
}

// Translates map coordinates into labirynth instructions coordinates
static int get_lar(int l)
{
    int on_sector = l / (SECTOR_SIZE + 1);
    int additional_tiles = l % (SECTOR_SIZE + 1);

    if (additional_tiles == 0)
        return on_sector * 2 - 1;
    else
        return on_sector * 2;
}

static void fill_wall(Map* map, int x, int y, bool vertical, string pattern = "-------")
{
    // TEMPORARY CONDITION, WILL BE FIXED IN THE FUTURE!
    if (map->get_tile(x, y) == C::WALL ||
        map->get_tile(x, y) == C::NUMBER ||
        map->get_tile(x, y) == C::BLOCK)
        return;

    for (int i = -3; i <= 3; i++)
    {
        char ptc = pattern[i + 3];
        int dx = vertical ? 0 : i;
        int dy = vertical ? i : 0;
        switch (ptc)
        {
        case ' ':
            map->spawn(x + dx, y + dy, ' ', false);
            break;
        case '#':
            map->spawn(x + dx, y + dy, C::WALL, false);
            break;
        case 'B':
            map->spawn(x + dx, y + dy, C::BLOCK, false);
            break;
        case '-':
            break;
        default:
            map->spawn(x + dx, y + dy, C::NUMBER, false).set_health(ptc - '0');
            break;
        }
    }
}

// Pre-generates terrain on a given Map
void generate(Map* map, std::mt19937& ms_twister)
{
    string* lab_inst = labirynth_instructions(ms_twister);

    const int BLIND_ROOM_APPEAR_CHANCE = 200; // (in promiles)
    const int LINEAR_ROOM_APPEAR_CHANCE = 200; // (in promiles)
    const int UNDEFINED_ROOM_APPEAR_CHANCE = 200; // (in promiles)

    // LAYER 1 - Unbreakable walls (C::WALL)
    for (int x = 0; x < MAP_SIZE; x++)
        for (int y = 0; y < MAP_SIZE; y++)
        {
            // Border creating, WARNING: these borders prevent wrong array indexes. Don't remove them,
            // or at least replace them with something unbreakable and unmovable.
            if (x == 0 || x == MAP_SIZE - 1 || y == 0 || y == MAP_SIZE - 1)
            {
                map->spawn(x, y, C::WALL, false);
                continue;
            }

            // Labirynth wall creating
            int lar_x = get_lar(x);
            int lar_y = get_lar(y);
            if ((lar_x % 2) + (lar_y % 2) > 0)
            {
                if (lab_inst[lar_y][lar_x] == '#')
                    map->spawn(x, y, C::WALL, false);
                continue;
            }
        }

    // LAYER 2 - Technical anchors (C::ANCHOR)
    for (int x = 0; x < MAP_SIZE; x++)
        for (int y = 0; y < MAP_SIZE; y++)
        {
            // Generating room anchors if suitable position
            if (x % (SECTOR_SIZE + 1) == 3 && y % (SECTOR_SIZE + 1) == 3)
            {
                // Get lar coordinates
                int lar_x = get_lar(x);
                int lar_y = get_lar(y);

                // Checking directions
                bool N = map->get_tile(x, y + 3) != C::WALL;
                bool S = map->get_tile(x, y - 3) != C::WALL;
                bool E = map->get_tile(x + 3, y) != C::WALL;
                bool W = map->get_tile(x - 3, y) != C::WALL;

                // Checking if suitable position
                if (N + S + E + W == 1 && ms_twister() % 1000 < BLIND_ROOM_APPEAR_CHANCE)
                {
                    // Blind route room
                    Tile& tile = map->spawn(x, y, C::ANCHOR, false);
                    if (N) tile.set_score(1);
                    else if (S) tile.set_score(2);
                    else if (E) tile.set_score(3);
                    else if (W) tile.set_score(4);
                }
                else if (N + S + E + W == 2 && (N + S == 2 || E + W == 2) && ms_twister() % 1000 < LINEAR_ROOM_APPEAR_CHANCE)
                {
                    // Linear route room
                    Tile& tile = map->spawn(x, y, C::ANCHOR, false);
                    if (N + S == 2) tile.set_score(5);
                    if (E + W == 2) tile.set_score(6);
                }
                else if(ms_twister() % 1000 < UNDEFINED_ROOM_APPEAR_CHANCE)
                {
                    // Undefined type of room
                    Tile& tile = map->spawn(x, y, C::ANCHOR, false);
                    tile.set_score(0);
                }

                continue;
            }
        }

    // LAYER 3 - Player spawn
    int spawn_xy = (MAP_SIZE - 1) / 2;
    map->spawn(spawn_xy, spawn_xy, C::PLAYER);
    
    // LAYER 4 - Random field fill
    for (int x = 0; x < MAP_SIZE; x++)
        for (int y = 0; y < MAP_SIZE; y++)
        {
            if (map->get_tile(x, y) == ' ')
            {
                int rnd = ms_twister() % 1000;
                
                if (rnd >= 0 && rnd <= 24)
                    map->spawn(x, y, C::BLOCK, false);

                else if (rnd >= 25 && rnd <= 49)
                    map->spawn(x, y, C::NUMBER, false).set_health(ms_twister() % 3 + 2);

                else if (rnd >= 50 && rnd <= 59)
                    map->spawn(x, y, C::ANIMAL, true);
            }
        }

    // LAYER 5 - Rooms expand from anchors
    for (int x = 0; x < MAP_SIZE; x++)
        for (int y = 0; y < MAP_SIZE; y++)
        {
            if (map->get_tile(x, y) == C::ANCHOR)
            {
                Tile& tile = map->get_tile_ref(x, y);
                int connection_mode = tile.get_score();
                int rand_entity = ms_twister() % 1000;

                // Choose room type
                string chosen_room = "";
                if (connection_mode >= 1 && connection_mode <= 4) // single gate
                {
                    if (rand_entity >= 0 && rand_entity <= 499) chosen_room = "animal_room";
                    else if (rand_entity >= 500 && rand_entity <= 999) chosen_room = "monster_room";
                }
                else if (connection_mode >= 5 && connection_mode <= 6) // two linear gates
                {
                    if (rand_entity >= 0 && rand_entity <= 332) chosen_room = "fruit_storage";
                    if (rand_entity >= 333 && rand_entity <= 665) chosen_room = "corridor";
                    if (rand_entity >= 666 && rand_entity <= 999) chosen_room = "spawner_room";
                }
                else if (connection_mode == 0) // other configurations
                {
                    if (rand_entity >= 0 && rand_entity <= 499) chosen_room = "spawner_room";
                    if (rand_entity >= 500 && rand_entity <= 999) chosen_room = "middle_square";
                }

                // Fill walls
                string wall_pattern = "-------";
                if (chosen_room == "animal_room")
                {
                    wall_pattern = "##BBB##";
                }
                else if (chosen_room == "monster_room")
                {
                    wall_pattern = "##111##";
                }
                else if (chosen_room == "fruit_storage" || chosen_room == "spawner_room")
                {
                    wall_pattern = "##   ##";
                }
                fill_wall(map, x - 3, y, true, wall_pattern);
                fill_wall(map, x + 3, y, true, wall_pattern);
                fill_wall(map, x, y - 3, false, wall_pattern);
                fill_wall(map, x, y + 3, false, wall_pattern);
                
                // Create rooms
                for (int x1 = x - 2; x1 <= x + 2; x1++)
                    for (int y1 = y - 2; y1 <= y + 2; y1++)
                    {
                        if (chosen_room == "animal_room")
                        {
                            const int ANIMAL_ROOM_CHANCE = 220; // (promiles)
                            if (ms_twister() % 1000 < ANIMAL_ROOM_CHANCE)
                                map->spawn(x1, y1, C::ANIMAL, true);
                            else
                                map->spawn(x1, y1, ' ', false);
                        }
                        else if (chosen_room == "monster_room")
                        {
                            const int MONSTER_ROOM_CHANCE = 160; // (promiles)
                            if (ms_twister() % 1000 < MONSTER_ROOM_CHANCE)
                                map->spawn(x1, y1, C::MONSTER, true);
                            else
                                map->spawn(x1, y1, ' ', false);
                        }
                        else if (chosen_room == "fruit_storage")
                        {
                            const int FRUIT_ROOM_CHANCE = 80; // (promiles)
                            if (ms_twister() % 1000 < FRUIT_ROOM_CHANCE)
                                map->spawn(x1, y1, C::FRUIT, false);
                            else
                                map->spawn(x1, y1, ' ', false);
                        }
                        else if (chosen_room == "spawner_room")
                        {
                            if (x1 == x && y1 == y)
                                map->spawn(x1, y1, C::SPAWNER, true, true);
                            else if (
                                (x1 == x - 2 && y1 == y - 2) ||
                                (x1 == x + 2 && y1 == y - 2) ||
                                (x1 == x + 2 && y1 == y + 2) ||
                                (x1 == x - 2 && y1 == y + 2)
                                )
                                map->spawn(x1, y1, C::BLOCK, false, true);
                            else
                                map->spawn(x1, y1, ' ', false);
                        }
                        else if (chosen_room == "middle_square")
                        {
                            if (x1 == x && y1 == y)
                                map->spawn(x1, y1, C::NUMBER, false).set_health(ms_twister() % 10);
                            else if (x1 >= x - 1 && x1 <= x + 1 && y1 >= y - 1 && y1 <= y + 1)
                                map->spawn(x1, y1, C::WALL, false);
                        }
                    }
            }
        }

    // LAYER 6 - Decorational gates in random positions

    delete[] lab_inst;
}
