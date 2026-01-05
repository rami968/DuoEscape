#include "GameManager.h"
#include <iostream>
#include <sstream>
#define NOMINMAX
#include <windows.h>
#include <conio.h>
#include <algorithm>
#include <cstdlib>
#include "Player.h"
#include "Point.h"
#include "screen.h"
#include "Bomb.h"
#include "Doors.h"
#include "utils.h"
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

enum Keys { ESC = 27 };

GameManager::GameManager() :  
	// Initialize players with nullptr screen initially
	player1(Point(1, 19, 0, 0, '$'), "wdxase", nullptr, this),  
	player2(Point(1, 23, 0, 0, '&'), "ilmjko", nullptr, this)
{
    currentScreenID = 0;
}

bool GameManager::init(std::vector<std::string>& errors) {
	// 1. Load Riddles
	if (!screen::loadRiddlesFromFile("riddles.txt", errors)) {
		return false; // Critical failure
	}

	// 2. Find all screen files
    std::vector<std::string> screenFiles;
    for (const auto& entry : fs::directory_iterator(".")) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (filename.rfind("adv-world", 0) == 0 && filename.find(".screen") != std::string::npos) {
                screenFiles.push_back(filename);
            }
        }
    }
    std::sort(screenFiles.begin(), screenFiles.end());

	if (screenFiles.empty()) {
		errors.push_back("Critical Error: No 'adv-world*.screen' files found in working directory.");
		return false;
	}

    // 3. Initialize screens
    for (int i = 0; i < screenFiles.size(); ++i) {
        screens.emplace_back(i, screenFiles[i]);
		// Collect errors from this screen load
		const auto& screenErrors = screens.back().getErrors();
		errors.insert(errors.end(), screenErrors.begin(), screenErrors.end());
    }
    
    // 4. Set players to first screen if available
    if (!screens.empty()) {
        player1.setScreen(&screens[0]);
        player2.setScreen(&screens[0]);
    }
    
	currentScreenID = 0;
	return true;
}

void GameManager::start() {
	std::vector<std::string> errors;
	if (!init(errors)) {
		std::cerr << "Initialization failed:" << std::endl;
		for (const auto& err : errors) {
			std::cerr << err << std::endl;
		}
		std::cout << "Press any key to exit..." << std::endl;
		std::cin.get();
		return;
	}

	// Print non-critical warnings
	if (!errors.empty()) {
		std::cerr << "Warnings:" << std::endl;
		for (const auto& err : errors) {
			std::cerr << err << std::endl;
		}
		std::cout << "Press Enter to continue..." << std::endl;
		std::cin.get();
	}

	showMenuAndHandleInput();
}

void GameManager::changeScreen(int newScreenID, const Point& destinationPos, Player& p1, Player& p2) { // Change active screen and update player positions
    applyFastExitBonus();
    if (newScreenID >= 0 && newScreenID < (int)screens.size()) {
        currentScreenID = newScreenID;
        screen& currentScreen = getCurrentScreen();

        p1.setScreen(&currentScreen);
        p2.setScreen(&currentScreen);
        p1.resetTransitionSignal();
        p2.resetTransitionSignal();

        isFinalScreen = (currentScreenID == (int)screens.size() - 1);
        if (isFinalScreen) {
            currentScreen.initScreenData(currentScreenID); // Re-initialize final screen for presentation
            currentScreen.draw();
            _getch();
            return;
        }
        currentScreen.setTorchLit(p1.hasTorch() || p2.hasTorch()); // Update lighting based on held elements
        p1.setPosition(destinationPos);
        p2.setPosition(destinationPos);
        screenTicks = 0;
        lastTimeLossStepApplied = 0;
        currentScreen.draw();
        p1.draw();
        p2.draw();
    }
}

void GameManager::resetGameState() {
    // Re-initialize all screens to their original state
    for (int i = 0; i < (int)screens.size(); ++i) {
        screens[i].initScreenData(i);
    }
    currentScreenID = 0;
    p1_has_exited = false;
    p2_has_exited = false;
    p1_exit_door = nullptr;
    p2_exit_door = nullptr;
    isFinalScreen = false;
    player1.resetHeldElement();
    player2.resetHeldElement();
    activeBombs.clear();
    player1.resetLives();
    player2.resetLives();
    player1.setPosition(Point(1, 19, 0, 0, '$'));
    player2.setPosition(Point(1, 23, 0, 0, '&'));
    // Re-assign players to the first screen instance
    player1.setScreen(&screens[0]);
    player2.setScreen(&screens[0]);
    teamScore = 0;
    screenTicks = 0;
    lastTimeLossStepApplied = 0;
}

void GameManager::displayMenu() const {
    cls();
    std::cout << "================================================================================" << std::endl;
    std::cout << "                                TEXT ADVENTURE WORLD           " << std::endl;
    std::cout << "================================================================================" << std::endl;
    std::cout << "                              (1) Start a new game" << std::endl;
    std::cout << "                              (8) Present instructions and keys" << std::endl;
    std::cout << "                              (9) EXIT" << std::endl;
    std::cout << "================================================================================" << std::endl;
    std::cout << "                                  Enter your choice: ";
}

void GameManager::displayInstructions() const {  // Display game instructions
    cls();
    std::cout << "================ INSTRUCTIONS ==================================================" << std::endl;
    std::cout << " Player 1 controls: " << std::endl;
    std::cout << "   Move Up: W" << std::endl;
    std::cout << "   Move Down: X" << std::endl;
    std::cout << "   Move Left: A" << std::endl;
    std::cout << "   Move Right: D" << std::endl;
    std::cout << "   Stay: S" << std::endl;
    std::cout << "   Dispose Element: E" << std::endl;
    std::cout << " Player 2 controls: " << std::endl;
    std::cout << "   Move Up: I" << std::endl;
    std::cout << "   Move Down: M" << std::endl;
    std::cout << "   Move Left: J" << std::endl;
    std::cout << "   Move Right: L" << std::endl;
    std::cout << "   Stay: K" << std::endl;
    std::cout << "   Dispose Element: E" << std::endl;
    std::cout << " Other controls:" << std::endl;
    std::cout << "   Pause the game: Esc" << std::endl;
    std::cout << " Press any key to return to the menu..." << std::endl;
    _getch();
}

void GameManager::displayPauseScreen() const { // Display pause screen message
    cls();
    std::cout << "===================================== PAUSED ===================================" << std::endl;
    std::cout << "Game paused, press ESC again to continue or H to go back to the main menu." << std::endl;
}

void GameManager::displayGameOver(const Player& p1, const Player& p2) const {
    cls();

    std::cout << "================================================================================" << std::endl;
    std::cout << "                                   GAME OVER                                    " << std::endl;
    std::cout << "================================================================================" << std::endl;
    std::cout << std::endl;

    bool p1Dead = p1.isDead();
    bool p2Dead = p2.isDead();

    if (p1Dead && p2Dead) {
        std::cout << "                             Both players died!" << std::endl;
    }
    else if (p1Dead) {
        std::cout << "                                 Player 1 died." << std::endl;
    }
    else if (p2Dead) {
        std::cout << "                                 Player 2 died." << std::endl;
    }
    else {
        std::cout << "                                 Game ended." << std::endl;
    }

    std::cout << std::endl;
    std::cout << "================================================================================" << std::endl;
    std::cout << "                         Press any key to return to the menu..." << std::endl;
    _getch();
}

void GameManager::showMenuAndHandleInput() { // Main menu loop
    char choice;
    bool running = true;

    while (running) {
        displayMenu();
        if (_kbhit()) {
            choice = static_cast<char>(_getch());
        }
        else {
            std::cin >> choice;
        }
        switch (choice) {
        case '1':
            resetGameState(); // Reset all game state for new run
            run();
            break;
        case '8':
            displayInstructions();
            break;
        case '9':
            running = false;
            break;
        default:
            std::cout << "\n Invalid choice. Please try again (press any key to continue)...";
            _getch();
            break;
        }
    }
}

void GameManager::handleRiddleCheck(Player* player, screen& currentScreen) { // Check if player has an active riddle
    if (player->getActiveRiddle() != nullptr) {
        handleRiddleSolving(player, currentScreen);
    }
}

void GameManager::handleRiddleSolving(Player* player, screen& currentScreen) { // Handle riddle solving interaction
    Riddle* riddle = player->getActiveRiddle();
    if (riddle != nullptr) {
        cls();
        std::cout << "Riddle: " << riddle->getQuestion() << std::endl;
        std::cout << "Your answer: ";
        std::string answer;
        std::cin >> answer;
        if (riddle->checkAnswer(answer)) {
            // Correct answer: apply positive score change for successful riddle solving
            addTeamScore(30);
            std::cout << "Correct! You have solved the riddle." << std::endl;
            currentScreen.setCharAt(riddle->getPosition(), ' ');
            player->resetActiveRiddle();
            player->setPosition(riddle->getPosition());
            player->setDirection(Direction::STAY);
            std::cout << "Press any key to continue...";
            _getch();
            displayingPlayerStatus(player1, player2, currentScreen);
            currentScreen.draw();
            player1.draw();
            player2.draw();
        }
        else {
            // Incorrect answer: apply negative score change for failed riddle attempt
            removeTeamScore(10);
            std::cout << "Incorrect answer. Try again later." << std::endl;
            std::cout << "Press any key to continue...";
            _getch();
            currentScreen.draw();
            player->setDirection(Direction::STAY);
            player->draw();
            player->resetActiveRiddle();
        }
    }
}

void GameManager::checkAndHandleTorchUpdate(Player& p1, Player& p2, bool& lastTorchState) { // Update lighting if torch state changes
    bool torchActive = p1.hasTorch() || p2.hasTorch();
    if (torchActive != lastTorchState) { // Check if lighting state needs updating
        screens[currentScreenID].setTorchLit(torchActive);
        screens[currentScreenID].draw();

        Player* players[] = { &p1, &p2 };
        for (auto p : players) {
            p->draw(); // Redraw players over the map change
        }
        lastTorchState = torchActive;
    }
}

bool GameManager::handleScreenTransition(Player& p1, Player& p2, bool& lastTorchState) { // Manage screen transitions
    // Check if both players are signaling transition
    Doors* p1_door_signal = p1.getTransitionDoor();
    Doors* p2_door_signal = p2.getTransitionDoor();

    if (p1_door_signal && !p1_has_exited) {
        p1_has_exited = true; p1_exit_door = p1_door_signal;
    }
    if (p2_door_signal && !p2_has_exited) {
        p2_has_exited = true; p2_exit_door = p2_door_signal;
    }
    if (p1_has_exited && p2_has_exited) {
        // Both players are ready to transition
        Doors* final_door = p2_exit_door;
        changeScreen(final_door->getDestinationScreenID(), final_door->getDestinationPosition(), p1, p2);
        // Check for game over condition
        if (isFinalScreen) {
            return true; // Game Over
        }
        // Reset transition state for the next screen
        lastTorchState = p1.hasTorch() || p2.hasTorch();
        p1_has_exited = false;
        p2_has_exited = false;
        p1.resetTransitionSignal();
        p2.resetTransitionSignal();
        p1_exit_door = nullptr;
        p2_exit_door = nullptr;
    }
    return false;
}

void GameManager::displayingPlayerStatus(Player& p1, Player& p2, screen& currentScreen) {
    if (!currentScreen.hasLegendArea()) {
        return;
    }

    const int baseY = currentScreen.getLegendY();

    if (baseY < 0 || baseY + screen::LEGEND_TOTAL_H > screen::MAX_Y) {
        return;
    }

    const int topY = baseY;
    const int bottomY = baseY + 4;

    const bool drawTopBorder = (topY > 1);

    const bool drawBottomBorder = (bottomY < screen::MAX_Y - 2);

    if (drawTopBorder) {
        for (int x = 0; x < screen::MAX_X; ++x) {
            currentScreen.setCharAt(Point(x, topY, 0, 0, ' '), 'W');
        }
    }
    if (drawBottomBorder) {
        for (int x = 0; x < screen::MAX_X; ++x) {
            currentScreen.setCharAt(Point(x, bottomY, 0, 0, ' '), 'W');
        }
    }

    for (int y = topY; y <= bottomY; ++y) {
        currentScreen.setCharAt(Point(0, y, 0, 0, ' '), 'W');
        currentScreen.setCharAt(Point(screen::MAX_X - 1, y, 0, 0, ' '), 'W');
    }

    int clearStartY = (topY == 0) ? topY + 1 : (drawTopBorder ? topY + 1 : topY);
    int clearEndY = (bottomY == screen::MAX_Y - 1) ? bottomY - 1 : (drawBottomBorder ? bottomY - 1 : bottomY);

    if (clearStartY <= clearEndY) {
        for (int y = clearStartY; y <= clearEndY; ++y) {
            for (int x = 1; x <= screen::MAX_X - 2; ++x) {
                currentScreen.setCharAt(Point(x, y, 0, 0, ' '), ' ');
            }
        }
    }

    int roomIdToDisplay = currentScreenID;
    if (p1.isAwaitingTransition() && p2.isAwaitingTransition()) {
        roomIdToDisplay = currentScreenID + 1;
    }

    char p1Held = (p1.hasElement() ? p1.getHeldElement() : '-');
    char p2Held = (p2.hasElement() ? p2.getHeldElement() : '-');

    int p1BombTimer = -1;
    int p2BombTimer = -1;

    for (const auto& bomb : activeBombs) {
        int t = bomb.getTicksRemaining();
        if (t < 0) t = 0;

        if (bomb.getBombOwner() == 1) {
            if (p1BombTimer == -1 || t < p1BombTimer) p1BombTimer = t;
        }
        else if (bomb.getBombOwner() == 2) {
            if (p2BombTimer == -1 || t < p2BombTimer) p2BombTimer = t;
        }
    }

    int pointsTeam = teamScore;
    const int LEGEND_LINE_WIDTH = 78;
    const int COL_PLAYER1_LIVES = 1;
    const int COL_CURRENT_ROOM = 32;
    const int COL_PLAYER2_LIVES = 52;
    const int COL_P1_INVENTORY_TIMER = 10;
    const int COL_POINTS_TEXT = 35;
    const int COL_P2_INVENTORY_TIMER = 61;

    const int firstTextY = (topY == 1 && !drawTopBorder) ? baseY : baseY + 1;

    std::string line0(LEGEND_LINE_WIDTH, ' ');
    writeTextAtColumn(line0, COL_PLAYER1_LIVES, "Player1: Lives:" + std::to_string(p1.getLives()));
    writeTextAtColumn(line0, COL_CURRENT_ROOM, "Current Room:" + std::to_string(roomIdToDisplay));
    writeTextAtColumn(line0, COL_PLAYER2_LIVES, "Player2: Lives:" + std::to_string(p2.getLives()));
    writeLegendLine(currentScreen, firstTextY, line0);

    std::string line1(LEGEND_LINE_WIDTH, ' ');
    writeTextAtColumn(line1, COL_P1_INVENTORY_TIMER, "Inventory: " + std::string(1, p1Held));
    writeTextAtColumn(line1, COL_P2_INVENTORY_TIMER, "Inventory: " + std::string(1, p2Held));
    writeLegendLine(currentScreen, firstTextY + 1, line1);

    std::string line2(LEGEND_LINE_WIDTH, ' ');
    writeTextAtColumn(line2, COL_P1_INVENTORY_TIMER,
        "Timer Bomb:" + (p1BombTimer < 0 ? std::string("-") : std::to_string(p1BombTimer)));
    writeTextAtColumn(line2, COL_POINTS_TEXT, "Points:" + std::to_string(pointsTeam));
    writeTextAtColumn(line2, COL_P2_INVENTORY_TIMER,
        "Timer Bomb:" + (p2BombTimer < 0 ? std::string("-") : std::to_string(p2BombTimer)));
    writeLegendLine(currentScreen, firstTextY + 2, line2);

    for (int dy = 0; dy < 5; ++dy) {
        int y = baseY + dy;
        gotoxy(0, y);
        for (int x = 0; x < screen::MAX_X; ++x) {
            std::cout << currentScreen.getCharAt(Point(x, y, 0, 0, ' '));
        }
    }
    std::cout.flush();
    gotoxy(p1.getPosition().getX(), p1.getPosition().getY());
}

bool GameManager::isOtherPlayerAt(const Point& pos, Player* callingPlayer) {
	Player* other = (callingPlayer == &player1) ? &player2 : &player1;
	return (other->getPosition().getX() == pos.getX() && other->getPosition().getY() == pos.getY());
}

void GameManager::transferLaunch(Player* jumpingPlayer, const Point& impactPos) {
	Player* targetPlayer = (jumpingPlayer == &player1) ? &player2 : &player1;
	targetPlayer->receiveLaunch(
		jumpingPlayer->getActiveSpringDir(),
		jumpingPlayer->getSpringSpeed(),
		jumpingPlayer->getSpringTimer(),
		jumpingPlayer->getDirection()
	);
}

bool GameManager::canObstacleMove(Obstacle* obs, Direction dir, Player* pushingPlayer) {
	int currentForce = calculateCombinedForce(obs, dir);
	int totalWeight = obs->getRequiredForce();

	std::vector<Obstacle*> chain;
	chain.push_back(obs);

	for (size_t i = 0; i < chain.size(); ++i) {
		Obstacle* current = chain[i];
		for (const auto& part : current->getPositions()) {
			Point nextPos = part.calculateNext(dir);
			if (current->containsPoint(nextPos)) continue;

			if (getCurrentScreen().isObstacle(nextPos)) {
				Obstacle* nextObs = getCurrentScreen().getObstacleByPosition(nextPos);
				if (nextObs && std::find(chain.begin(), chain.end(), nextObs) == chain.end()) {
					chain.push_back(nextObs);
					totalWeight += nextObs->getRequiredForce();
				}
			}
		}
	}
	if (currentForce < totalWeight) return false;
	Player* other = (pushingPlayer == &player1) ? &player2 : &player1;

	for (Obstacle* o : chain) {
		if (!o->handleInteractionsWithObstacle(dir, true)) return false;
		for (const auto& part : o->getPositions()) {
			if (part.calculateNext(dir) == other->getPosition()) {
				if (!isPlayerPushingObstacle(other, o, dir)) {
					return false;
				}
			}
		}
	}
	return true;
}

int GameManager::calculateCombinedForce(Obstacle* obs, Direction pushDir) {
	int totalForce = 0;

	if (isPlayerPushingObstacle(&player1, obs, pushDir)) {
		totalForce += player1.getForce();
	}
	if (isPlayerPushingObstacle(&player2, obs, pushDir)) {
		totalForce += player2.getForce();
	}

	return totalForce;
}

bool GameManager::isPlayerPushingObstacle(Player* player, Obstacle* obs, Direction pushDir) {
	if (player->getDirection() != pushDir || pushDir == Direction::STAY) {
		return false;
	}

	Point currentPos = player->getPosition();
	Point nextPos = currentPos.calculateNext(pushDir);

	return obs->containsPoint(currentPos) || obs->containsPoint(nextPos);
}


void GameManager::run() {
    hideCursor();
    Player& p1 = player1;
    Player& p2 = player2;
    Player* players[] = { &p1, &p2 };

    bool lastTorchState = player1.hasTorch() || player2.hasTorch();
    displayingPlayerStatus(p1, p2, getCurrentScreen());
    screens[currentScreenID].setTorchLit(lastTorchState);
    screens[currentScreenID].draw();

    for (auto p : players) p->draw();
    bool gameOver = false;
    while (!gameOver) {
        screenTicks++;
        applyOngoingTimeLoss();
        for (auto p : players) p->move();

        checkAndHandleTorchUpdate(p1, p2, lastTorchState);

        processBombs(p1, p2);
        pollBombRequests(p1, p2);

        displayingPlayerStatus(p1, p2, getCurrentScreen());

        tryCollectStar(p1);
        tryCollectStar(p2);

        for (auto p : players) p->draw();

        for (auto p : players) handleRiddleCheck(p, getCurrentScreen());

        if (p1.isDead() || p2.isDead()) { gameOver = true; }
        if (handleScreenTransition(p1, p2, lastTorchState)) { gameOver = true; }
        if (gameOver) break;
if (_kbhit()) {
			char key = _getch();
			if (key == Keys::ESC) {
				// Handle pause/return to menu
				if (!handlePauseInput()) {
					break; // Break the while loop to end run()
				}
			}
			else if (key == 'r' || key == 'R') {
				resetGameState();
				// Redraw after reset
				screens[currentScreenID].draw();
				player1.draw();
				player2.draw();
				displayingPlayerStatus(player1, player2, screens[currentScreenID]);
				lastTorchState = player1.hasTorch() || player2.hasTorch(); 
			}
			else {
				// Handle movement/dispose key presses
				for (auto p : players) {
					p->handleKeyPressed(key);
				}
			}
		}
        Sleep(50);
    }
    if (player1.isDead() || player2.isDead()) {
        displayGameOver(player1, player2);
    }
	cls();
    return;
	
}

void GameManager::armBombAt(const Point& pos, int bOwner) {
    int countdown = BOMB_GAME_CYCLES * (Player::MOVE_TICK_INTERVAL + 1) + 1;

    activeBombs.emplace_back(pos, countdown, bOwner);
    getCurrentScreen().markBombAsArmedAt(pos, true);
}

bool GameManager::processBombs(Player& p1, Player& p2) {
    if (activeBombs.empty()) return false;
    bool playerHit = false;
    bool needsRedraw = false;

    std::vector<Bomb> stillActive;
    for (auto& b : activeBombs) {
        b.tick();
        if (b.isReadyToExplode()) {
            if (explodeBomb(b, p1, p2)) playerHit = true;
            needsRedraw = true;
        }
        else {
            stillActive.push_back(b);
        }
    }
    activeBombs = std::move(stillActive);
    if (needsRedraw) {
        getCurrentScreen().draw();
        p1.draw(); p2.draw();
    }
    return playerHit;
}

void GameManager::pollBombRequests(Player& p1, Player& p2) {
    Point pos;
    if (p1.tryPopBombRequest(pos)) armBombAt(pos, 1);
    if (p2.tryPopBombRequest(pos)) armBombAt(pos, 2);
}

bool GameManager::explodeBomb(const Bomb& bomb, Player& p1, Player& p2) {
    screen& currentScreen = getCurrentScreen();
    Point bombPos = bomb.getPosition();
    bool p1Hit = false, p2Hit = false;

    for (int dy = -BOMB_RADIUS; dy <= BOMB_RADIUS; ++dy) for (int dx = -BOMB_RADIUS; dx <= BOMB_RADIUS; ++dx) {
        int x = bombPos.getX() + dx;
        int y = bombPos.getY() + dy;
        if (x < 0 || x >= screen::MAX_X || y < 0 || y >= screen::MAX_Y) continue;

        if (currentScreen.isInLegendArea(x, y)) continue;

        if (p1.getPosition().getX() == x && p1.getPosition().getY() == y) p1Hit = true;
        if (p2.getPosition().getX() == x && p2.getPosition().getY() == y) p2Hit = true;

        Point target(x, y, 0, 0, ' ');
        char ch = currentScreen.getCharAt(target);
        if (ch == 'W') continue;

        currentScreen.markBombAsArmedAt(target, false);

        if (currentScreen.isDoor(target)) {
            Doors* d = currentScreen.getDoorByChar(ch);
            if (d && d->isExternalDoor()) {
                continue;
            }
        }
        int dist = std::max(std::abs(dx), std::abs(dy));
        if (ch == 'w') {
            if (dist == 1) {
                currentScreen.setCharAt(target, ' ');
            }
            continue;
        }
        currentScreen.setCharAt(target, ' ');
    }

    if (p1Hit) p1.loseLife(1);
    if (p2Hit) p2.loseLife(1);
    return (p1Hit || p2Hit);
}

bool GameManager::handlePauseInput() {
    displayPauseScreen();
    char key = _getch();

    if (key == 'H' || key == 'h') {
        // Player wants to return to the main menu
        return false; // Signal run() to stop
    }
    else if (key == Keys::ESC) {
        // Player wants to resume game
        getCurrentScreen().draw();
        Player* players[] = { &player1, &player2 };
        for (auto p : players) {
            p->draw();
        }
        return true; // Signal run() to continue
    }
    else {
        // Handle unexpected key during pause
        while (1) {
            key = _getch();
            if (key == Keys::ESC) {
                // Resume game
                getCurrentScreen().draw();
                Player* players[] = { &player1, &player2 };
                for (auto p : players) {
                    p->draw();
                }
                return true; // Signal run() to continue
            }
            else if (key == 'H' || key == 'h') {
                // Return to main menu
                return false; // Signal run() to stop
            }
        }
    }
    return true;
}

void GameManager::addTeamScore(int points) {
    teamScore += points;
}

void GameManager::removeTeamScore(int points) {
    teamScore -= points;
    if (teamScore < 0) teamScore = 0;
}

void GameManager::applyFastExitBonus() {
    if (screenTicks <= FAST_EXIT_TICKS) {
        addTeamScore(FAST_EXIT_BONUS_POINTS);
    }
}

void GameManager::applyOngoingTimeLoss() {
    if (screenTicks <= MAX_TICKS_WITHOUT_SCORE_LOSS) {
        return; 
    }
    int extraTicks = screenTicks - MAX_TICKS_WITHOUT_SCORE_LOSS;
    int stepNow = extraTicks / TICKS_PER_SCORE_LOSS;
    if (stepNow > lastTimeLossStepApplied) {
        int newSteps = stepNow - lastTimeLossStepApplied;
        removeTeamScore(newSteps * SCORE_LOSS_PER_STEP);
        lastTimeLossStepApplied = stepNow;
    }
}

void GameManager::tryCollectStar(Player& p) {
    screen& sc = getCurrentScreen();
    Point pos = p.getPosition();
    if (sc.getCharAt(pos) == STAR_CHAR) {
        addTeamScore(STAR_POINTS);
        sc.setCharAt(pos, ' '); 
    }
}

// Helper function to write a line of text in the legend area
void GameManager::writeLegendLine(screen& sc, int y, const std::string& text) {
    const int LEGEND_WIDTH = 78;
    const int baseX = 1;

    for (int i = 0; i < LEGEND_WIDTH; ++i) {
        char ch = (i < (int)text.size()) ? text[i] : ' ';
        sc.setCharAt(Point(baseX + i, y, 0, 0, ' '), ch);
    }
}

void GameManager::writeTextAtColumn(std::string& legendLine,
    int startColumn,
    const std::string& textToWrite)
{
    int legendLineLength = (int)legendLine.size();
    int textLength = (int)textToWrite.size();

    for (int textCharIndex = 0;
        textCharIndex < textLength && startColumn + textCharIndex < legendLineLength;
        ++textCharIndex)
    {
        int targetColumn = startColumn + textCharIndex;
        legendLine[targetColumn] = textToWrite[textCharIndex];
    }
}
