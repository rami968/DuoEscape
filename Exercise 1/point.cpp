#include "Point.h"
#include "screen.h"

void Point::move() {
	x = (x + diff_x + screen::MAX_X) % screen::MAX_X;
	y = (y + diff_y + screen::MAX_Y) % screen::MAX_Y;
}

void Point::setDirection(Direction dir) {
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
    if (diff_x == 1 && diff_y == 0)
        return Direction::RIGHT;
    if (diff_x == -1 && diff_y == 0)
        return Direction::LEFT;
    if (diff_x == 0 && diff_y == -1)
        return Direction::UP;
    if (diff_x == 0 && diff_y == 1)
        return Direction::DOWN;
    return Direction::STAY;
}

