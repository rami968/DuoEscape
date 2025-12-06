#pragma once
#include "screen.h"
#include "Point.h"
#include <vector>

class Player;


class GameManager {
private:
    struct ArmedBomb {
        int x;
        int y;
        int ticksRemaining;
    };
    std::vector<screen> screens; 
    int currentScreenID = 0;  
    std::vector<ArmedBomb> activeBombs;
    static constexpr int BOMB_COUNTDOWN = 30;
    static constexpr int BOMB_RADIUS = 3;
    void armBombAt(const Point& pos);
    bool processBombs(Player& p1, Player& p2);
    bool explodeBomb(const ArmedBomb& bomb, Player& p1, Player& p2);
public:
    GameManager();
    void run();
    void changeScreen(int newScreenID, const Point& destinationPos, Player& p1, Player& p2);
    void queueBombAt(const Point& pos);

    screen& getCurrentScreen() {
        return screens[currentScreenID];
    }
};

