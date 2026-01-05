#pragma once

#include <iostream>
#include <vector>
#include "utils.h"
#include "Direction.h"
#include "Point.h"
#include "Riddle.h"
#include "screen.h"
#include "Doors.h"

class Player {
	static constexpr int NUM_KEYS = 6;
	char the_keys[NUM_KEYS];
	Point p;
	Doors* currDoor = nullptr;
	bool awaitingScreenTransition = false;
	screen* theScreen;
	Point heldElementPos;
	Point keyFirstPos = Point(-1, -1, 0, 0, ' ');
	Riddle* activeRiddle = nullptr;
	char heldElement = ' ';
	int ticksUntilNextMove = 0;
	std::vector<Point> collectedKeys;
	Direction lastMoveDir = Direction::STAY;
	bool hasKeyInInventory(const Point& keyPos) const;
	bool removeKeyFromInventory(const Point& keyPos);
	bool bombRequested = false;
	Point bombRequestPos = Point(-1, -1, 0, 0, ' ');
	int lives = 3;

public:
	static constexpr int MOVE_TICK_INTERVAL = 1;
	Player(const Point& point, const char(&keys)[NUM_KEYS + 1], screen& screen);
	void disposeElement();
	void handleKeyPressed(char key_pressed);
	void move();
	void draw();
	char getHeldElement() const;
	Point getHeldElementPos() const;
	bool hasElement() const;
	bool hasTorch() const;
	void pickUpElement(char element, const Point& pos);
	Doors* getTransitionDoor() const { return currDoor; }
	bool isAwaitingTransition() const { return awaitingScreenTransition; }
	void resetTransitionSignal();
	void setPosition(const Point& newPos);
	void setScreen(screen* newScreen) {
		theScreen = newScreen;
	}
	Riddle* getActiveRiddle() const { return activeRiddle; }
	void setActiveRiddle(Riddle* riddle) { activeRiddle = riddle; }
	void resetActiveRiddle() { activeRiddle = nullptr; }
	const Point& getPosition() const { return p; }
	const std::vector<Point>& getCollectedKeys() const { return collectedKeys; }
	void consumeHeldKey();
	void setDirection(Direction dir) {
		p.setDirection(dir);
	}
	void handleDoorInteraction(const Point& p_orig, char targetChar, Doors* currentDoor);
	void resetHeldElement() {
		heldElement = ' ';
	}
	bool tryPopBombRequest(Point& out);
	int getLives() const { return lives; }
	void loseLife(int amount = 1) { lives = (lives > amount ? lives - amount : 0); }
	bool isDead() const { return lives == 0; }
	void resetLives(int v = 3) { lives = v; }
};