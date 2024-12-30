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
static bool frame_could_be_delayed = false;

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

// Ignore next info about frame delay
void allow_frame_delay()
{
    frame_could_be_delayed = true;
}

// Loads all game data from file
void load_all_data()
{
    string read = read_from_file(L"mg_data.se3");
    string str_highscore = "";
    string str_last_seed = "";

    int reading_mode = 0;
    for (auto it = read.begin(); it != read.end(); ++it)
    {
        if (*it != ';')
        {
            if(reading_mode == 0)
                str_highscore += *it;

            if (reading_mode == 1)
                str_last_seed += *it;
        }
        else reading_mode++;
    }

    highscore = int_parse_any_string(str_highscore.c_str());
    last_seed = int_parse_any_string(str_last_seed.c_str());

    if (highscore < 0)
        highscore = 0;
}

// Saves all game data to file
void save_all_data(int not_updated_score)
{
    int score = highscore > not_updated_score ? highscore : not_updated_score;
    stringstream ss;
    ss << score << ";" << last_seed;
    save_to_file(L"mg_data.se3", ss.str());
}

// Starts gameplay with a given (or random when empty) seed
static void play(unsigned int seed = 0)
{
    Map* map = new Map(seed);
    set_last_seed(map->get_seed());
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

    you_died_menu(got_score);
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
        }
    } while (ask_result != 5);

    // Program ending
    return 0;
}
