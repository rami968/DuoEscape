#include "Spring.h"
#include "screen.h"

Spring::Spring(const std::vector<Point>& pts, Direction dir, screen* theScreen) :
	positions(pts),
	releaseDir(dir),
	originalLength(static_cast<int>(pts.size())),
    currScreen(theScreen)  {
}

// Compresses the spring
void Spring::compress() {
	if (currentCompressedCount < originalLength) {
		++currentCompressedCount;
	}
	currScreen->setCharAt(positions[originalLength - currentCompressedCount], ' ');
	isSpringCompressed = true;
}

// Resets the spring
void Spring::resetSpring() {
	currentCompressedCount = 0;
	for (const auto& pos : positions) {
		currScreen->setCharAt(pos, '#');
	}
	isSpringCompressed = false;
}
