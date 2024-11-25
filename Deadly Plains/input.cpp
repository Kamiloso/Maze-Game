#include <windows.h>

#include "input.h"
#include "common.h"

static char preferred_axis = 'x';
static char pre_direction = 0;

// Checks if the selected input type was pressed recently
static bool get_async_direction_state(char ch)
{
    bool b1, b2, b3;
    switch (ch)
    {
    case 'W':
        b1 = GetAsyncKeyState(VK_UP);
        b2 = GetAsyncKeyState(0x57); // key W
        b3 = GetAsyncKeyState(0x49); // key I
        break;
    case 'S':
        b1 = GetAsyncKeyState(VK_DOWN);
        b2 = GetAsyncKeyState(0x53); // key S
        b3 = GetAsyncKeyState(0x4B); // key K
        break;
    case 'D':
        b1 = GetAsyncKeyState(VK_RIGHT);
        b2 = GetAsyncKeyState(0x44); // key D
        b3 = GetAsyncKeyState(0x4C); // key L
        break;
    case 'A':
        b1 = GetAsyncKeyState(VK_LEFT);
        b2 = GetAsyncKeyState(0x41); // key A
        b3 = GetAsyncKeyState(0x4A); // key J
        break;
    default:
        return false;
    }
    return b1 || b2 || b3;
}

// Resets all registered inputs
void reset_inputs()
{
    preferred_axis = 'x';
    pre_direction = 0;

    get_async_direction_state('W');
    get_async_direction_state('S');
    get_async_direction_state('D');
    get_async_direction_state('A');

    GetAsyncKeyState(VK_SPACE);
}

// Checks if space was pressed recently
bool is_space_pressed()
{
    return GetAsyncKeyState(VK_SPACE);
}

// Returns vector of player movement
Coords get_movement_input()
{
    Coords mov_input = { 0,0 };
    if (get_async_direction_state('W')) mov_input.y++;
    if (get_async_direction_state('S')) mov_input.y--;
    if (get_async_direction_state('D')) mov_input.x++;
    if (get_async_direction_state('A')) mov_input.x--;

    if (mov_input.x != 0 && mov_input.y != 0)
    {
        if (preferred_axis == 'x')
            mov_input.y = 0;
        if (preferred_axis == 'y')
            mov_input.x = 0;
    }

    if (mov_input.x != 0)
        preferred_axis = 'y';
    if (mov_input.y != 0)
        preferred_axis = 'x';

    return mov_input;
}

// Returns vector of player shooting
Coords get_shooting_input()
{
    Coords mov_input = { 0,0 };

    bool north = get_async_direction_state('W');
    bool south = get_async_direction_state('S');
    bool east = get_async_direction_state('D');
    bool west = get_async_direction_state('A');

    if (pre_direction == 'N')
    {
        if (east) mov_input = { 1,0 };
        else if (south) mov_input = { 0,-1 };
        else if (west) mov_input = { -1,0 };
        else if (north) mov_input = { 0,1 };
    }
    else if (pre_direction == 'E')
    {
        if (south) mov_input = { 0,-1 };
        else if (west) mov_input = { -1,0 };
        else if (north) mov_input = { 0,1 };
        else if (east) mov_input = { 1,0 };
    }
    else if (pre_direction == 'S')
    {
        if (west) mov_input = { -1,0 };
        else if (north) mov_input = { 0,1 };
        else if (east) mov_input = { 1,0 };
        else if (south) mov_input = { 0,-1 };
    }
    else // 'W' or 0
    {
        if (north) mov_input = { 0,1 };
        else if (east) mov_input = { 1,0 };
        else if (south) mov_input = { 0,-1 };
        else if (west) mov_input = { -1,0 };
    }

    pre_direction = 0;
    if (mov_input.y == 1) pre_direction = 'N';
    if (mov_input.x == 1) pre_direction = 'E';
    if (mov_input.y == -1) pre_direction = 'S';
    if (mov_input.x == -1) pre_direction = 'W';

    return mov_input;
}
