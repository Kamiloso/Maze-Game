#include <iostream>
#include <vector>

#include "map.h"
#include "generator.h"
#include "display.h"
#include "input.h"
#include "entities.h"
#include "pathfinding.h"

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

// Does allow on_kill() method?
static bool activates_on_kill(char c)
{
    if (
        c == C::SPAWNER
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
                spawn(x, y, C::BLOCK, false);

            // Fruits
            if (get_tile(x, y) == ' ' && ms_twister() % 1000 < 3)
                spawn(x, y, C::FRUIT, false);

            // Animals
            if (get_tile(x, y) == ' ' && ms_twister() % 1000 < 8)
                spawn(x, y, C::ANIMAL);

            // Monsters
            if (get_tile(x, y) == ' ' && ms_twister() % 1000 < 2)
                spawn(x, y, C::MONSTER);

            // Insects
            if (get_tile(x, y) == ' ' && ms_twister() % 1000 < 2)
                spawn(x, y, C::INSECT);

            // Snipers
            if (get_tile(x, y) == ' ' && ms_twister() % 1000 < 2)
                spawn(x, y, C::SNIPER);

            // Spawners
            if (get_tile(x, y) == ' ' && ms_twister() % 1000 < 3)
                spawn(x, y, C::SPAWNER).make_magical();

            // Insectors
            if (get_tile(x, y) == ' ' && ms_twister() % 1000 < 1)
                spawn(x, y, C::INSECTOR);

            // Numbers
            if (get_tile(x, y) == ' ' && ms_twister() % 1000 < 15) {
                spawn(x, y, C::NUMBER, false).set_health(ms_twister() % 4 + 1);
            }
        }
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
        if (i == 1) // Drawing pathmaps for pathfinding
        {
            static const int PLAYER_SMELL = 20;
            static const int ANIMAL_SMELL = 15;

            pathfinding.draw_pathmap(this, entities[0], { 
                { C::PLAYER, PLAYER_SMELL } }, "melee");

            pathfinding.draw_pathmap(this, entities[0], { 
                { C::PLAYER, PLAYER_SMELL }, { C::ANIMAL, ANIMAL_SMELL } }, "predator");

            pathfinding.draw_pathmap(this, entities[0], { 
                { C::PLAYER, PLAYER_SMELL } }, "distance");
        }

        if (entities[i].x != MAP_SIZE)
        {
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

    // Execute delayed on_kill()
    for (int i = 0; i < delayed_kills.size(); i++)
    {
        KillDelayed& kdl = delayed_kills[i];
        Tile::on_kill(this, ms_twister, kdl.id, kdl.mag, kdl.coords.x, kdl.coords.y);
    }
    delayed_kills.clear();

    // Auto display map
    DisplayData ddt;
    ddt.center = entities[0];
    ddt.difficulty = (int)entities.size();
    ddt.score = score;
    ddt.health = tiles[entities[0].x][entities[0].y].get_health();
    display(this, ddt);

    // Frame number increase
    frame++;
}

// Returns frame number
int Map::get_frame() const
{
    return frame;
}

// Checks if game is over and if yes, returns score greater or equal 0
int Map::end() const
{
    if (entities.size() == 0 || get_tile(entities[0].x, entities[0].y) != C::PLAYER) return score;
    else return -1;
}

// Returns the map seed
unsigned int Map::get_seed() const
{
    return public_seed;
}

// Executes actions of entity in the specific position (should not be called outside entity movement loop)
void Map::entity_move(int x, int y)
{
    // Entity reference get
    if (get_tile(x, y) == '=') return;
    Tile& entity = tiles[x][y];

    // Disable distant entities
    int dist_x = x - entities[0].x;
    int dist_y = y - entities[0].y;
    if (dist_x < 0) dist_x *= -1;
    if (dist_y < 0) dist_y *= -1;
    if (dist_x > SIMULATION_DISTANCE || dist_y > SIMULATION_DISTANCE)
        return;

    // Execute entity behaviour
    entity.execute_behaviour(this, ms_twister, x, y);
}

// Returns the character which represents object in the specific position
char Map::get_tile(int x, int y) const
{
    if (x >= 0 && x < MAP_SIZE && y >= 0 && y < MAP_SIZE)
    {
        Tile& tile = tiles[x][y];
        return tile.get_id();
    }
    else return '=';
}

// Returns character and color ID, which will be displayed as a tile
TileDisplay Map::get_tile_display(int x, int y) const
{
    if (x >= 0 && x < MAP_SIZE && y >= 0 && y < MAP_SIZE)
    {
        Tile& tile = tiles[x][y];
        char ch = tile.get_id();
        char health = tile.get_health();

        if (ch == ' ')
            return { ' ', COLOR::DARK_GRAY };

        if (ch == C::NUMBER)
        {
            if (health >= 0 && health <= 9)
            {
                char new_ch = '0' + health;
                return { new_ch, COLOR::DARK_GRAY };
            }
            else return { '9', COLOR::DARK_GRAY };
        }

        if (health >= 0 && health <= 9)
        {
            unsigned char visibility = tile.is_dmg_visible();
            if (visibility != 0)
            {
                char new_ch = '0' + health;
                if (tile.is_magical() && MAGICAL_DAMAGE_IS_MAGENTA)
                    return { new_ch, COLOR::MAGENTA };
                else if (visibility == 1)
                    return { new_ch, COLOR::DARK_RED };
                else if(visibility == 2)
                    return { new_ch, COLOR::BLUE };
            }
        }

        if (ch == C::BULLET)
        {
            if (tile.is_magical())
                return { ch, COLOR::MAGENTA };
            else if (tile.was_shot_by_player())
                return { ch, COLOR::DARK_GREEN };
            else
                return { ch, COLOR::DARK_RED };
        }

        if (ch == C::INSECT && tile.get_score() == 0)
            ch = C::INSECT_DRIED;

        if (tile.is_magical())
            return { ch, COLOR::MAGENTA };

        if (ch == C::WALL ||
            ch == C::BLOCK)
            return { ch, COLOR::DARK_GRAY };

        if (ch == C::PLAYER)
            return { ch, COLOR::GREEN };

        if (ch == C::ANIMAL)
            return { ch, COLOR::YELLOW };

        if (ch == C::FRUIT)
            return { ch, COLOR::BLUE };

        if (ch == C::MONSTER ||
            ch == C::SNIPER ||
            ch == C::INSECTOR ||
            ch == C::INSECT ||
            ch == C::INSECT_DRIED ||
            ch == C::SPAWNER)
            return { ch, COLOR::RED };
    }
    else return { '=', COLOR::DARK_GRAY };
}

// Spawns the object (can overwrite tiles)
Tile& Map::spawn(int x, int y, char type, bool has_ai, bool magical)
{
    if (get_tile(x, y) == '=')
        return tiles[0][0];

    if (get_tile(x, y) != ' ')
        remove(x, y, true);

    tiles[x][y] = Tile(type);

    if (has_ai) entities.push_back({ x,y });
    if (magical) tiles[x][y].make_magical();

    return tiles[x][y];
}

// Spawns a bullet in a specific direction or makes a 1-tick damage pulse
void Map::spawn_bullet(int x, int y, int dx, int dy, bool by_player, bool magical)
{
    if (get_tile(x + dx, y + dy) == ' ' || damage(x + dx, y + dy, by_player))
    {
        entities.push_back({ x + dx,y + dy });
        tiles[x + dx][y + dy] = Tile(C::BULLET, { dx, dy }, by_player);
        if(magical) tiles[x + dx][y + dy].make_magical();
    }
}

// Damages the object and returns true if object was killed by this method
bool Map::damage(int x, int y, bool dmg_by_player)
{
    if (get_tile(x, y) == '=') return false;

    Tile& tile = tiles[x][y];
    if (tile.damage_by_one())
    {
        if (dmg_by_player)
            score += tile.get_score();
        remove(x, y);
        return true;
    }
    else return false;
}

// Removes the object from the map
void Map::remove(int x, int y, bool despawn_mode)
{
    if (get_tile(x, y) == '=') return;

    if (!despawn_mode && activates_on_kill(tiles[x][y].get_id()))
        delayed_kills.push_back({ tiles[x][y].get_id(), tiles[x][y].is_magical(), {x,y}});

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

// Tries to execute object's movement in a specific direction
bool Map::try_move(int x, int y, int dx, int dy, string mode)
{
    if (get_tile(x, y) == '=') return false;

    if (mode == "push" && can_be_pushed(get_tile(x + dx, y + dy)))
        if (!try_move(x + dx, y + dy, dx, dy, "push"))
            return false;

    char picking_up = ' ';
    if (get_tile(x + dx, y + dy) != ' ')
    {
        bool bullets_ignore_tag = false;
        if (mode == "bullets_ignore" && get_tile(x + dx, y + dy) == C::BULLET)
            bullets_ignore_tag = true;

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
            if (!tiles[x][y].heal_by_one(MAX_PLAYER_HEALTH))
                score += OVERFRUIT_EAT_SCORE;
        }
    }
    else if (mode == "bullets_ignore")
    {
        if (picking_up == C::BULLET)
        {
            bool by_player = tiles[x + dx][y + dy].was_shot_by_player();
            if (damage(x, y, by_player))
                return true;
        }
    }

    remove(x + dx, y + dy, true);
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
