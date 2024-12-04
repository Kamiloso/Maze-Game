#pragma once

#include <string>

#include "common.h"

class Map;

struct DisplayData
{
	Coords center = { 0,0 };
	int difficulty = 0;
	int score = 0;
	int health = 0;
};

struct TileDisplay
{
	char character = ' ';
	char color = COLOR::LIGHT_GRAY;
};

void display(Map* map, DisplayData ddt);
void display_array(const TileDisplay pixels[DISPLAY_COLUMNS][DISPLAY_ROWS]);
void debug_log(string log);
