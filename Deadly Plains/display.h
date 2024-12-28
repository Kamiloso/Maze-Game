#pragma once

#include <string>

#include "common.h"

class Map;

struct DisplayData
{
	Coords player_pos = { 0,0 };
	std::string difficulty_id = "00";
	std::string difficulty_name = "UNKNOWN";
	unsigned char difficulty_color = COLOR::LIGHT_GRAY;
	char health = 0;
	int score = 0;
	int next_score = 0;
};

void display(Map* map, DisplayData& disp_data);
