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
	static constexpr int MOVE_TICK_INTERVAL = 1; // number of game loops to wait between moves
	char the_keys[NUM_KEYS];
	Point p;
	Doors* currDoor = nullptr;
	bool awaitingScreenTransition = false; // true when player already passed through a door
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
public:
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
};
