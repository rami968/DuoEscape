#include "Bomb.h"

Bomb::Bomb(const Point& position, int countdown, int bOwner) : pos(position), ticksRemaining(countdown), bombOwner(bOwner){}

void Bomb::tick() {
    if (ticksRemaining > 0) {
        --ticksRemaining;
    }
}

bool Bomb::isReadyToExplode() const {
    return ticksRemaining <= 0;
}



