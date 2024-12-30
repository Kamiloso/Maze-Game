#include <iostream>
#include <sstream>
#include <conio.h>

#include "console.h"
#include "menu.h"
#include "main.h"
#include "difficulty.h"

// Displays the menu header
static void display_header(unsigned char info_flags = 0b001, int score = -1)
{
	const string MIDDLE_LINE = C::LINE_VERTICAL + string(" ") + string(FULL_VERSION) + string(" ") + C::LINE_VERTICAL;
	const string UPPER_LINE = C::LINE_SE + string(MIDDLE_LINE.length() - 2, C::LINE_HORIZONTAL) + C::LINE_SW;
	const string LOWER_LINE = C::LINE_NE + string(MIDDLE_LINE.length() - 2, C::LINE_HORIZONTAL) + C::LINE_NW;
	
	if ((info_flags >> 0) % 2 == 1)
	{
		cout << " " << UPPER_LINE << endl;
		cout << " " << MIDDLE_LINE << endl;
		cout << " " << LOWER_LINE << endl;
		cout << endl;
	}
	else cout << endl;

	bool written_anything_2 = false;

	if ((info_flags >> 1) % 2 == 1 && get_last_seed() != 0)
	{
		cout << "  Last used seed: " << get_last_seed() << endl;
		written_anything_2 = true;
	}

	if (score != -1)
	{
		cout << "  Score: " << score;
		if (score > get_highscore())
			cout << " - New record!";
		cout << endl;
		written_anything_2 = true;
	}

	if ((info_flags >> 2) % 2 == 1)
	{
		int highscore = get_highscore() > score ? get_highscore() : score;
		cout << "  Highscore: " << highscore << endl;
		written_anything_2 = true;
	}

	if (written_anything_2)
		cout << endl;
}

// Displays the menu
static void display_menu(vector<string> lines)
{
	for (auto it = lines.begin(); it != lines.end(); ++it)
		cout << " " << *it << endl;
}

// Asks the user for a text
static string ask_for_text()
{
	char text_var[1024]{};
	cout << " > ";
	cursor_set_active(true);
	cin.getline(text_var, 1024);
	if (cin.fail())
	{
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
	cursor_set_active(false);
	return string(text_var);
}

// Asks the user to press ENTER to continue
static void any_key_delay()
{
	cout << " Press ENTER to continue..." << endl;
	while (_kbhit()) {
		_getch();
	}
	while (true) {
		char c = _getch();
		if (c == '\n') break;
		if (c == '\r') break;
	}
}

// ----------------------------------------------- //
// ----------- INDIVIDUAL MENU METHODS ----------- //
// ----------------------------------------------- //

// Displays main menu
int main_menu()
{
	while (true)
	{
		clear_screen();
		display_header(0b111);
		display_menu({
			string(3, C::LINE_HORIZONTAL) + string(" MAIN MENU ") + string(3, C::LINE_HORIZONTAL),
			"",
			" 1 - Play",
			" 2 - Set seed & play",
			" 3 - Instructions",
			" 4 - Game phases",
			" 5 - Quit",
			"",
		});

		string out = ask_for_text();
		if (out == "1") return 1;
		if (out == "2") return 2;
		if (out == "3") return 3;
		if (out == "4") return 4;
		if (out == "5") return 5;
	}
}

unsigned int seed_menu()
{
	while (true)
	{
		clear_screen();
		display_header(0b011);
		display_menu({
			string(3, C::LINE_HORIZONTAL) + string(" SET SEED & PLAY ") + string(3, C::LINE_HORIZONTAL),
			"",
			" What seed do you want to use?",
			"",
			" - Enter any text as a seed. Text will be converted into a positive number.",
			" - Enter 'recent' or '0' to use the last used seed.",
			" - Enter 'back' or '-1' to return to the main menu.",
			"",
			" Type one of the options above.",
			"",
		});

		string out = ask_for_text();
		if (out == "recent" || out == "0")
		{
			unsigned int last_seed = get_last_seed();
			if (last_seed != 0)
				return last_seed;
			else
				continue;
		}
		if (out == "back" || out == "-1") return 0;
		if (out == "") continue;

		unsigned int build_seed = int_parse_any_string(out.c_str());
		if (build_seed == 0)
			build_seed = 1;

		return build_seed;
	}
}

void instructions_menu()
{
	clear_screen();
	display_header(0b001);
	display_menu({
		string(3, C::LINE_HORIZONTAL) + string(" INSTRUCTIONS ") + string(3, C::LINE_HORIZONTAL),
		"",
		" 1. Start gameplay from the main menu. You can play",
		"    with a random seed (option 1) or a set seed (option 2).",
		"",
		" 2. Move around using arrows, WASD or IJKL keys.",
		"",
		" 3. Hold space and use movement keys to shoot bullets.",
		"    You can't move while shooting.",
		"",
		" 4. Kill enemies and avoid their attacks.",
		"    Each killed enemy will add a few points to your score.",
		"",
		" 5. The higher your score, the harder gameplay becomes.",
		"    Check all phases you've discovered in the main menu (option 4).",
		"",
		" 6. Achieve the highest score possible before you die.",
		"    Good luck! :)",
		"",
	});
	any_key_delay();
}

void phases_menu()
{
	clear_screen();
	display_header(0b101);
	display_menu({
		" |----|------------------|-------------|",
		" | ID |    GAME PHASE    |    SCORE    |",
		" |----|------------------|-------------|",
		});

	for (int i = 1; i <= 15; i++)
	{
		cout << "  | ";
		if (i < 10) cout << "0" << i;
		else cout << i;
		cout << " | ";
		cout.width(16);
		set_color(COLOR::RED);
		if (i > 7) set_color(COLOR::DARK_RED);
		cout << "TRAINING AREA";
		set_color();
		cout << " | 0000 - 0000 |";
		cout << endl;
	}
	cout << "  |----|------------------|-------------|" << endl;
	cout << endl;

	any_key_delay();
}

int pause_menu(int score)
{
	save_all_data(score);

	while (true)
	{
		clear_screen();
		display_header(0b101, score);
		display_menu({
			string(3, C::LINE_HORIZONTAL) + string(" PAUSE ") + string(3, C::LINE_HORIZONTAL),
			"",
			" 1 - Resume game",
			" 2 - End the game",
			"",
			});

		string out = ask_for_text();
		if (out == "1") return 1;
		if (out == "2") return 2;
	}
}

void you_died_menu(int score)
{
	save_all_data(score);

	stringstream ss;
	ss << " Score: " << score;
	string info = string(3, C::LINE_HORIZONTAL) + string(" GAME OVER ") + string(3, C::LINE_HORIZONTAL);

	clear_screen();
	if (score <= get_highscore())
	{
		display_header(0b111);
		display_menu({
			info,
			"",
			ss.str(),
			"",
		});
	}
	else
	{
		ss << " - New record!";
		set_highscore(score);
		display_header(0b111);
		display_menu({
			info,
			"",
			ss.str(),
			"",
		});
	}
	any_key_delay();
}
