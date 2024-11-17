#pragma once

#include "common.h"

Tile::Tile(char _id)
{
	id = _id;
	switch (id)
	{
	case C::PLAYER:
		health = MAX_PLAYER_HEALTH;
		break;

	case C::ANIMAL:
		health = 1;
		break;

	case C::KNIGHT:
		health = 5;
		break;

	case C::BULLET:
		health = 1;
		break;
	case C::FRUIT:
		health = 1;
		break;
	}
}
