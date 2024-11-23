#include <random>

#include "entities.h"
#include "common.h"

Tile::Tile(char _id)
{
	id = _id;

	// Player
	if (id == C::PLAYER) {
		health = MAX_PLAYER_HEALTH;
		has_score = 0;
	}

	// Animal
	if (id == C::ANIMAL) {
		health = 1;
		has_score = 2;
	}

	// Monster
	if (id == C::MONSTER) {
		health = 5;
		has_score = 6;
	}

	// Sniper
	if (id == C::SNIPER) {
		health = 3;
		has_score = 8;
	}

	// Insector
	if (id == C::INSECTOR) {
		health = 10;
		has_score = 20;
	}

	// Insect
	if (id == C::INSECT) {
		health = 1;
		has_score = 3;
	}

	// Spawner
	if (id == C::SPAWNER) {
		
	}

	// Bullet
	if (id == C::BULLET) {
		health = 1;
		has_score = 0;
	}

	// Fruit
	if (id == C::FRUIT) {
		health = 1;
		has_score = 0;
	}

	// Number
	if (id == C::NUMBER) {
		health = 9;
	}
}

// Default constructor
Tile2::Tile2(char _id, Coords bul_vect, bool by_player)
{
	if (_id == ' ') return;

	bool dry = false;
	id = _id;
	if (id == C::INSECT_DRIED)
	{
		id = C::INSECT;
		dry = true;
	}

	if (id == C::BULLET)
	{
		if (bul_vect == Coords{ 0,1 }) bullet_movement = 'N';
		else if (bul_vect == Coords{ 1,0 }) bullet_movement = 'E';
		else if (bul_vect == Coords{ 0,-1 }) bullet_movement = 'S';
		else if (bul_vect == Coords{ -1,0 }) bullet_movement = 'W';
		shot_by_player = by_player;
	}
	initialize_tile_values(dry);
}

// Hard set health constructor
Tile2::Tile2(char _id, char hard_set_health)
{
	if (_id == ' ') return;

	bool dry = false;
	id = _id;
	if (id == C::INSECT_DRIED)
	{
		id = C::INSECT;
		dry = true;
	}

	initialize_tile_values(dry);
	health = hard_set_health;
}

// Initializes default tile values depending on their type
void Tile2::initialize_tile_values(bool dry)
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
		health = 1;
		reward_score = 6;
		break;

	case C::SNIPER:
		health = 1;
		reward_score = 8;
		break;

	case C::INSECTOR:
		health = 1;
		reward_score = 20;
		break;

	case C::INSECT:
		health = 1;
		reward_score = 3;
		break;

	case C::SPAWNER:
		health = 1;
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

	if (dry)
		reward_score = 0;
}

// Health getter
char Tile2::get_health() const
{
	return health;
}

// Stored score getter
char Tile2::get_score() const
{
	return reward_score;
}

// Heals entity by one hp and returns whether it was healed
bool Tile2::heal_by_one(char max_health)
{
	dmg_show_time = DAMAGE_SHOW_TIME;
	dmg_show_from_dmg = false;

	if (health + 1 != max_health)
	{
		health++;
		return true;
	}
	else return false;
}

// Damages entity by one hp and returns whether health has decremented to 0
bool Tile2::damage_by_one()
{
	if (health == -1) return false;

	dmg_show_time = DAMAGE_SHOW_TIME;
	dmg_show_from_dmg = true;

	health--;
	return health == 0;
}

// Mark tile as acted temporarily
void Tile2::mark_as_acted(char cooldown)
{
	action_cooldown = cooldown;
}

// Checks if can act now
bool Tile2::can_act_now() const
{
	return action_cooldown == 0;
}

// Decrements action_cooldown if possible
void Tile2::action_decrement()
{
	if (action_cooldown != 0)
		action_cooldown--;
}

// Decrements dmg_show_time if possible
void Tile2::dmg_show_decrement()
{
	if (dmg_show_time != 0)
		dmg_show_time--;
}

// Checks if this function is called the first time
bool Tile2::check_initialization()
{
	if (initialization_flag)
	{
		initialization_flag = false;
		return true;
	}
	else return false;
}

// Returns bullet movement coordinates
Coords Tile2::get_bullet_movement() const
{
	if (bullet_movement == 'N') return { 0,1 };
	if (bullet_movement == 'E') return { 1,0 };
	if (bullet_movement == 'S') return { 0,-1 };
	if (bullet_movement == 'W') return { -1,0 };
	return { 0,0 };
}
