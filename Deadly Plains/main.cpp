#include <iostream>
#include <windows.h>
#include <conio.h>

#include "map.h"

using namespace std;

void hide_cursor() {
    cout << "\x1B[?25l";
}

void show_cursor() {
    cout << "\x1B[?25h";
}

void play(unsigned int seed = 0)
{
    hide_cursor();
    Map* map = new Map(seed);
    bool delay_on = false;
    while (map->end() == -1)
    {
        // Player Loop
        if (delay_on) Sleep(FRAME_MS);
        else delay_on = true;
        map->frame_update();
    }
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
    system("title Deadly Plains v.1.0");
    system("color 0A");

    // Play
    play();

    // Program ending
    return 0;
}
