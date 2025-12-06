#pragma once

#include <cstddef>
#include "Point.h"

class BombHelper {
public:
	static constexpr size_t MAX_PENDING_BOMBS = 16;

	static void queueBomb(const Point& pos);
	static bool tryPopNext(Point& out);
	static void clear();

private:
	static Point bombSlots[MAX_PENDING_BOMBS];
	static size_t bombCount;
};
