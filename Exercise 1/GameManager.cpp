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
			player->setPosition(riddle->getPosition()); // Move player to riddle position
			player->setDirection(Direction::STAY);
			std::cout << "Press any key to continue...";
			_getch();
			currentScreen.draw();
			player->draw();
		}
		else {
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

void GameManager::displayingPlayerStatus(Player& p1, Player& p2, screen& currentScreen) {
	int screenIdToDisplay = currentScreenID; 
	if (p1.isAwaitingTransition() || p2.isAwaitingTransition()) {
		screenIdToDisplay = currentScreenID + 1;
	}
	currentScreen.setCharAt(Point(17, 3, 0, 0, ' '), '0' + screenIdToDisplay);
	currentScreen.setCharAt(Point(55, 3, 0, 0, ' '), '0' + screenIdToDisplay);

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
}

void GameManager::run() {
	// Main game loop would go here
	hideCursor();
	screens[0].initScreenData(0);
	screens[0].draw();
	Player& p1 = player1;
	Player& p2 = player2;
	Player* players[] = {&p1, &p2};
	BombHelper::clear();
	bool lastTorchState = player1.hasTorch() || player2.hasTorch();
	screens[currentScreenID].setTorchLit(lastTorchState);
	screens[currentScreenID].draw();
	displayingPlayerStatus(p1, p2, getCurrentScreen());
	for (auto p : players) {
		p->draw();
	}
	bool gameOver = false;
	while (!gameOver) {
		for (auto p : players) {
			p->move();
		}
		displayingPlayerStatus(p1, p2, getCurrentScreen());
		for (auto p : players) {
			if (p->getActiveRiddle() != nullptr) {
				handleRiddleSolving(p, getCurrentScreen());
			}
		}
		Point pendingBomb;
		while (BombHelper::tryPopNext(pendingBomb)) {
			queueBombAt(pendingBomb);
		}
		bool torchActive = player1.hasTorch() || player2.hasTorch();
		if (torchActive != lastTorchState) {
			screens[currentScreenID].setTorchLit(torchActive);
			screens[currentScreenID].draw();
			for (auto p : players) {
				p->draw();
			}
			lastTorchState = torchActive;
		}
		Doors* p1_door_signal = player1.getTransitionDoor();
		Doors* p2_door_signal = player2.getTransitionDoor();
		//if (p1_door_signal != nullptr && p2_door_signal != nullptr) {
			//changeScreen(p2_door_signal->getDestinationScreenID(), p2_door_signal->getDestinationPosition(), player1, player2);
			//player1.resetTransitionSignal();
			//player2.resetTransitionSignal();
		if (p1_door_signal && !p1_has_exited) {
			p1_has_exited = true; p1_exit_door = p1_door_signal;
		}
		if (p2_door_signal && !p2_has_exited) {
			p2_has_exited = true; p2_exit_door = p2_door_signal;
		}

		// 4. בדיקת מעבר מסך (כלל השחקן השני)
		if (p1_has_exited && p2_has_exited) {

			// המשיכו לפי הדרישה: המשחק ממשיך עם השחקן השני שעזב.
			// אם שניהם עזבו, נלך לפי הסיגנל האחרון שהגיע (p2_exit_door).
			Doors* final_door = p2_exit_door;

			// בצע מעבר מסך
			changeScreen(final_door->getDestinationScreenID(), final_door->getDestinationPosition(), p1, p2);
			lastTorchState = player1.hasTorch() || player2.hasTorch();
			// איפוס הסטטוסים לחדר החדש
			p1_has_exited = false;
			p2_has_exited = false;
			p1.resetTransitionSignal();
			p2.resetTransitionSignal();
			p1_exit_door = nullptr;
			p2_exit_door = nullptr;
		}
		if (processBombs(player1, player2)) {
			gotoxy(0, screen::MAX_Y);
			gameOver = true;
			continue;
		}
		if (_kbhit()) {
			char key = _getch();
			if (key == Keys::ESC) {
				displayPauseScreen();
				key = _getch();
				if (key == 'H' || key == 'h') {
					break;
				}
				else if (key == Keys::ESC) {
					getCurrentScreen().draw();
					for (auto p : players) {
						p->draw();
					}
				}
				else {
					while (1) {
						key = _getch();
						if (key == Keys::ESC) {
							getCurrentScreen().draw();
							for (auto p : players) {
								p->draw();
							}
							break;
						}
						else if (key == 'H' || key == 'h') {
							return;
						}
					}
				}
			}
			else {
				for (auto p : players) {
					p->handleKeyPressed(key);
				}
			}
		}
		Sleep(50);
	}
	cls();
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
			if (current.getCharAt(target) != ' ') {
				current.setCharAt(target, ' ');
			}
		}
	}
	return playerHit;
}



