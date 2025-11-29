#include <iostream>
#include <windows.h>
#include <conio.h>
#include "Player.h"
#include "Point.h"
#include "screen.h"
#include "utils.h"

enum Keys { ESC = 27 };

int main() {
	hideCursor();
	screen screen;
	screen.draw();
	Player players[] = {
		Player(Point(10, 10, 1, 0, '$'), "wdxase", screen),
		Player(Point(15, 5, 0, 1, '&'), "ilmjko", screen)
	};
	for (auto& p : players) {
		p.draw();
	}
	while (true) {
		for (auto& p : players) {
			p.move();
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