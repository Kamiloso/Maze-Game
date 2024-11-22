#include "entities.h"
#include "common.h"

using namespace std;

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
