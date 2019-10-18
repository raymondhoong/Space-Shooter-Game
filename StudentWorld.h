#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <sstream>
#include <list>
using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
		// Constructor and Destructor
	StudentWorld(std::string assetDir);
	virtual ~StudentWorld();

		// Virtual Functions from Base Class
	virtual int init();
	virtual int move();
	virtual void cleanUp();

		// Mutators
	void addActor(Actor* a);
	void recordAlienDestroyed();
	void addNewAlien(int remainder);

		// Accessors
	Actor* getOneCollidingAlien(const Actor* a) const;
	Player* getCollidingPlayer(const Actor* a) const;
	bool playerInSight(const Actor* a) const;

private:
		// Data Members
	list<Actor*> m_actors;
	list<Actor*>::iterator it;
	Actor* ptr;
	Player* m_player;
	int m_alienCount;
	int m_aliensDestroyed;

		// Help Functions
	bool objectsCollided(double x1, double y1, double r1, double x2, double y2, double r2) const;
	void setStatusText();
};

#endif // STUDENTWORLD_H_


