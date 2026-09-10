#pragma once

#include <vector>
#include "Direction.h"
#include "Point.h"
//#include "screen.h"

class screen;

class Spring{
	std::vector<Point> positions;
	Direction releaseDir;
	int currentCompressedCount = 0;
	int originalLength;     
	screen* currScreen;
	bool isSpringCompressed = false;
public:
	Spring(const std::vector<Point>& pts, Direction dir, screen* theScreen);
	const Direction getReleaseDirection() const {
		return releaseDir;
	}
	void compress();
	int getReleaseSpeed() const {
		return currentCompressedCount;
	}
	int getReleaseDuration() const {
		return currentCompressedCount * currentCompressedCount;
	}
	void resetSpring();
	const std::vector<Point>& getPositions() const {return positions;}
	bool isCompressed() const { return isSpringCompressed; }
};

