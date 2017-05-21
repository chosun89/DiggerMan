#include "Actor.h"
#include "StudentWorld.h"
using namespace std;
// initialze SWptr
StudentWorld* Actor::SWptr = 0;

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

// Sonar c'tor, sets the Ticks to the appropriate number
Sonar::Sonar(int startX, int startY)
	: DistGameObj(IMID_SONAR, startX, startY, right, 1.0, 2, waiting) {

	setVisible(true);
	setTicks(getWorld()->max(100, 300 - 10 * getWorld()->getLevel()));
}

// WaterPool c'tor
WaterPool::WaterPool(int startX, int startY)
	: DistGameObj(IMID_WATER_POOL, startX, startY, right, 1.0, 2, waiting) {
	setVisible(true);
	setTicks(getWorld()->max(100, 300 - 10 * getWorld()->getLevel()));
}

void Protester::setTicksToWait(int t) { TicksToWait = t; }

// Protester C'tor
Protester::Protester(int IMID, int hp) : Person(IMID , 60, 60, left, 1.0, 0, hp, alive)
{
	resetTicks();
	resetNRTicks();
	setVisible(true);
	TicksToWait = getWorld()->max(0, 3 - getWorld()->getLevel() / 4);
	getWorld()->rng(8, 64);
	resetNumSquaresToMove();

}

void DiggerMan::doSomething() {
	if (getHP() <= 0) {
		setState(dead);
		return;
	}


	int ch;
	if (getWorld()->getKey(ch) == true)
	{
		switch (ch)
		{
		case KEY_PRESS_LEFT:
			if (canMove(KEY_PRESS_LEFT))
				moveTo(getX() - 1, getY());
			else (setDirection(left));
			break;
		case KEY_PRESS_RIGHT:
			if (canMove(KEY_PRESS_RIGHT))
				moveTo(getX() + 1, getY());
			else (setDirection(right));
			break;
		case KEY_PRESS_DOWN:
			if (canMove(KEY_PRESS_DOWN))
				moveTo(getX(), getY() - 1);
			else (setDirection(down));
			break;
		case KEY_PRESS_UP:
			if (canMove(KEY_PRESS_UP))
				moveTo(getX(), getY() + 1);
			else (setDirection(up));
			break;
		case KEY_PRESS_ESCAPE:
			setState(dead);
			// getWorld()->skipLevel();
			break;
		case KEY_PRESS_SPACE:
		{
			if (getSquirt() > 0) {
				decSq();
				getWorld()->playSound(SOUND_PLAYER_SQUIRT);			
				// create Squirt using new coord and DM's direction
				getWorld()->addSquirt();
				break;
			}
			else break;
		}
		case KEY_PRESS_TAB:
		{
			if (getNugg() > 0) {
				getWorld()->dropNugget();
				break;
			}
			else break;

		}
		case 'Z': case 'z':
		{
			if (getSonar() > 0) {
				decSon();
				getWorld()->sonarCheck();
				getWorld()->playSound(SOUND_SONAR);
				break;
			}
			else break;
		}
		}
	}
}

// returns TRUE if DiggerMan CAN move into the Direction provided in param
// returns FALSE if Diggerman CANNOT move into Direction
bool Person::canMove(int dir) {
	if (dir == KEY_PRESS_LEFT && getDirection() == left && getX() > 0
		&& getWorld()->checkIfBoulder(this))
		return true;

	// must check +4 spaces away from digger man if he is moving right 
	else if (dir == KEY_PRESS_RIGHT && getDirection() == right && getX() < 60
		&& getWorld()->checkIfBoulder(this))
		return true;

	// must check +4 spaces above DiggerMan if he is moving up
	else if (dir == KEY_PRESS_UP && getDirection() == up && getY() < 60
		&& getWorld()->checkIfBoulder(this))
		return true;

	else if (dir == KEY_PRESS_DOWN && getDirection() == down && getY() > 0
		&& getWorld()->checkIfBoulder(this))
		return true;

	else return false;
}

// checks if a point (x,y) is inside an object (x0, y0)
// returns false is (x,y) is outside (x0,y0)
// returns true if it is inside
bool Actor::isWithin(int x, int y, int x0, int y0) {

	return (x >= x0 && x <= x0 + 3 && y >= y0 && y <= y0 + 3);

}

// returns false if the object is within a radius r (inclusive) of DM
bool Actor::checkDistDM(double r) {

	int x, y;
	getWorld()->getDMLoc(x, y);
	return (r*r < (pow(getX() - x, 2) + pow(getY() - y, 2)));
}

void Boulder::doSomething()
{
	if (getState() == dead) {
		return;
	}

	// check if there is Dirt in the 4 squares below Boulder
	if (getState() == stable)
	{
		// if checkUnder() returns true, NO Dirt is supporting Boulder, change state to waiting
		if (getWorld()->checkUnder(this)) {
			setState(waiting);
			return;
		}

		else return;
	}

	if (getState() == waiting) {
		// If the Boulder has been waiting for less than 30 ticks, 
		// increment the Wait counter
		if (getTicks() < 30)
			incTicks();

		// If Boulder has been waiting for 30 ticks, change state to falling
		else {

			setState(falling);
			getWorld()->playSound(SOUND_FALLING_ROCK);
			return;
		}
	}

	if (getState() == falling) {
		// if there is Dirt under the falling Boulder 
		//or it is at the bottom, change its state to dead

		// returns true if Boulder hits DM, Dirt, or Boulder
		if (getWorld()->boulderFalling(this) || !getWorld()->checkUnder(this)) {
			setState(dead);
		}

		else {
			//getWorld()->playSound(SOUND_FALLING_ROCK);
			// If Boulder is falling, decrement its Y-coord by 1 each tick
			moveTo(getX(), getY() - 1);
			return;
		}
	}
}

void Nugget::doSomething()
{
	if (getState() == dead)
		return;

	// If nugget is within radius of 3 or less, remove it, increment score
	if (getState() == alive && !checkDistDM(3.0))
	{
		setState(dead);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(10);
		getWorld()->incN();
		return;
	}

	// If nugget is within a radius 4, set it visible by DM
	if (getState() == alive && !checkDistDM(4.0))
	{
		setVisible(true);
		return;
	}

	// add code for Nugget's "waiting" state (pickup-able by protesters)
	else if (getState() == waiting)
	{
		if (getTicks() == 100) {
			setState(dead);
			return;
		}

		else incTicks();

		// check if NuggetTrap is touching a protester
		if (getWorld()->nuggetTrap(this)) {
			setState(dead);
		}
	}
}

void Barrel::doSomething()
{
	if (getState() == dead)
		return;

	// If oil barrel is within radius of 3 or less, remove it, increment score
	if (!checkDistDM(3.0))
	{
		getWorld()->playSound(SOUND_FOUND_OIL);
		getWorld()->increaseScore(1000);
		getWorld()->decLCount();
		setState(dead);
		return;
	}

	// If oil barrel is within a radius 4, set it visible by DM
	if (!checkDistDM(4.0))
	{
		setVisible(true);
		return;
	}
}

void Squirt::doSomething() {

	if (getWorld()->squirtCollision(this)) {
		setState(dead);
	}

	// move Squirt in the direction it is facing
	if (getDirection() == left) {
		moveTo(getX() - 1, getY());
	}

	else if (getDirection() == right) {
		moveTo(getX() + 1, getY());
	}

	else if (getDirection() == up) {
		moveTo(getX(), getY() + 1);
	}

	else if (getDirection() == down) {
		moveTo(getX(), getY() - 1);
	}


}

void Sonar::doSomething() {
	if (getTicks() == 0) {
		setState(dead);
		return;
	}

	else {
		decTicks();
		// If DM is within a radius of 3.0
		if (!checkDistDM(3.0))
		{
			setState(dead);
			getWorld()->increaseScore(75);
			getWorld()->playSound(SOUND_GOT_GOODIE);
			getWorld()->incSon();
			return;
		}


	}
}

void WaterPool::doSomething() {

	if (getTicks() == 0)
	{
		setState(dead);
		return;
	}

	else {
		decTicks();

		if (!checkDistDM(3.0)) {
			setState(dead);
			getWorld()->incSq();
			getWorld()->increaseScore(100);
			getWorld()->playSound(SOUND_GOT_GOODIE);
			return;
		}

	}
}

void Protester::resetNumSquaresToMove()
{
	numSquaresToMoveinCurrentDirection = getWorld()->rng(8, 64);
}

void Protester::doSomething() {
	if (getState() == dead) { return; }

	// chase DiggerMan
	else if (getState() == alive) {

		// resting state, Protester does nothing but wait until
		// its current tick count is high enough to doSomething()
		if (Ticks < TicksToWait) {
			Ticks++;
			return;
		}

		// if current ticks equals tick threshold, change state and reset tick count
		else {
			NRTicks++;
			Ticks = 0;
			//extract DM's coordinates
			int x, y;
			getWorld()->getDMLoc(x, y);
			// If Prot is within 4 of DM, yell at DM
			if (!checkDistDM(4.0) && NRTicks >= 15) {
				if (getDirection() == left && getX() >= x) {
					getWorld()->yell(this);
				}

				else if (getDirection() == right && getX() <= x) {
					getWorld()->yell(this);
				}

				else if (getDirection() == up && getY() <= y) {
					getWorld()->yell(this);
				}

				else if (getDirection() == down && getY() >= y) {
					getWorld()->yell(this);
				}

				resetNRTicks();
			}

			else {
				// code movement here
				//// if Protester is in an unobstructed line of sight to DM
				//if (getWorld()->lineofsight(this) && checkDistDM(4)) {
				//	return;
				//}
				//// if the protester is at an intersection
				//else if (getWorld()->isIntersection(this)) {

				//}
				//// if the protester is not in line of sight of DM nor at an inters.
				//else {

				//}

				// move in shortest path to dm
				getWorld()->shortestPathToDM(this);
			}
		}
		return;
	}

	// Waiting state is when the Protester is "stunned" by Squirt
	else if (getState() == waiting) {
		if (Ticks < getWorld()->max(50, 100 - getWorld()->getLevel() * 10)) {
			Ticks++;
			return;
		}

		else {
			setState(alive);
			Ticks = TicksToWait;
			return;
		}
	}

	// if State == leaving then leave the oil field (60,60)
	// somewhere in this block, Protester's state must be set to dead
	else if (getState() == leaving) {
		if (getX() == 60 && getY() == 60) {
			setState(dead);
			return;
		}

		if (Ticks < TicksToWait) {
			Ticks++;
			return;
		}

		// if current ticks equals tick threshold, change state and reset tick count
		else {
			NRTicks++;
			Ticks = 0;
			getWorld()->shortestPathToExit(this); // update the protester's Path queue
			resetTicks();
		}

		return;
	}
}