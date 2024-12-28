#include <iostream>
#include <sstream>

#include "console.h"
#include "menu.h"
#include "main.h"

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
		cout << "  Recent seed: " << get_last_seed() << endl;
		written_anything_2 = true;
	}

	if (score != -1)
	{
		cout << "  Score: " << score << endl;
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

// Asks the user to press any key to continue
static void any_key_delay()
{
	cout << " Press any key to continue..." << endl;
	cursor_set_active(true);
	system("pause >nul");
	cursor_set_active(false);
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
			" 1. Play",
			" 2. Set seed & play",
			" 3. Instructions",
			" 4. Known phases",
			" 5. Quit",
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
			" - Enter any positive number (or text) as a seed.",
			" - Enter 'recent' or '0' to use the recent seed.",
			" - Enter 'back' or '-1' to return to the main menu.",
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

		unsigned int build_seed = 0;
		while (!out.empty())
		{
			build_seed *= 10;
			build_seed += out[0] - '0';
			out = out.substr(1);
		}
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
		" - The tutorial is not ready yet.",
		" - Come back in January.",
		" - The horse will be with you!",
		"",
	});
	any_key_delay();
}

void phases_menu()
{
	clear_screen();
	display_header(0b101);
	display_menu({
		string(3, C::LINE_HORIZONTAL) + string(" KNOWN PHASES ") + string(3, C::LINE_HORIZONTAL),
		"",
		" 1 - BLAH BLAH",
		" 2 - OTHER DIFF",
		" 3 - THE HORSES",
		" 4 - ???",
		"",
		});
	any_key_delay();
}

int pause_menu(int score)
{
	while (true)
	{
		clear_screen();
		display_header(0b101, score);
		display_menu({
			string(3, C::LINE_HORIZONTAL) + string(" PAUSE ") + string(3, C::LINE_HORIZONTAL),
			"",
			" 1. Resume game",
			" 2. Give up",
			"",
			});

		string out = ask_for_text();
		if (out == "1") return 1;
		if (out == "2") return 2;
	}
}

void you_died_menu(int score, string info)
{
	stringstream ss;
	ss << " Score: " << score;
	info = string(3, C::LINE_HORIZONTAL) + string(" ") + info + string(" ") + string(3, C::LINE_HORIZONTAL);

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
		set_highscore(score);
		display_header(0b111);
		display_menu({
			info,
			"",
			" New record!",
			ss.str(),
			"",
		});
	}
	any_key_delay();
}
