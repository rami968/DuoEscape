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

GameManager::GameManager() {
	screens.push_back(screen(0));
	screens.push_back(screen(1));
	screens.push_back(screen(2));
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
void GameManager::run() {
	// Main game loop would go here
	hideCursor();
	screens[0].initScreenData(0);
	BombHelper::clear();
	Player player1 = Player(Point(10, 10, 1, 0, '$'), "wdxase", screens[0]);
	Player player2 = Player(Point(15, 5, 0, 1, '&'), "ilmjko", screens[0]);
	Player* players[] = { &player1, &player2 };
	bool lastTorchState = player1.hasTorch() || player2.hasTorch();
	screens[currentScreenID].setTorchLit(lastTorchState);
	screens[currentScreenID].draw();
	bool p1_has_exited = false;
	bool p2_has_exited = false;
	Doors* p1_exit_door = nullptr;
	Doors* p2_exit_door = nullptr;
	for (auto p : players) {
		p->draw();
	}
	bool gameOver = false;
	while (!gameOver) {
		for (auto p : players) {
			p->move();
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
			changeScreen(final_door->getDestinationScreenID(), final_door->getDestinationPosition(), player1, player2);
			lastTorchState = player1.hasTorch() || player2.hasTorch();

			// איפוס הסטטוסים לחדר החדש
			p1_has_exited = false;
			p2_has_exited = false;
			player1.resetTransitionSignal();
			player2.resetTransitionSignal();
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
				// Pause - till any key is pressed
				key = _getch();
				if (key == 'H' || key == 'h') {
					break;
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


