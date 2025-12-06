#pragma once

#include <iostream>
#include <vector>
#include "utils.h"
#include "Direction.h"
#include "Point.h"
#include "Riddle.h"
#include "screen.h"

class Player {
	static constexpr int NUM_KEYS = 6;
	static constexpr int MOVE_TICK_INTERVAL = 2;
	char the_keys[NUM_KEYS];
	Point p;
	Doors* currDoor = nullptr;
	screen* theScreen;
	Point heldElementPos;
	Riddle* activeRiddle = nullptr;
	char heldElement = ' ';
	int ticksUntilNextMove = 0;
	std::vector<Point> collectedKeys;
	bool hasKeyInInventory(const Point& keyPos) const;
	bool removeKeyFromInventory(const Point& keyPos);
	
public:
	Player(const Point& point, const char(&keys)[NUM_KEYS + 1], screen& screen);
	void disposeElement();
	void handleKeyPressed(char key);
	void move();
	void draw();
	char getHeldElement() const;
	Point getHeldElementPos() const;
	bool hasElement() const;
	void pickUpElement(char element, const Point& pos);
	Doors* getTransitionDoor() const { return currDoor; }
	void resetTransitionSignal() { currDoor = nullptr; }
	void setPosition(const Point& newPos);
	void setScreen(screen* newScreen) {
		theScreen = newScreen;
	}
	const std::vector<Point>& getCollectedKeys() const { return collectedKeys; }
	Riddle* getActiveRiddle() const { return activeRiddle; }
	void setActiveRiddle(Riddle* riddle) { activeRiddle = riddle; }
	void resetActiveRiddle() { activeRiddle = nullptr; }
};
