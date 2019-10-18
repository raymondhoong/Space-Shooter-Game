#ifndef ACTOR_INCLUDED
#define ACTOR_INCLUDED

#include "GraphObject.h"

const int HIT_BY_SHIP = 0;
const int HIT_BY_PROJECTILE = 1;

class StudentWorld;

///////////////////////////////////////////////////////////////////////////
//  Actor Definition
///////////////////////////////////////////////////////////////////////////

class Actor : public GraphObject
{
public:
		// Constructor and Destructor
	Actor(StudentWorld* w, double startX, double startY, int imageID, Direction imageDir, double size, unsigned int depth);
	virtual ~Actor();

		// Virtual Functions for Derived Classes
	virtual void doSomething();
	virtual void doSomethingDifferent() = 0;
	virtual bool markDead();
	virtual bool isAlien() const;

		// Mutators
	void setDead();

		// Accessors
	bool isDead() const;
	StudentWorld* world() const;

private:
	StudentWorld* m_world;
	bool m_isDead;
};

///////////////////////////////////////////////////////////////////////////
//  DamageableObject Definition
///////////////////////////////////////////////////////////////////////////

class DamageableObject : public Actor
{
public:
		// Constructor and Destructor
	DamageableObject(StudentWorld* w, double startX, double startY, int imageID, Direction startDir, double size, int depth, double hitPoints);
	virtual ~DamageableObject();

		// Virtual Functions from Base Class
	virtual bool markDead();

		// Virtual Functions for Derived Classes
	virtual void increaseHitPoints(double amt);
	virtual void sufferDamage(double amt, int cause);

		// Accessors
	double hitPoints() const;

private:
	double m_hitPoints;
};

///////////////////////////////////////////////////////////////////////////
//  Player Definition
///////////////////////////////////////////////////////////////////////////

class Player : public DamageableObject
{
public:
		// Constructor
	Player(StudentWorld* w);

		// Virtual Functions from Base Class
	virtual void doSomethingDifferent();
	virtual void moveActor();
	virtual void increaseHitPoints(double amt);
	virtual void sufferDamage(double amt, int cause);

		// Mutators
	void increaseTorpedos(int amt);

		// Accessors
	int healthPct() const;
	int cabbagePct() const;
	int numTorpedos() const;
private:
	int m_numCabbages;
	int m_numTorpedos;
};

///////////////////////////////////////////////////////////////////////////
//  Alien Definition
///////////////////////////////////////////////////////////////////////////

class Alien : public DamageableObject
{
public:
		// Constructor and Destructor
	Alien(StudentWorld* w, int imageID, double hitPoints, double damageAmt, double deltaX, double deltaY, double speed, unsigned int scoreValue);
	virtual ~Alien();

		// Virtual Functions from Base Class
	virtual bool isAlien() const;
	virtual void sufferDamage(double amt, int cause);

		// Virtual Functions for Derived Class
	virtual void setPath();
	virtual bool damageCollidingPlayer();
	virtual void possiblyDropGoodie();

		// Mutators
	virtual void moveAlien();
	void setDeltaY(double dy);
	void setDeltaX(double dx);
	void setSpeed(double speed);

private:
	double m_damageAmt;
	double m_speed;
	double m_deltaX;
	double m_deltaY;
	int m_scoreValue;
};

class Smallgon : public Alien
{
public:
	Smallgon(StudentWorld* w);
	virtual void doSomethingDifferent();
};

class Smoregon : public Alien
{
public:
	Smoregon(StudentWorld* w);
	virtual void doSomethingDifferent();
	virtual void possiblyDropGoodie();
};

class Snagglegon : public Alien
{
public:
	Snagglegon(StudentWorld* w);
	virtual void doSomethingDifferent();
	virtual void setPath();
	virtual void possiblyDropGoodie();
};

///////////////////////////////////////////////////////////////////////////
//  Projectile Definition
///////////////////////////////////////////////////////////////////////////

class Projectile : public Actor
{
public:
		// Constructor and Destructor
	Projectile(StudentWorld* w, double startX, double startY, int imageID, double damageAmt, double deltaX, Direction initialDir, bool rotates, bool friendly);
	virtual ~Projectile();

		// Virtual Function from Base Class
	virtual void doSomethingDifferent();

		// Mutators
	void moveProjectile();
	bool damageCollidedObject();

private:
	double m_damageAmt;
	double m_deltaX;
	bool m_rotates;
	bool m_friendly;
};

class Cabbage : public Projectile
{
public:
		// Constructor
	Cabbage(StudentWorld* w, double startX, double startY);

		// Virtual Functions from Base Class
	virtual bool markDead();
};

class Turnip : public Projectile
{
public:
		// Constructor
	Turnip(StudentWorld* w, double startX, double startY);
};

class PlayerTorpedo : public Projectile
{
public:
		// Constructor
	PlayerTorpedo(StudentWorld* w, double startX, double startY);

		// Virtual Functions from Base Class
	virtual bool markDead();
};

class AlienTorpedo : public Projectile
{
public:
		// Constructor
	AlienTorpedo(StudentWorld* w, double startX, double startY);
};

///////////////////////////////////////////////////////////////////////////
//  Goodie Definition
///////////////////////////////////////////////////////////////////////////

class Goodie : public Actor
{
public:
		// Constructor and Destructor
	Goodie(StudentWorld* w, double startX, double startY, int imageID);
	virtual ~Goodie();
	virtual bool markDead();
};

class ExtraLifeGoodie : public Goodie
{
public:
	ExtraLifeGoodie(StudentWorld* w, double startX, double startY);
	virtual void doSomethingDifferent();
};

class RepairGoodie : public Goodie
{
public:
	RepairGoodie(StudentWorld* w, double startX, double startY);
	virtual void doSomethingDifferent();
};

class TorpedoGoodie : public Goodie
{
public:
	TorpedoGoodie(StudentWorld* w, double startX, double startY);
	virtual void doSomethingDifferent();
};

///////////////////////////////////////////////////////////////////////////
//  Misc Definition
///////////////////////////////////////////////////////////////////////////

class Star : public Actor
{
public:
		// Constructor
	Star(StudentWorld* w, double startX, double startY);
	virtual void doSomethingDifferent();
};

class Explosion : public Actor
{
public:
		// Constructor
	Explosion(StudentWorld* w, double startX, double startY);
	virtual void doSomethingDifferent();
private:
	int m_tickCount;
};

#endif // ACTOR_H_
