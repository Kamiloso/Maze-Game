#include <iostream>

#include "spawning.h"
#include "entities.h"
#include "map.h"

int get_amount_of(Tile** tiles, char type, int x1, int y1, int x2, int y2)
{
	if (x1 > x2) std::swap(x1, x2);
	if (y1 > y2) std::swap(y1, y2);

	int amount = 0;
	for (int x = x1; x <= x2; x++)
		for (int y = x2; y <= y2; y++)
		{
			if (tiles[x][y].get_id() == type)
				amount++;
		}
	
	return amount;
}
