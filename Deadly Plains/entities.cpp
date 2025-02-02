#include <random>
#include <vector>

#include "entities.h"
#include "common.h"
#include "input.h"
#include "map.h"

// Buffs a value in magical style
static char magical_buff(char start, char limit)
{
    if (start <= 0 || start >= limit)
        return start;
    
    int start_int = start;
    start_int += (start_int + 2) / 3;

    if (start_int < limit)
        return (char)start_int;
    else
        return limit;
}

// Default constructor
Tile::Tile(char _id, Coords bul_vect, bool by_player)
{
	if (_id == ' ') return;
	id = _id;

	if (id == C::BULLET)
	{
		if (bul_vect == Coords{ 0,1 }) bullet_movement = 'N';
		else if (bul_vect == Coords{ 1,0 }) bullet_movement = 'E';
		else if (bul_vect == Coords{ 0,-1 }) bullet_movement = 'S';
		else if (bul_vect == Coords{ -1,0 }) bullet_movement = 'W';
		shot_by_player = by_player;
	}

	initialize_tile_values();
}

// Initializes default tile values depending on their type
void Tile::initialize_tile_values()
{
	switch (id)
	{
	case C::PLAYER:
		health = MAX_PLAYER_HEALTH;
		break;

	case C::ANIMAL:
		health = 1;
		reward_score = 2;
		break;

	case C::MONSTER:
		health = 5;
		reward_score = 6;
		break;

	case C::SNIPER:
		health = 3;
		reward_score = 6;
		break;

	case C::INSECTOR:
		health = 5;
		reward_score = 18;
		break;

	case C::INSECT:
		health = 1;
		reward_score = 3;
		break;

	case C::SPAWNER: /* Remember, this is always magical tile! */
		health = 5;
        reward_score = 7;
		break;

	case C::BULLET:
        if (shot_by_player) health = 2;
        else health = 1;
		break;

	case C::FRUIT:
		health = 1;
		break;

	case C::NUMBER:
		health = 4;
		break;

    case C::EGG:
        health = 1;
        break;
	}
}

// Health setter
void Tile::set_health(char value)
{
	health = value;
}

// Score setter
void Tile::set_score(char value)
{
	if (value >= 0)
		reward_score = value;
}

// Magical activator
void Tile::make_magical()
{
    if (!magical)
    {
        magical = true;
        if (id == C::EGG) return;

        // Magical entities are more powerful and give more score
        reward_score = magical_buff(reward_score, 127);
        health = magical_buff(health, 10);
    }
}

// ID getter
char Tile::get_id() const
{
	return id;
}

// Health getter
char Tile::get_health() const
{
	return health;
}

// Score getter
char Tile::get_score() const
{
	return reward_score;
}

// Check if was shot by player
bool Tile::was_shot_by_player() const
{
	return shot_by_player;
}

// Sets the wall flag
void Tile::set_wall_flag()
{
    wall_flag = true;
}

// Marks entity as spawned from an egg
void Tile::set_egg_spawned()
{
    is_egg_spawned = true;
}

// Initializes an egg
void Tile::egg_initialize(char type, char hatch_time)
{
    id = C::EGG; // force being an egg
    egg_type = type;
    mark_as_acted(hatch_time);
}

// Returns egg type
char Tile::get_egg_type() const
{
    return egg_type;
}

// Returns thether the wall flag exists
bool Tile::has_wall_flag() const
{
    return wall_flag;
}

// Returns whether entity was spawned by an egg
bool Tile::was_egg_spawned() const
{
    return is_egg_spawned;
}

// Heals entity by one hp and returns whether it was healed
bool Tile::heal_by_one(char max_health)
{
	dmg_show_time = DAMAGE_SHOW_TIME;
	dmg_show_from_dmg = false;

	if (health != max_health)
	{
		health++;
		return true;
	}
	else return false;
}

// Damages entity by one hp and returns whether health has decremented to 0
bool Tile::damage_by_one()
{
	if (health == -1) return false;

	dmg_show_time = DAMAGE_SHOW_TIME;
	dmg_show_from_dmg = true;

	health--;
    return health == 0;
}

// Mark tile as acted temporarily
void Tile::mark_as_acted(char cooldown)
{
	action_cooldown = cooldown;
}

// Checks if can act now
bool Tile::can_act_now() const
{
	return action_cooldown == 0;
}

// Returns action_cooldown
char Tile::get_act_number() const
{
    return action_cooldown;
}

// Checks if can act now
unsigned char Tile::is_dmg_visible() const
{
	if (dmg_show_time == 0 || id == C::BULLET)
		return 0; // invisible
	else if (dmg_show_from_dmg)
		return 1; // visible red
	else
		return 2; // visible green
}

// Decrements action_cooldown if possible
void Tile::action_decrement()
{
	if (action_cooldown != 0)
		action_cooldown--;
}

// Decrements dmg_show_time if possible
void Tile::dmg_show_decrement()
{
	if (dmg_show_time != 0)
		dmg_show_time--;
}

// Checks if this function is called the first time
bool Tile::check_initialization()
{
	if (initialization_flag)
	{
		initialization_flag = false;
		return true;
	}
	else return false;
}

bool Tile::is_magical() const
{
    return magical;
}

// Returns bullet movement coordinates
Coords Tile::get_bullet_movement() const
{
	if (bullet_movement == 'N') return { 0,1 };
	if (bullet_movement == 'E') return { 1,0 };
	if (bullet_movement == 'S') return { 0,-1 };
	if (bullet_movement == 'W') return { -1,0 };
	return { 0,0 };
}

// Executes tile behaviour based on its type [This is Map's friend]
void Tile::execute_behaviour(Map* map, mt19937& ms_twister, int x, int y)
{
	// DANGER! Be careful!
	// ---------------------
	// Tile object represent tile coordinates and not the actual object.
	// When entity moves, it technically copies all of its values into
	// another spot and overwrites them with the default tile instantion Tile().
	// ---------------------
	// This means, that you should be very careful, when modifying this method.
	// Never reference to anything in this object after moving, damaging, removing
	// or affecting the object in any other way using map API, unless you really
	// know what you're doing!

    dmg_show_decrement();
    int frame = map->get_frame();

    // Player
    if (id == C::PLAYER)
    {
        static const int MOVEMENT_PERIOD = 2; // amount of frames
        static const int ACTION_COOLDOWN = 2; // amount of MOVEMENT_PERIOD periods

        if (frame % MOVEMENT_PERIOD == 0)
        {
            action_decrement();

            if (!is_space_pressed())
            {
                // movement mode
                Coords mov_input = get_movement_input();
                map->try_move(x, y, mov_input.x, mov_input.y, "push");
            }
            else if (can_act_now())
            {
                // shooting mode
                Coords shoot_input = get_shooting_input();
                if (shoot_input.x != 0 || shoot_input.y != 0)
                {
                    mark_as_acted(ACTION_COOLDOWN);
                    map->spawn_bullet(x, y, shoot_input.x, shoot_input.y, true, magical);
                }
            }
        }
    }

    // Animal
    if (id == C::ANIMAL)
    {
        static const int MOVEMENT_PERIOD = 16;

        if (frame % MOVEMENT_PERIOD == 0)
            map->passive_movement(x, y);
    }

    // Monster / Insect
    if (id == C::MONSTER || id == C::INSECT)
    {
        const int movement_period = id == C::MONSTER ? 8 : 4;
        static const int DAMAGE_PERIOD = 8;
        static const int PLAYER_SMELL = 15;

        if (frame % movement_period == 0)
        {
            // Decide where to go
            Coords feels_path;
            if (id == C::MONSTER)
                feels_path = map->pathfinding.pathfind(map->entities[0], { x,y }, ms_twister, "predator");
            else
                feels_path = map->pathfinding.pathfind(map->entities[0], { x,y }, ms_twister, "melee");

            // Go where it has been decided
            if (feels_path.x == 0 && feels_path.y == 0)
            {
                map->passive_movement(x, y);
            }
            else
            {
                if (!map->try_move(x, y, feels_path.x, feels_path.y, "bullets_ignore") && frame % DAMAGE_PERIOD == 0)
                {
                    char tile_attack = map->get_tile(x + feels_path.x, y + feels_path.y);
                    if (tile_attack == C::PLAYER || (tile_attack == C::ANIMAL && id == C::MONSTER))
                        map->damage(x + feels_path.x, y + feels_path.y);
                }
            }
        }
    }

    // Sniper
    if (id == C::SNIPER)
    {
        // TEMPORARY BEHAVIOUR
        static const int MOVEMENT_PERIOD = 8;

        if (frame % MOVEMENT_PERIOD == 0)
        {
            action_decrement();
            Coords sniper_vision = map->pathfinding.get_sniper_direction(map->entities[0], { x,y }, ms_twister, "distance");
            if (sniper_vision != Coords{ 0,0 })
            {
                // Targeting player, shoot!
                if (can_act_now())
                {
                    int dx = sniper_vision.x;
                    int dy = sniper_vision.y;
                    map->spawn_bullet(x, y, dx, dy, false, magical);
                    mark_as_acted(1); // if you want to slow down shooting, change to 2
                }
            }
            else
            {
                // Distance pathfinding
                Coords dist_path = map->pathfinding.pathfind_distance(map->entities[0], { x,y }, ms_twister, "distance");
                if (dist_path != Coords{ 0,0 })
                    map->try_move(x, y, dist_path.x, dist_path.y, "bullets_ignore");
                else
                    map->passive_movement(x, y);
            }
        }
    }

    // Insector
    if (id == C::INSECTOR)
    {
        static const int MOVEMENT_PERIOD = 8;
        static const int ACTION_COOLDOWN_MIN = 4; // amount of MOVEMENT_PERIOD
        static const int ACTION_COOLDOWN_MAX = 8; // amount of MOVEMENT_PERIOD

        const int cooldown_set = ms_twister() % (ACTION_COOLDOWN_MAX - ACTION_COOLDOWN_MIN + 1) + ACTION_COOLDOWN_MIN;

        if (frame % MOVEMENT_PERIOD == 0)
        {
            // Only for checking if can see the player
            Coords feels_path = map->pathfinding.pathfind(map->entities[0], { x,y }, ms_twister, "melee");

            // Reloading (even if doesn't see the player)
            action_decrement();

            // Insector logic
            bool reroll_now = check_initialization();
            if ((feels_path.x != 0 || feels_path.y != 0) && can_act_now() && !reroll_now)
            {
                mark_as_acted(cooldown_set);
                spawn_insects(map, ms_twister, magical, x, y);
            }
            else
            {
                if (reroll_now)
                    mark_as_acted(cooldown_set);

                map->passive_movement(x, y);
            }
        }
    }

    // Bullet
    if (id == C::BULLET)
    {
        Coords bul_vect = get_bullet_movement();
        if (!map->try_move(x, y, bul_vect.x, bul_vect.y))
        {
            // WARNING: Meelee virtual attack exists too (if bullet summoning space is obstructed, in spawn_bullet() function)
            if (map->damage(x + bul_vect.x, y + bul_vect.y, was_shot_by_player()))
                map->try_move(x, y, bul_vect.x, bul_vect.y);
            else
                map->remove(x, y, true);
        }
    }

    // Spawner
    if (id == C::SPAWNER)
    {
        Coords feels_player = map->pathfinding.pathfind(map->entities[0], { x,y }, ms_twister, "spawner");
        if (feels_player != Coords{ 0,0 })
        {
            while (!map->damage(x, y, false));
        }
    }

    // Egg
    if (id == C::EGG)
    {
        if (frame % EGG_PERIOD == 0)
        {
            if (can_act_now())
            {
                // if will be summoning other static entities, modify has_ai input to increase performance
                map->spawn(x, y, egg_type, (egg_type != C::FRUIT /* && ... */ ), magical).set_egg_spawned();
            }
            else action_decrement();
        }
    }
}

// --- STATIC MEMBERS ---

void Tile::on_kill(Map* map, mt19937& ms_twister, char id, bool mag, int x, int y)
{
    // Removing bullet if killed by it
    if (map->get_tile(x, y) == C::BULLET)
        map->remove(x, y, true);

    // Activating spawner
    if(id == C::SPAWNER)
        spawner_activate(map, ms_twister, mag, x, y);
}

// Spawns insects around a tile in the insector style
void Tile::spawn_insects(Map* map, mt19937& ms_twister, bool mag, int x, int y)
{
    static const Coords sides[4] = { {0,1}, {0,-1}, {1,0}, {-1,0} };

    static const int SIDE_CHANCE = 350; // in promiles

    bool anything_spawned = false;
    int retries = 0;

    while (!anything_spawned && retries < 6)
    {
        for (int i = 0; i < 4; i++)
        {
            int dx = sides[i].x;
            int dy = sides[i].y;

            if (map->get_tile(x + dx, y + dy) == ' ' && ms_twister() % 1000 < SIDE_CHANCE)
            {
                map->spawn(x + dx, y + dy, C::INSECT, true, mag).set_score(0);
                anything_spawned = true;
            }
        }

        retries++;
    }
}

// Spawns entities around a tile in the spawner style
void Tile::spawner_activate(Map* map, mt19937& ms_twister, bool mag, int x, int y)
{
    // Actual code
    static const int SP_RADIUS = 2;
    const int x_min = x - SP_RADIUS;
    const int x_max = x + SP_RADIUS;
    const int y_min = y - SP_RADIUS;
    const int y_max = y + SP_RADIUS;

    static const int MIN_ENTITIES = 4;
    static const int MAX_ENTITIES = 8;
    static const int SPAWN_CHANCE = 250; // in promiles

    int left_amount = ms_twister() % (MAX_ENTITIES - MIN_ENTITIES + 1) + MIN_ENTITIES;
    vector<Coords> candidate_coords;

    for (int x1 = x_min; x1 <= x_max; x1++)
        for (int y1 = y_min; y1 <= y_max; y1++)
        {
            if (mag && map->get_tile(x1, y1) == C::BLOCK && map->get_tile_ref(x1, y1).is_magical())
                map->spawn(x1, y1, C::BLOCK, false);

            if (map->get_tile(x1, y1) == ' ')
                candidate_coords.push_back({ x1, y1 });
        }

    std::shuffle(candidate_coords.begin(), candidate_coords.end(), ms_twister);

    for (int i = 0; i < candidate_coords.size(); i++)
    {
        int rand = ms_twister() % 1000;
        int x2 = candidate_coords[i].x;
        int y2 = candidate_coords[i].y;

        // Spawner entities will be treated as egg entities (will despawn and will prevent eggs from appearing)
        if (rand >= 0 && rand < 500)
            map->spawn(x2, y2, C::MONSTER, true, mag).set_egg_spawned();
        if (rand >= 500 && rand < 1000)
            map->spawn(x2, y2, C::INSECT, true, mag).set_egg_spawned();

        left_amount--;
        if (left_amount <= 0)
            break;
    }
}
