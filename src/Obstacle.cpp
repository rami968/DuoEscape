#include "Obstacle.h"
#include "screen.h"
#include "MapChar.h"
#include <vector>

Obstacle::Obstacle(const std::vector<Point>& points, screen* scr)
    : parts(points), totalWeight(static_cast<int>(points.size())), theScreen(scr) {
}

// Checks if the obstacle can move in a given direction with a given force
bool Obstacle::canMove(Direction dir, int force, bool isColorEnabled) {
    if (force < totalWeight) {
        return false;
    }
    if (!handleInteractionsWithObstacle(dir, true)) {
        return false;
    }
    return true;
}

// Checks if the obstacle contains a given point
bool Obstacle:: containsPoint(const Point& p) const {
	for (const auto& part : parts) {
		if (part == p) {
			return true;
		}
	}
	return false;
}

// Moves the obstacle in a given direction
void Obstacle::moveObstacle(Direction dir, bool isColorEnabled) {
    handleInteractionsWithObstacle(dir, false);

    for (auto& part : parts) {
		theScreen->setCharAt(part, static_cast<char>(MapChar::Empty)); 
		part.draw(static_cast<char>(MapChar::Empty), isColorEnabled);
	}

	for (auto& part : parts) {
		part = part.calculateNext(dir);
	}

	for (auto& part : parts) {
		theScreen->setCharAt(part, static_cast<char>(MapChar::Obstacle)); 
		part.draw(static_cast<char>(MapChar::Obstacle), isColorEnabled);
	}
}

// Handles interactions with the obstacle in a given direction
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

