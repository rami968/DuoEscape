#pragma once

#include <iostream>
#include "utils.h"
#include "Direction.h"
#include "point.h"

class Player {
	static constexpr int NUM_KEYS = 6;
	char the_keys[NUM_KEYS];
	point p;
public:
	Player(const point& point, const char(&keys)[NUM_KEYS + 1]) {}

	void disposeElement();
	void handleKeyPressed(char key);
	
	

