#ifndef GRAPHOBJ_H_
#define GRAPHOBJ_H_

#include "SpriteManager.h"
#include "GameConstants.h"

#include <set>
#include <cmath>

const int ANIMATION_POSITIONS_PER_TICK = 1;
const int NUM_LAYERS = 4;

using Direction = int;

class GraphObject
{
  public:

	GraphObject(int imageID, double startX, double startY, Direction dir = 0, double size = 1.0, unsigned int depth = 0)
	 : m_imageID(imageID), m_visible(true), m_x(startX), m_y(startY),
	   m_destX(startX), m_destY(startY), m_brightness(1.0),
	   m_animationNumber(0), m_direction(dir), m_size(size), m_depth(depth)
	{
		if (m_size <= 0)
			m_size = 1;

		getGraphObjects(m_depth).insert(this);
	}

	virtual ~GraphObject()
	{
		getGraphObjects(m_depth).erase(this);
	}

	void setVisible(bool shouldIDisplay)
	{
		m_visible = shouldIDisplay;
	}

	void setBrightness(double brightness)
	{
		m_brightness = brightness;
	}

	double getX() const
	{
		  // If already moved but not yet animated, use new location anyway.
		return m_destX;
	}

	double getY() const
	{
		  // If already moved but not yet animated, use new location anyway.
		return m_destY;
	}

	virtual void moveTo(double x, double y)
	{
		m_destX = x;
		m_destY = y;
		increaseAnimationNumber();
	}

	Direction getDirection() const
	{
		return m_direction;
	}

	void setDirection(Direction d)
	{
		while (d < 0)
			d += 360;

		m_direction = d % 360;
	}

	void setSize(double size)
	{
		m_size = size;
	}

	double getSize() const
	{
		return m_size;
	}

	double getRadius() const
	{
		const int kRadiusPerUnit = 8;
		return kRadiusPerUnit * m_size;
	}

	  // The following should be used by only the framework, not the student

	bool isVisible() const
	{
		return m_visible;
	}

	double getBrightness() const
	{
		return m_brightness;
	}

	unsigned int getAnimationNumber() const
	{
		return m_animationNumber;
	}

	void getAnimationLocation(double& x, double& y) const
	{
		x = m_x;
		y = m_y;
	}

	void animate()
	{
		m_x = m_destX;
		m_y = m_destY;
//		moveALittle(m_x, m_destX);
	//	moveALittle(m_y, m_destY);
	}

	static std::set<GraphObject*>& getGraphObjects(unsigned int layer)
	{
		static std::set<GraphObject*> graphObjects[NUM_LAYERS];
		if (layer < NUM_LAYERS)
			return graphObjects[layer];
		else
			return graphObjects[0];		// empty;
	}

private:
	friend class GameController;
	unsigned int getID() const
	{
		return m_imageID;
	}

  private:
	  // Prevent copying or assigning GraphObjects
	GraphObject(const GraphObject&);
	GraphObject& operator=(const GraphObject&);

	int		m_imageID;
	bool	m_visible;
	double	m_x;
	double	m_y;
	double	m_destX;
	double	m_destY;
	double	m_brightness;
	int		m_animationNumber;
	Direction	m_direction;
	double	m_size;
	int		m_depth;

	void moveALittle(double& from, double& to)
	{
		static const double DISTANCE = 1.0/ANIMATION_POSITIONS_PER_TICK;
		if (to - from >= DISTANCE)
			from += DISTANCE;
		else if (from - to >= DISTANCE)
			from -= DISTANCE;
		else
			from = to;
	}

	void increaseAnimationNumber()
	{
		m_animationNumber++;
	}

};

#endif // GRAPHOBJ_H_
