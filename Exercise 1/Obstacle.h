#pragma once
#include <vector>
#include "Point.h"
#include "Direction.h"

class screen;	

class Obstacle{
	std::vector<Point> parts;
	int totalWeight;
	screen* theScreen;

public:
	Obstacle(const std::vector<Point>& points, screen* scr);
	int getRequiredForce() const { return totalWeight; }
	bool canMove(Direction dir, int force);
	void moveObstacle(Direction dir);
	bool containsPoint(const Point& p) const;
	const std::vector<Point>& getPositions() const { return parts; }
	bool handleInteractionsWithObstacle(Direction dir, bool simulate = false);
};

