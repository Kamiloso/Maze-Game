#include <random>

#include "entities.h"
#include "common.h"

// Default constructor
Tile::Tile(char _id, Coords bul_vect, bool by_player)
{
	if (_id == ' ') return;
	id = _id;

	if (id == C::BULLET)
	{
		if (bul_vect == Coords{ 0,1 }) bullet_movement = 'N';
		else if (bul_vect == Coords{ 1,0 }) bullet_movement = 'E';
		else if (bul_vect == Coords{ 0,-1 }) bullet_movement = 'S';
		else if (bul_vect == Coords{ -1,0 }) bullet_movement = 'W';
		shot_by_player = by_player;
	}
	initialize_tile_values();
}

// Initializes default tile values depending on their type
void Tile::initialize_tile_values()
{
	switch (id)
	{
	case C::PLAYER:
		health = MAX_PLAYER_HEALTH;
		break;

	case C::ANIMAL:
		health = 1;
		reward_score = 2;
		break;

	case C::MONSTER:
		health = 5;
		reward_score = 6;
		break;

	case C::SNIPER:
		health = 3;
		reward_score = 8;
		break;

	case C::INSECTOR:
		health = 10;
		reward_score = 20;
		break;

	case C::INSECT:
		health = 1;
		reward_score = 3;
		break;

	case C::SPAWNER:
		health = 10;
		reward_score = 20;
		break;

	case C::BULLET:
		health = 1;
		break;

	case C::FRUIT:
		health = 1;
		break;

	case C::NUMBER:
		health = 4;
		break;
	}
}

// Health setter
void Tile::set_health(char value)
{
	health = value;
}

// Score setter
void Tile::set_score(char value)
{
	if (value >= 0)
		reward_score = value;
}

// ID getter
char Tile::get_id() const
{
	return id;
}

// Health getter
char Tile::get_health() const
{
	return health;
}

// Score getter
char Tile::get_score() const
{
	return reward_score;
}

// Check if was shot by player
bool Tile::was_shot_by_player() const
{
	return shot_by_player;
}

// Heals entity by one hp and returns whether it was healed
bool Tile::heal_by_one(char max_health)
{
	dmg_show_time = DAMAGE_SHOW_TIME;
	dmg_show_from_dmg = false;

	if (health != max_health)
	{
		health++;
		return true;
	}
	else return false;
}

// Damages entity by one hp and returns whether health has decremented to 0
bool Tile::damage_by_one()
{
	if (health == -1) return false;

	dmg_show_time = DAMAGE_SHOW_TIME;
	dmg_show_from_dmg = true;

	health--;
	return health == 0;
}

// Mark tile as acted temporarily
void Tile::mark_as_acted(char cooldown)
{
	action_cooldown = cooldown;
}

// Checks if can act now
bool Tile::can_act_now() const
{
	return action_cooldown == 0;
}

// Checks if can act now
unsigned char Tile::is_dmg_visible() const
{
	if (dmg_show_time == 0)
		return 0; // invisible
	else if (dmg_show_from_dmg)
		return 1; // visible red
	else
		return 2; // visible green
}

// Decrements action_cooldown if possible
void Tile::action_decrement()
{
	if (action_cooldown != 0)
		action_cooldown--;
}

// Decrements dmg_show_time if possible
void Tile::dmg_show_decrement()
{
	if (dmg_show_time != 0)
		dmg_show_time--;
}

// Checks if this function is called the first time
bool Tile::check_initialization()
{
	if (initialization_flag)
	{
		initialization_flag = false;
		return true;
	}
	else return false;
}

// Returns bullet movement coordinates
Coords Tile::get_bullet_movement() const
{
	if (bullet_movement == 'N') return { 0,1 };
	if (bullet_movement == 'E') return { 1,0 };
	if (bullet_movement == 'S') return { 0,-1 };
	if (bullet_movement == 'W') return { -1,0 };
	return { 0,0 };
}

void Tile::execute_behaviour()
{
	;
}
