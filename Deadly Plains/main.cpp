#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>

#include "map.h"
#include "console.h"
#include "menu.h"
#include "main.h"
#include "files.h"

using namespace std;

static int highscore = 0;
static unsigned int last_seed = 0;
static unsigned int played_normally = 0;

static bool frame_could_be_delayed = false;
static int debug_phase = -1;

// Sets the console title to game version + additional debug info
static void title_debug_info(string info)
{
    if (info != "")
        info = "title " + string(FULL_VERSION) + " ; " + info;
    else
        info = "title " + string(FULL_VERSION);
    system(info.c_str());
}

// Highscore setter
void set_highscore(int _highscore)
{
    highscore = _highscore;
}

// Highscore getter
int get_highscore()
{
    return highscore;
}

// Last seed setter
void set_last_seed(unsigned int _last_seed)
{
    last_seed = _last_seed;
}

// Last seed getter
unsigned int get_last_seed()
{
    return last_seed;
}

// Informs that already playing normally
void playing_phase_01()
{
    played_normally = 1;
}

// Checks if already played normally
bool discovered_phase_01()
{
    return played_normally;
}

// Ignore next info about frame delay
void allow_frame_delay()
{
    frame_could_be_delayed = true;
}

// Sets next debug phase, which will be used
void set_next_debug_phase(int _debug_phase)
{
    debug_phase = _debug_phase;
}

// Loads all game data from file
void load_all_data()
{
    string read = read_from_file(L"mg_data.se3");
    string str_highscore = "";
    string str_last_seed = "";
    string str_played_normally = "";

    int reading_mode = 0;
    for (auto it = read.begin(); it != read.end(); ++it)
    {
        if (*it != ';')
        {
            if(reading_mode == 0)
                str_highscore += *it;

            if (reading_mode == 1)
                str_last_seed += *it;

            if (reading_mode == 2)
                str_played_normally += *it;
        }
        else reading_mode++;
    }

    //highscore is [int]
    unsigned int pre_highscore = int_parse_any_string(str_highscore.c_str());
    if (pre_highscore <= static_cast<unsigned int>(std::numeric_limits<int>::max()))
        highscore = pre_highscore;
    else
        highscore = 0;

    // last_seed is [unsigned int]
    last_seed = int_parse_any_string(str_last_seed.c_str());

    // played_normally is [unsigned int] (works as bool)
    played_normally = int_parse_any_string(str_played_normally.c_str());
}

// Saves all game data to file
void save_all_data(int not_updated_score)
{
    int score = highscore > not_updated_score ? highscore : not_updated_score;
    stringstream ss;
    ss << score << ";" << last_seed << ";" << played_normally;
    save_to_file(L"mg_data.se3", ss.str());
}

// Starts gameplay with a given (or random when empty) seed
static void play(unsigned int seed = 0)
{
    Map* map = new Map(seed, debug_phase);
    set_last_seed(map->get_seed());
    if (debug_phase == -1) playing_phase_01();

    int next_frame_wait = 0;
    while (map->end() == -1)
    {
        // Game loop
        std::this_thread::sleep_for(std::chrono::milliseconds(next_frame_wait));
        
        auto clock_before = chrono::steady_clock::now();
        map->frame_update();
        auto clock_after = chrono::steady_clock::now();

        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(clock_after - clock_before).count();
        next_frame_wait = FRAME_MS - static_cast<int>(milliseconds);
        
        if (frame_could_be_delayed)
        {
            next_frame_wait = FRAME_MS;
            frame_could_be_delayed = false;
        }

        if (next_frame_wait < 0)
        {
            stringstream ss;
            ss << "The game is " << (-next_frame_wait) << " ms behind!";
            title_debug_info(ss.str());
            next_frame_wait = 0;
        }
        else title_debug_info("");
    }
    title_debug_info("");
    int got_score = map->end();
    delete map;

    you_died_menu(got_score, debug_phase);
    debug_phase = -1;
}

// Asks user for seed and starts gameplay
static void play_set_seed()
{
    unsigned int seed = seed_menu();
    if (seed != 0)
        play(seed);
    else
        return;
}

// The main function from which the whole program begins
int main()
{
    // Program initialization
    title_debug_info("");
    std::ios::sync_with_stdio(false); // removes compatibility with printf() from C, be careful and only use cout!
    cursor_set_active(false);
    load_all_data();

    // Main menu
    int ask_result;
    do {
        ask_result = main_menu();
        switch (ask_result)
        {
            case 1: play(); break;
            case 2: play_set_seed(); break;
            case 3: instructions_menu(); break;
            case 4: phases_menu(); break;

            case -1: play(); break;
            case -2: play(get_last_seed()); break;
        }

    } while (ask_result != 5);

    // Program ending
    return 0;
}
