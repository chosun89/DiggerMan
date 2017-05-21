#include "StudentWorld.h"
using namespace std;

//custom constants added by Jeff A
const double TOTAL_HP = 10;
const int DB_VALUE = 200; // default path values for Dirt and Boulder

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

// StudentWorld c'tor
StudentWorld::StudentWorld(std::string assetDir)
	: GameWorld(assetDir)
{

	// initialize Dirt ptr
	std::vector < std::vector<Dirt* >> temp(DIRT_HEIGHT, std::vector<Dirt*>(DIRT_WIDTH));
	DP = temp;

}

// Initializes a 2D array of Nodes based on the OilField
// Dirt Boulder and empty spaces are appropriately tagged
void StudentWorld::initNode2D(Node arr[][DIRT_WIDTH]) {
	// initialize the int NodeArray[][] for Protesters
	// if dirt is not visible, set its DB to false
	for (int i = 0; i < DIRT_HEIGHT; i++)
		for (int j = 0; j < DIRT_WIDTH; j++) {
			arr[i][j].X = j;
			arr[i][j].Y = i;
			arr[i][j].Visited = false;
			arr[i][j].PathValue = 200;
			// returns true if no dirt/boulder in 4x4 area
			if (checkIfDirtOrBoulder(j, i)) {
				arr[i][j].PathValue = 0;
			}
		}
}

/// StudentWorld d'tor

// create DiggerMan
void StudentWorld::createDM()
{
	DM = new DiggerMan;

	// set the location of studentWorld so getKey() can be used
	DM->setStudentWorld(this);
}

void StudentWorld::createOilField()
{
	// order of creation is IMPORTANT
	setBGLCount();
	addBoulders();

	createDirtField();
	addNuggets();
	addBarrels();
}

// creates and sets Dirt objects in the appropriate places
void StudentWorld::createDirtField()
{

	// create 2D Dirt array
	for (int h = 0; h < DIRT_HEIGHT; h++)
		for (int w = 0; w < DIRT_WIDTH; w++) {

			DP[h][w] = new Dirt(w, h);

			if (w <= 33 && w >= 30 && h >= 4 && h <= 59) // mineshaft
				DP[h][w]->setVisible(false);

			if (h >= 60) { // Top part of Map that has no dirt

				DP[h][w]->setVisible(false);
			}
		}

	// iterate through All objects
	for (unsigned int i = 0; i < All.size(); i++)
	{
		// search for Boulders and Sonar
		if (dynamic_cast<Boulder*>(All[i]) != 0)
		{
			// get (x,y) of Boulder obj
			int xB = All[i]->getX();
			int yB = All[i]->getY();

			//loop through the Boulders 4x4 sprite
			for (unsigned int w = 0; w < 4; w++)
				for (unsigned int h = 0; h < 4; h++)
				{
					DP[h + yB][xB + w]->setVisible(false);
				}
		}
	}
}

// sets the required number of boulders nuggets and barrels for each level
void StudentWorld::setBGLCount()
{
	BCount = min(getLevel() / 2 + 2, 7);
	GCount = max(5 - getLevel() / 2, 2);
	LCount = min(2 + getLevel(), 18);
}

// adds Boulders to the level
void StudentWorld::addBoulders()
{
	unsigned int i = 0;
	while (i < BCount)
	{
		// add boulder to the right of the start of the mineshaft for debugging
		/*All.push_back(new Boulder(30, 60));
		i++;*/

		int x, y;
		BrandXY(x, y);

		// The first object added to All will be a Boulder obj.
		if (All.empty())
		{
			//All.push_back(new Boulder(x, y)); 
			All.push_back(new Boulder(20, 24)); // for debugging

			i++; // increment Boulder counter
			continue; // control flow moves to the top of while-loop 
		}
		else {
			// iterate thru all Actor objects in the game
			for (unsigned int j = 0; j <= All.size();)
			{
				if (j == All.size()) { // The boulder position finishes comparing to all other objects
					All.push_back(new Boulder(x, y)); // add the Boulder to All
					i++; // increment Boulder counter
					break; // exit the for-loop, start from the top of while-loop
				}

				if (All[j]->isDistGameObj()) {
					if (All[j]->checkDistance(x, y, 6.0))
					{
						j++;
						continue;
					}
					else { // if the distance is <= 6.0, then find another set of coordinates and start over
						BrandXY(x, y);
						j = 0;
						continue;
					}
				}
				else {
					j++;
					continue;
				}
			}
		}
	}
}

// adds Nuggets (with appropriate restrictions) to the StudentWorld
void StudentWorld::addNuggets()
{
	unsigned int i = 0;
	while (i < GCount)
	{
		// obtain random coordinates in the Oil Field
		int x, y;
		randXY(x, y);

		// iterate thru all Actor objects in the game
		for (unsigned int j = 0; j <= All.size();)
		{
			if (j == All.size())
			{
				All.push_back(new Nugget(x, y, Actor::alive));
				i++;
				break;
			}

			if (All[j]->isDistGameObj()) {
				if (All[j]->checkDistance(x, y, 6.0)) {
					j++;
					continue;
				}
				else {
					randXY(x, y); // if the distance is > 6.0, find another random location
					j = 0;
					continue;
				}
			}
			else {
				j++;
				continue;
			}
		}
	}
}

// adds Oil Barrels (with appropriate restrictions) to the StudentWorld
void StudentWorld::addBarrels()
{
	unsigned int i = 0;
	while (i < LCount)
	{
		int x, y;
		randXY(x, y);

		// iterate thru all Actor objects in the game
		for (unsigned int j = 0; j <= All.size();)
		{
			if (j == All.size()) {
				All.push_back(new Barrel(x, y));
				i++;
				break;
			}

			if (All[j]->isDistGameObj()) {
				if (All[j]->checkDistance(x, y, 6.0)) {
					j++;
					continue;
				}
				else {
					randXY(x, y);
					j = 0;
					continue;
				}
			}
			else {
				j++;
				continue;
			}
		}
	}
}

void StudentWorld::addSonar() {
	All.push_back(new Sonar(0, 60));
}

void StudentWorld::addWaterPool() {
	// obtain random coordinates in the Oil Field
	int x, y;

	do {
		x = rng(0, 60);
		y = rng(0, 56);
	} while (!isEmpty(x, y));

	All.push_back(new WaterPool(x, y));
}

void StudentWorld::addRegularProtester() {
	All.push_back(new Protester(IMID_PROTESTER, 5));
	CurrentProtesterCount++;
	CurrentProtesterTick = 0;
}

void StudentWorld::addProtester() {
	// check to see if adding a protester is needed
	if (CurrentProtesterCount < PCount && CurrentProtesterTick >= PTick) {
		// we must roll to see if we add a HCProtester or not
		int roll1 = rng(1, probabilityOfHardcore);
		int roll2 = rng(1, probabilityOfHardcore);

		// add a HCProtester if successfully rolled
		if (roll1 == roll2) {
			All.push_back(new HCProtester(IMID_HARD_CORE_PROTESTER, 10));
		}

		// add a regular protester
		else {
			All.push_back(new Protester(IMID_PROTESTER, 5));
		}

		CurrentProtesterCount++;
		CurrentProtesterTick = 0;
		return;
	}

	else {
		CurrentProtesterTick++;
		return;
	}
}

// randomizes the location of Boulders
void StudentWorld::BrandXY(int& x, int& y) {
	do {
		x = rng(0, 60);

	} while (x >= 30 - 4 && x <= 33);
	y = rng(20, 56);
}

// randomizes the location of Nuggets and Oil Barrels
void StudentWorld::randXY(int& x, int& y) {
	do {
		x = rng(0, 60);

	} while (!(x < 30 - 4 || x > 33));
	y = rng(0, 56);
}

// Checks if DM's current position overlaps with any active Dirt objects. 
//If so, the appropriate Dirt objs are removed
void StudentWorld::dig()
{
	// get DM's position
	int x, y;
	x = DM->getX();
	y = DM->getY();

	// iterate thru all of DM's sprites (4x4) 
	for (unsigned int h = 0; h < 4; h++)
		for (unsigned int w = 0; w < 4; w++)
			// check if DM's coordinates is inside the Dirt field
			if (x + w < DIRT_WIDTH && x + w >= 0 && y + h < DIRT_HEIGHT && y + h >= 0)
			{
				{
					// set the Dirt obj. to false visibility and play a sound
					if (DP[y + h][x + w]->isVisible()) {
						DP[y + h][x + w]->setVisible(false);
						playSound(SOUND_DIG);
					}
				}
			}
}

// clears current Dirt Field
void StudentWorld::clearDirt()
{
	for (unsigned int i = 0; i < DIRT_HEIGHT; i++)
		for (unsigned int j = 0; j < DIRT_WIDTH; j++) {

			delete DP[i][j];
		}
}

// Delete the All<Actor*> 
void StudentWorld::clearActors()
{
	// iterate thru All and delete all Actor objects
	for (unsigned int i = 0; i < All.size(); i++)
	{
		delete All[i];
	}

	// delete all Actor pointers in All
	All.clear();
}

// sets the display text
void StudentWorld::setDisplayText() {

	int level = getLevel();
	int lives = getLives();
	int health = DM->getHP();
	int squirts = DM->getSquirt();
	int nuggets = DM->getNugg();
	int sonar = DM->getSonar();
	int barrelsLeft = getLCount();
	int score = getScore();

	//formatDisplay(level, lives, health, squirts, nuggets, sonar, barrelsLeft, score);

	setGameStatText(formatDisplay(level, lives, health, squirts, nuggets, sonar, barrelsLeft, score));
}

// helper method for display text (formatting)
string StudentWorld::formatDisplay(int level, int lives, int health, int squirts,
	int nuggets, int sonar, int barrelsLeft, int score) {
	stringstream ss; string s;
	ss << "Lvl: " << setfill('0') << setw(2) << level << ' ';
	ss << "Lives: " << lives << ' ';
	ss << "Hlth: " << setfill(' ') << setw(3) << health / TOTAL_HP*100.0 << '%' << "  ";
	ss << "Wtr: " << setfill(' ') << setw(2) << squirts << "  ";
	ss << "Gld: " << setfill(' ') << setw(2) << nuggets << "  ";
	ss << "Sonar: " << setfill(' ') << setw(2) << sonar << "  ";
	ss << "Oil: " << setfill(' ') << setw(2) << barrelsLeft << "  ";
	ss << "Score: " << setfill('0') << setw(6) << score << "  ";
	s = ss.str();

	return s;
}

void StudentWorld::rollGoodie() {
	int n = rng(1, G);
	int r = rng(1, G);

	if (n == r) {
		if (n <= static_cast<int>(1 / 5.0*G)) {
			addSonar();
		}

		else addWaterPool();
	}
}

// checks if the 4x4 space @ (x,y) is void of Dirt and Boulders
// returns true if empty, returns false if occupied
bool StudentWorld::isEmpty(int x, int y) {

	// loop through all actors
	for (unsigned int k = 0; k < All.size(); k++) {

		// WaterPool cannot be added within a 4.0 radius of another Actor
		if (checkDistance(x, y, All[k]->getX(), All[k]->getY(), 4.0))
		{
			// AND check if current coord is empty of dirt in a 4x4 region
			for (unsigned int i = 0; i < 4; i++)
				for (unsigned int j = 0; j < 4; j++) {
					if (DP[y + j][x + i]->isVisible())
						return false;
				}
			// return true; // i don't think this is right
		}
		else return false;
	}

	return true;
}

void StudentWorld::addSquirt() {

	int a = DM->getX();
	int b = DM->getY();
	DM->addDirection(a, b); // add +4 in the direction of DM
	All.push_back(new Squirt(a, b, DM->getDirection()));
}

void StudentWorld::dropNugget() {

	DM->decN();
	Nugget* np = new Nugget(DM->getX(), DM->getY(), Actor::waiting);
	np->setVisible(true);
	All.push_back(np);
}

void StudentWorld::sonarCheck() {

	for (unsigned int i = 0; i < All.size(); i++)
	{
		
		if (All[i]->isDistGameObj()
														// reset sonarCheck radius to 12.0
			&& !checkDistance(DM->getX(), DM->getY(), All[i]->getX(), All[i]->getY(), 12.0))

			All[i]->setVisible(true);
	}
}

bool StudentWorld::nuggetTrap(const Nugget* N) {

	// find only the Protesters
	for (unsigned int i = 0; i < All.size(); i++) {

		// hcprotesters do not leave when finding gold, instead they are "stunned" for the same
		// duration as when they are hit by squirts
		if (dynamic_cast<HCProtester*>(All[i]) != NULL) {
			if (!checkDistance(N->getX(), N->getY(), All[i]->getX(), All[i]->getY(), 3.0)
				&& All[i]->getState() == Actor::alive)
			{
				playSound(SOUND_PROTESTER_FOUND_GOLD);
				increaseScore(50);
				All[i]->setState(Actor::waiting);
				return true;
			}
		}

		else if (dynamic_cast<Protester*>(All[i]) != NULL) {

			// check if Protester is touching Nugget
			if (!checkDistance(N->getX(), N->getY(), All[i]->getX(), All[i]->getY(), 3.0)
				&& All[i]->getState() == Actor::alive)
			{

				playSound(SOUND_PROTESTER_FOUND_GOLD);
				increaseScore(25);
				All[i]->setState(Actor::leaving);
				return true;
			}
		}
	}

	return false;
}

// returns true if Squirt comes into contact with Dirt/Boundaries/Prot/Boulder
bool StudentWorld::squirtCollision(const Squirt* Sq) {
	// get Squirt's position
	int x, y;
	x = Sq->getX();
	y = Sq->getY();


	// iterate thru all of Squirt's sprites (4x4) 
	for (unsigned int w = 0; w < 4; w++)
		for (unsigned int h = 0; h < 4; h++) {

			// check if Squirt's coordinates is within the Oil field boundaries
			if (x + w >= DIRT_WIDTH || x <= 0 || y + h >= DIRT_HEIGHT || y <= 0)
			{
				return true;
			}

			// check if Squirt overlaps with Dirt
			else if (DP[y + h][x + w]->isVisible())
			{
				return true;
			}
		}

	// check if Squirt is colliding with a Boulder or Protester
	for (unsigned int i = 0; i < All.size(); i++)
	{
		// check if Squirt is touching a Boulder
		if (dynamic_cast<Boulder*>(All[i]) != 0)
		{
			if (!checkDistance(x, y, All[i]->getX(), All[i]->getY(), 3.0))
			{
				return true;
			}
		}

		// check is Squirt is touching a Protester
		else if (dynamic_cast<Protester*>(All[i]) != NULL)
		{
			// squirt does not affect Protesters in the leaving state
			if (!checkDistance(x, y, All[i]->getX(), All[i]->getY(), 3.0)
				&& All[i]->getState() != Actor::leaving)
			{
				dynamic_cast<Protester*>(All[i])->decHP(2);

				// if the Squirt makes the Protesters HP <= 0, then change the Pro's state
				if (dynamic_cast<Protester*>(All[i])->getHP() <= 0) {
					if (dynamic_cast<HCProtester*>(All[i]) != NULL)
						increaseScore(250); // hcprot increase score by 250
					else increaseScore(100); // pro increase score by 100

					All[i]->setState(Actor::leaving);
					playSound(SOUND_PROTESTER_GIVE_UP);
				}

				else {
					playSound(SOUND_PROTESTER_ANNOYED);
					dynamic_cast<Protester*>(All[i])->setState(Actor::waiting);
				}

				return true;
			}
		}
	}

	return false; // Squirt has passed all objects and does not collide with anything
}

// returns true if Boulder hits DiggerMan or Dirt or another Boulder
bool StudentWorld::boulderFalling(const Boulder* B) {
	// if boulder hits ground, return true
	if (B->getY() == 0)
		return true;

	// find only the Protesters
	for (unsigned int i = 0; i < All.size(); i++) {
		if (dynamic_cast<Protester*>(All[i]) != NULL) {
			// if Boulder is within 3.0 units of a Protester, set Protester's state to leaving
			if (!checkDistance(B->getX(), B->getY(), All[i]->getX(), All[i]->getY(), 3.0)
				&& (All[i]->getState() == Actor::alive || All[i]->getState() == Actor::waiting))
			{
				playSound(SOUND_PROTESTER_GIVE_UP);
				dynamic_cast<Protester*>(All[i])->decHP(1000);
				increaseScore(500);
				All[i]->setState(Actor::leaving);
				return false;
			}

		} 
	}

	// if boulder is within 3.0 units of DM, set DM's state to Dead
	if (!checkDistance(B->getX(), B->getY(), DM->getX(), DM->getY(), 3.0)) {
		DM->setState(Actor::dead);
		return true;;

	}

	return false;
}

// PARAM: coordinates to check if a Boulder exists there
// returns TRUE if no Boulder exists at the coordinate
// returns FALSE if Boulder exists at the coordinate
bool StudentWorld::checkIfBoulder(const Actor* A)
{
	// Find all Boulder objects
	for (unsigned int i = 0; i < All.size(); i++)
	{
		// search only for Boulders
		if (dynamic_cast<Boulder*>(All[i]) != 0)
		{
			// if a Boulder is in any other state than stable or waiting, 
			// return true (DM is able to move into the Boulder)
			if (dynamic_cast<Boulder*>(All[i])->getState() == Actor::stable
				|| dynamic_cast<Boulder*>(All[i])->getState() == Actor::waiting)
			{
				// get coordinates of Boulder obj
				int xB = All[i]->getX();
				int yB = All[i]->getY();
				int xA = A->getX();
				int yA = A->getY();

				Actor::Direction dir = A->getDirection();

				// REMEMBER: x,y represents DM's PSEUDO COORD not CURRENT COORD
				// Where DM WILL be after he moves +/- 1
				if ((dir == Actor::left && (isWithin(xA - 1, A->getY(), xB, yB)
					|| isWithin(xA - 1, A->getY() + 3, xB, yB))))
					return false;
				else if (dir == Actor::right && (isWithin(xA + 3 + 1, yA, xB, yB)
					|| isWithin(xA + 3 + 1, yA + 3, xB, yB)))
					return false;
				else if (dir == Actor::up && (isWithin(xA, yA + 3 + 1, xB, yB)
					|| isWithin(xA + 3, yA + 3 + 1, xB, yB)))
					return false;
				else if (dir == Actor::down && (isWithin(xA, yA - 1, xB, yB)
					|| isWithin(xA + 3, yA - 1, xB, yB)))
					return false;
			}
		}
	}

	return true;
}

// checks if a point (x,y) is inside an object (x0, y0)
// returns false is (x,y) is outside (x0,y0)
// returns true if it is inside
bool StudentWorld::isWithin(int x, int y, int x0, int y0) {

	return (x >= x0 && x <= x0 + 3 && y >= y0 && y <= y0 + 3);

}

// returns false if Dirt is under a boulder, True if no dirt
bool StudentWorld::checkUnder(const Boulder* B) {

	// Get coordinates of the Boulder object to be checked
	int xB = B->getX();
	int yB = B->getY();

	// iterate four spaces to the right (1 for each Dirt obj. under the Boulder)
	for (unsigned int i = 0; i < 4; i++) {

		// yB - 1 is 1 space underneath Boulder
		if (!(DP[yB - 1][xB + i]->isVisible()))
			continue;
		else return false; // if there is Dirt directly below Boulder, return false
	}

	return true; // return true, no dirt
}

void StudentWorld::yell(Protester* P) {
	DM->decHP(2);
	playSound(SOUND_PROTESTER_YELL);
	P->resetNRTicks();

}

// returns true if no dirt or boulder in coordinates , false if dirt or out of bounds
bool StudentWorld::checkIfDirtOrBoulder(int x, int y) {

	// iterate thru all of (x,y)'s sprites (4x4) 
	for (unsigned int w = 0; w < 4; w++)
		for (unsigned int h = 0; h < 4; h++) {

			// check if (x,y)'s coordinates are within the Oil field boundaries
			if (x + w >= DIRT_WIDTH || x < 0 || y + h >= DIRT_HEIGHT || y < 0)
			{
				return false;
			}

			// return false if Protster overlaps with any Dirt
			else if (DP[y + h][x + w]->isVisible())
			{
				return false;
			}
		}

	// Search through all Actors
	for (unsigned int i = 0; i < All.size(); i++)
	{
		// Filter only for Boulders
		if (dynamic_cast<Boulder*>(All[i]) != 0)
		{
			// if a Boulder is in any other state than stable or waiting, 
			// return true (DM is able to move into the Boulder)
			if (dynamic_cast<Boulder*>(All[i])->getState() == Actor::stable
				|| dynamic_cast<Boulder*>(All[i])->getState() == Actor::waiting)
			{
				// get coordinates of Boulder obj
				int xB = All[i]->getX();
				int yB = All[i]->getY();

				// only need to check the corners of argument (x,y)
				// if they overlap with a Boulder
				if ((isWithin(x, y, xB, yB)
					|| isWithin(x, y + 3, xB, yB)))
					return false;
				else if ((isWithin(x + 3, y, xB, yB)
					|| isWithin(x + 3, y + 3, xB, yB)))
					return false;
				else if ((isWithin(x, y + 3, xB, yB)
					|| isWithin(x + 3, y + 3, xB, yB)))
					return false;
				else if ((isWithin(x, y, xB, yB)
					|| isWithin(x + 3, y, xB, yB)))
					return false;
			}
		}
	}

	return true;
}

//returns TRUE if Protester CAN move into the Direction provided in param
bool StudentWorld::canMove(int x, int y, int dir) {
	if (dir == KEY_PRESS_LEFT && x > 0
		&& checkIfDirtOrBoulder(x - 1, y))
		return true;

	// must check +4 spaces away from digger man if he is moving right 
	else if (dir == KEY_PRESS_RIGHT && x < DIRT_WIDTH - 4
		&& checkIfDirtOrBoulder(x + 1, y))
		return true;

	// must check +4 spaces above DiggerMan if he is moving up
	else if (dir == KEY_PRESS_UP && y < DIRT_HEIGHT - 4
		&& checkIfDirtOrBoulder(x, y + 1))
		return true;

	else if (dir == KEY_PRESS_DOWN && y > 0
		&& checkIfDirtOrBoulder(x, y - 1))
		return true;

	else return false;
}

// updates the path values the EXIT array  (exit is (60,60))
// this is called once every tick in main(), shared by all P's
void StudentWorld::exitBFS() {
	// update the current state of the OilField into the exitPath
	initNode2D(exitPath);

	queue<Node> Q;
	Node* C; // Pointer to the front of the Queue for processing
	int x = 60; int y = 60;

	// push the terminal location into queue
	Q.push(exitPath[y][x]);
	exitPath[y][x].Visited = true; // set it as visited
	int a = exitPath[y][x].PathValue;

	while (!Q.empty()) {
		C = &Q.front(); // Current points to front of queue
		x = C->X; // Front Node's x coord
		y = C->Y; // Front Node's y coord
		a = C->PathValue;
		// add neighboring nodes to the queue and update values
		// check if the node is within bounds, has NOT been visited, AND is NOT a D/B
		// check left
		if (x - 1 >= 0 && !exitPath[y][x - 1].Visited && exitPath[y][x - 1].PathValue < DB_VALUE)
		{
			exitPath[y][x - 1].Visited = true;
			exitPath[y][x - 1].PathValue = a + 1;
			Q.push(exitPath[y][x - 1]);

		}

		// check right
		if (x + 1 < DIRT_WIDTH - 3 && !exitPath[y][x + 1].Visited && exitPath[y][x + 1].PathValue < DB_VALUE) {
			exitPath[y][x + 1].Visited = true;
			exitPath[y][x + 1].PathValue = a + 1;
			Q.push(exitPath[y][x + 1]);
		}

		// check down
		if (y - 1 >= 0 && !exitPath[y - 1][x].Visited && exitPath[y - 1][x].PathValue < DB_VALUE) {
			exitPath[y - 1][x].Visited = true;
			exitPath[y - 1][x].PathValue = C->PathValue + 1;
			Q.push(exitPath[y - 1][x]);
		}

		// check up
		if (y + 1 < DIRT_HEIGHT - 3 && !exitPath[y + 1][x].Visited && exitPath[y + 1][x].PathValue < DB_VALUE) {
			exitPath[y + 1][x].Visited = true;
			exitPath[y + 1][x].PathValue = C->PathValue + 1;
			Q.push(exitPath[y + 1][x]);
		}

		Q.pop();
	}

}

// updates the shortest path array to DIGGERMAN 
// this is called once every tick in main(), shared by all P's
void StudentWorld::DiggerManBFS() {
	initNode2D(DMPath);

	queue<Node> Q;
	Node* C;
	int x = DM->getX();
	int y = DM->getY();
	Q.push(DMPath[y][x]);
	DMPath[y][x].Visited = true;
	int a = DMPath[y][x].PathValue;

	while (!Q.empty()) {
		C = &Q.front();
		x = C->X;
		y = C->Y;
		a = C->PathValue;

		// check if the node is within bounds, has NOT been visited, AND is NOT a D/B
		// check left
		if (x - 1 >= 0 && !DMPath[y][x - 1].Visited && DMPath[y][x - 1].PathValue < DB_VALUE)
		{
			DMPath[y][x - 1].Visited = true;
			DMPath[y][x - 1].PathValue = a + 1;
			Q.push(DMPath[y][x - 1]);
		}

		// check right
		if (x + 1 < DIRT_WIDTH - 3 && !DMPath[y][x + 1].Visited && DMPath[y][x + 1].PathValue < DB_VALUE) {
			DMPath[y][x + 1].Visited = true;
			DMPath[y][x + 1].PathValue = a + 1;
			Q.push(DMPath[y][x + 1]);
		}

		// check down
		if (y - 1 >= 0 && !DMPath[y - 1][x].Visited && DMPath[y - 1][x].PathValue < DB_VALUE) {
			DMPath[y - 1][x].Visited = true;
			DMPath[y - 1][x].PathValue = C->PathValue + 1;
			Q.push(DMPath[y - 1][x]);
		}

		// check up
		if (y + 1 < DIRT_HEIGHT - 3 && !DMPath[y + 1][x].Visited && DMPath[y + 1][x].PathValue < DB_VALUE) {
			DMPath[y + 1][x].Visited = true;
			DMPath[y + 1][x].PathValue = C->PathValue + 1;
			Q.push(DMPath[y + 1][x]);
		}

		Q.pop();
	}

}

// Moves P in the direction of shortest path to DM
bool StudentWorld::shortestPathToDM(Protester* P) {
	// start from the current destination until DiggerMan is reached
	int cX = P->getX(); int cY = P->getY(); //  destination of Protester

	if (cX == DM->getX() && cY == DM->getY()) return false;
	//printPathArray(DMPath);

	int currentPathValue = DMPath[cY][cX].PathValue;
	int leftPathValue = DMPath[cY][cX - 1].PathValue;
	int rightPathValue = DMPath[cY][cX + 1].PathValue;
	int upPathValue = DMPath[cY + 1][cX].PathValue;
	int downPathValue = DMPath[cY - 1][cX].PathValue;

	// check left
	if (cX - 1 >= 0 && canMove(cX, cY, KEY_PRESS_LEFT)
		&& leftPathValue < currentPathValue) {
		P->moveTo(cX - 1, cY);
	}

	//check up
	else if (cY + 1 < DIRT_HEIGHT && canMove(cX, cY, KEY_PRESS_UP)
		&& upPathValue < currentPathValue) {
		P->moveTo(cX, cY + 1);
	}

	// check right
	else if (cX + 1 < DIRT_WIDTH && canMove(cX, cY, KEY_PRESS_RIGHT)
		&& rightPathValue < currentPathValue) {
		P->moveTo(cX + 1, cY);
	}

	// check down from current pos (cX,cY)
	else if (cY - 1 >= 0 && canMove(cX, cY, KEY_PRESS_DOWN)
		&& downPathValue < currentPathValue) {
		P->moveTo(cX, cY - 1);
	}

	// change the P's direction after moving (if needed)
	int xf = P->getX(); int yf = P->getY();

	if (xf - cX < 0)
		P->setDirection(Actor::left);
	else if (xf - cX > 0)
		P->setDirection(Actor::right);
	else if (yf - cY < 0)
		P->setDirection(Actor::down);
	else if (yf - cY > 0)
		P->setDirection(Actor::up);

	return true;
}

// Moves P in the direction of shortest path to Exit
bool StudentWorld::shortestPathToExit(Protester* P) {
	// start from the current destination until exit (60,60) is reached
	int cX = P->getX(); int cY = P->getY(); //  destination of Protester

	if (cX == 60 && cY == 60) return false;

	int currentPathValue = exitPath[cY][cX].PathValue;
	int leftPathValue = exitPath[cY][cX - 1].PathValue;
	int rightPathValue = exitPath[cY][cX + 1].PathValue;
	int upPathValue = exitPath[cY + 1][cX].PathValue;
	int downPathValue = exitPath[cY - 1][cX].PathValue;

	//printexitPathArray();
	// check left
	if (cX - 1 >= 0 && canMove(cX, cY, KEY_PRESS_LEFT)
		&& leftPathValue < currentPathValue) {
		P->moveTo(cX - 1, cY);
	}

	//check up
	else if (cY + 1 < DIRT_HEIGHT && canMove(cX, cY, KEY_PRESS_UP)
		&& upPathValue < currentPathValue) {
		P->moveTo(cX, cY + 1);
	}

	// check right
	else if (cX + 1 < DIRT_WIDTH && canMove(cX, cY, KEY_PRESS_RIGHT)
		&& rightPathValue < currentPathValue) {
		P->moveTo(cX + 1, cY);
	}

	// check down from current pos (cX,cY)
	else if (cY - 1 >= 0 && canMove(cX, cY, KEY_PRESS_DOWN)
		&& downPathValue < currentPathValue) {
		P->moveTo(cX, cY - 1);
	}

	// change the P's direction after moving (if needed)
	int xf = P->getX(); int yf = P->getY();

	if (xf - cX < 0)
		P->setDirection(Actor::left);
	else if (xf - cX > 0)
		P->setDirection(Actor::right);
	else if (yf - cY < 0)
		P->setDirection(Actor::down);
	else if (yf - cY > 0)
		P->setDirection(Actor::up);

	return true;
}

// for debugging
void StudentWorld::printPathArray(Node arr[][DIRT_WIDTH]) {
	for (int i = DIRT_HEIGHT; i >= 0; i--) {
		for (int j = 0; j < DIRT_WIDTH; j++) {
			cout << setw(3) << arr[i][j].PathValue << " ";
		}
		cout << endl;
	}
}

// returns true if DM is in an unobstructed line of sight of P
bool StudentWorld::lineofsight(Protester* P) {
	int pX = P->getX(); int pY = P->getY();
	int DMX = DM->getX(); int DMY = DM->getY();

	// find delta X and delta Y to see if P is lateral to DM
	int dX = DMX - pX; int dY = DMY - pY;



	// fire a beam up towards DM
	if (dY > 0) {
		for (int i = 0; i < dY; i++) {
			if (!checkIfDirtOrBoulder(pX, pY + i))
				return false;
		}
	}

	// fire a beam down towards DM
	else if (dY < 0) {
		for (int i = 0; i < abs(dY); i++) {
			if (!checkIfDirtOrBoulder(pX, pY - i))
				return false;
		}
	}

	// fire a beam right towards DM
	if (dX > 0) {
		for (int i = 0; i < dX; i++) {
			if (!checkIfDirtOrBoulder(pX + i, pY))
				return false;
		}
	}

	// fire a beam left towards DM
	else {
		for (int i = 0; i < abs(dX); i++) {
			if (!checkIfDirtOrBoulder(pX - i, pY))
				return false;
		}
	}


	// if all tests are passed, P's line of sight to DM is unobstructed, return true
	// change direction before leaving
	if (dX > 0) {
		P->setDirection(Actor::right);
		P->moveTo(pX + 1, pY);
	}
	else if (dX < 0) {
		P->setDirection(Actor::left);
		P->moveTo(pX - 1, pY);
	}
	else if (dY > 0) {
		P->setDirection(Actor::up);
		P->moveTo(pX, pY + 1);
	}
	else {
		P->setDirection(Actor::down);
		P->moveTo(pX, pY - 1);
	}

	return true;
}

// returns false if Protester is NOT at an intersection
// returns true if Protester is at an intersection AND changes its Direction 
bool StudentWorld::isIntersection(Protester* P) {



	return false;
}

void StudentWorld::hardcoreMove() {

}