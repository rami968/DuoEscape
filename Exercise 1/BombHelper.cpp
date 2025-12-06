#include "BombHelper.h"

Point BombHelper::bombSlots[MAX_PENDING_BOMBS]{};
size_t BombHelper::bombCount = 0;

void BombHelper::queueBomb(const Point& pos) {
    if (bombCount >= MAX_PENDING_BOMBS) {
        return;
    }
    bombSlots[bombCount++] = pos;
}

bool BombHelper::tryPopNext(Point& out) {
    if (bombCount == 0) {
        return false;
    }
    out = bombSlots[0];
    for (size_t i = 1; i < bombCount; ++i) {
        bombSlots[i - 1] = bombSlots[i];
    }
    --bombCount;
    return true;
}

void BombHelper::clear() {
    bombCount = 0;
}
