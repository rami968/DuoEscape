#pragma once
#include "screen.h"
#include "Point.h"
#include "Player.h"
#include "Doors.h"     
#include <vector>
#include <cstddef>



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
    bool isFinalScreen = false;
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
    void displayingPlayerStatus(Player& p1, Player& p2, screen& currentScreen);
    screen& getCurrentScreen() {
        return screens[currentScreenID];
    }
	int getCurrentScreenID() const {
		return currentScreenID;
	}
	void handleRiddleCheck(Player* player, screen& currentScreen);
    void checkAndHandleTorchUpdate(Player& p1, Player& p2, bool& lastTorchState);
    bool handleScreenTransition(Player& p1, Player& p2, bool& lastTorchState);
	bool handlePauseInput();
};

