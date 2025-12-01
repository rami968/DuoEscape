#pragma once
#include "screen.h"
#include "Player.h"
#include "Point.h"
#include <vector>


class GameManager {
private:
    std::vector<screen> screens; 
    int currentScreenID = 0;  
public:
    GameManager();
    void run();
    void changeScreen(int newScreenID, const Point& destinationPos, Player& p1, Player& p2);

    screen& getCurrentScreen() {
        return screens[currentScreenID];
    }
};

