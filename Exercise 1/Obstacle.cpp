#include "Obstacle.h"
#include "screen.h"
#include <vector>

Obstacle::Obstacle(const std::vector<Point>& points, screen* scr)
    : parts(points), totalWeight(points.size()),  theScreen(scr) {
}

bool Obstacle::canMove(Direction dir, int force) {
    if (force < totalWeight) {
        return false;
    }
    if (!handleInteractionsWithObstacle(dir, true)) {
        return false;
    }
    return true;
}


bool Obstacle:: containsPoint(const Point& p) const {
	for (const auto& part : parts) {
		if (part == p) {
			return true;
		}
	}
	return false;
}

void Obstacle::moveObstacle(Direction dir) {
    handleInteractionsWithObstacle(dir, false);

    for (auto& part : parts) {
		theScreen->setCharAt(part, ' '); 
		part.draw(' ');
	}

	for (auto& part : parts) {
		part = part.calculateNext(dir);
	}

	for (auto& part : parts) {
		theScreen->setCharAt(part, '*'); 
		part.draw('*');
	}
}

bool Obstacle::handleInteractionsWithObstacle(Direction dir, bool simulate) {
    for (const auto& part : parts) {
        Point nextPos = part.calculateNext(dir);

        if (containsPoint(nextPos)) continue;

        if (theScreen->isWall(nextPos) || theScreen->isRiddle(nextPos) || theScreen->isDoor(nextPos) ||
            theScreen->isSpring(nextPos)) {
            return false;
        }
		if (theScreen->isPlayer(nextPos)) {
			return false;
		}
        if (theScreen->isObstacle(nextPos)) {
            continue;
        }

        if (!simulate) {
            if (theScreen->isSwitchOn(nextPos) || theScreen->isSwitchOff(nextPos)) {
                theScreen->toggleSwitchAt(nextPos);
            }
            if (theScreen->isKey(nextPos) || theScreen->isTorch(nextPos)) {
                theScreen->setCharAt(nextPos, ' ');
            }
        }
    }
    return true;
}

