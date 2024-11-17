#pragma once

#include <random>

struct Tile;

void generate(Tile** tiles, std::mt19937& ms_twister);
