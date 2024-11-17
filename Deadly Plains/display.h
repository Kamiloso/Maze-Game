#pragma once

#include "common.h"

class Map;

struct DisplayData
{
	Coords center = { 0,0 };
	int difficulty = 0;
	int score = 0;
	int health = 0;
};

void display(Map* map, DisplayData ddt);
