#pragma once
#include "screen.h"
#include "Player.h"
#include <vector>


class GameManager {
private:
    std::vector<screen> screens; 
    int currentScreenID = 0;
    Player player1;
    Player player2;
public:
    GameManager(Player p_1, Player p_2);
    void run();
    void changeScreen(int newScreenID, Point destinationPos, Player p1, Player p2);

    screen& getCurrentScreen() {
        return screens[currentScreenID];
    }
};

