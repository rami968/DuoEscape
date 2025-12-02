#pragma once
#include <iostream>
#include "Point.h"
#include "Doors.h"
#include <vector>

using std::cout, std::endl;

class screen {
public:
	enum { MAX_X = 80, MAX_Y = 25 };
private:
	int currentScreenID = 0;
	char mapData[MAX_Y][MAX_X + 1];
	std::vector<Doors> doors;

public:
	screen(int id = 0) { initScreenData(id); }
	char getCharAt(const Point& p) const {
		return mapData[p.getY()][p.getX()];
	}
	Doors* getDoorByChar(char doorChar);
	void initScreenData(int ScreenID);
	void draw() const;
	bool isWall(const Point& p) const {
		return getCharAt(p) == 'W';
	}
	bool isDoor(const Point& p) const {
		return isdigit(getCharAt(p));
	}
	bool isSwitchOff(const Point& p) const {
		return getCharAt(p) == '/';
	}
	bool isSwitchOn(const Point& p) const {
		return getCharAt(p) == '\\';
	}
	bool isKey(const Point& p) const {
		return getCharAt(p) == 'K';
	}
	bool isRiddle(const Point& p) const {
		return getCharAt(p) == '?';
	}
	void setCharAt(const Point& pos, char ch);
};
