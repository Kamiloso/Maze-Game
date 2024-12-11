#pragma once

#include "common.h"

class Tile;
class Map;

struct EntityAmount
{
	char entity = ' ';
	int amount = 0;
};

int get_amount_of(Tile** tiles, char type, int x1, int y1, int x2, int y2);

