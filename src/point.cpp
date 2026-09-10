#include "Point.h"
#include "screen.h"

void Point::move() {
	// Move according to current diff_x/diff_y and wrap around screen edges (toroidal)
	x = (x + diff_x + screen::MAX_X) % screen::MAX_X;
	y = (y + diff_y + screen::MAX_Y) % screen::MAX_Y;
}

void Point::setDirection(Direction dir) {
	// Set movement delta (diff_x, diff_y) based on direction
	switch (dir) {
	case Direction::UP:
		diff_x = 0;
		diff_y = -1;
		break;
	case Direction::RIGHT:
		diff_x = 1;
		diff_y = 0;
		break;
	case Direction::DOWN:
		diff_x = 0;
		diff_y = 1;
		break;
	case Direction::LEFT:
		diff_x = -1;
		diff_y = 0;
		break;
	case Direction::STAY:
		diff_x = 0;
		diff_y = 0;
		break;
	}
}

Direction Point::getDirection() const {
	// Infer direction from the current movement delta
	if (diff_x == 1 && diff_y == 0)
		return Direction::RIGHT;
	if (diff_x == -1 && diff_y == 0)
		return Direction::LEFT;
	if (diff_x == 0 && diff_y == -1)
		return Direction::UP;
	if (diff_x == 0 && diff_y == 1)
		return Direction::DOWN;
	// Any other case is treated as STAY (no movement)
	return Direction::STAY;
}


Point Point::calculateNext(Direction dir) const {
	Point nextPos = *this; 

	switch (dir) {
	case Direction::UP:
		nextPos.y--;
		break;
	case Direction::DOWN:
		nextPos.y++;
		break;
	case Direction::LEFT:
		nextPos.x--;
		break;
	case Direction::RIGHT:
		nextPos.x++;
		break;
	case Direction::STAY:
		break;
	}
	return nextPos;
}