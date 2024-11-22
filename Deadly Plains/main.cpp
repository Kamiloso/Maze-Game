#include <iostream>
#include <windows.h>
#include <chrono>
#include <conio.h>
#include <sstream>

#include "map.h"

using namespace std;

static void hide_cursor() {
    cout << "\x1B[?25l";
}

static void show_cursor() {
    cout << "\x1B[?25h";
}

void title_debug_info(string info) {
    if (info != "")
        info = "title Maze Game v.1.0 ; " + info;
    else
        info = "title Maze Game v.1.0";
    system(info.c_str());
}

void play(unsigned int seed = 0)
{
    hide_cursor();
    Map* map = new Map(seed);
    int next_frame_wait = 0;
    while (map->end() == -1)
    {
        // Game loop
        Sleep(next_frame_wait);
        
        auto clock_before = chrono::steady_clock::now();
        map->frame_update();
        auto clock_after = chrono::steady_clock::now();

        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(clock_after - clock_before).count();
        next_frame_wait = FRAME_MS - static_cast<int>(milliseconds);
        
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

    system("cls");
    show_cursor();
    cout << "You died with score: " << got_score << endl;
    cout << "Press ENTER to continue..." << endl;

    char pressedKey;
    do {
        pressedKey = _getch();
    } while (pressedKey != '\r' && pressedKey != '\n');
}

int main()
{
    // Program initialization
    title_debug_info("");
    std::ios::sync_with_stdio(false); // removes compatibility with printf() from C, be careful and only use cout!

    // Play
    play();

    // Program ending
    return 0;
}
