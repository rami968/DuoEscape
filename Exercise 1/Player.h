#pragma once

#include <iostream>
#include "utils.h"
#include "Direction.h"
#include "Point.h"
#include "screen.h"

class Player {
	static constexpr int NUM_KEYS = 6;
	char the_keys[NUM_KEYS];
	Point p;
	Doors* currDoor = nullptr;
	screen* theScreen;
	Point heldElementPos;
	char heldElement = ' ';
	
public:
	Player(const Point& point, const char(&keys)[NUM_KEYS + 1], screen& screen);
	void disposeElement();
	void handleKeyPressed(char key_pressed);
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


};
