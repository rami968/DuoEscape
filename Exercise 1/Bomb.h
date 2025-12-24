#pragma once
#include "Point.h"

class Bomb {
public:
    // יוצרת פצצה במיקום position עם טיימר countdown
    Bomb(const Point& position, int countdown);

    // מורידה 1 מהטיימר (כל "טיק" במשחק)
    void tick();

    // האם הגיע הזמן להתפוצץ?
    bool isReadyToExplode() const;

    // המיקום של הפצצה
    const Point& getPosition() const;

private:
    Point pos;           // איפה הפצצה נמצאת על המפה
    int ticksRemaining;  // כמה טיקים נשארו עד פיצוץ
};
#pragma once
