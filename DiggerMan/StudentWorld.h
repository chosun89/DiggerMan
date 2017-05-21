#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <random>
#include <queue>
#include <stack>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	// c'tor
	StudentWorld(std::string assetDir);

	// d'tor
	//~StudentWorld();

	// initialize the oil field
	virtual int init()
	{

		// Create DiggerMan
		createDM();

		// Appropriates Boulders, Nuggets, and Oil Barrels into the Oil Field
		createOilField();

		// Initialize the 2D int array Path which mimics Dirt Field

		initNode2D(DMPath);


		// need to update protester count/ticks at the start of a new level
		updateProtester();
		addRegularProtester();

		// set the Goodie probability
		setGoodieProb();

		return GWSTATUS_CONTINUE_GAME;
	}

	// Called once every tick of the game
	virtual int move()
	{
		if ((DM->getState() == Actor::dead))
		{
			decLives();
			playSound(SOUND_PLAYER_GIVE_UP);
			return GWSTATUS_PLAYER_DIED;
		}

		if (LCount == 0) {
			playSound(SOUND_FINISHED_LEVEL);
			return GWSTATUS_FINISHED_LEVEL;
		}

		else {
			setDisplayText();

			DM->doSomething();
			dig(); // removes dirt (if applicapable)
			exitBFS(); // update the exit Node2D array every tick
			DiggerManBFS();

			// contains all logic for adding regular/hcprotester IF possible
			addProtester();

			// check to see if a Sonar/WaterPool needs to be added
			rollGoodie();

			// iterate thru All objects and call their virtual doSomething() method
			for (unsigned int i = 0; i < All.size(); i++) {
				// if the object is dead, erase it from the vector of
				// active objects
				if (All[i]->getState() == Actor::dead) {
					// if the deleted object was a Protester, decrement its count before deleting
					if (dynamic_cast<Protester*>(All[i]) != NULL)
						decProtesterCount();

					// deallocate and remove dead objects
					delete All[i];
					All.erase(All.begin() + i);
				}

				// if HCP is within a certain distance of DM, set TicksToWait to 0
				else if (dynamic_cast<HCProtester*>(All[i]) != NULL) {
					int M = 16 + getLevel() * 2;
					if (DMPath[All[i]->getY()][All[i]->getX()].PathValue <= M) {
						dynamic_cast<HCProtester*>(All[i])->setTicksToWait(0);
					}

					All[i]->doSomething();
				}

				else All[i]->doSomething();
			}
		}

		return GWSTATUS_CONTINUE_GAME;
	}

	virtual void cleanUp()
	{
		delete DM; // delete DiggerMan
		clearDirt(); // erases entire current Dirt Field
		clearActors(); // clears BGLs from the Oil Field

	}

private:
	std::vector < std::vector<Dirt* >> DP; // Dirt Pointer to all Dirt objects
	DiggerMan* DM; // Pointer to DiggerMan
	std::vector<Actor*> All; // Holds pointers to all other objects in the game
	unsigned int BCount, GCount, LCount, Score; // number of Boulders, Gold Nuggets and Oil Barrels respectively
	void dig();
	int getLCount() const { return LCount; } // returns Boulder count

	// All functions related to moving Protesters via BFS
	struct Node {
		short X, Y;
		short PathValue;
		bool Visited;
	};

	Node exitPath[DIRT_HEIGHT][DIRT_WIDTH] = { {} }; // Path to exit shared by all P's
	Node DMPath[DIRT_HEIGHT][DIRT_WIDTH] = { {} }; // Path to DM
	void initNode2D(Node[][DIRT_WIDTH]); // initalizes the 2D Path int array
	bool canMove(int x, int y, int dir);

	// Regular protester movement

	int probabilityOfHardcore; // probability of rolling a HCProt
	int PCount; // Max number of protesters allowed in the level
	int PTick; // Minimum threshold for generating protesters
	int CurrentProtesterCount; // current # of protesters
	int CurrentProtesterTick; // current # of protester ticks
	void decProtesterCount() { CurrentProtesterCount--; }
	// resets the protester tick and spawn thresholds at the beginning of each level
	void updateProtester() {
		PCount = min(15, static_cast<int>(2 + getLevel()*1.5));
		//PCount = 1; // for debugging
		PTick = max(25, 200 - getLevel());
		CurrentProtesterCount = 0;
		CurrentProtesterTick = 0;
		probabilityOfHardcore = min(90, getLevel() * 10 + 30);
	}

	// adds a protester and updates PCount and PTick
	void addRegularProtester();
	void addProtester();

	int G; // Chance of rolling a goodie
	void setGoodieProb() { G = getLevel() * 25 + 300; }
	void rollGoodie();
	bool isEmpty(int x, int y);

	// randomizes the location of Boulders
	void BrandXY(int& x, int& y);

	// randomizes the location of Nuggets and Oil Barrels
	void randXY(int& x, int& y);

	void createDM();
	void createOilField();
	void createDirtField();

	void setBGLCount();
	void addBoulders();
	void addNuggets();
	void addBarrels();
	void addSonar();
	void addWaterPool();

	void setDisplayText();

	// removes Actors and Dirt
	void clearDirt();
	void clearActors();

	// returns false if the coordinates are less than or equal to r
	bool checkDistance(int x, int y, int x0, int y0, double r)
	{
		return (r*r < pow(x0 - x, 2) + pow(y0 - y, 2));
	}
	std::string formatDisplay(int, int, int, int, int, int, int, int);

public:
	void skipLevel() { LCount = 0; } // for debugging
	void printPathArray(Node arr[][DIRT_WIDTH]); // for dbugging

	void addSquirt();
	void dropNugget(); // used by DM to set Nuggets as a trap
	void sonarCheck();
	bool nuggetTrap(const Nugget*);// checks if nuggettrap is touching a protester returns true if touching
	bool squirtCollision(const Squirt*); // manages all of Squirt's interactions with Boulders/Dirt/Protesters
	bool boulderFalling(const Boulder*);
	bool checkIfBoulder(const Actor*);
	bool isWithin(int, int, int, int); // returns true if (x,y) is within (x0,y0)
	bool checkUnder(const Boulder*); // returns false if Dirt is under a Boulder, true if no Dirt
	void decLCount() { LCount--; } // decrease Oil barrel(s) remaining count

	void incSq() { DM->incSq(); }
	void decSq() { DM->decSq(); }
	void incSon() { DM->incSon(); }
	void decSon() { DM->decSon(); }
	void incN() { DM->incN(); }
	void decN() { DM->decN(); }
	bool checkIfDirtOrBoulder(int x, int y);
	void yell(Protester*);

	void exitBFS(); // globally updates array for shortest path to exit (60,60)
	void DiggerManBFS(); // globally updates array for shortest path to DM
	bool shortestPathToExit(Protester*);  // sets Protester's Path stack with directions to the exit
	bool shortestPathToDM(Protester*);
	void hardcoreMove(); // If a HCP is within a certain distance of DM, make them move faster
	bool lineofsight(Protester*);
	bool isIntersection(Protester*);

	void getDMLoc(int&x, int& y) {
		x = DM->getX();
		y = DM->getY();
	}

	int min(int x, int y) const
	{
		if (x <= y) return x;
		else return y;
	}
	int max(int x, int y) const
	{
		if (x >= y) return x;
		else return y;
	}

	// returns a random number between min, max inclusive
	int rng(int min, int max) {
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_int_distribution<int> dist(min, max);

		return dist(mt);
	}
};
#endif // STUDENTWORLD_H_