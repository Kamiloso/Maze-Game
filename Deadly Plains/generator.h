#pragma once

#include <random>

#include "common.h"

class Map;

void generate(Map* map, std::mt19937& ms_twister);
