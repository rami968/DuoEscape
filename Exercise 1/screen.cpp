#include "screen.h"

void screen::draw() const {
	cls();
	gotoxy(0, 0);
	for (size_t i = 0; i < MAX_Y - 1; ++i) {
		cout << s[i] << endl;
	}
	cout << s[MAX_Y - 1];
	cout.flush();
}

