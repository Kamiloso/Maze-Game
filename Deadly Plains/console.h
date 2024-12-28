#pragma once

#include <string>

#include "common.h"

constexpr int CONSOLE_SIZE_X = 49;
constexpr int CONSOLE_SIZE_Y = 29;

struct ConsoleChar
{
	char character = ' ';
	unsigned char color = COLOR::LIGHT_GRAY;
};

void set_color(unsigned char text_color = COLOR::LIGHT_GRAY);
void move_cursor_up(int lines = -1);
void cursor_set_active(bool set_active);
void clear_screen();
ConsoleChar get_character(int x, int y);
void set_character(int x, int y, ConsoleChar character);
void fill_characters(ConsoleChar fill_char, Coords start, Coords end);
void insert_text(std::string text, Coords start, int max_length, unsigned char color = COLOR::LIGHT_GRAY);
Coords create_text_space(Coords start, int size);
void fill_all_characters(ConsoleChar fill_char);
void make_linebox(Coords start, Coords end);
void reload_screen(bool do_offset);
