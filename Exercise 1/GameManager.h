#pragma once
#include "screen.h"
#include "Point.h"
#include "Player.h"
#include "Doors.h"     
#include "BombHelper.h"
#include <vector>



class GameManager {
private:
    struct ArmedBomb {
        int x;
        int y;
        int ticksRemaining;
    };
    std::vector<screen> screens;
    int currentScreenID = 0;
    Player player1;
    Player player2;
    bool p1_has_exited = false;
    bool p2_has_exited = false;
    Doors* p1_exit_door = nullptr;
    Doors* p2_exit_door = nullptr;
    std::vector<ArmedBomb> activeBombs;
    static constexpr int BOMB_COUNTDOWN = 30;
    static constexpr int BOMB_RADIUS = 3;
    void armBombAt(const Point& pos);
    bool processBombs(Player& p1, Player& p2);
    bool explodeBomb(const ArmedBomb& bomb, Player& p1, Player& p2);
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
    void queueBombAt(const Point& pos);
    void displayingPlayerStatus(Player& p1, Player& p2, screen& currentScreen);
    screen& getCurrentScreen() {
        return screens[currentScreenID];
    }
	int getCurrentScreenID() const {
		return currentScreenID;
	}
};

