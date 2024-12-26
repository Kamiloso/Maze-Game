#include <iostream>
#include <windows.h>
#include <chrono>
#include <conio.h>
#include <sstream>

#include "map.h"
#include "console.h"

using namespace std;

static void title_debug_info(string info) {
    if (info != "")
        info = "title " + (string)FULL_VERSION + " ; " + info;
    else
        info = "title " + (string)FULL_VERSION;
    system(info.c_str());
}

static unsigned int Highscore = 0;
static unsigned int Last_seed = 0;

static void play(unsigned int seed = 0)
{
    cursor_set_active(false);
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
    Last_seed = map->get_seed();
    delete map;

    clear_screen();
    cursor_set_active(true);

    // Inform about the end
    if (got_score > Highscore) {
        Highscore = got_score;
        cout << "New record!" << endl;
    }
    cout << "You died with score: " << got_score << endl;
    cout << "Press ENTER to continue..." << endl;

    char pressedKey;
    do {
        pressedKey = _getch();
    } while (pressedKey != '\r' && pressedKey != '\n');
}

static void ask_seed_and_play()
{
    try_again:
    unsigned int seed = 0;
    string str_seed;

    clear_screen();
    cout << "What seed do you want to set? (0 - last)" << endl;
    cout << "> "; cin >> str_seed;

    try {
        unsigned int seed_try = stoi(str_seed);
        if (seed_try != 0) seed = seed_try;
        else seed = Last_seed;
    }
    catch(exception) {
        cout << "There was an error reading seed! Try again." << endl;
        system("pause");
        goto try_again;
    }

    play(seed);
}

static void repeat_char(char c, int amount)
{
    for (int i = 0; i < amount; i++)
        cout << c;
}

int main()
{
    // Program initialization
    title_debug_info("");
    std::ios::sync_with_stdio(false); // removes compatibility with printf() from C, be careful and only use cout!

    // Main menu
    char pressedKey;
    do {
        clear_screen();
        
        repeat_char(C::LINE_HORIZONTAL, 21); cout << endl;
        
        cout << " " << C::WALL_SE; repeat_char(C::WALL_HORIZONTAL, 17); cout << C::WALL_SW << endl;
        cout << " " << C::WALL_VERTICAL << " " << (string)FULL_VERSION << " " << C::WALL_VERTICAL << endl;
        cout << " " << C::WALL_NE; repeat_char(C::WALL_HORIZONTAL, 17); cout << C::WALL_NW << endl;
        
        repeat_char(C::LINE_HORIZONTAL, 21); cout << endl;

        cout << " Highscore: " << Highscore << endl;
        if (Last_seed != 0) cout << " Last seed: " << Last_seed << endl;
        else cout << " Last seed: none" << endl;

        repeat_char(C::LINE_HORIZONTAL, 21); cout << endl;

        cout << " 1 - Play on random seed" << endl;
        cout << " 2 - Play on set seed" << endl;
        cout << " 3 - Quit" << endl;

        repeat_char(C::LINE_HORIZONTAL, 21); cout << endl;

        pressedKey = _getch();

        switch (pressedKey)
        {
        case '1': play(); break;
        case '2': ask_seed_and_play(); break;
        }

    } while (pressedKey != '3');

    // Program ending
    return 0;
}
