#pragma once

#include <string>

#include "common.h"

constexpr int CONSOLE_SIZE_X = 49;
constexpr int CONSOLE_SIZE_Y = 29;

struct ConsoleChar
{
	char character = ' ';
	unsigned char color = 0x07;
};

void cursor_set_active(bool set_active);
void clear_screen();
ConsoleChar get_character(int x, int y);
void set_character(int x, int y, ConsoleChar character);
void fill_characters(ConsoleChar fill_char, Coords start, Coords end);
void insert_text(std::string text, Coords start, int max_length, unsigned char color = 0x07);
Coords create_text_space(Coords start, int size);
void naturalize_lines();
void reload_screen();
