#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>

using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

int StudentWorld::init()
{
    player = new GhostRacer(this);
    createRoad(true);
    soul2Save = 2 * getLevel() + 5;
    bonus = 5000;
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    int level = getLevel();
    createRoad(false);
    randomSpawnActors(level);
    player->doSomething();
    for(auto actor : m_actors){
        actor->doSomething();
    }
    if (player->isDead()) {
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    if (soul2Save <= 0) {
        increaseScore(bonus);
        return GWSTATUS_FINISHED_LEVEL;
    }
    for (auto it = m_actors.begin(); it != m_actors.end();) {
        if ((*it)->isDead()) {
            delete* it;
            it = m_actors.erase(it);
        }
        else {
            ++it;
        }
    }
    std::string game_stats = "Score: " + std::to_string(getScore()) + " Lvl : " + std::to_string(getLevel()) + " Souls2Save : " + std::to_string(soul2Save) + " Lives : " 
            + std::to_string(getLives()) + " Health : " + std::to_string(player->getHealth()) + " Sprays : " + std::to_string(player->getHolyWaterCount()) + " Bonus : " + std::to_string(bonus);
    setGameStatText(game_stats);
    bonus--;
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete player;
    for (auto it = m_actors.begin(); it != m_actors.end();) {
        delete* it;
        it = m_actors.erase(it);
    }
}

GhostRacer* StudentWorld::getPlayer()
{
    return player;
}

vector<Actor*> StudentWorld::getActors()
{
    return m_actors;
}

void StudentWorld::addActorToWorld(Actor* new_actor)
{
    if (new_actor) {
        m_actors.push_back(new_actor);
    }
}

void StudentWorld::decSoul2Save()
{
    --soul2Save;
}

void StudentWorld::createRoad(bool is_init)
{
    if (is_init) {
        double left_edge = ROAD_CENTER - ROAD_WIDTH / 2.0;
        double right_edge = ROAD_CENTER + ROAD_WIDTH / 2.0;
        for (int j = 0; j < VIEW_HEIGHT / SPRITE_HEIGHT; j++) {
            BorderLine* left_lane = new BorderLine(IID_YELLOW_BORDER_LINE, left_edge, j * SPRITE_HEIGHT, this);
            BorderLine* right_lane = new BorderLine(IID_YELLOW_BORDER_LINE, right_edge, j * SPRITE_HEIGHT, this);
            addActorToWorld(left_lane);
            addActorToWorld(right_lane);
        }
        for (int j = 0; j < VIEW_HEIGHT / (4 * SPRITE_HEIGHT); j++) {
            BorderLine* middle_lane1 = new BorderLine(IID_WHITE_BORDER_LINE, left_edge + ROAD_WIDTH / 3.0, j * (4 * SPRITE_HEIGHT), this);
            BorderLine* middle_lane2 = new BorderLine(IID_WHITE_BORDER_LINE, right_edge - ROAD_WIDTH / 3.0, j * (4 * SPRITE_HEIGHT), this);
            addActorToWorld(middle_lane1);
            addActorToWorld(middle_lane2); 
            lastWhiteBorderAdded = middle_lane2;
        }
        return;
    }
    double new_border_y = (double)VIEW_HEIGHT - (double)SPRITE_HEIGHT;
    double delta_y = new_border_y - lastWhiteBorderAdded->getY();
    if (delta_y >= SPRITE_HEIGHT) {
        BorderLine* left_lane = new BorderLine(IID_YELLOW_BORDER_LINE, ROAD_CENTER - ROAD_WIDTH / 2.0, new_border_y, this);
        BorderLine* right_lane = new BorderLine(IID_YELLOW_BORDER_LINE, ROAD_CENTER + ROAD_WIDTH / 2.0, new_border_y, this);
        addActorToWorld(left_lane);
        addActorToWorld(right_lane);
    }
    if (delta_y >= 4.0 * SPRITE_HEIGHT) {
        BorderLine* middle_lane1 = new BorderLine(IID_WHITE_BORDER_LINE, ROAD_CENTER - ROAD_WIDTH / 2.0 +
            ROAD_WIDTH / 3, new_border_y, this);
        BorderLine* middle_lane2 = new BorderLine(IID_WHITE_BORDER_LINE, ROAD_CENTER + ROAD_WIDTH / 2.0 -
            ROAD_WIDTH / 3, new_border_y, this);
        addActorToWorld(middle_lane1);
        addActorToWorld(middle_lane2);
        lastWhiteBorderAdded = middle_lane2;
    }
}

void StudentWorld::randomSpawnActors(int L)
{
    int ChanceOilSlick = max(150 - L * 10, 40);
    int ChanceHumanPed = max(200 - L * 10, 30);
    int ChanceZombiePed = max(100 - L * 10, 20);
    int ChanceHolyWater = 100 + 10 * L;
    int ChanceLostSoul = 100;
    int ChanceVehicle = max(100 - L * 10, 20);
    if (randInt(0, ChanceOilSlick) == 0) {
        double new_X = randInt((int)(ROAD_CENTER - ROAD_WIDTH / 2.0), (int)(ROAD_CENTER + ROAD_WIDTH / 2.0));
        m_actors.push_back(new OilSlick(new_X, VIEW_HEIGHT, this));
    }
    if (randInt(0, ChanceHumanPed) == 0) {
        double new_X = randInt(0, VIEW_WIDTH);
        m_actors.push_back(new HumanPed(new_X, VIEW_HEIGHT, this));
    }
    if (randInt(0, ChanceZombiePed) == 0) {
        double new_X = randInt(0, VIEW_WIDTH);
        m_actors.push_back(new ZombiePed(new_X, VIEW_HEIGHT, this));
    }
    if (randInt(0, ChanceHolyWater) == 0) {
        double new_X = randInt((int)(ROAD_CENTER - ROAD_WIDTH / 2.0), (int)(ROAD_CENTER + ROAD_WIDTH / 2.0));
        m_actors.push_back(new HolyWaterGD(new_X, VIEW_HEIGHT, this));
    }
    if (randInt(0, ChanceLostSoul) == 0) {
        double new_X = randInt((int)(ROAD_CENTER - ROAD_WIDTH / 2.0), (int)(ROAD_CENTER + ROAD_WIDTH / 2.0));
        m_actors.push_back(new SoulGD(new_X, VIEW_HEIGHT, this));
    }
    if (randInt(0, ChanceVehicle) == 0) {
        spawnZombieCab();
    }
}

void StudentWorld::spawnZombieCab()
{
    int cur_lane = randInt(0, 2);
    double start_y = -1.0;
    double start_x = -1.0;
    double init_vspeed;

    for (int i = 0; i < 3; i++) {
        double lane_left;
        double lane_right;
        switch (cur_lane) {
        case 0:
            lane_left = ROAD_CENTER - ROAD_WIDTH / 2.0;
            lane_right = ROAD_CENTER - ROAD_WIDTH / 2.0 + ROAD_WIDTH / 3;
            start_x = ROAD_CENTER - ROAD_WIDTH / 3.0;
            break;
        case 1:
            lane_left = ROAD_CENTER - ROAD_WIDTH / 2.0 + ROAD_WIDTH / 3;
            lane_right = ROAD_CENTER + ROAD_WIDTH / 2.0 - ROAD_WIDTH / 3;
            start_x = ROAD_CENTER;
            break;
        case 2:
            lane_left = ROAD_CENTER + ROAD_WIDTH / 2.0 - ROAD_WIDTH / 3;
            lane_right = ROAD_CENTER + ROAD_WIDTH / 2.0;
            start_x = ROAD_CENTER + ROAD_WIDTH / 3.0;
            break;
        }
        double closest_bottom_y = VIEW_HEIGHT + 1.0;
        double closest_top_y = -1.0;

        if (player->getX() >= lane_left && player->getX() < lane_right) {
            cur_lane = (cur_lane + 1) % 3;
            continue;
        }

        for (auto actor : m_actors) {
            if (actor->getType() == "Damagedable" && actor->getX() >= lane_left && actor->getX() < lane_right) {
                if (actor->getY() < closest_bottom_y) {
                    closest_bottom_y = actor->getY();
                }
                if (actor->getY() > closest_top_y) {
                    closest_top_y = actor->getY();
                }
            }
        }
        if (closest_bottom_y == VIEW_HEIGHT + 1.0 || closest_bottom_y > VIEW_HEIGHT / 3.0) {
            start_y = SPRITE_HEIGHT / 2.0;
            init_vspeed = player->getVSpeed() + randInt(2, 4);
            break;
        }
        else if (closest_top_y == -1.0 || closest_top_y < VIEW_HEIGHT * 2 / 3.0) {
            start_y = VIEW_HEIGHT - SPRITE_HEIGHT / 2.0;
            init_vspeed = player->getVSpeed() - randInt(2, 4);
            break;
        }
        else {
            cur_lane = (cur_lane + 1) % 3;
        }
    }
    if (start_y > 0) {
        m_actors.push_back(new ZombieCab(start_x, start_y, init_vspeed, this));
    }
}
