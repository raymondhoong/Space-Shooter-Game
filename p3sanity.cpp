#include "GameWorld.h"
#include "GameConstants.h"
#include "GraphObject.h"
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <utility>
#include <algorithm>
#include <cstdlib>
using namespace std;

const bool PART2 = true;

GameWorld* createStudentWorld(string);

void die(string msg)
{
	cout << "***FAILED***\n" << msg << endl;
	cout << "****** FAILED the sanity check! ******" << endl;
	exit(1);
}

void part2Checks(GameWorld* gw, GraphObject* player);

set<GraphObject*>& depth0 = GraphObject::getGraphObjects(0);
set<GraphObject*>& depth1 = GraphObject::getGraphObjects(1);
set<GraphObject*>& depth3 = GraphObject::getGraphObjects(3);

int main()
{
	GameWorld* gw = createStudentWorld("dummyAssets");

	  // **********************
	  // init
	cout << "Calling init for the StudentWorld..." << flush;
	if (gw->init() != GWSTATUS_CONTINUE_GAME)
		die("StudentWorld::init did not return GWSTATUS_CONTINUE_GAME");

	  // The only GraphObjects at depth 3 are supposed to be stars
	  // Check that init produced 30 good stars and save their
	  // inital positions in starMap
	if (depth3.size() != 30)
		die("StudentWorld::init did not create 30 stars at depth 3");
	using StarMap = map<GraphObject*, pair<double,double>>;
	StarMap starMap;
	for (auto go : depth3)
	{
		if (go->getSize() < .05  ||  go->getSize() > .50)
			die("A star has the wrong size");
		if (go->getX() < 0  ||  go->getX() >= VIEW_WIDTH  ||
		    go->getY() < 0  ||  go->getY() >= VIEW_HEIGHT)
			die("A star was created at bad coordinates");
		starMap[go] = make_pair(go->getX(), go->getY());
	}

	  // The only GraphObject at depth 0 must be the NachenBlaster
	if (depth0.size() != 1)
		die("StudentWorld::init did not create exactly one Nachenblaster at depth 0");
	GraphObject* player = *(depth0.begin());
	if (player->getX() != 0  ||  player->getY() != 128)
		die("NachenBlaster created in wrong place");
	if (player->getSize() != 1)
		die("NachenBlaster has the wrong size");

	  // There should be nothing at depth 1
	if (!depth1.empty())
		die("Something was created at depth 1");

	cout << "OK" << endl;

	  // **********************
	  // Move 1.  Act as if a right arrow key was pressed.
	cout << "Calling move for the StudentWorld with simulated right arrow key press..." << flush;
	gw->setKey(KEY_PRESS_RIGHT);
    if (gw->move() != GWSTATUS_CONTINUE_GAME)
		die("StudentWorld::move did not return GWSTATUS_CONTINUE_GAME");

      // 30 stars must have moved left by 1.  Any that moved off screen might
	  // be gone.  New stars might have been created and possibly moved.
	for (auto go : depth3)
	{
		StarMap::iterator p = starMap.find(go);
		if (p == starMap.end())  //  newly-created star
		{
			if (go->getSize() < .05  ||  go->getSize() > .50)
				die("A newly-created star has the wrong size");
			if ((go->getX() != VIEW_WIDTH-1  &&  go->getX() != VIEW_WIDTH-2)  ||
		    	go->getY() < 0  ||  go->getY() >= VIEW_HEIGHT)
				die("A newly-created star was created at wrong coordinates");
		}
		else
		{
			int xold = p->second.first;
			int yold = p->second.second;
			if (go->getX() != xold-1  ||  go->getY() != yold)
				die("A star moved incorrectly");
			starMap.erase(p);
		}
	}
	  // starMap may still have stars that moved off screen
	for (auto& pr : starMap)
	{
		if (pr.second.first > 0)  // x > 0 should not have moved off screen
			die("A star with x coordinate > 0 was destroyed");
	}

	  // The player must have moved right.
	for (auto go : depth0)
	{
		if (go != player)
			die("Something other than the original NachenBlaster was created at depth 0");
		if (player->getX() != 6  ||  player->getY() != 128)
			die("NachenBlaster did not move right six pixels");
	}

	  // Part 2 has many more checks
	if (PART2)
		part2Checks(gw, player);
	else
		cout << "OK" << endl;

	  // **********************
	  // Cleanup
	cout << "About to call cleanup for the StudentWorld" << endl;
    gw->cleanUp();
	for (int depth = 3; depth >= 0; depth--)
	{
		if (!GraphObject::getGraphObjects(depth).empty())
		{
			string d = to_string(depth);
			die("Memory leak:  Not all GraphObjects at depth " + d + " were destroyed");
		}
	}

	  // **********************
	  // Destroy StudentWorld object
	cout << "About to destroy StudentWorld object" << endl;
	delete gw;

	cout << "****** SUCCESS! Passed the sanity check. ******" << endl;
}

void part2Checks(GameWorld* gw, GraphObject* player)
{
	  // An alien must have been added at depth 1.  It might have moved or
	  // it might have fired, adding a projectile at depth 1.
	GraphObject* alien1 = nullptr;
	GraphObject* projectile = nullptr;
	for (auto go : depth1)
	{
		if (go->getY() < 0  ||  go->getY() >= VIEW_HEIGHT)
			die("Alien (or possibly projectile) created in wrong place");
		if (alien1 == nullptr)
		{
			alien1 = go;
			continue;
		}
		if (projectile != nullptr)
			die("Too many objects were created at depth 1");
		if (go->getY() != alien1->getY())
			die("Projectile not created at same y coordinate as the alien that fired it");
		if (go->getX() + 14 == alien1->getY())
			projectile = go;
		else if (go->getX() - 14 == alien1->getY())
		{
			projectile = alien1;
			alien1 = go;
		}
		else
			die("Projectile fired by alien was created at wrong x coordinate");
	}
	if (alien1 == nullptr)
		die("An alien was not created at depth 1");

	double alien1X = alien1->getX();
	if (alien1X > VIEW_WIDTH-1  ||
		(alien1X < VIEW_WIDTH-3  &&  alien1X != VIEW_WIDTH-6 /* ramming speed */))
		die("Alien created in wrong place");
	if (alien1->getSize() != 1.5)
		die("Newly-created alien has the wrong size");

	if (projectile != nullptr)
	{
		  // Get player out of the line of fire
		double playerY = alien1->getY();
		if (playerY >= 100)
			playerY -= 50;
		else
			playerY += 50;
		player->moveTo(player->getX(), playerY);
	}

	cout << "OK" << endl;

	  // **********************
	  // Move 2.  Act as if a space was pressed.
	int oldDepth1Size = depth1.size();
	cout << "Calling move for the StudentWorld with simulated space key press..." << flush;
	gw->setKey(' ');
    if (gw->move() != GWSTATUS_CONTINUE_GAME)
		die("StudentWorld::move did not return GWSTATUS_CONTINUE_GAME");

	  // A second alien and a cabbage must have been created at depth 1.
	  // The cabbage may have moved.  The alien may have moved or fired.
	if (depth1.size() < oldDepth1Size+2)
		die("Did not create a cabbage and a new alien");
	  //
	GraphObject* cabbage = nullptr;

	bool alien1StillExists = false;
	for (auto go : depth1)
	{
		if (go == alien1)
		{
			alien1StillExists = true;
			continue;
		}
		if (go->getX() < VIEW_WIDTH/2 )  // cabbage in left half
			cabbage = go;
	}
	if (cabbage == nullptr)
		die("A cabbage was not created");
	double cabbageX = cabbage->getX();
	if ((cabbageX != 6+12  && cabbageX != 6+12+8)  ||
		cabbage->getY() != player->getY())
		die("A cabbage in the proper place was not created");
	double cabbageDir = cabbage->getDirection();
	if (cabbageDir != 0  &&  cabbageDir != 20)
		die("A new cabbage was not created with the proper direction");
	if (!alien1StillExists)
		die("The alien created in the first move call has disappeared");

	if (alien1->getX() > alien1X  ||
		(alien1->getX() == alien1X  &&  depth1.size() != 4)  ||
		(alien1->getX() < alien1X - 2  &&  alien1->getX() != alien1X - 5))
		die("The alien created in the first move call did not move to the proper place");
	alien1X = alien1->getX();

	cout << "OK" << endl;

	  // **********************
	  // Move 3.
	cout << "Calling move for the StudentWorld..." << flush;
    if (gw->move() != GWSTATUS_CONTINUE_GAME)
		die("StudentWorld::move did not return GWSTATUS_CONTINUE_GAME");

	if (cabbage->getX() != cabbageX+8)
		die("The cabbage did not move to the proper place");
	if (cabbage->getDirection() != cabbageDir+20)
		die("The cabbage did not rotate 20 degrees counterclockwise");

	cout << "OK" << endl;

	  // Teleport alien to be within collision range of cabbage
	  // sqrt(6*6 + 10*10) < .75*(8*.5 + 8*1.5)
	cout << "Teleporting alien to collision range of cabbage" << endl;
	double newY = cabbage->getY()-4;
	if (newY <= 0)
		newY += 4+4;
	alien1->moveTo(cabbage->getX()+10, newY);

	  // **********************
	  // Move 4.  The cabbage should be gone.
	cabbageDir = cabbage->getDirection();
	cout << "Calling move for the StudentWorld..." << flush;
    if (gw->move() != GWSTATUS_CONTINUE_GAME)
		die("StudentWorld::move did not return GWSTATUS_CONTINUE_GAME");

	alien1StillExists = false;
	for (auto go : depth1)
	{
		if (go == alien1)
			alien1StillExists = true;
		if (go == cabbage  &&
				(go->getDirection() == cabbageDir  ||  go->getDirection() == cabbageDir+20))
			die("The collision did not destroy the cabbage");
	}
	if (!alien1StillExists)
		die("The collision mistakenly destroyed the alien");

	cout << "OK" << endl;

	  // **********************
	  // Call move 800 times with no player action.
	cout << "Calling move for the StudentWorld 800 times with no player action..." << flush;
	for (int k = 0; k < 800  &&  gw->move() == GWSTATUS_CONTINUE_GAME; k++)
		;

	cout << "didn't crash" << endl;
}
