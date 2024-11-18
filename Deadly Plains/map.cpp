#include <iostream>
#include <vector>

#include "map.h"
#include "generator.h"
#include "input.h"
#include "display.h"
#include "entities.h"

using namespace std;

// Can be pushed by a player?
static bool can_be_pushed(char c)
{
    if (
        c == C::BLOCK ||
        c == C::ANIMAL ||
        c == C::INSECT
    ) return true;
    else return false;
}

// Can be picked up or broken using pushed object?
static bool can_be_picked_up(char c)
{
    if (
        c == C::BULLET ||
        c == C::FRUIT
        ) return true;
    else return false;
}

// Map constructor
Map::Map(unsigned int seed)
{
    if (seed == 0)
        public_seed = std::random_device{}();
    else
        public_seed = seed;
    ms_twister = std::mt19937{ public_seed };

    tiles = new Tile* [MAP_SIZE];
    for (int i = 0; i < MAP_SIZE; i++) {
        tiles[i] = new Tile[MAP_SIZE];
    }
    
    generate(tiles, ms_twister);

    int spawn_xy = (MAP_SIZE - 1) / 2;
    spawn(spawn_xy, spawn_xy, C::PLAYER);

    // Pre-generate objects (TEMPORARY)
    for (int x = 0; x < MAP_SIZE; x++)
        for (int y = 0; y < MAP_SIZE; y++)
        {
            // Blocks
            if (get_tile(x, y) == ' ' && ms_twister() % 1000 < 20)
                tiles[x][y] = Tile(C::BLOCK);

            // Fruits
            if (get_tile(x, y) == ' ' && ms_twister() % 1000 < 3)
                tiles[x][y] = Tile(C::FRUIT);

            // Animals
            if (get_tile(x, y) == ' ' && ms_twister() % 1000 < 15)
                spawn(x, y, C::ANIMAL);

            // Monsters
            if (get_tile(x, y) == ' ' && ms_twister() % 1000 < 3)
                spawn(x, y, C::MONSTER);

            // Insects
            if (get_tile(x, y) == ' ' && ms_twister() % 1000 < 2)
                spawn(x, y, C::INSECT);

            // Snipers
            if (get_tile(x, y) == ' ' && ms_twister() % 1000 < 3)
                spawn(x, y, C::SNIPER);
        }

    if (get_tile(200, 205) == ' ')
        spawn(200, 205, C::INSECTOR);

    //if (get_tile(200, 195) == ' ')
    //    spawn(200, 195, C::INSECTOR);
}

// Map destructor
Map::~Map()
{
    for (int i = 0; i < MAP_SIZE; i++) {
        delete[] tiles[i];
    }
    delete[] tiles;
}

// Executes game frame
void Map::frame_update()
{
    // Clean input getting before starting gameplay
    if(frame==0)
        reset_inputs();

    // !!! DON'T TOUCH THIS VARIABLE (vector size may be modified during loops execution) !!!
    int lngt = entities.size();

    // Randomize entity movement priority
    for (int i = 1; i < lngt; i++) // i = 1 to not include player with i = 0
    {
        int j = ms_twister() % (lngt - i) + i;
        swap(entities[i], entities[j]);
    }

    // Movement priority sorting: bullets > monsters > [others]
    int i_ptr = 1;
    for (char priority : {C::BULLET, C::MONSTER})
        for (int i = i_ptr; i < lngt; i++)
        {
            int lx = entities[i].x;
            int ly = entities[i].y;
            if (get_tile(lx, ly) == priority)
            {
                swap(entities[i], entities[i_ptr]);
                i_ptr++;
            }
        }

    // Entity movement
    for (int i = 0; i < lngt; i++)
    {
        if (i == 1) // Drawing pathmaps for patfinding
        {
            static const int PLAYER_SMELL = 20;
            static const int ANIMAL_SMELL = 15;

            draw_pathmap(this, entities[0], { { C::PLAYER, PLAYER_SMELL } }, "melee");
            draw_pathmap(this, entities[0], { { C::PLAYER, PLAYER_SMELL }, { C::ANIMAL, ANIMAL_SMELL } }, "predator");
            draw_pathmap(this, entities[0], { { C::PLAYER, PLAYER_SMELL } }, "distance");
        }

        if (entities[i].x != MAP_SIZE)
        {
            if (tiles[entities[i].x][entities[i].y].dmg_show_time > 0)
                tiles[entities[i].x][entities[i].y].dmg_show_time--;
            entity_move(entities[i].x, entities[i].y);
        }
    }

    // Removed entity cleaning
    for (int i = 0; i < lngt; i++)
    {
        if (entities[i].x == MAP_SIZE)
        {
            if (i == 0) return;
            entities.erase(entities.begin() + i);
            lngt--; i--;
        }
    }

    // Auto display map
    DisplayData ddt;
    ddt.center = entities[0];
    ddt.difficulty = (int)entities.size();
    ddt.score = score;
    ddt.health = tiles[entities[0].x][entities[0].y].health;
    display(this, ddt);

    // Frame number increase
    frame++;
}

// Checks if game is over and if yes, returns score greater or equal 0
int Map::end()
{
    if (entities.size() == 0 || get_tile(entities[0].x, entities[0].y) != C::PLAYER) return score;
    else return -1;
}

// Returns the map seed
unsigned int Map::get_seed()
{
    return public_seed;
}

// Moves the entity in a random direction
void Map::passive_movement(int x, int y)
{
    // Vectors for passive side movements
    Coords sides[4] = {
        {1,0}, {-1,0},
        {0,1}, {0,-1}
    };
    for (int i = 0; i < 4; i++)
    {
        int j = ms_twister() % (4 - i) + i;
        swap(sides[i], sides[j]);
    }

    // passive movement
    for (int i = 0; i < 4; i++)
        if (try_move(x, y, sides[i].x, sides[i].y))
            break;
}

// Executes actions of entity in the specific position (should not be called outside entity movement loop)
void Map::entity_move(int x, int y)
{
    // Entity reference get
    if (get_tile(x, y) == '=') return;
    Tile& entity = tiles[x][y];

    // Disable distant entities
    int distance_x = x - entities[0].x;
    int distance_y = y - entities[0].y;
    if (distance_x < 0) distance_x *= -1;
    if (distance_y < 0) distance_y *= -1;
    if (distance_x > SIMULATION_DISTANCE || distance_y > SIMULATION_DISTANCE)
        return;

    // ---------------------- //

    // Player
    if (entity.id == C::PLAYER)
    {
        static const int MOVEMENT_PERIOD = 2; // amount of frames
        static const int ACTION_COOLDOWN = 2; // amount of MOVEMENT_PERIOD periods

        if (frame % MOVEMENT_PERIOD == 0)
        {
            if (entity.action_cooldown > 0)
                entity.action_cooldown--;

            if (!is_space_pressed())
            {
                // movement mode
                Coords mov_input = get_movement_input();
                try_move(x, y, mov_input.x, mov_input.y, "push");
            }
            else if (entity.action_cooldown == 0)
            {
                // shooting mode
                Coords shoot_input = get_shooting_input();
                if (shoot_input.x != 0 || shoot_input.y != 0)
                {
                    entity.action_cooldown = ACTION_COOLDOWN;
                    spawn_bullet(x, y, shoot_input.x, shoot_input.y, true);
                }
            }
        }
    }

    // Animal
    if (entity.id == C::ANIMAL)
    {
        static const int MOVEMENT_PERIOD = 16;

        if (frame % MOVEMENT_PERIOD == 0)
            passive_movement(x, y);
    }

    // Monster / Insect
    if (entity.id == C::MONSTER || entity.id == C::INSECT)
    {
        int movement_period = entity.id == C::MONSTER ? 8 : 4;
        static const int DAMAGE_PERIOD = 8;
        static const int PLAYER_SMELL = 15;

        if (frame % movement_period == 0)
        {
            // Decide where to go
            Coords feels_path;
            if(entity.id == C::MONSTER)
                feels_path = pathfind(this, entities[0], {x,y}, ms_twister, "predator");
            else
                feels_path = pathfind(this, entities[0], { x,y }, ms_twister, "melee");

            // Go where it has been decided
            if (feels_path.x == 0 && feels_path.y == 0)
            {
                passive_movement(x, y);
            }
            else
            {
                if (!try_move(x, y, feels_path.x, feels_path.y, "bullets_ignore") && frame % DAMAGE_PERIOD == 0)
                {
                    char tile_attack = get_tile(x + feels_path.x, y + feels_path.y);
                    if (tile_attack == C::PLAYER || (tile_attack == C::ANIMAL && entity.id == C::MONSTER))
                        damage(x + feels_path.x, y + feels_path.y);
                }
            }
        }
    }

    // Sniper
    if (entity.id == C::SNIPER)
    {
        // TEMPORARY BEHAVIOUR
        static const int MOVEMENT_PERIOD = 4;

        if (frame % MOVEMENT_PERIOD == 0)
        {
            if (frame % 8 == 0)
            {
                passive_movement(x, y);
            }
            else
            {
                spawn_bullet(x, y, 0, 1, false);
            }
        }
    }

    // Insector
    if (entity.id == C::INSECTOR)
    {
        static const int MOVEMENT_PERIOD = 8;
        static const int ACTION_COOLDOWN_MIN = 4; // amount of MOVEMENT_PERIOD
        static const int ACTION_COOLDOWN_MAX = 8; // amount of MOVEMENT_PERIOD
        static const int SIDE_SUMMON_CHANCE = 350; // (in promiles)
        
        const int cooldown_set = ms_twister() % (ACTION_COOLDOWN_MAX - ACTION_COOLDOWN_MIN + 1) + ACTION_COOLDOWN_MIN;

        if (frame % MOVEMENT_PERIOD == 0)
        {
            // Only for checking if can see the player
            Coords feels_path = pathfind(this, entities[0], { x,y }, ms_twister, "melee");

            // Reloading (even if doesn't see the player)
            if (entity.action_cooldown > 0)
                entity.action_cooldown--;

            // Insector logic
            if ((feels_path.x != 0 || feels_path.y != 0) && entity.action_cooldown == 0 && !entity.should_reroll)
            {
                entity.action_cooldown = cooldown_set;
                spawn_around(x, y, C::INSECT, SIDE_SUMMON_CHANCE);
            }
            else
            {
                if (entity.should_reroll)
                {
                    entity.should_reroll = false;
                    entity.action_cooldown = cooldown_set;
                }
                passive_movement(x, y);
            }
        }
    }

    // Bullet
    if (entity.id == C::BULLET)
    {
        if (!try_move(x, y, entity.movement.x, entity.movement.y))
        {
            // WARNING: Meelee virtual attack exists too (if bullet summoning space is obstructed)
            if (damage(x + entity.movement.x, y + entity.movement.y, entity.shot_by_player))
                try_move(x, y, entity.movement.x, entity.movement.y);
            else
                remove(x, y);
        }
    }
}

// Returns the character which represents object in the specific position
char Map::get_tile(int x, int y)
{
    if (x >= 0 && x < MAP_SIZE && y >= 0 && y < MAP_SIZE)
    {
        Tile& tile = tiles[x][y];
        return tile.id;
    }
    else return '=';
}

// Returns character which should be displayed in a specific map position
char Map::get_tile_display(int x, int y)
{
    if (x >= 0 && x < MAP_SIZE && y >= 0 && y < MAP_SIZE)
    {
        Tile& tile = tiles[x][y];
        char ch = tile.id;
        if (ch == C::NUMBER || tile.dmg_show_time > 0)
            if (tile.health > 0 && tile.health <= 9)
                return '0' + tile.health;

        if (ch == C::INSECT && tile.has_score == 0)
            return C::INSECT_DRIED;

        return ch;
    }
    else return '=';
}

// Spawns the object (unsafe to use without checking if empty space)
void Map::spawn(int x, int y, char type, bool score_rich)
{
    entities.push_back({ x,y });
    tiles[x][y] = Tile(type);
    if (!score_rich)
        tiles[x][y].has_score = 0;
}

// Tries to spawn entities randomly next to specific coordinates
void Map::spawn_around(int x, int y, char type, const int SIDE_CHANCE, bool score_rich)
{
    static const Coords sides[4] = { {0,1}, {0,-1}, {1,0}, {-1,0} };
    for (int i = 0; i < 4; i++)
    {
        int dx = sides[i].x;
        int dy = sides[i].y;

        if (get_tile(x + dx, y + dy) == ' ' && ms_twister() % 1000 < SIDE_CHANCE)
        {
            spawn(x + dx, y + dy, type, score_rich);
        }
    }
}

// Spawns a bullet in a specific direction or makes a 1-tick damage pulse
void Map::spawn_bullet(int x, int y, int dx, int dy, bool by_player)
{
    if (get_tile(x + dx, y + dy) == ' ' || damage(x + dx, y + dy, by_player))
    {
        spawn(x + dx, y + dy, C::BULLET);
        Tile& tile = tiles[entities.back().x][entities.back().y];
        tile.movement = { dx,dy };
        tile.shot_by_player = by_player;
    }
}

// Removes the object from the map
void Map::remove(int x, int y)
{
    tiles[x][y] = Tile();
    for (int i = 0; i < entities.size(); i++)
    {
        if (entities[i].x == x && entities[i].y == y)
        {
            entities[i].x = MAP_SIZE; // !!! marking entity as non-existing (removing it after every behaviour loop)
            break;
        }
    }
}

// Damages the object and returns true if object was killed by this method
bool Map::damage(int x, int y, bool dmg_by_player)
{
    Tile& tile = tiles[x][y];
    tile.health--;
    if (tile.health == 0) // don't change to <= 0, because -1 serves as sort of infinite health
    {
        if (dmg_by_player)
            score += tile.has_score;
        remove(x, y);
        return true;
    }
    else
    {
        tile.dmg_show_time = DAMAGE_SHOW_TIME;
        return false;
    }
}

// Tries to execute object's movement in a specific direction
bool Map::try_move(int x, int y, int dx, int dy, string mode)
{
    if (mode == "push" && can_be_pushed(get_tile(x + dx, y + dy)))
        if (!try_move(x + dx, y + dy, dx, dy, "push"))
            return false;

    char picking_up = ' ';
    if (get_tile(x + dx, y + dy) != ' ')
    {
        bool bullets_ignore_tag = false;
        if (mode == "bullets_ignore" && get_tile(x + dx, y + dy) == C::BULLET)
            bullets_ignore_tag = true;
        /*{
            bool axis_go_x = (dx != 0);
            bool axis_bl_x = (tiles[x + dx][y + dy].movement.x != 0);
            bullets_ignore_tag = (axis_go_x == axis_bl_x); // ignore bullet only if on the same axis as movement
        }*/

        if ((mode == "push" && can_be_picked_up(get_tile(x + dx, y + dy))) || bullets_ignore_tag)
            picking_up = get_tile(x + dx, y + dy);
        else
            return false;
    }

    if (get_tile(x, y) == C::PLAYER)
    {
        if (picking_up == C::BULLET)
            if (damage(x, y))
                return true;

        if (picking_up == C::FRUIT)
        {
            if (tiles[x][y].health < MAX_PLAYER_HEALTH)
            {
                tiles[x][y].health++;
                tiles[x][y].dmg_show_time = DAMAGE_SHOW_TIME;
            }
            else
                score += OVERFRUIT_EAT_SCORE;
        }
    }

    if (mode == "bullets_ignore")
    {
        if (picking_up == C::BULLET)
        {
            bool by_player = tiles[x + dx][y + dy].shot_by_player == true;
            if (damage(x, y, by_player))
                return true;
        }
    }

    remove(x + dx, y + dy);
    tiles[x + dx][y + dy] = tiles[x][y];
    tiles[x][y] = Tile();

    for (int i = 0; i < entities.size(); i++)
    {
        if (entities[i].x == x && entities[i].y == y)
        {
            entities[i].x += dx;
            entities[i].y += dy;
            break;
        }
    }
    return true;
}
