#include <iostream>
#include <iomanip>
#include <windows.h>

#include "map.h"
#include "display.h"

static void setColor(int textColor = 15, int backgroundColor = 0)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, textColor + (backgroundColor << 4));
}

static int tile_color(char c)
{
	if (c >= '1' && c <= '9')
		return 12;

	switch (c)
	{
		// Map objects, Entities
		case C::BLOCK: return 14;
		case C::PLAYER: return 9;
		case C::ANIMAL: return 6;
		case C::MONSTER: return 6;
		case C::SNIPER: return 6;
		case C::INSECTOR: return 11;
		case C::INSECT: return 6;
		case C::INSECT_DRIED: return 11;
		case C::SPAWNER: return 10;
		case C::BULLET: return 4;
		case C::FRUIT: return 10;

		// Walls
		case C::WALL:
		case C::WALL_VERTICAL:
		case C::WALL_HORIZONTAL:
		case C::WALL_NE:
		case C::WALL_NW:
		case C::WALL_SE:
		case C::WALL_SW:
		case C::WALL_NOT_N:
		case C::WALL_NOT_E:
		case C::WALL_NOT_S:
		case C::WALL_NOT_W:
			return 14;
	
		// Lines
		case C::LINE_VERTICAL:
		case C::LINE_HORIZONTAL:
		case C::LINE_NE:
		case C::LINE_NW:
		case C::LINE_SE:
		case C::LINE_SW:
		case C::LINE_NOT_N:
		case C::LINE_NOT_E:
		case C::LINE_NOT_S:
		case C::LINE_NOT_W:
			return 14;
	
		// Default case
		default: return 14;
	}
}

static bool is_wall_type(char c)
{
	if (c >= (char)0xC8 && c <= C::WALL) return true;
	if (c >= (char)0xB9 && c <= (char)0xBC) return true;
	return false;
}

void display(Map* map, DisplayData ddt)
{
	// Tile array declaration
	const int DISP_SIZE = 2 * VISION_RANGE + 3;
	char tiles[DISP_SIZE][DISP_SIZE];
	int x1 = ddt.center.x - VISION_RANGE - 1;
	int y1 = ddt.center.y - VISION_RANGE - 1;
	for (int x = 0; x < DISP_SIZE; x++)
		for (int y = 0; y < DISP_SIZE; y++)
			tiles[x][y] = map->get_tile_display(x1 + x, y1 + y);

	// Pixel array declaration
	const int ROWS = DISP_SIZE;
	const int COLUMNS = DISP_SIZE * 2 - 1;
	char pixels[COLUMNS][ROWS];
	for (int x = 0; x < COLUMNS; x++)
		for (int y = 0; y < ROWS; y++)
			pixels[x][y] = ' ';

	// Tile into pixels data transfer
	for (int x = 0; x < DISP_SIZE; x++)
		for (int y = 0; y < DISP_SIZE; y++)
			pixels[x * 2][y] = tiles[x][y];

	// Walls proper type choose
	for (int x = 1; x < COLUMNS - 1; x++)
		for (int y = 1; y < ROWS - 1; y++)
		{
			if (x % 2 == 0) // tile exists
			{
				if (!is_wall_type(pixels[x][y]))
					continue;

				bool north = is_wall_type(pixels[x][y + 1]);
				bool south = is_wall_type(pixels[x][y - 1]);
				bool east = is_wall_type(pixels[x + 2][y]);
				bool west = is_wall_type(pixels[x - 2][y]);
				
				// line
				if ((north || south) && !east && !west)
					pixels[x][y] = C::WALL_VERTICAL;
				if (!north && !south && (east || west))
					pixels[x][y] = C::WALL_HORIZONTAL;

				// corner
				if (north && !south && east && !west)
					pixels[x][y] = C::WALL_NE;
				if (north && !south && !east && west)
					pixels[x][y] = C::WALL_NW;
				if (!north && south && east && !west)
					pixels[x][y] = C::WALL_SE;
				if (!north && south && !east && west)
					pixels[x][y] = C::WALL_SW;

				// triple
				if (!north && south && east && west)
					pixels[x][y] = C::WALL_NOT_N;
				if (north && !south && east && west)
					pixels[x][y] = C::WALL_NOT_S;
				if (north && south && !east && west)
					pixels[x][y] = C::WALL_NOT_E;
				if (north && south && east && !west)
					pixels[x][y] = C::WALL_NOT_W;
			}
			else // empty column
			{
				if (is_wall_type(pixels[x - 1][y]) && is_wall_type(pixels[x + 1][y]))
					pixels[x][y] = C::WALL_HORIZONTAL;
			}
		}

	// Lines around map generate
	for (int x = 0; x < COLUMNS; x++)
		for (int y = 0; y < ROWS; y++)
		{
			// lines
			if (x == 0 || x == COLUMNS - 1)
				pixels[x][y] = C::LINE_VERTICAL;
			if (y == 0 || y == ROWS - 1)
				pixels[x][y] = C::LINE_HORIZONTAL;

			// corners
			if (x == 0 && y == 0)
				pixels[x][y] = C::LINE_NE;
			if (x == COLUMNS - 1 && y == 0)
				pixels[x][y] = C::LINE_NW;
			if (x == 0 && y == ROWS - 1)
				pixels[x][y] = C::LINE_SE;
			if (x == COLUMNS - 1 && y == ROWS - 1)
				pixels[x][y] = C::LINE_SW;
		}

	// Final map display
	int reserve_rows = ROWS + 3;
	cout << "\033[" << reserve_rows << "A";

	int show_x = ddt.center.x - (MAP_SIZE / 2);
	int show_y = ddt.center.y - (MAP_SIZE / 2);

	cout << " | Seed: " << map->get_seed() << " |" << endl;
	cout << " | X: ";
	cout.width(4);
	cout << show_x;
	cout << " | Y: ";
	cout.width(4);
	cout << show_y;
	cout << " |" << endl;
	cout << " | Difficulty: " << ddt.difficulty << " | ";
	cout << " Score: " << ddt.score << " | ";
	cout << " Health: " << ddt.health << " |" << endl;

	for (int y = ROWS - 1; y >= 0; y--)
	{
		cout << " ";
		for (int x = 0; x < COLUMNS; x++)
		{
			setColor(tile_color(pixels[x][y]));
			cout << pixels[x][y];
		}
		cout << endl;
	}

	setColor();
}
