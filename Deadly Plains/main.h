#pragma once

#include <string>

void set_highscore(int _highscore);
int get_highscore();
void set_last_seed(unsigned int _last_seed);
unsigned int get_last_seed();
void allow_frame_delay();

void load_all_data();
void save_all_data(int not_updated_score = 0);
