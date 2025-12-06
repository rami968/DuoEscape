#pragma once
#include "screen.h"
#include "Player.h"
#include "Point.h"
#include <vector>


class GameManager {
private:
    std::vector<screen> screens; 
    int currentScreenID = 0;
    Player player1;
    Player player2;
    bool p1_has_exited = false;
    bool p2_has_exited = false;
    Doors* p1_exit_door = nullptr;
    Doors* p2_exit_door = nullptr;
public:
    GameManager();
    void resetGameState();
    void run();
    void showMenuAndHandleInput();
    void displayMenu() const;
    void displayPauseScreen() const;
    void changeScreen(int newScreenID, const Point& destinationPos, Player& p1, Player& p2);
	void displayInstructions() const;
	void handleRiddleSolving(Player* player, screen& currentScreen);
    screen& getCurrentScreen() {
        return screens[currentScreenID];
    }
};

