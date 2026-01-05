#pragma once
#include "Point.h"

class Bomb {
private:
    Point pos;
    int ticksRemaining;
    int bombOwner;
public:
    Bomb(const Point& position, int countdown, int bOwner);
    void tick();
    bool isReadyToExplode() const;
    const Point& getPosition() const {return pos;}
    int getTicksRemaining() const { return ticksRemaining; }
    int getBombOwner() const { return bombOwner; }
};