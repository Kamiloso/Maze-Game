#include <random>
#include <vector>
#include <iostream>

#include "common.h"
#include "generator.h"
#include "entities.h"

using namespace std;

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

    // Connecting
    vector<Coords> all_wall_spaces;
    for (int i = 0; i < LAB_ARRAY_SIZE; i++)
        for (int j = 0; j < LAB_ARRAY_SIZE; j++)
        {
            int r_sum = (i % 2) + (j % 2);
            if (r_sum == 1)
                all_wall_spaces.push_back({j,i});
        }

    while (true)
    {
        reset_marked_spaces(spaces);
        int rand_x = 2 * (ms_twister() % SECTORS_IN_LINE);
        int rand_y = 2 * (ms_twister() % SECTORS_IN_LINE);
        int connected_sectors = mark_spaces(spaces, rand_x, rand_y);
        if (connected_sectors == TOTAL_SECTORS)
            break;

        std::shuffle(all_wall_spaces.begin(), all_wall_spaces.end(), ms_twister);

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

static int get_lar(int l) // Translates map coordinates into labirynth instructions coordinates
{
    int on_sector = l / (SECTOR_SIZE + 1);
    int additional_tiles = l % (SECTOR_SIZE + 1);

    if (additional_tiles == 0)
        return on_sector * 2 - 1;
    else
        return on_sector * 2;
}

void generate(Tile** tiles, std::mt19937& ms_twister)
{
    string* lab_inst = labirynth_instructions(ms_twister);

    for (int x = 0; x < MAP_SIZE; x++)
        for (int y = 0; y < MAP_SIZE; y++)
        {
            // Border creating, WARNING: these borders prevent wrong array indexes. Don't remove them!
            // or at least replace them with something unbreakable and unmovable.
            if (x == 0 || x == MAP_SIZE - 1 || y == 0 || y == MAP_SIZE - 1)
            {
                tiles[x][y] = Tile(C::WALL);
                continue;
            }

            // Labirynth wall creating
            int lar_x = get_lar(x);
            int lar_y = get_lar(y);
            if ((lar_x % 2) + (lar_y % 2) > 0)
            {
                if (lab_inst[lar_y][lar_x] == '#')
                    tiles[x][y] = Tile(C::WALL);
                continue;
            }

            // Other tiles empty
            tiles[x][y] = Tile();
        }

    delete[] lab_inst;
}
