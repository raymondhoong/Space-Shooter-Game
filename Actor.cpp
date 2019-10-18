#include "Actor.h"
#include "StudentWorld.h"

///////////////////////////////////////////////////////////////////////////
//  Actor Implementation
///////////////////////////////////////////////////////////////////////////

Actor::Actor(StudentWorld* w, double startX, double startY, int imageID, Direction imageDir, double size, unsigned int depth)
	: GraphObject(imageID, startX, startY, imageDir, size, depth)
{
	m_world = w;
	m_isDead = false;
}

Actor::~Actor()
{
}

void Actor::doSomething()
{
	if (isDead())
		return;

		// If Actor has either flown off screen of lost all of its hit points
	if (markDead())
	{
		setDead();
		return;
	}

	doSomethingDifferent();
}

bool Actor::markDead()
{
		// Covers enemy Projectiles that have flown off screen
	if (getX() <= -1)
		return true;
	return false;
}

void Actor::setDead()
{
	m_isDead = true;
}

bool Actor::isDead() const
{
	return m_isDead;
}

bool Actor::isAlien() const
{
	return false;
}

StudentWorld* Actor::world() const
{
	return m_world;
}

///////////////////////////////////////////////////////////////////////////
//  DamageableObject Implementation
///////////////////////////////////////////////////////////////////////////

DamageableObject::DamageableObject(StudentWorld* w, double startX, double startY, int imageID, Direction startDir, double size, int depth, double hitPoints)
	: Actor(w, startX, startY, imageID, startDir, size, depth), m_hitPoints(hitPoints)
{
}

DamageableObject::~DamageableObject()
{
}

bool DamageableObject::markDead()
{
	if (getX() <= -1 || m_hitPoints <= 0)
		return true;
	return false;
}

void DamageableObject::increaseHitPoints(double amt)
{
	m_hitPoints += amt;
}

void DamageableObject::sufferDamage(double amt, int cause)
{
	m_hitPoints -= amt;
}

double DamageableObject::hitPoints() const
{
	return m_hitPoints;
}

///////////////////////////////////////////////////////////////////////////
//  Player Implementation
///////////////////////////////////////////////////////////////////////////

Player::Player(StudentWorld* w)
	: DamageableObject(w, 0, 128, IID_NACHENBLASTER, 0, 1.0, 0, 50), m_numCabbages(30), m_numTorpedos(0)
{
}

void Player::doSomethingDifferent()
{
	moveActor();

	// Replenishes one cabbage energy point to a max of 30
	if (m_numCabbages < 30)
		m_numCabbages++;
}

void Player::moveActor()
{
	int key;
	if (world()->getKey(key))
	{
		switch (key)
		{
		case KEY_PRESS_DOWN:
			if ((getY() - 6) >= 0)
				moveTo(getX(), getY() - 6);
			break;
		case KEY_PRESS_UP:
			if ((getY() + 6) < VIEW_HEIGHT)
				moveTo(getX(), getY() + 6);
			break;
		case KEY_PRESS_LEFT:
			if ((getX() - 6) >= 0)
				moveTo(getX() - 6, getY());
			break;
		case KEY_PRESS_RIGHT:
			if ((getX() + 6) < VIEW_WIDTH)
				moveTo(getX() + 6, getY());
			break;
		case KEY_PRESS_TAB:
			if (m_numTorpedos >= 1)
			{
				m_numTorpedos--;
				Actor* ptr = new PlayerTorpedo(world(), getX() + 12, getY());
				world()->playSound(SOUND_TORPEDO);
				world()->addActor(ptr);
			}
			break;
		case KEY_PRESS_SPACE:
			if (m_numCabbages >= 5)
			{
				m_numCabbages = m_numCabbages - 5;
				Actor* ptr = new Cabbage(world(), getX() + 12, getY());
				world()->playSound(SOUND_PLAYER_SHOOT);
				world()->addActor(ptr);
			}
			break;
		}
	}
}

void Player::increaseHitPoints(double amt)
{
	double m = hitPoints() + amt;
	if (m > 50)
		DamageableObject::increaseHitPoints(50 - hitPoints());
	else
		DamageableObject::increaseHitPoints(m);
}

void Player::sufferDamage(double amt, int cause)
{
	DamageableObject::sufferDamage(amt, cause);
	if (cause == HIT_BY_PROJECTILE)
		world()->playSound(SOUND_BLAST);
}

void Player::increaseTorpedos(int amt)
{
	m_numTorpedos += amt;
}

int Player::healthPct() const
{
	// MIGHT BE WRONG
	return (hitPoints() / 50.0 * 100.0);
}

int Player::cabbagePct() const
{
	return (m_numCabbages / 30.0 * 100.0);
}

int Player::numTorpedos() const
{
	return m_numTorpedos;
}

///////////////////////////////////////////////////////////////////////////
//  Alien Implementation
///////////////////////////////////////////////////////////////////////////

Alien::Alien(StudentWorld* w, int imageID, double hitPoints, double damageAmt, double deltaX, double deltaY, double speed, unsigned int scoreValue)
	: DamageableObject(w, VIEW_WIDTH - 1, randInt(0, VIEW_HEIGHT - 1), imageID, 0, 1.5, 1, hitPoints)
{
	m_damageAmt = damageAmt;
	m_speed = speed;
	m_deltaX = deltaX;
	m_deltaY = deltaY;
	m_scoreValue = scoreValue;
}

Alien::~Alien()
{}

bool Alien::isAlien() const
{
	return true;
}

bool Alien::damageCollidingPlayer()
{
	Player* player = world()->getCollidingPlayer(this);

	if (player == nullptr)
		return false;

	player->sufferDamage(m_damageAmt, HIT_BY_SHIP);
	sufferDamage(0, HIT_BY_SHIP);

	return true;
}

void Alien::sufferDamage(double amt, int cause)
{
	if (cause == HIT_BY_SHIP)
	{
		setDead();
		possiblyDropGoodie();
		world()->recordAlienDestroyed();
		world()->increaseScore(m_scoreValue);
		world()->playSound(SOUND_DEATH);
		Actor* exp = new Explosion(world(), getX(), getY());
		world()->addActor(exp);
	}
	else
	{
		DamageableObject::sufferDamage(amt, cause);
		if (hitPoints() <= 0)
		{
			setDead();
			possiblyDropGoodie();
			world()->recordAlienDestroyed();
			world()->increaseScore(m_scoreValue);
			world()->playSound(SOUND_DEATH);
			Actor* exp = new Explosion(world(), getX(), getY());
			world()->addActor(exp);
		}
		else
			world()->playSound(SOUND_BLAST);
	}
}

void Alien::possiblyDropGoodie()
{
	return;
}

void Alien::setPath()
{
	if (m_deltaX == 0 || getY() >= VIEW_HEIGHT - 1 || getY() <= 0)
	{
		if (getY() >= VIEW_HEIGHT - 1)								// DOWN AND LEFT
			m_deltaY = m_deltaX = -randInt(1, 32);
		else if (getY() <= 0)										// UP AND LEFT
		{
			m_deltaX = -randInt(1, 32);
			m_deltaY = -1 * m_deltaX;
		}
		else
		{
			int k = randInt(1, 3);
			if (k == 1)												// DOWN AND LEFT
				m_deltaY = m_deltaX = -randInt(1, 32);
			else if (k == 2)										// UP AND LEFT
			{
				m_deltaX = -randInt(1, 32);
				m_deltaY = -1 * m_deltaX;
			}
			else													// DUE LEFT
			{
				m_deltaX = -randInt(1, 32);
				m_deltaY = 0;
			}
		}
	}
}

void Alien::moveAlien()
{
	if (m_deltaY == 0)										// DUE LEFT
	{
		moveTo(getX() - m_speed, getY());
		m_deltaX++;
	}
	else if (m_deltaY < 0)									// DOWN AND LEFT
	{
		moveTo(getX() - m_speed, getY() - m_speed);
		m_deltaX++;
		m_deltaY++;
	}
	else													// UP AND LEFT
	{
		moveTo(getX() - m_speed, getY() + m_speed);
		m_deltaX++;
		m_deltaY--;
	}
}

void Alien::setDeltaY(double dy)
{
	m_deltaY = dy;
}

void Alien::setDeltaX(double dx)
{
	m_deltaX = dx;
}

void Alien::setSpeed(double speed)
{
	m_speed = speed;
}

Smallgon::Smallgon(StudentWorld* w)
	: Alien(w, IID_SMALLGON, 5 * (1 + (w->getLevel() - 1)*0.1), 5, 0, 0, 2, 250)
{}

void Smallgon::doSomethingDifferent()
{
		// Check for collision with Player
	if (damageCollidingPlayer())
		return;

		// Determine the flight path for the Alien
	setPath();

		// Shoot a Turnip if possible
	if (world()->playerInSight(this))
	{
		if (randInt(1, (20 / world()->getLevel()) + 5) == 1)
		{
			Actor* ptr = new Turnip(world(), getX() - 14, getY());
			world()->playSound(SOUND_ALIEN_SHOOT);
			world()->addActor(ptr);
			return;
		}
	}

		// Make a move in specified direction
	moveAlien();

	// Check for collision with Player AGAIN
	if (damageCollidingPlayer())
		return;
}

Smoregon::Smoregon(StudentWorld* w) 
	: Alien(w, IID_SMOREGON, 5 * (1 + (w->getLevel() - 1)*0.1), 5, 0, 0, 2.0, 250)
{
}

void Smoregon::doSomethingDifferent()
{
		// Check for collision with Player
	if (damageCollidingPlayer())
		return;

		// Determine the flight path for the Alien
	setPath();

		// Shoot a Turnip or speed ahead if possible
	if (world()->playerInSight(this))
	{
		if (randInt(1, (20 / world()->getLevel()) + 5) == 1)
		{
			Actor* ptr = new Turnip(world(), getX() - 14, getY());
			world()->playSound(SOUND_ALIEN_SHOOT);
			world()->addActor(ptr);
			return;
		}
		if (randInt(1, (20 / world()->getLevel()) + 5) == 1)
		{
			setDeltaX(VIEW_WIDTH);
			setDeltaY(0);
			setSpeed(5);
		}
	}

		// Make a move in specified direction
	moveAlien();

		// Check for collision with Player AGAIN
	if (damageCollidingPlayer())
		return;
}

void Smoregon::possiblyDropGoodie()
{
	if (randInt(1, 3) == 2)
	{
		Actor* g;
		if (randInt(1, 2) == 2)
			g = new RepairGoodie(world(), getX(), getY());
		else
			g = new TorpedoGoodie(world(), getX(), getY());
		world()->addActor(g);
	}
}

Snagglegon::Snagglegon(StudentWorld* w)
	: Alien(w, IID_SNAGGLEGON, 10 * (1 + (w->getLevel() - 1)*0.1), 15, 1, -VIEW_HEIGHT, 1.75, 1000)
{}

void Snagglegon::setPath()
{
	if (getY() >= VIEW_HEIGHT - 1)								// DOWN AND LEFT
		setDeltaY(-VIEW_HEIGHT);
	else if (getY() <= 0)										// UP AND LEFT
		setDeltaY(VIEW_HEIGHT);
}

void Snagglegon::doSomethingDifferent()
{
		// Check for collision with Player
	if (damageCollidingPlayer())
		return;

		// Determine the flight path for the Alien
	setPath();

		// Shoot a Torpedo if possible
	if (world()->playerInSight(this))
	{
		if (randInt(1, (15 / world()->getLevel()) + 10) == 1)
		{
			Actor* ptr = new AlienTorpedo(world(), getX() - 14, getY());
			world()->playSound(SOUND_TORPEDO);
			world()->addActor(ptr);
			return;
		}
	}

		// Make a move in specified direction
	moveAlien();

		// Check for collision with Player AGAIN
	if (damageCollidingPlayer())
		return;
}

void Snagglegon::possiblyDropGoodie()
{
	if (randInt(1, 6) == 3)
	{
		Actor* g;
		g = new ExtraLifeGoodie(world(), getX(), getY());
		world()->addActor(g);
	}
}

///////////////////////////////////////////////////////////////////////////
//  Projectile Implementation
///////////////////////////////////////////////////////////////////////////

Projectile::Projectile(StudentWorld* w, double startX, double startY, int imageID, double damageAmt, double deltaX, Direction initialDir, bool rotates, bool friendly)
	: Actor(w, startX, startY, imageID, initialDir, 0.5, 1), m_damageAmt(damageAmt), m_deltaX(deltaX), m_rotates(rotates), m_friendly(friendly)
{}

Projectile::~Projectile()
{}

void Projectile::moveProjectile()
{
	if (m_friendly)
	{
		if (getX() < VIEW_WIDTH)
			moveTo(getX() + m_deltaX, getY());
	}
	else
	{
		if (getX() >= 0)
			moveTo(getX() - m_deltaX, getY());
	}

	if (m_rotates)
		setDirection(getDirection() + 20);
}

bool Projectile::damageCollidedObject()
{
	if (m_friendly)
	{
		Actor* a = world()->getOneCollidingAlien(this);

		if (a != nullptr)
		{
			Alien* alien = static_cast<Alien*>(a);
			alien->sufferDamage(m_damageAmt, HIT_BY_PROJECTILE);
			setDead();
			return true;
		}
	}
	else
	{
		Player* player = world()->getCollidingPlayer(this);

		if (player != nullptr)
		{
			player->sufferDamage(m_damageAmt, HIT_BY_PROJECTILE);
			setDead();
			return true;
		}
	}
	return false;
}

void Projectile::doSomethingDifferent()
{
	if (damageCollidedObject())
		return;

	// Move Cabbage projectile across the screen
	moveProjectile();

	// Checks for collision with Alien ship
	if (damageCollidedObject())
		return;
}

Cabbage::Cabbage(StudentWorld* w, double startX, double startY)
	: Projectile(w, startX, startY, IID_CABBAGE, 2.0, 8.0, 0, true, true)
{}

bool Cabbage::markDead()
{
	if (getX() >= VIEW_WIDTH)
		return true;
	return false;
}

Turnip::Turnip(StudentWorld* w, double startX, double startY)
	: Projectile(w, startX, startY, IID_TURNIP, 2, 6, 0, true, false)
{}

PlayerTorpedo::PlayerTorpedo(StudentWorld* w, double startX, double startY)
	: Projectile(w, startX, startY, IID_TORPEDO, 8, 8, 0, false, true)
{}

bool PlayerTorpedo::markDead()
{
	if (getX() >= VIEW_WIDTH)
		return true;
	return false;
}

AlienTorpedo::AlienTorpedo(StudentWorld* w, double startX, double startY)
	: Projectile(w, startX, startY, IID_TORPEDO, 8, 8, 180, false, false)
{}

///////////////////////////////////////////////////////////////////////////
//  Goodies Implementation
///////////////////////////////////////////////////////////////////////////

Goodie::Goodie(StudentWorld* w, double startX, double startY, int imageID)
	: Actor(w, startX, startY, imageID, 0, 0.5, 1)
{}

Goodie::~Goodie()
{}

bool Goodie::markDead()
{
	if (getX() <= -1 || getY() <= - 1)
		return true;
	return false;
}

ExtraLifeGoodie::ExtraLifeGoodie(StudentWorld* w, double startX, double startY)
	: Goodie(w, startX, startY, IID_LIFE_GOODIE)
{}

void ExtraLifeGoodie::doSomethingDifferent()
{
		// Check for any collision with Player
	Player* p = world()->getCollidingPlayer(this);

	if (p != nullptr)
	{
		world()->increaseScore(100);
		setDead();
		world()->playSound(SOUND_GOODIE);
		world()->incLives();
		return;
	}

		// Move the goodie DOWN and LEFT
	moveTo(getX() - 0.75, getY() - 0.75);

		// Check for any collision with Player AGAIN
	p = world()->getCollidingPlayer(this);

	if (p != nullptr)
	{
		world()->increaseScore(100);
		setDead();
		world()->playSound(SOUND_GOODIE);
		world()->incLives();
		return;
	}
}

RepairGoodie::RepairGoodie(StudentWorld* w, double startX, double startY)
	: Goodie(w, startX, startY, IID_REPAIR_GOODIE)
{}

void RepairGoodie::doSomethingDifferent()
{
		// Check for any collision with Player
	Player* p = world()->getCollidingPlayer(this);

	if (p != nullptr)
	{
		world()->increaseScore(100);
		setDead();
		world()->playSound(SOUND_GOODIE);
		p->increaseHitPoints(10);
		return;
	}

		// Move the goodie DOWN and LEFT
	moveTo(getX() - 0.75, getY() - 0.75);

		// Check for any collision with Player AGAIN
	p = world()->getCollidingPlayer(this);

	if (p != nullptr)
	{
		world()->increaseScore(100);
		setDead();
		world()->playSound(SOUND_GOODIE);
		p->increaseHitPoints(10);
		return;
	}
}

TorpedoGoodie::TorpedoGoodie(StudentWorld* w, double startX, double startY)
	: Goodie(w, startX, startY, IID_TORPEDO_GOODIE)
{}

void TorpedoGoodie::doSomethingDifferent()
{
	// Check for any collision with Player
	Player* p = world()->getCollidingPlayer(this);

	if (p != nullptr)
	{
		world()->increaseScore(100);
		setDead();
		world()->playSound(SOUND_GOODIE);
		p->increaseTorpedos(5);
		return;
	}

	// Move the goodie DOWN and LEFT
	moveTo(getX() - 0.75, getY() - 0.75);

	// Check for any collision with Player AGAIN
	p = world()->getCollidingPlayer(this);

	if (p != nullptr)
	{
		world()->increaseScore(100);
		setDead();
		world()->playSound(SOUND_GOODIE);
		p->increaseTorpedos(5);
		return;
	}
}

///////////////////////////////////////////////////////////////////////////
//  Misc Implementation
///////////////////////////////////////////////////////////////////////////

Star::Star(StudentWorld* w, double startX, double startY)
	: Actor(w, startX, startY, IID_STAR, 0, randInt(5, 50) / 100.0, 3)
{
}

void Star::doSomethingDifferent()
{
	moveTo(getX() - 1, getY());
}

Explosion::Explosion(StudentWorld* w, double startX, double startY)
	: Actor(w, startX, startY, IID_EXPLOSION, 0, 1, 0), m_tickCount(0)
{}

void Explosion::doSomethingDifferent() 
{
	m_tickCount++;
	setSize(getSize()*1.5);

	if (m_tickCount == 4)
		setDead();
}





