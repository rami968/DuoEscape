#pragma once
#include <vector>
#include "Direction.h"
#include "point.h"

class Spring{
	std::vector<char> springChars;
	Direction releaseDir;
	Point anchorWall;           
	int currentCompressedCount = 0;
public:
	Spring(const Point& anchor, Direction dir, std::vector<char> spring)
		: anchorWall(anchor), releaseDir(dir), springChars(spring) {
	}
	const Point& getAnchorWall() const {
		return anchorWall;
	}
	const Direction getReleaseDirection() const {
		return releaseDir;
	}
	void compress() {
		if (currentCompressedCount < springChars.size()) {
			currentCompressedCount++;
		}
	}
	int getReleaseSpeed() const {
		return currentCompressedCount;
	}
	int getReleaseDuration() const {
		return currentCompressedCount * currentCompressedCount;
	}

	
};

