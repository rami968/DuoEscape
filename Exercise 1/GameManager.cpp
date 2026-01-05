#include "GameManager.h"
#include <iostream>
#define NOMINMAX
#include <windows.h>
#include <conio.h>
#include <algorithm>
#include <cstdlib>
#include "Player.h"
#include "Point.h"
#include "screen.h"
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
	if (newScreenID >= 0 && newScreenID < screens.size()) {
		currentScreenID = newScreenID;
		screen& currentScreen = getCurrentScreen();

		p1.setScreen(&currentScreen); 
		p2.setScreen(&currentScreen);
		p1.resetTransitionSignal();
		p2.resetTransitionSignal();

		isFinalScreen = (currentScreenID == screens.size() - 1);
		if (isFinalScreen) {
			currentScreen.initScreenData(currentScreenID); // Re-initialize final screen for presentation
			currentScreen.draw();
			_getch();
			return;
		}
		currentScreen.setTorchLit(p1.hasTorch() || p2.hasTorch()); // Update lighting based on held elements
		p1.setPosition(destinationPos);
		p2.setPosition(destinationPos);

		currentScreen.draw();
		p1.draw();
		p2.draw();
	}
}

void GameManager::resetGameState() {
	// Re-initialize all screens to their original state
	for (int i = 0; i < screens.size(); ++i) {
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
	player1.setPosition(Point(1, 19, 0, 0, '$'));
	player2.setPosition(Point(1, 23, 0, 0, '&'));
	// Re-assign players to the first screen instance
	player1.setScreen(&screens[0]);
	player2.setScreen(&screens[0]);
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
			// Correct answer: clear riddle and resume
			std::cout << "Correct! You have solved the riddle." << std::endl;
			currentScreen.setCharAt(riddle->getPosition(), ' ');
			player->resetActiveRiddle();
			player->setPosition(riddle->getPosition());
			player->setDirection(Direction::STAY);
			std::cout << "Press any key to continue...";
			_getch();
			currentScreen.draw();
			player->draw();
		}
		else {
			// Incorrect answer: resume game
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

void GameManager::displayingPlayerStatus(Player& p1, Player& p2, screen& currentScreen) { // Update and display player status area
	int screenIdToDisplay = currentScreenID;
	if (p1.isAwaitingTransition() && p2.isAwaitingTransition()) {
		screenIdToDisplay = currentScreenID + 1; // Show target screen ID during transition
	}
	// Update Current Room display for P1 and P2
	currentScreen.setCharAt(Point(17, 3, 0, 0, ' '), '0' + screenIdToDisplay);
	currentScreen.setCharAt(Point(55, 3, 0, 0, ' '), '0' + screenIdToDisplay);

	// Update Inventory display for P1 and P2
	if (p1.hasElement()) {
		char heldElement = p1.getHeldElement();
		currentScreen.setCharAt(Point(14, 2, 0, 0, ' '), heldElement);
	}
	else {
		currentScreen.setCharAt(Point(14, 2, 0, 0, ' '), ' ');
	}
	if (p2.hasElement()) {
		char heldElement = p2.getHeldElement();
		currentScreen.setCharAt(Point(53, 2, 0, 0, ' '), heldElement);
	}
	else {
		currentScreen.setCharAt(Point(53, 2, 0, 0, ' '), ' ');
	}

	// Redraw top status area
	for (int y = 0; y <= 4; ++y) {
		gotoxy(0, y);
		for (int x = 0; x < screen::MAX_X; ++x) {
			std::cout << currentScreen.getCharAt(Point(x, y, 0, 0, ' '));
		}
	}
	std::cout.flush();
	gotoxy(player1.getPosition().getX(), player1.getPosition().getY()); // Reset cursor position
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
	// Main game loop
	hideCursor();
	Player& p1 = player1;
	Player& p2 = player2;
	Player* players[] = { &p1, &p2 };

	// Initial setup of the first screen
	bool lastTorchState = player1.hasTorch() || player2.hasTorch();
	displayingPlayerStatus(p1, p2, getCurrentScreen());
	screens[currentScreenID].setTorchLit(lastTorchState);
	screens[currentScreenID].draw();

	for (auto p : players) {
		p->draw();   // Initial player draw
	}
	bool gameOver = false;
	while (!gameOver) {
		//Movement and Interaction Phase
		for (auto p : players) {
			p->move(); // Move, check collisions, and signal screen transitions
		}
		//Display Status and Draw
		displayingPlayerStatus(p1, p2, getCurrentScreen());
		for (auto p : players) {
			p->draw();
		}
		//Post-Movement/Riddle Phase
		for (auto p : players) {
			handleRiddleCheck(p, getCurrentScreen()); // Check if player is on a riddle
		}
		//State Updates
		checkAndHandleTorchUpdate(p1, p2, lastTorchState);
		//Screen Transition Check
		gameOver = handleScreenTransition(p1, p2, lastTorchState);
		if (gameOver) {
			break;
		}
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
		Sleep(50); // Control game speed
	}
	cls();
}