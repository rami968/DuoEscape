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
#include "BombHelper.h"

enum Keys { ESC = 27 };

GameManager::GameManager() :
	screens{ screen(0), screen(1), screen(2) },
	player1(Point(1, 22, 0, 0, '$'), "wdxase", screens[0]),
	player2(Point(1, 23, 0, 0, '&'), "ilmjko", screens[0])
{
	currentScreenID = 0;
}


void GameManager::changeScreen(int newScreenID, const Point& destinationPos, Player& p1, Player& p2) {
	if (newScreenID >= 0 && newScreenID < screens.size()) {
		currentScreenID = newScreenID;
		screen& currentScreen = getCurrentScreen();
		currentScreen.initScreenData(currentScreenID);
		p1.setScreen(&currentScreen);
		p2.setScreen(&currentScreen);
		activeBombs.clear();
		BombHelper::clear();
		p1.resetTransitionSignal();
		p2.resetTransitionSignal();
		p1.setPosition(destinationPos);
		p2.setPosition(destinationPos);
		bool torchActive = p1.hasTorch() || p2.hasTorch();
		currentScreen.setTorchLit(torchActive);
		currentScreen.draw();
		p1.draw();
		p2.draw();
	}
}
void GameManager::resetGameState() {
	currentScreenID = 0;
	p1_has_exited = false;
	p2_has_exited = false;
	p1_exit_door = nullptr;
	p2_exit_door = nullptr;
	player1.setPosition(Point(1, 22, 0, 0, '$'));
	player2.setPosition(Point(1, 23, 0, 0, '&'));
}

void GameManager::displayMenu() const {
	cls();      //01234567890123456789012345678901234567890123456789012345678901234567890123456789
	std::cout << "================================================================================" << std::endl;
	std::cout << "                                 TEXT ADVENTURE WORLD           " << std::endl;
	std::cout << "================================================================================" << std::endl;
	std::cout << "                               (1) Start a new game" << std::endl;   
	std::cout << "                               (8) Present instructions and keys" << std::endl;  
	std::cout << "                               (9) EXIT" << std::endl;  
	std::cout << "================================================================================" << std::endl;
	std::cout << "                                   Enter your choice: ";
}

void GameManager::displayInstructions() const {
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

void GameManager::displayPauseScreen() const {
	cls();      //01234567890123456789012345678901234567890123456789012345678901234567890123456789
	std::cout << "===================================== PAUSED ===================================" << std::endl;
	std::cout << "Game paused, press ESC again to continue or H to go back to the main menu." << std::endl;
}

void GameManager::showMenuAndHandleInput() {
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
			resetGameState();
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

void GameManager::handleRiddleSolving(Player* player, screen& currentScreen) {
	Riddle* riddle = player->getActiveRiddle();
	if (riddle != nullptr) {
		cls();
		std::cout << "Riddle: " << riddle->getQuestion() << std::endl;
		std::cout << "Your answer: ";
		std::string answer;
		std::cin >> answer;
		if (riddle->checkAnswer(answer)) {
			std::cout << "Correct! You have solved the riddle." << std::endl;
			currentScreen.setCharAt(riddle->getPosition(), ' '); // Remove riddle from screen
			player->resetActiveRiddle();
		}
		else {
			std::cout << "Incorrect answer. Try again later." << std::endl;
		}
		std::cout << "Press any key to continue...";
		_getch();
		currentScreen.draw();
		player->draw();
	}
}

void GameManager::run() {
	Player* players[] = { &player1, &player2 };
	const size_t playerCount = sizeof(players) / sizeof(players[0]);
	bool torchState = false;

	prepareRun(players, playerCount, torchState);

	bool running = true;
	while (running) {
		movePlayers(players, playerCount);
		resolveActiveRiddles(players, playerCount);
		flushQueuedBombs();
		updateTorchLighting(torchState, players, playerCount);
		handleDoorTransitions(*players[0], *players[1], torchState);

		if (processBombs(*players[0], *players[1])) {
			gotoxy(0, screen::MAX_Y);
			break;
		}

		running = handleUserInput(players, playerCount);
		Sleep(50);
	}

	cls();
}

void GameManager::prepareRun(Player* players[], size_t playerCount, bool& torchState) {
	hideCursor();
	screen& current = getCurrentScreen();
	current.initScreenData(currentScreenID);
	BombHelper::clear();
	activeBombs.clear();
	p1_has_exited = false;
	p2_has_exited = false;
	p1_exit_door = nullptr;
	p2_exit_door = nullptr;
	torchState = false;

	for (size_t i = 0; i < playerCount; ++i) {
		players[i]->setScreen(&current);
		players[i]->resetTransitionSignal();
		if (players[i]->hasTorch()) {
			torchState = true;
		}
	}

	current.setTorchLit(torchState);
	current.draw();
	drawPlayers(players, playerCount);
}

void GameManager::movePlayers(Player* players[], size_t playerCount) {
	for (size_t i = 0; i < playerCount; ++i) {
		players[i]->move();
	}
}

void GameManager::drawPlayers(Player* players[], size_t playerCount) const {
	for (size_t i = 0; i < playerCount; ++i) {
		players[i]->draw();
	}
}

void GameManager::resolveActiveRiddles(Player* players[], size_t playerCount) {
	for (size_t i = 0; i < playerCount; ++i) {
		if (players[i]->getActiveRiddle() != nullptr) {
			handleRiddleSolving(players[i], getCurrentScreen());
		}
	}
}

void GameManager::flushQueuedBombs() {
	Point pendingBomb;
	while (BombHelper::tryPopNext(pendingBomb)) {
		queueBombAt(pendingBomb);
	}
}

void GameManager::updateTorchLighting(bool& torchState, Player* players[], size_t playerCount) {
	bool torchActive = false;
	for (size_t i = 0; i < playerCount; ++i) {
		if (players[i]->hasTorch()) {
			torchActive = true;
			break;
		}
	}
	if (torchActive == torchState) {
		return;
	}
	torchState = torchActive;
	screen& current = getCurrentScreen();
	current.setTorchLit(torchState);
	current.draw();
	drawPlayers(players, playerCount);
}

bool GameManager::handleUserInput(Player* players[], size_t playerCount) {
	if (!_kbhit()) {
		return true;
	}

	char key = _getch();
	if (key == Keys::ESC) {
		displayPauseScreen();
		while (true) {
			key = _getch();
			if (key == Keys::ESC) {
				getCurrentScreen().draw();
				drawPlayers(players, playerCount);
				return true;
			}
			if (key == 'H' || key == 'h') {
				return false;
			}
		}
	}

	for (size_t i = 0; i < playerCount; ++i) {
		players[i]->handleKeyPressed(key);
	}
	return true;
}

void GameManager::handleDoorTransitions(Player& p1, Player& p2, bool& torchState) {
	Doors* p1Door = p1.getTransitionDoor();
	Doors* p2Door = p2.getTransitionDoor();

	if (p1Door && !p1_has_exited) {
		p1_has_exited = true;
		p1_exit_door = p1Door;
	}
	if (p2Door && !p2_has_exited) {
		p2_has_exited = true;
		p2_exit_door = p2Door;
	}

	if (p1_has_exited && p2_has_exited) {
		Doors* finalDoor = p2_exit_door ? p2_exit_door : p1_exit_door;
		if (finalDoor) {
			changeScreen(finalDoor->getDestinationScreenID(), finalDoor->getDestinationPosition(), p1, p2);
			torchState = p1.hasTorch() || p2.hasTorch();
		}
		p1_has_exited = false;
		p2_has_exited = false;
		p1_exit_door = nullptr;
		p2_exit_door = nullptr;
	}
}

void GameManager::armBombAt(const Point& pos) {
	ArmedBomb bomb{ pos.getX(), pos.getY(), BOMB_COUNTDOWN };
	activeBombs.push_back(bomb);
}

void GameManager::queueBombAt(const Point& pos) {
	armBombAt(pos);
}

bool GameManager::processBombs(Player& p1, Player& p2) {
	if (activeBombs.empty()) {
		return false;
	}
	bool playerHit = false;
	bool needsRedraw = false;
	std::vector<ArmedBomb> nextBombs;
	nextBombs.reserve(activeBombs.size());
	for (auto& bomb : activeBombs) {
		ArmedBomb updated = bomb;
		--updated.ticksRemaining;
		if (updated.ticksRemaining <= 0) {
			if (explodeBomb(updated, p1, p2)) {
				playerHit = true;
			}
			needsRedraw = true;
		}
		else {
			nextBombs.push_back(updated);
		}
	}
	activeBombs.swap(nextBombs);
	if (needsRedraw) {
		screens[currentScreenID].draw();
		p1.draw();
		p2.draw();
	}
	return playerHit;
}

bool GameManager::explodeBomb(const ArmedBomb& bomb, Player& p1, Player& p2) {
	screen& current = getCurrentScreen();
	bool playerHit = false;
	for (int dy = -BOMB_RADIUS; dy <= BOMB_RADIUS; ++dy) {
		for (int dx = -BOMB_RADIUS; dx <= BOMB_RADIUS; ++dx) {
			int chebyshev = std::max(std::abs(dx), std::abs(dy));
			if (chebyshev > BOMB_RADIUS) {
				continue;
			}
			int targetX = bomb.x + dx;
			int targetY = bomb.y + dy;
			if (targetX < 0 || targetX >= screen::MAX_X || targetY < 0 || targetY >= screen::MAX_Y) {
				continue;
			}
			if ((p1.getPosition().getX() == targetX && p1.getPosition().getY() == targetY) ||
				(p2.getPosition().getX() == targetX && p2.getPosition().getY() == targetY)) {
				playerHit = true;
			}
			Point target(targetX, targetY, 0, 0, ' ');

			char ch = current.getCharAt(target);

            if (ch == 'W') {
               continue;
            }
            if (ch != ' ')
            current.setCharAt(target, ' '); 
		    }
	}
	return playerHit;
}



