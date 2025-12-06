#include "BombHelper.h"

namespace BombHelper {
	static std::queue<Point> g_pending;

	void queueBomb(const Point& pos) {
		g_pending.push(pos);
	}

	bool tryPopNext(Point& out) {
		if (g_pending.empty()) {
			return false;
		}
		out = g_pending.front();
		g_pending.pop();
		return true;
	}

	void clear() {
		std::queue<Point> empty;
		std::swap(g_pending, empty);
	}
}
