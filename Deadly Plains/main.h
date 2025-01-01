#pragma once

#include <string>

void set_highscore(int _highscore);
int get_highscore();
void set_last_seed(unsigned int _last_seed);
unsigned int get_last_seed();
void playing_phase_01();
bool discovered_phase_01();
void allow_frame_delay();
void set_next_debug_phase(int _debug_phase);

void load_all_data();
void save_all_data(int not_updated_score = 0);
