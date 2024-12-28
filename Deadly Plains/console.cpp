#include <iostream>
#include <windows.h>

#include "console.h"
#include "common.h"

// Static console characters array
static ConsoleChar console_characters[CONSOLE_SIZE_X][CONSOLE_SIZE_Y]{};

// Sets text and background color for the console output
void set_color(unsigned char text_color)
{
	static unsigned char last_argument = COLOR::LIGHT_GRAY;
	if (text_color != last_argument)
	{
		last_argument = text_color;
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, text_color);
	}
}

// Moves cursor up certain number of lines
void move_cursor_up(int lines)
{
	if (lines == -1)
		std::cout << "\033[H";
	else
		std::cout << "\033[" << lines << "A";
}

// Sets the cursor to be visible or not
void cursor_set_active(bool set_active)
{
	if (set_active)
		std::cout << "\x1B[?25h";
	else
		std::cout << "\x1B[?25l";
}

// Clears the screen with CLS
void clear_screen()
{
	system("cls");
}

// Returns the character from a specific position
ConsoleChar get_character(int x, int y)
{
	if (x < 0 || y < 0 || x >= CONSOLE_SIZE_X || y >= CONSOLE_SIZE_Y)
		return { '\0' };
	else
		return console_characters[x][y];
}

// Sets the character in a specific position
void set_character(int x, int y, ConsoleChar character)
{
	if(get_character(x, y).character != '\0')
		console_characters[x][y] = character;
}

// Fills some space with one character
void fill_characters(ConsoleChar fill_char, Coords start, Coords end)
{
	if (end.x < start.x) std::swap(end.x, start.x);
	if (end.y < start.y) std::swap(end.y, start.y);

	for (int x = start.x; x <= end.x; x++)
		for (int y = start.y; y <= end.y; y++)
		{
			set_character(x, y, fill_char);
		}
}

// Inserts some text into a given space
void insert_text(std::string text, Coords start, int max_length, unsigned char color)
{
	int lngt = text.length();
	if (lngt > max_length)
		lngt = max_length;

	for (int i = 0; i < lngt; i++)
	{
		set_character(start.x + i, start.y, { text[i], color });
	}
}

// Creates a free area for a text and returns start coordinates for a text (max_length should be (size - 2))
Coords create_text_space(Coords start, int size)
{
	fill_characters({ ' ' }, start, { start.x + size - 1, start.y });
	return { start.x + 1, start.y };
}

// Sets all characters to a given type
void fill_all_characters(ConsoleChar fill_char)
{
	fill_characters(fill_char, { 0, 0 }, { CONSOLE_SIZE_X - 1, CONSOLE_SIZE_Y - 1 });
}

// Creates a box made of lines
void make_linebox(Coords start, Coords end)
{
	set_character(start.x, start.y, { C::LINE_SE });
	set_character(end.x, start.y, { C::LINE_SW });
	set_character(start.x, end.y, { C::LINE_NE });
	set_character(end.x, end.y, { C::LINE_NW });

	for (int x = start.x + 1; x < end.x; x++)
	{
		set_character(x, start.y, { C::LINE_HORIZONTAL });
		set_character(x, end.y, { C::LINE_HORIZONTAL });
	}

	for (int y = start.y + 1; y < end.y; y++)
	{
		set_character(start.x, y, { C::LINE_VERTICAL });
		set_character(end.x, y, { C::LINE_VERTICAL });
	}
}

// Clears screen and displays all characters
void reload_screen(bool do_offset)
{
	move_cursor_up();

	for (int y = 0; y < CONSOLE_SIZE_Y; y++)
	{
		if(do_offset)
			std::cout << " ";
		for (int x = 0; x < CONSOLE_SIZE_X; x++)
		{
			ConsoleChar con_char = get_character(x, y);
			if(con_char.character != ' ')
				set_color(con_char.color);
			std::cout << con_char.character;
		}
		std::cout << std::endl;
	}
}
