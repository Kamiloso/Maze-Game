#include <iostream>
#include <sstream>
#include <conio.h>

#include "console.h"
#include "menu.h"
#include "main.h"
#include "spawning.h"

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
		cout << "  Score: " << Difficulty::score_to_str(score);
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

		// Here CHEAT CODES can be inserted
		for (int i = 0; i <= 15; i++)
		{
			stringstream ss1;
			ss1 << "practice-" << get_difficulty_by_id(i).get_id_str();
			if (out == ss1.str())
			{
				set_next_debug_phase(i);
				return -1;
			}

			stringstream ss2;
			ss2 << "recent-seed-practice-" << get_difficulty_by_id(i).get_id_str();
			if (out == ss2.str())
			{
				set_next_debug_phase(i);
				return -2;
			}
		}
	}
}

unsigned int seed_menu()
{
	while (true)
	{
		stringstream ss;
		ss << get_last_seed();
		string str_last_seed = ss.str();
		if (str_last_seed == "0")
			str_last_seed = "only if you've played before";

		clear_screen();
		display_header(0b011);
		display_menu({
			string(3, C::LINE_HORIZONTAL) + string(" SET SEED & PLAY ") + string(3, C::LINE_HORIZONTAL),
			"",
			" What seed do you want to use?",
			"",
			" - Enter any 32-bit positive number as a seed e.g. '12345'.",
			" - Enter any text to convert it into a seed e.g 'maze game'.",
			" - Enter '0' or 'recent' to use the last used seed (" + str_last_seed + ").",
			" - Enter '-1' or 'back' to return to the main menu.",
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
		" TIP: To practice a specific phase, type 'practice-00' or 'recent-seed-practice-00'",
		"      in the main menu. You can choose any number between 00 and 15.",
		"",
	});
	any_key_delay();
}

void phases_menu()
{
	clear_screen();
	display_header(0b101);

	// Clear all previous screen characters (printing using console.h)
	fill_all_characters({ ' ' });

	// Horizontal lines
	fill_characters({ C::LINE_HORIZONTAL }, { 0,0 }, { 38,0 });
	fill_characters({ C::LINE_HORIZONTAL }, { 0,2 }, { 38,2 });
	fill_characters({ C::LINE_HORIZONTAL }, { 0,18 }, { 38,18 });

	// Vertical lines
	fill_characters({ C::LINE_VERTICAL }, { 0,0 }, { 0,18 });
	fill_characters({ C::LINE_VERTICAL }, { 5,0 }, { 5,18 });
	fill_characters({ C::LINE_VERTICAL }, { 24,0 }, { 24,18 });
	fill_characters({ C::LINE_VERTICAL }, { 38,0 }, { 38,18 });

	// Corners y = 0
	set_character(0, 0, { C::LINE_SE });
	set_character(5, 0, { C::LINE_NOT_N });
	set_character(24, 0, { C::LINE_NOT_N });
	set_character(38, 0, { C::LINE_SW });

	// Corners y = 2
	set_character(0, 2, { C::LINE_NOT_W });
	set_character(5, 2, { C::LINE });
	set_character(24, 2, { C::LINE });
	set_character(38, 2, { C::LINE_NOT_E });

	// Corners y = 18
	set_character(0, 18, { C::LINE_NE });
	set_character(5, 18, { C::LINE_NOT_S });
	set_character(24, 18, { C::LINE_NOT_S });
	set_character(38, 18, { C::LINE_NW });

	// Text inserting
	insert_text("ID", { 2,1 }, 2);
	insert_text("GAME PHASE", { 10,1 }, 10);
	insert_text("SCORE", { 29,1 }, 5);
	fill_characters({ '-' }, { 31,3 }, { 31,17 });
	bool first_unknown = true;
	for (int i = 1; i <= 15; i++)
	{
		Difficulty difficulty = get_difficulty_by_id(i);

		ColoredString name;
		string str_score_min, str_score_max;
		if (get_highscore() >= difficulty.get_score_min() && discovered_phase_01()) // Known difficulty
		{
			name = difficulty.get_name();
			str_score_min = Difficulty::score_to_str(difficulty.get_score_min());
			str_score_max = Difficulty::score_to_str(difficulty.get_score_max());
		}
		else // Unknown difficulty
		{
			name = { string(16, '?'), COLOR::DARK_GRAY };
			str_score_min = "?";
			str_score_max = "?";

			if (first_unknown)
			{
				str_score_min = Difficulty::score_to_str(difficulty.get_score_min());
				first_unknown = false;
			}
		}

		// ID numbers
		insert_text(difficulty.get_id_str(), { 2,2 + i }, 2);
		
		// PHASE names
		insert_text_backwards(name.str, { 22,2 + i }, 16, name.color);

		// SCORE info
		insert_text_backwards(str_score_min, { 29,2 + i }, 4);
		insert_text(str_score_max, { 33,2 + i }, 4);
	}

	print_part_of_screen(0, 19, 2);
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
	ss << " Score: " << Difficulty::score_to_str(score);
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
