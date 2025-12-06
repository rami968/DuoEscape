#pragma once

#include <queue>
#include "Point.h"

namespace BombHelper {
	void queueBomb(const Point& pos);
	bool tryPopNext(Point& out);
	void clear();
}
