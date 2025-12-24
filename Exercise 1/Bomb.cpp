#include "Bomb.h"

Bomb::Bomb(const Point& position, int countdown)
    : pos(position), ticksRemaining(countdown) {
}

void Bomb::tick() {
    if (ticksRemaining > 0) {
        --ticksRemaining;
    }
}

bool Bomb::isReadyToExplode() const {
    return ticksRemaining <= 0;
}

const Point& Bomb::getPosition() const {
    return pos;
}
