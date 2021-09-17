#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>
#include <stdio.h>
#include <typeinfo>
#include <random>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

using namespace std;

#define PI 3.14159265

bool Actor::isOverlap(Actor* otherActor)
{
    double delta_x = abs(getX() - otherActor->getX());
    double delta_y = abs(getY() - otherActor->getY());
    double radius_sum = getRadius() + otherActor->getRadius();
    return delta_x < radius_sum * 0.25 && delta_y < radius_sum * 0.6;
}

void Actor::setDead()
{
    dead = true;
}

bool Damagedable::isDead()
{
    if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT) {
        return true;
    }
    return m_health <= 0;
}

void Damagedable::takeDamage(int amount)
{
    m_health -= amount;
    if (m_health < 0) {
        m_health = 0;
    }
    if (m_health > 100) {
        m_health = 100;
    }
}

void Damagedable::pedMovement()
{
    double vert_speed = getVSpeed() - getWorld()->getPlayer()->getVSpeed();
    double horiz_speed = getHSpeed();
    double new_y = getY() + vert_speed;
    double new_x = getX() + horiz_speed;
    moveTo(new_x, new_y);
}

void Damagedable::newMovementPlan()
{
    int rng = randInt(0, 1);
    int new_hspeed = 0;
    switch (rng) {
    case 0:
        new_hspeed = randInt(1, 3);
        break;
    case 1:
        new_hspeed = randInt(-3, -1);
        break;
    }
    setHSpeed(new_hspeed);
    m_movement_plan = randInt(4, 32);
    if (getHSpeed() < 0) {
        setDirection(180);
    }
    else {
        setDirection(0);
    }
}

void GhostRacer::doSomething()
{
    //Check if the actor is dead
    if (isDead()) {
        return;
    }

    //Check if Hit the boundary
    if (hitLeftRoad()) {
        if (getDirection() > 90) {
            takeDamage(10);
            setDirection(82);
            getWorld()->playSound(SOUND_VEHICLE_CRASH);
            GRMovement();
            return;
        }
    }
    else if (hitRightRoad()) {
        if (getDirection() < 90) {
            takeDamage(10);
            setDirection(98);
            getWorld()->playSound(SOUND_VEHICLE_CRASH);
            GRMovement();
            return;
        }
    }

    //Checking input From the User
    int ch;
    if (getWorld()->getKey(ch)) {
        switch (ch) {
        case KEY_PRESS_SPACE:
            if (m_holy_water_count > 0) {
                spawnHolyWater();
            }
            break;
        case KEY_PRESS_LEFT:
            if (getDirection() < 114) {
                setDirection(getDirection() + 8);
            }
            break;
        case KEY_PRESS_RIGHT:
            if (getDirection() > 66) {
                setDirection(getDirection() - 8);
            }
            break;
        case KEY_PRESS_UP:
            if (getVSpeed() < 5) {
                setVSpeed(getVSpeed() + 1);
            }
            break;
        case KEY_PRESS_DOWN:
            if (getVSpeed() > -1) {
                setVSpeed(getVSpeed() - 1);
            }
            break;
        }
    }
    GRMovement();
}

bool GhostRacer::isDead()
{
    if (getHealth() <= 0) {
        setDead();
        getWorld()->playSound(SOUND_PLAYER_DIE);
    }
    return Actor::isDead();
}

int GhostRacer::getHolyWaterCount()
{
    return m_holy_water_count;
}

void GhostRacer::addHolyWater(int amount)
{
    m_holy_water_count += amount;
}

void GhostRacer::spin()
{
    int rng = randInt(0, 1);
    int new_dir = 0;
    if (rng == 0) {
        new_dir = getDirection() + randInt(5, 20);
        if (new_dir > 120) {
            new_dir = 120;
        }
    }
    else {
        new_dir = getDirection() - randInt(5, 20);
        if (new_dir < 60) {
            new_dir = 60;
        }
    }
    setDirection(new_dir);
}

bool GhostRacer::hitLeftRoad()
{
    return getX() <= (VIEW_WIDTH - ROAD_WIDTH) / 2.0;
}

bool GhostRacer::hitRightRoad()
{
    return getX() >= VIEW_WIDTH - (VIEW_WIDTH - ROAD_WIDTH) / 2.0;
}

void GhostRacer::GRMovement()
{
    double max_shift_per_tick = 4.0;
    int direction = getDirection();
    double delta_x = cos(direction * PI / 180) * max_shift_per_tick;
    double cur_x = getX();
    double cur_y = getY();
    moveTo(cur_x + delta_x, cur_y);
}

void GhostRacer::spawnHolyWater()
{
    double offset = 5;
    double spray_x = getX() - cos(getDirection() * PI / 180) * 5;
    double spray_y = getY() + sin(getDirection() * PI / 180) * 5;
    HolyWater* spray = new HolyWater(spray_x, spray_y, getDirection(), getWorld());
    getWorld()->addActorToWorld(spray);
    getWorld()->playSound(SOUND_PLAYER_SPRAY);
    m_holy_water_count--;
}

void BorderLine::doSomething()
{
    double vert_speed = getVSpeed() - getWorld()->getPlayer()->getVSpeed();
    double horiz_speed = getHSpeed();
    double new_y = getY() + vert_speed;
    double new_x = getX() + horiz_speed;
    moveTo(new_x, new_y);
}

bool BorderLine::isDead()
{
    return getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT;
}

void HolyWater::doSomething()
{
    if (isDead()) {
        return;
    }
    for (auto actor : getWorld()->getActors()) {
        if (isOverlap(actor)) {
            char* actor_type = actor->getType();
            if (actor_type == "Damagedable") {
                if (actor != getWorld()->getPlayer() && actor != this) {
                    actor->takeDamage(1);
                    dead = true;
                    return;
                }
            }
            else if (actor_type == "HealingGD" || actor_type == "HolyWaterGD") {
                actor->setDead();
                dead = true;
                return;
            }
        }
    }
    moveForward(SPRITE_HEIGHT);
}

bool HolyWater::isDead()
{
    if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT){
        dead = true;
    }
    if (sqrt(pow(getX() - startX, 2) + pow(getY() - startY, 2) * 1.0) >= 160) {
        dead = true;
    }
    return dead;
}

void HumanPed::doSomething()
{
    if (isDead()) {
        return;
    }
    if (isOverlap(getWorld()->getPlayer())) {
        getWorld()->getPlayer()->setDead();
        return;
    }
    pedMovement();
    setMovementPlan(getMovementPlan() - 1);
    if (getMovementPlan() > 0) {
        return;
    }
    else {
        newMovementPlan();
    }
}

void HumanPed::takeDamage(int amount)
{
    setHSpeed(getHSpeed() * -1);
    if (getDirection() == 0) {
        setDirection(180);
    }
    else {
        setDirection(0);
    }
    getWorld()->playSound(SOUND_PED_HURT);
}

void ZombiePed::doSomething()
{
    if (isDead()) {
        return;
    }
    GhostRacer* player = getWorld()->getPlayer();
    if (isOverlap(player)) {
        player->takeDamage(5);
        takeDamage(2);
        return;
    }
    if (abs(getX() - player->getX()) <= 30 && getY() > player->getY()) {
        setDirection(270);
        if (getX() < player->getX()) {
            setHSpeed(1);
        }
        else if (getX() > player->getX()){
            setHSpeed(-1);
        }
        else {
            setHSpeed(0);
        }
        --grunt_tick;
        if (grunt_tick <= 0) {
            getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
            grunt_tick = 20;
        }
    }
    pedMovement();
    if (getMovementPlan() > 0) {
        setMovementPlan(getMovementPlan() - 1);
        return;
    }
    else {
        newMovementPlan();
    }
}

void ZombiePed::takeDamage(int amount)
{
    StudentWorld* world = getWorld();
    Damagedable::takeDamage(amount);
    if (isDead()) {
        world->playSound(SOUND_PED_DIE);
        if (!isOverlap(world->getPlayer())) {
            if (randInt(0, 4) == 0) {
                world->addActorToWorld(new HealingGD(getX(), getY(), world));
            }
        }
        world->increaseScore(150);
    }
    else {
        world->playSound(SOUND_PED_HURT);
    }
}

void Goodies::doSomething()
{
    if (isDead()) {
        return;
    }
    double vert_speed = getVSpeed() - getWorld()->getPlayer()->getVSpeed();
    double horiz_speed = getHSpeed();
    double new_y = getY() + vert_speed;
    double new_x = getX() + horiz_speed;
    moveTo(new_x, new_y);
    GhostRacer* player = getWorld()->getPlayer();
    if (isOverlap(player)) {
        activate();
    }
}

bool Goodies::isDead()
{
    if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT) {
        setDead();
    }
    return Actor::isDead();
}

void HealingGD::activate()
{
    StudentWorld* world = getWorld();
    world->getPlayer()->takeDamage(-10);
    world->playSound(SOUND_GOT_GOODIE);
    world->increaseScore(250);
    setDead();
}

void HolyWaterGD::activate()
{
    StudentWorld* world = getWorld();
    world->getPlayer()->addHolyWater(10);
    world->playSound(SOUND_GOT_GOODIE);
    world->increaseScore(50);
    setDead();
}

void SoulGD::doSomething()
{
    Goodies::doSomething();
    setDirection((getDirection() + 10) % 360);
}

void SoulGD::activate()
{
    StudentWorld* world = getWorld();
    world->decSoul2Save();
    world->playSound(SOUND_GOT_SOUL);
    world->increaseScore(100);
    setDead();
}

void OilSlick::activate()
{
    StudentWorld* world = getWorld();
    world->playSound(SOUND_OIL_SLICK);
    world->getPlayer()->spin();
}

void ZombieCab::doSomething()
{
    if (isDead()) {
        return;
    }
    GhostRacer* player = getWorld()->getPlayer();
    if (isOverlap(player)) {
        if (damagedPlayer) {
            pedMovement();
        }
        else {
            getWorld()->playSound(SOUND_VEHICLE_CRASH);
            player->takeDamage(20);
            if (getX() <= player->getX()) {
                setHSpeed(-5);
                setDirection(120 + randInt(0, 19));
            }
            else {
                setHSpeed(5);
                setDirection(60 - randInt(0, 19));
            }
            damagedPlayer = true;
        }
    }
    pedMovement();
    if (getVSpeed() > player->getVSpeed()) {
        for (auto actor : getWorld()->getActors()) {
            if (actor->getType() == "Damagedable") {
                if (actor->getY() > getY() && actor->getY() - getY() < 96) {
                    setVSpeed(getVSpeed() - 0.5);
                    return;
                }
            }
        }
    }
    else {
        for (auto actor : getWorld()->getActors()) {
            if (actor->getType() == "Damagedable") {
                if (actor->getY() < getY() && getY() - actor->getY() < 96) {
                    setVSpeed(getVSpeed() + 0.5);
                    return;
                }
            }
        }
    }
    setMovementPlan(getMovementPlan() - 1);
    if (getMovementPlan() > 0) {
        return;
    }
    else {
        newMovementPlan();
    }
}

void ZombieCab::takeDamage(int amount)
{
    StudentWorld* world = getWorld();
    Damagedable::takeDamage(amount);
    if (isDead()) {
        world->playSound(SOUND_VEHICLE_DIE);
        if (!isOverlap(world->getPlayer())) {
            if (randInt(0, 4) == 0) {
                world->addActorToWorld(new OilSlick(getX(), getY(), world));
            }
        }
        world->increaseScore(200);
        return;
    }
    else {
        world->playSound(SOUND_VEHICLE_HURT);
    }
}

void ZombieCab::newMovementPlan()
{
    setMovementPlan(randInt(4, 32));
    setVSpeed(getVSpeed() + randInt(-2, 2));
}
