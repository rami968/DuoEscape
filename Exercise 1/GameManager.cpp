#include "GameManager.h"
#include <iostream>
#include <windows.h>
#include <conio.h>
#include "Player.h"
#include "Point.h"
#include "screen.h"
#include "utils.h"

enum Keys { ESC = 27 };

GameManager::GameManager() {
	screens.push_back(screen(0));
	screens.push_back(screen(1));
	screens.push_back(screen(2));
}


void GameManager::changeScreen(int newScreenID, const Point& destinationPos, Player p1, Player p2) {
	if (newScreenID >= 0 && newScreenID < screens.size()) {
		currentScreenID = newScreenID;
		getCurrentScreen().initScreenData(currentScreenID);
		getCurrentScreen().draw();
		p1.setPosition(destinationPos);
		p2.setPosition(destinationPos);
		p1.draw();
		p2.draw();
	}
}
void GameManager::run() {
	// Main game loop would go here
	hideCursor();
	screen screen;
	screen.initScreenData(0);
	screen.draw();
	Player player1 = Player(Point(10, 10, 1, 0, '$'), "wdxase", screens[0]);
	Player player2 = Player(Point(15, 5, 0, 1, '&'), "ilmjko", screens[0]);
	Player players[] = { player1, player2 };
	bool p1_has_exited = false;
	bool p2_has_exited = false;
	Doors* p1_exit_door = nullptr;
	Doors* p2_exit_door = nullptr;
	for (auto& p : players) {
		p.draw();
	}
	while (true) {
		for (auto& p : players) {
			p.move();
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

			// איפוס הסטטוסים לחדר החדש
			p1_has_exited = false;
			p2_has_exited = false;
			player1.resetTransitionSignal();
			player2.resetTransitionSignal();
			p1_exit_door = nullptr;
			p2_exit_door = nullptr;
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
				for (auto& p : players) {
					p.handleKeyPressed(key);
				}
			}
		}
		Sleep(50);
	}
	cls();
}


