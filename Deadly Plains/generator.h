#pragma once

#include <random>

class Tile;

void generate(Tile** tiles, std::mt19937& ms_twister);
