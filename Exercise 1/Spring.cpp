#include "Spring.h"
#include "screen.h"

Spring::Spring(const std::vector<Point>& pts, Direction dir, screen* theScreen) :
	positions(pts),
	releaseDir(dir),
	originalLength(pts.size()),
    currScreen(theScreen)  {
}

void Spring::compress() {
	if (currentCompressedCount < originalLength) {
		++currentCompressedCount;
	}
	currScreen->setCharAt(positions[originalLength - currentCompressedCount], ' ');
	isSpringCompressed = true;
}

void Spring::resetSpring() {
	currentCompressedCount = 0;
	for (const auto& pos : positions) {
		currScreen->setCharAt(pos, '#');
	}
	isSpringCompressed = false;
}
