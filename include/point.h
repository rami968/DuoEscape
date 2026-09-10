#pragma once

#include <iostream>
#include "utils.h"
#include "Direction.h"
#include "ColorManager.h"
#include "MapChar.h"

class Point {
	int x = 1, y = 1;
	int diff_x = 1, diff_y = 0;
	char ch = '*';
public:
	Point() {}
	Point(int x1, int y1, int diffx, int diffy, char c) {
		x = x1;
		y = y1;
		diff_x = diffx;
		diff_y = diffy;
		ch = c;
	}
	// Draws the stored character at the current position
	void draw(bool isColorEnabled) {
		draw(ch, isColorEnabled);
	}

	// Draws a given character at the current position
	void draw(char c, bool isColorEnabled) {
		gotoxy(x, y);
		if (c == static_cast<char>(MapChar::Player1) || c == static_cast<char>(MapChar::Player2))
			ColorManager::setTextColor(ColorManager::GREEN, isColorEnabled);
		else if (c == static_cast<char>(MapChar::Wall) || c == static_cast<char>(MapChar::SoftWall))
			ColorManager::setTextColor(ColorManager::GREY, isColorEnabled);
		else if (c == static_cast<char>(MapChar::Obstacle) || c == static_cast<char>(MapChar::Spring))
			ColorManager::setTextColor(ColorManager::YELLOW, isColorEnabled);
		else if (c == static_cast<char>(MapChar::BombItem))
			ColorManager::setTextColor(ColorManager::RED, isColorEnabled);
		else if (c == static_cast<char>(MapChar::Star))
			ColorManager::setTextColor(ColorManager::MAGENTA, isColorEnabled);
		else if (c == static_cast<char>(MapChar::SwitchOn) || c == static_cast<char>(MapChar::SwitchOff))
			ColorManager::setTextColor(ColorManager::CYAN, isColorEnabled);
		else if (c == static_cast<char>(MapChar::Key) || c == static_cast<char>(MapChar::Torch))
			ColorManager::setTextColor(ColorManager::YELLOW, isColorEnabled);
		else if (c >= '0' && c <= '9')
			ColorManager::setTextColor(ColorManager::BLUE, isColorEnabled);
		else
			ColorManager::setTextColor(ColorManager::DEFAULT, isColorEnabled);
		std::cout << c;
		ColorManager::setTextColor(ColorManager::DEFAULT, isColorEnabled);
	}

	// Moves the point one step using diff_x and diff_y
	void move();

	// Sets the movement direction using a Direction value
	void setDirection(Direction dir);

	// Returns the x coordinate
	int getX() const {
		return x;
	}

	// Returns the current movement direction based on diff_x and diff_y
	Direction getDirection() const;

	// Returns the y coordinate
	int getY() const {
		return y;
	}

	// Returns the character stored at this point
	char getChar() const {
		return ch;
	}

	// Sets the character stored at this point
	void setChar(char c) {
		ch = c;
	}
	void setX(int newX) {
		x = newX;
	}
	void setY(int newY) {
		y = newY;
	}

	Point calculateNext(Direction dir) const;
	bool operator==(const Point& other) const {
		return (x == other.x && y == other.y);
	}
};
