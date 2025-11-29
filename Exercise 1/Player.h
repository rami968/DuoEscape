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
	screen& theScreen;
public:
	Player(const Point& point, const char(&keys)[NUM_KEYS + 1], screen& screen);
	//void disposeElement();
	void handleKeyPressed(char key);
	void move();
	void draw();
};
//