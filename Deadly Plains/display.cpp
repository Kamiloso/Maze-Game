#include <iostream>
#include <iomanip>
#include <windows.h>
#include <sstream>

#include "display.h"
#include "map.h"
#include "console.h"

using namespace std;

// Checks whether given character represents wall
static bool is_wall_type(char c)
{
	if (c >= (char)0xC8 && c <= C::WALL) return true;
	if (c >= (char)0xB9 && c <= (char)0xBC) return true;
	return false;
}

// Displays the whole frame on a screen
void display(Map* map, DisplayData& disp_data)
{
	// Tile array declaration
	ConsoleChar tiles[DISP_SIZE][DISP_SIZE];
	int x1 = disp_data.player_pos.x - VISION_RANGE - 1;
	int y1 = disp_data.player_pos.y - VISION_RANGE - 1;
	for (int x = 0; x < DISP_SIZE; x++)
		for (int y = 0; y < DISP_SIZE; y++)
		{
			tiles[x][y] = map->get_tile_display(x1 + x, y1 + y);
		}

	// Pixel array declaration
	const int ROWS = DISPLAY_ROWS;
	const int COLUMNS = DISPLAY_COLUMNS;
	ConsoleChar pixels[COLUMNS][ROWS];
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

	// Fill special characters to mark undefined pixels
	fill_all_characters({ '?',8 });

	// Gameplay space create
	for(int x = 0 ; x < COLUMNS ; x++)
		for (int y = 0; y < ROWS ; y++)
		{
			set_character(x, y + 4, pixels[x][ROWS - 1 - y]);
		}

	// Gameplay contures create
	fill_characters({ C::LINE_HORIZONTAL }, { 0, 0 }, { 48, 0 });
	fill_characters({ C::LINE_HORIZONTAL }, { 0, 2 }, { 48, 2 });
	fill_characters({ C::LINE_VERTICAL }, { 0, 1 }, { 48, 1 });
	fill_characters({ C::LINE_VERTICAL }, { 0, 3 }, { 48, 3 });
	set_character(0, 0, { C::LINE_SE });
	set_character(0, 2, { C::LINE_NOT_W });
	set_character(0, 4, { C::LINE_NOT_W });
	set_character(48, 0, { C::LINE_SW });
	set_character(48, 2, { C::LINE_NOT_E });
	set_character(48, 4, { C::LINE_NOT_E });

	// Gameplay menu create
	stringstream ss;

	insert_text("SCORE: ", create_text_space({ 1, 3 }, 27), 25);
	insert_text(Difficulty::score_to_str(disp_data.score) + " / " + Difficulty::score_to_str(disp_data.next_score), {9, 3}, 18);

	insert_text("PHASE " + disp_data.difficulty_id + ": ", create_text_space({1, 1}, 27), 25);
	insert_text(disp_data.difficulty_name, { 12, 1 }, 15, disp_data.difficulty_color);

	ss.str("");  ss.clear();
	ss << "X: " << (disp_data.player_pos.x - MAP_SIZE / 2);
	insert_text(ss.str(), create_text_space({29, 1}, 9), 7);

	ss.str("");  ss.clear();
	ss << "Y: " << (disp_data.player_pos.y - MAP_SIZE / 2);
	insert_text(ss.str(), create_text_space({39, 1}, 9), 7);
	
	insert_text("HEALTH: ", create_text_space({ 29, 3 }, 19), 17);
	for (int i = 1; i <= 5; i++)
	{
		if(i <= disp_data.health)
			set_character(36 + i * 2, 3, { C::FRUIT, COLOR::BLUE });
		else
			set_character(36 + i * 2, 3, { C::BULLET, COLOR::DARK_GRAY});
	}

	// Gameplay menu contures create
	set_character(38, 0, { C::LINE_NOT_N });
	set_character(38, 2, { C::LINE_NOT_S });
	set_character(28, 0, { C::LINE_NOT_N });
	set_character(28, 2, { C::LINE });
	set_character(28, 4, { C::LINE_NOT_S });

	// Reload console
	reload_screen(1);
}
