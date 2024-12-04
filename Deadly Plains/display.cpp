#include <iostream>
#include <iomanip>
#include <windows.h>
#include <sstream>

#include "map.h"
#include "display.h"

using namespace std;

static string debug_info = "";

// Sets text and background color of the console
static void set_color(int text_color = 15, int bg_color = 0)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, text_color + (bg_color << 4));
}

// Checks whether given character represents wall
static bool is_wall_type(char c)
{
	if (c >= (char)0xC8 && c <= C::WALL) return true;
	if (c >= (char)0xB9 && c <= (char)0xBC) return true;
	return false;
}

// Displays the whole frame on a screen
void display(Map* map, const DisplayData disp_data)
{
	// Tile array declaration
	TileDisplay tiles[DISP_SIZE][DISP_SIZE];
	int x1 = disp_data.center.x - VISION_RANGE - 1;
	int y1 = disp_data.center.y - VISION_RANGE - 1;
	for (int x = 0; x < DISP_SIZE; x++)
		for (int y = 0; y < DISP_SIZE; y++)
		{
			tiles[x][y] = map->get_tile_display(x1 + x, y1 + y);
		}

	// Pixel array declaration
	const int ROWS = DISPLAY_ROWS;
	const int COLUMNS = DISPLAY_COLUMNS;
	TileDisplay pixels[COLUMNS][ROWS];
	for (int x = 0; x < COLUMNS; x++)
		for (int y = 0; y < ROWS; y++)
			pixels[x][y] = { ' ',COLOR::DARK_GRAY };

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
				if (!is_wall_type(pixels[x][y].character))
					continue;

				bool north = is_wall_type(pixels[x][y + 1].character);
				bool south = is_wall_type(pixels[x][y - 1].character);
				bool east = is_wall_type(pixels[x + 2][y].character);
				bool west = is_wall_type(pixels[x - 2][y].character);
				
				// line
				if ((north || south) && !east && !west)
					pixels[x][y].character = C::WALL_VERTICAL;
				if (!north && !south && (east || west))
					pixels[x][y].character = C::WALL_HORIZONTAL;

				// corner
				if (north && !south && east && !west)
					pixels[x][y].character = C::WALL_NE;
				if (north && !south && !east && west)
					pixels[x][y].character = C::WALL_NW;
				if (!north && south && east && !west)
					pixels[x][y].character = C::WALL_SE;
				if (!north && south && !east && west)
					pixels[x][y].character = C::WALL_SW;

				// triple
				if (!north && south && east && west)
					pixels[x][y].character = C::WALL_NOT_N;
				if (north && !south && east && west)
					pixels[x][y].character = C::WALL_NOT_S;
				if (north && south && !east && west)
					pixels[x][y].character = C::WALL_NOT_E;
				if (north && south && east && !west)
					pixels[x][y].character = C::WALL_NOT_W;
			}
			else // empty column
			{
				if (is_wall_type(pixels[x - 1][y].character) && is_wall_type(pixels[x + 1][y].character))
				{
					pixels[x][y].character = C::WALL_HORIZONTAL;
					pixels[x][y].color = pixels[x - 1][y].color; // copy color from neighbour
				}
			}
		}

	// Lines around map generate
	for (int x = 0; x < COLUMNS; x++)
		for (int y = 0; y < ROWS; y++)
		{
			// lines
			if (x == 0 || x == COLUMNS - 1)
				pixels[x][y].character = C::LINE_VERTICAL;
			if (y == 0 || y == ROWS - 1)
				pixels[x][y].character = C::LINE_HORIZONTAL;

			// corners
			if (x == 0 && y == 0)
				pixels[x][y].character = C::LINE_NE;
			if (x == COLUMNS - 1 && y == 0)
				pixels[x][y].character = C::LINE_NW;
			if (x == 0 && y == ROWS - 1)
				pixels[x][y].character = C::LINE_SE;
			if (x == COLUMNS - 1 && y == ROWS - 1)
				pixels[x][y].character = C::LINE_SW;

			// color set on borders
			if (x == 0 || x == COLUMNS - 1 || y == 0 || y == ROWS - 1)
				pixels[x][y].color = COLOR::LIGHT_GRAY;
		}

	// Final map display (temporary code)
	cout << "\033[" << DISPLAY_ROWS << "A"; // Clear screen (fast)
	cout << "\033[" << 4 << "A";

	int show_x = disp_data.center.x - (MAP_SIZE / 2);
	int show_y = disp_data.center.y - (MAP_SIZE / 2);
	cout << " | Seed: " << map->get_seed() << " |" << endl;
	cout << " | X: ";
	cout.width(4);
	cout << show_x;
	cout << " | Y: ";
	cout.width(4);
	cout << show_y;
	cout << " |" << endl;
	cout << " | Difficulty: " << disp_data.difficulty << " | ";
	cout << " Score: " << disp_data.score << " | ";
	cout << " Health: " << disp_data.health << " |" << endl;
	cout << " " << debug_info << endl;

	display_array(pixels);
}

// Displays an array of pixels (tiles) on a screen
void display_array(const TileDisplay pixels[DISPLAY_COLUMNS][DISPLAY_ROWS])
{
	char last_color = 255; // Set to something, which is never used

	for (int y = DISPLAY_ROWS - 1; y >= 0; y--)
	{
		cout << " ";
		for (int x = 0; x < DISPLAY_COLUMNS; x++)
		{
			// Change color if differs
			char current_color = pixels[x][y].color;
			if (last_color != current_color)
			{
				set_color(current_color);
				last_color = current_color;
			}

			// Display pixel
			cout << pixels[x][y].character;
		}
		cout << "\n";
	}
	set_color();
}

void debug_log(string log)
{
	debug_info = log;
}
