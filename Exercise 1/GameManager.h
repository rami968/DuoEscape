#pragma once
#include "screen.h"
#include "Point.h"
#include "Player.h"
#include "Doors.h"
#include "Bomb.h"
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

    static constexpr int BOMB_GAME_CYCLES = 10;
    static constexpr int BOMB_RADIUS = 3;
    std::vector<Bomb> activeBombs;
    void armBombAt(const Point& pos, int ownerId);
    bool processBombs(Player& p1, Player& p2);
    bool explodeBomb(const Bomb& bomb, Player& p1, Player& p2);
    void pollBombRequests(Player& p1, Player& p2);
    
    int teamScore = 0;
    int screenTicks = 0;
    int lastTimeLossStepApplied = 0;
    static constexpr int FAST_EXIT_TICKS = 1800; // ~90s if Sleep(50)
    static constexpr int FAST_EXIT_BONUS_POINTS = 15;
    static constexpr int MAX_TICKS_WITHOUT_SCORE_LOSS = 3000; // ~150s if Sleep(50)
    static constexpr int TICKS_PER_SCORE_LOSS = 400; // ~20s if Sleep(50)
    static constexpr int SCORE_LOSS_PER_STEP = 5;
    static constexpr int RIDDELE_SUCSESS_SCORE = 30;
    static constexpr int RIDDLE_FAIL_PENALTY = 10;
    static constexpr int STAR_POINTS = 20;
    static constexpr char STAR_CHAR = '^';
    static constexpr int LOOP_DELAY_MS = 50;
    static constexpr int PLAYER1_ID = 1;
    static constexpr int PLAYER2_ID = 2;

    void addTeamScore(int points);
    void removeTeamScore(int points);
    void applyFastExitBonus();
    void applyOngoingTimeLoss();
    void tryCollectStar(Player& p);

    void writeLegendLine(screen& screen, int baseY, const std::string& text); 
    static void writeTextAtColumn(std::string& legendLine, int startColumn, const std::string& textToWrite);

public:
    GameManager();
	bool init(std::vector<std::string>& errors);
	void start();
    void resetGameState();
    void run();
    void showMenuAndHandleInput();
    void displayMenu() const;
    void displayPauseScreen() const;
    void displayInstructions() const;
    void displayGameOver(const Player& p1, const Player& p2) const;
    void changeScreen(int newScreenID, const Point& destinationPos, Player& p1, Player& p2);
    void handleRiddleSolving(Player* player, screen& currentScreen);
    void displayingPlayerStatus(Player& p1, Player& p2, screen& currentScreen);

    screen& getCurrentScreen() { return screens[currentScreenID]; }
    int getCurrentScreenID() const { return currentScreenID; }

    void handleRiddleCheck(Player* player, screen& currentScreen);
    void checkAndHandleTorchUpdate(Player& p1, Player& p2, bool& lastTorchState);
    bool handleScreenTransition(Player& p1, Player& p2, bool& lastTorchState);
	bool handlePauseInput();
	bool isOtherPlayerAt(const Point& pos, Player* callingPlayer);
	void transferLaunch(Player* jumpingPlayer, const Point& impactPos);
	bool canObstacleMove(Obstacle* obs, Direction dir, Player* pushingPlayer);
	int calculateCombinedForce(Obstacle* obs, Direction pushDir);
	bool isPlayerPushingObstacle(Player* player, Obstacle* obs, Direction pushDir);
};


