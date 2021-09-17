#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <vector>
#include "Actor.h"

using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();

    GhostRacer* getPlayer();
    vector<Actor*> getActors();
    void addActorToWorld(Actor* new_actor);
    void decSoul2Save();

private:
    GhostRacer* player;
    vector<Actor*> m_actors;
    BorderLine* lastWhiteBorderAdded;
    int soul2Save;
    int bonus;

    void createRoad(bool is_init);
    void randomSpawnActors(int L);
    void spawnZombieCab();
};

#endif // STUDENTWORLD_H_
