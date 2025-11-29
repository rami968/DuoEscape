#pragma once
#include <iostream>
#include "Point.h"

using std::cout, std::endl;

class screen {
public:
	enum { MAX_X = 80, MAX_Y = 25 };
private:
	const char* s[MAX_Y] = {
		//   01234567890123456789012345678901234567890123456789012345678901234567890123456789
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
			"W####                                                                  #    ###W", // 1
			"W####                                                                  #    ###W", // 2
			"W####                                                                  #    ###W", // 3
			"W####                                                                  #    ###W", // 4
			"W####                                                                  #    ###W", // 5
			"W####                                                                  #    ###W", // 6
			"W####                                                                  #    ###W", // 7
			"W####               WWWWWWWWW                                               ###W", // 8
			"W####               #########                                               ###W", // 9
			"W                                                                              W", // 10
			"W                                                                              W", // 11
			"W                                                                           ###W", // 12
			"W                                                                              W", // 13
			"W                                                                              W", // 14
			"W                                                                              W", // 15
			"W                                                                              W", // 16
			"W                                                                              W", // 17
			"W                                                                              W", // 18
			"W                                                                              W", // 19
			"W                                                                              W", // 20
			"W                                                                              W", // 21
			"W                                                                              W", // 22
			"W                                                                              W", // 23
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"  // 24
	};
	char getCharAt(const Point& p) const {
		return s[p.getY()][p.getX()];
	}
public:
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
};

