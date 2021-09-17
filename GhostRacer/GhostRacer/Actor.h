#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
//#include "StudentWorld.h"

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject 
{
public:
	Actor(int imageID, double startX, double startY, int dir = 0, double size = 1.0, unsigned int depth = 0, double v_speed = 0, double h_speed = 0, StudentWorld* world=nullptr)
		: GraphObject(imageID, startX, startY, dir, size, depth)
	{
		m_vspeed = v_speed;
		m_hspeed = h_speed;
		m_world = world;
		dead = false;
	}

	virtual void doSomething() = 0;

	virtual bool isDead() {
		return dead;
	}

	virtual void takeDamage(int amount) 
	{
		return;
	}

	virtual char* getType() {
		return "Actor";
	}

	bool isOverlap(Actor* otherActor);

	void setDead();

	double getVSpeed() {
		return m_vspeed;
	}

	double getHSpeed() {
		return m_hspeed;
	}

	void setVSpeed(double new_speed) {
		m_vspeed = new_speed;
	}

	void setHSpeed(double new_speed) {
		m_hspeed = new_speed;
	}

	StudentWorld* getWorld() 
	{
		return m_world;
	}

private:
	StudentWorld* m_world;
	double m_vspeed;
	double m_hspeed;
	bool dead;
};

class Damagedable : public Actor 
{
public:
	Damagedable(int imageID, double startX, double startY, int dir = 0, double size = 1.0, unsigned int depth = 0, int health = 100, double v_speed=0, double h_speed=0, StudentWorld* s_world = nullptr)
		:Actor(imageID, startX, startY, dir, size, depth, v_speed, h_speed, s_world)
	{
		m_health = health;
		m_movement_plan = 0;
	}

	virtual bool isDead();

	virtual void takeDamage(int amount);

	virtual void pedMovement();

	virtual void newMovementPlan();

	virtual char* getType() {
		return "Damagedable";
	}

	int getHealth() {
		return m_health;
	}

	double getMovementPlan() {
		return m_movement_plan;
	}

	void setMovementPlan(double new_mp) {
		m_movement_plan = new_mp;
	}

private:
	int m_health;
	double m_movement_plan;
};

class GhostRacer : public Damagedable
{
public:
	GhostRacer(StudentWorld* world = nullptr)
		:Damagedable(IID_GHOST_RACER, VIEW_WIDTH/2, VIEW_HEIGHT/8, 90, 4.0, 0, 100, 0, 0, world)
	{
		m_holy_water_count = 10;
	}

	void doSomething();

	bool isDead();

	int getHolyWaterCount();

	void addHolyWater(int amount);

	void spin();

private:
	int m_holy_water_count;
	bool hitLeftRoad();
	bool hitRightRoad();
	void GRMovement();
	void spawnHolyWater();
};

class HumanPed : public Damagedable
{
public:
	HumanPed(double x, double y, StudentWorld* world = nullptr)
		:Damagedable(IID_HUMAN_PED, x, y, 0, 2.0, 0, 2, -4, 0, world)
	{
	}

	void doSomething();

	void takeDamage(int amount);
};

class ZombiePed : public Damagedable
{
public:
	ZombiePed(double x, double y, StudentWorld* world = nullptr)
		:Damagedable(IID_ZOMBIE_PED, x, y, 0, 3.0, 0, 2, -4, 0, world)
	{
		grunt_tick = 0;
	}

	void doSomething();

	void takeDamage(int amount);

private:
	int grunt_tick;
};

class ZombieCab : public Damagedable
{
public:
	ZombieCab(double x, double y, double v_speed, StudentWorld* world = nullptr)
		:Damagedable(IID_ZOMBIE_CAB, x, y, 90, 4.0, 0, 3, v_speed, 0, world)
	{
		damagedPlayer = false;
	}

	void doSomething();

	void takeDamage(int amount);

	void newMovementPlan();

private:
	bool damagedPlayer;
};

class Goodies : public Actor
{
public:
	Goodies(int imageID, double startX, double startY, int dir = 0, double size = 1.0, StudentWorld* s_world = nullptr)
		:Actor(imageID, startX, startY, dir, size, 2, -4.0, 0, s_world) 
	{
	}

	void doSomething();

	virtual bool isDead();

	virtual void activate() = 0;

	virtual char* getType() {
		return "Goodies";
	}
};

class HealingGD : public Goodies
{
public:
	HealingGD(double x, double y, StudentWorld* world = nullptr)
		:Goodies(IID_HEAL_GOODIE, x, y, 0, 1.0, world)
	{
	}
	
	char* getType() {
		return "HealingGD";
	}

	void activate();

};

class HolyWaterGD : public Goodies
{
public:
	HolyWaterGD(double x, double y, StudentWorld* world = nullptr)
		:Goodies(IID_HOLY_WATER_GOODIE, x, y, 90, 2.0, world)
	{
	}

	char* getType() {
		return "HolyWaterGD";
	}

	void activate();

};

class SoulGD : public Goodies
{
public:
	SoulGD(double x, double y, StudentWorld* world = nullptr)
		:Goodies(IID_SOUL_GOODIE, x, y, 0, 4.0, world)
	{
	}

	void doSomething();

	void activate();
};

class OilSlick : public Goodies
{
public:
	OilSlick(double x, double y, StudentWorld* world = nullptr)
		:Goodies(IID_OIL_SLICK, x, y, 0, randInt(2,5), world)
	{
	}

	void activate();
};

class BorderLine : public Actor 
{
public:
	BorderLine(int image, double x, double y, StudentWorld* world = nullptr) 
		:Actor(image, x, y, 0, 2.0, 2, -4.0, 0, world)
	{
	}

	void doSomething();
	
	bool isDead();
};

class HolyWater : public Actor
{
public:
	HolyWater(double x, double y, int dir, StudentWorld* world = nullptr)
		:Actor(IID_HOLY_WATER_PROJECTILE, x, y, dir, 1.0, 1, 0, 0, world)
	{
		startX = x;
		startY = y;
		dead = false;
	}

	void doSomething();

	bool isDead();

private:
	double startX;
	double startY;
	bool dead;
};
#endif // ACTOR_H_