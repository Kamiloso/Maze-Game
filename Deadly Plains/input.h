#pragma once

struct Coords;

void reset_inputs();
bool is_space_pressed();
bool is_escape_pressed();
Coords get_movement_input();
Coords get_shooting_input();
