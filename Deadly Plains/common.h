#pragma once

// --- CONSTANTS ---

// Game name & version in primitive form
const char FULL_VERSION[] = "Maze Game v.1.0";

// Basic constants
const int FRAME_MS = 45;                    // Length of game frame
const int SECTOR_SIZE = 5;                  // Size of single sector
const int LEVEL_RANGE = 33;                 // Amount of sectors in each direction from spawn (excluding [0,0] sector)
const int LABIRYNTH_WB = 50;                // Wall-breaking parameter for the labirynth generating alghorithm (in promiles)
const int DAMAGE_SHOW_TIME = 5;             // Entity health visibility duration after taking damage (in frames)
const int MAX_PLAYER_HEALTH = 5;            // Max health that player can have
const int OVERFRUIT_EAT_SCORE = 5;          // Score for eating fruit while having max health
const int VISION_RANGE = 11;                // Range of player vision
const int SIMULATION_DISTANCE = 22;         // Distance in which entity behaviour is simulated (has a square shape)
const int DESPAWN_DISTANCE = 22;            // Distance outside which entities spawned with an egg despawn (has a square shape)
const int PLAYER_SMELL = 26;                // Smell of the player for enemies
const int ANIMAL_SMELL = 18;                // Smell of animals for monsters
const int SPAWNING_SMELL_INIT = 22;         // Determines how far from the player entities can spawn
const int SPAWNING_SMELL_MID = 10;          // Determines the maximum spawning smell where entities can spawn
const int SPAWNER_ACTIVATION_RANGE = 5;     // Range from player in which spawner will release entities automatically
const int ANTI_TERRAIN_DEG = 10;            // Parameter, which slows down terrain degradation (removal of blocks)
const int EGG_PERIOD = 3;                   // Duration of an egg frame in game frames
const char EGG_HATCH_TIME = 8;              // Egg hatch duration in egg frames

// Calculated constants
const int SECTORS_IN_LINE = LEVEL_RANGE * 2 + 1;                // Number of sectors in one row or column
const int MAP_SIZE = SECTORS_IN_LINE * (SECTOR_SIZE + 1) + 1;   // Total number of tiles in one row or column (with borders)
const int LAB_ARRAY_SIZE = SECTORS_IN_LINE * 2 - 1;             // Labirynth instructions array size
const int TOTAL_SECTORS = SECTORS_IN_LINE * SECTORS_IN_LINE;    // Total sectors on the whole map
const int PATHFIND_TABLE_SIZE = 2 * SIMULATION_DISTANCE + 1;    // Size of the array used for pathfinding
const int DISP_SIZE = 2 * VISION_RANGE + 3;                     // Size of display-array-1
const int DISPLAY_ROWS = DISP_SIZE;                             // Amount of display-array-2 rows
const int DISPLAY_COLUMNS = DISP_SIZE * 2 - 1;                  // Amount of display-array-2 columns
const int MIN_SNIPER_SMELL = PLAYER_SMELL;                      // Min smell of player, which the sniper can shoot at

// Common characters
namespace C
{
    // Map objects
    const char WALL = (char)0xCE;
    const char BLOCK = (char)0xFE;
    const char NUMBER = '9';
    const char PLAYER = 'P';
    const char ANIMAL = 'n';
    const char MONSTER = 'M';
    const char SNIPER = 'S';
    const char INSECTOR = 'I';
    const char INSECT = 'x', INSECT_DRIED = 'i';
    const char SPAWNER = (char)0xF5;
    const char BULLET = 'o';
    const char FRUIT = (char)0xA2;
    const char EGG = '?';
    const char ANCHOR = '@'; // technical object

    // Walls
    const char WALL_VERTICAL = (char)0xBA;
    const char WALL_HORIZONTAL = (char)0xCD;
    const char WALL_NE = (char)0xC8;
    const char WALL_NW = (char)0xBC;
    const char WALL_SE = (char)0xC9;
    const char WALL_SW = (char)0xBB;
    const char WALL_NOT_N = (char)0xCB;
    const char WALL_NOT_E = (char)0xB9;
    const char WALL_NOT_S = (char)0xCA;
    const char WALL_NOT_W = (char)0xCC;

    // Lines
    const char LINE = (char)0xC5;
    const char LINE_VERTICAL = (char)0xB3;
    const char LINE_HORIZONTAL = (char)0xC4;
    const char LINE_NE = (char)0xC0;
    const char LINE_NW = (char)0xD9;
    const char LINE_SE = (char)0xDA;
    const char LINE_SW = (char)0xBF;
    const char LINE_NOT_N = (char)0xC2;
    const char LINE_NOT_E = (char)0xB4;
    const char LINE_NOT_S = (char)0xC1;
    const char LINE_NOT_W = (char)0xC3;
}

// Console color IDs
namespace COLOR
{
    const char BLACK = 0;           // a
    const char DARK_BLUE = 1;       // b
    const char DARK_GREEN = 2;      // c
    const char DARK_CYAN = 3;       // d
    const char DARK_RED = 4;        // e
    const char DARK_MAGENTA = 5;    // f
    const char DARK_YELLOW = 6;     // g
    const char LIGHT_GRAY = 7;      // h
    const char DARK_GRAY = 8;       // i
    const char BLUE = 9;            // j
    const char GREEN = 10;          // k
    const char CYAN = 11;           // l
    const char RED = 12;            // m
    const char MAGENTA = 13;        // n
    const char YELLOW = 14;         // o
    const char WHITE = 15;          // p
}

// --- STRUCTS ---

struct Coords {
    int x = 0;
    int y = 0;

    bool operator==(const Coords& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Coords& other) const {
        return !(*this == other);
    }
};
