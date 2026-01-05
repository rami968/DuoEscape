#pragma once
#include "Point.h"

class Bomb {
public:
    Bomb(const Point& position, int countdown);

    void tick();

    bool isReadyToExplode() const;

    const Point& getPosition() const;

private:
    Point pos;           
    int ticksRemaining;  
};

