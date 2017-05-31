#ifndef ACTOR_H_
#define ACTOR_H_
#include "GraphObject.h"
#include "GameWorld.h"
#include <queue>
class StudentWorld;

class Actor : public GraphObject {
public:
	enum StateType { alive, stable, leaving, waiting, falling, dead };

	Actor(int imageID, int startX, int startY, Direction dir = right, double size = 1.0,
		unsigned int depth = 0, StateType  s = alive)
		: GraphObject(imageID, startX, startY, dir, size, depth), State(s) {};

	StudentWorld* getWorld() const
	{
		return SWptr;
	}

	void setStudentWorld(StudentWorld* sw)
	{
		SWptr = sw;
	}

	StateType getState() const
	{
		return State;
	}

	void setState(StateType s)
	{
		State = s;
	}

	// returns FALSE if the coordinates are WITHIN a radius r inclusive
	bool checkDistance(int x, int y, double r)
	{
		return (r*r < (pow(getX() - x, 2) + pow(getY() - y, 2)));
	}

	// returns False if the object is within a radius r of the DiggerMan
	bool checkDistDM(double r);

	// returns false is (x,y) is outside (x0,y0)
	bool isWithin(int, int, int, int);

	virtual void doSomething() = 0;

	virtual bool requiresRandomization() const { return false; }

private:

	// returns false if the coordinates are less than or equal to r
	bool checkDistance(int x, int y, int x0, int y0, double r)
	{
		return (r*r < pow(x0 - x, 2) + pow(y0 - y, 2));
	}

	StateType State;
	static StudentWorld* SWptr; // pointer to the StudentWorld is shared with all Actors

};

// base class for Boulder/Nugget/Barrel
class DistGameObj : public Actor
{
private:
	int Ticks;
public:
	DistGameObj(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StateType state)
		: Actor(imageID, startX, startY, dir, size, depth, state) {};

	// 
	void incTicks() { Ticks++; }
	void decTicks() { Ticks--; }
	int getTicks() { return Ticks; }
	void setTicks(int T) { Ticks = T; }
	virtual bool requiresRandomization() const { return true; }

};

class Boulder : public DistGameObj {
private:
public:
	Boulder(int startX, int startY) : DistGameObj(IMID_BOULDER, startX, startY, down, 1.0, 1, stable)
	{
		setTicks(0);
		setVisible(true);
	}

	virtual void doSomething();
};

class Nugget : public DistGameObj {
private:
	int Ticks;
public:
	Nugget(int startX, int startY, StateType s)
		: DistGameObj(IMID_GOLD, startX, startY, right, 1.0, 2, s), Ticks(0)
	{
		setVisible(false);
	}

	int getTicks() { return Ticks; }
	void incTicks() { Ticks++; }
	void doSomething();
	// bool requiresRandomization() const { return true; }
};

class Barrel : public DistGameObj {
private:

public:
	Barrel(int startX, int startY) : DistGameObj(IMID_BARREL, startX, startY, right, 1.0, 2, alive)
	{
		setVisible(false);
	}

	void doSomething();
	//bool requiresRandomization() const { return true; }
};

// base class for DiggerMan/Protesters/HCProtesters
class Person : public Actor {
private:
	int HP;

public:
	Person(int imageID, int startX, int startY, Direction dir = right, double size = 1.0,
		unsigned int depth = 0, int hp = 10, StateType state = alive) :
		Actor(imageID, startX, startY, dir, size, depth, state), HP(hp) {
		setVisible(true);
	}

	virtual void doSomething() = 0;

	int getHP() const
	{
		return HP;
	}

	bool isAlive() const
	{
		return (HP > 0);
	}

	virtual bool canMove(int dir);

	void setHP(int hp)
	{
		HP = hp;
	}

	// decrease by how much?
	void decHP(int k) {
		HP -= k;
	}
	// increase hp by how much
	void incHP(int k) {
		HP += k;
	}

};

class DiggerMan : public Person {
private:
	int SquirtC;
	int SonarC;
	int NuggC;

public:
	DiggerMan() : Person(IMID_PLAYER, 30, 60, right, 1.0, 0, 10, alive), SquirtC(5), SonarC(1), NuggC(0) {};

	int getSquirt() const
	{
		return SquirtC;
	}

	int getSonar() const
	{
		return SonarC;
	}

	int getNugg() const
	{
		return NuggC;
	}

	// adds +/-4 to the DM's current direction, this is for his squirt gun
	void addDirection(int& x, int& y) {

		if (getDirection() == left) {
			x -= 4;
		}
		else if (getDirection() == right) {
			x += 4;
		}
		else if (getDirection() == up) {
			y += 4;
		}
		else if (getDirection() == down) {
			y -= 4;
		}
	}

	virtual void doSomething();

	void incSq() { SquirtC += 5; }
	void decSq() { SquirtC--; }
	void incSon() { SonarC++; }
	void decSon() { SonarC--; }
	void incN() { NuggC++; }
	void decN() { NuggC--; }
};

class Protester : public Person {
private:
	int TicksToWait;
	int Ticks;
	int NRTicks; // non rest ticks
	int numSquaresToMoveinCurrentDirection;

public:
	Protester(int IMID, int hp);

	void resetNumSquaresToMove();

	int getNumSquaresToMove() { return numSquaresToMoveinCurrentDirection; }

	void setTicksToWait(int);
	void resetTicks() { Ticks = 0; } // resets current Ticks to 0 (NOT TicksToWait)
	int getTicksToWait() { return TicksToWait; }
	int getTicks() { return TicksToWait; }

	// resets the Non-rest ticks needed to perform an action such as yelling
	void resetNRTicks() { NRTicks = 0; }
	void doSomething();
};

class HCProtester : public Protester {
public:
	HCProtester(int imageID, int hp) : Protester(imageID, hp) {};
};

class Dirt : public Actor {
private:
public:
	Dirt(int startX, int startY)
		: Actor(IMID_DIRT, startX, startY, right, .25, 3)
	{
		setVisible(true);
	}

	virtual void doSomething() {};
	//bool requiresRandomization() const { return false; }
};

class Squirt : public Person {
private:
public:
	Squirt(int startX, int startY, Direction dir) :
		Person(IMID_WATER_SPURT, startX, startY, dir, 1.0, 1, 0, alive) {
		setVisible(true);
	}

	virtual void doSomething();

};

class Sonar : public DistGameObj {
private:
	int Ticks;

public:
	Sonar(int startX, int startY);

	int max(int x, int y) const
	{
		if (x >= y) return x;
		else return y;
	}
	void doSomething();

};

class WaterPool : public DistGameObj {

public:
	WaterPool(int startX, int startY);
	void doSomething();
};
#endif // ACTOR_H_