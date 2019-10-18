#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
using namespace std;

#include <iostream>

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetDir)
	: GameWorld(assetDir), m_alienCount(0), m_aliensDestroyed(0)
{
}

StudentWorld::~StudentWorld()
{
	cleanUp();
}


int StudentWorld::init()
{
	// Initializes 30 stars randomly onto the screen
	for (int k = 0; k < 30; k++)
	{
		ptr = new Star(this, randInt(0, VIEW_WIDTH - 1), randInt(0, VIEW_HEIGHT - 1));
		m_actors.push_back(ptr);
	}

	// Initialize NachenBlaster 
	m_player = new Player(this);
	m_actors.push_back(m_player);

	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	setStatusText();

		// Calls on each actor in the game to do something
	for (it = m_actors.begin(); it != m_actors.end(); it++)
	{
		(*it)->doSomething();
	}

		// Check if Player is dead
	if (m_player->isDead())
	{
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}

	// Determine if level is completed
	int totalShips = 6 + (4 * getLevel());
	int remainder = totalShips - m_aliensDestroyed;
	if (remainder == 0)
	{
		playSound(SOUND_FINISHED_LEVEL);
		return GWSTATUS_FINISHED_LEVEL;
	}

		// Remove any dead objects from the game
	for (it = m_actors.begin(); it != m_actors.end();)
	{
		if ((*it)->isDead())
		{				
			if ((*it)->isAlien())
				m_alienCount--;
			delete (*it);
			it = m_actors.erase(it);
		}
		else
			it++;
	}

		// Add new Alien ship to the game
	addNewAlien(remainder);

		// Add new star to the game
	if (randInt(1, 15) == 1)
	{
		ptr = new Star(this, VIEW_WIDTH - 1, randInt(0, VIEW_HEIGHT - 1));
		m_actors.push_back(ptr);
	}

	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	m_alienCount = 0;
	m_aliensDestroyed = 0;
	for (it = m_actors.begin(); it != m_actors.end(); it++)
	{
		delete (*it);
	}
	m_actors.clear();
}

Actor* StudentWorld::getOneCollidingAlien(const Actor* a) const
{
	list<Actor*>::const_iterator itc;
	for (itc = m_actors.begin(); itc != m_actors.end(); itc++)
	{
		if ((*itc)->isAlien())
		{
			if (objectsCollided((*itc)->getX(), (*itc)->getY(), (*itc)->getRadius(), a->getX(), a->getY(), a->getRadius()))
				return (*itc);
		}
	}
	return nullptr;
}

Player* StudentWorld::getCollidingPlayer(const Actor* a) const
{
	if (objectsCollided(m_player->getX(), m_player->getY(), m_player->getRadius(), a->getX(), a->getY(), a->getRadius()))
		return m_player;
	return nullptr;
}

bool StudentWorld::playerInSight(const Actor* a) const 
{
	if (a->getX() > m_player->getX() && a->getY() >= (m_player->getY() - 4) && a->getY() <= (m_player->getY() + 4))
		return true;
	return false;
}

void StudentWorld::addActor(Actor* a)
{
	m_actors.push_back(a);
}

void StudentWorld::recordAlienDestroyed()
{
	m_aliensDestroyed++;
}

void StudentWorld::addNewAlien(int remainder)
{
	int max = 4 + (0.5 * getLevel());

	if (m_alienCount < min(max, remainder))
	{
			// Determine which alien ship to add
		int s1 = 60;
		int s2 = 20 + (getLevel() * 5);
		int s3 = 5 + (getLevel() * 10);
		int s = s1 + s2 + s3;

		int r = randInt(0, s);
		if (r <= s1)
			ptr = new Smallgon(this);
		else if (r <= (s1 + s2))
			ptr = new Smoregon(this);
		else
			ptr = new Snagglegon(this);

		m_alienCount++;
		m_actors.push_back(ptr);
	}
}

bool StudentWorld::objectsCollided(double x1, double y1, double r1, double x2, double y2, double r2) const
{
	int k = sqrt(pow(x2 - x1, 2.0) + pow(y2 - y1, 2.0));

	if (k < 0.75*(r1 + r2))
		return true;
	return false;
}

void StudentWorld::setStatusText()
{
	ostringstream oss;
	oss << "Lives: ";
	oss << getLives();
	oss << "  Health: ";
	oss << m_player->healthPct();
	oss << "%  Score: ";
	oss << getScore();
	oss << "  Level: ";
	oss << getLevel();
	oss << "  Cabbages: ";
	oss << m_player->cabbagePct();
	oss << "%  Torpedos: ";
	oss << m_player->numTorpedos();
	string s = oss.str();
	setGameStatText(s);
}
