#include "utils.h"
#include "Console.h"

void gotoxy(int x, int y) {
    DuoEscape::Console::gotoXY(x, y);
}

void hideCursor() {
    DuoEscape::Console::hideCursor();
}

void cls() {
    DuoEscape::Console::clearScreen();
}
