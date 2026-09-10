#include "Console.h"
#include <iostream>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#endif

namespace DuoEscape {

void Console::gotoXY(int x, int y) {
    std::cout.flush();
#ifdef _WIN32
    COORD coord;
    coord.X = static_cast<SHORT>(x);
    coord.Y = static_cast<SHORT>(y);
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
    std::cout << "\033[" << (y + 1) << ";" << (x + 1) << "H";
#endif
}

void Console::hideCursor() {
#ifdef _WIN32
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO curInfo;
    GetConsoleCursorInfo(hStdOut, &curInfo);
    curInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hStdOut, &curInfo);
#else
    std::cout << "\033[?25l";
#endif
}

void Console::clearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::cout << "\033[2J\033[1;1H";
#endif
}

void Console::setTextColor(Color color, bool isColorEnabled) {
#ifdef _WIN32
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (isColorEnabled) {
        SetConsoleTextAttribute(hStdOut, static_cast<WORD>(color));
    } else {
        SetConsoleTextAttribute(hStdOut, static_cast<WORD>(Color::DEFAULT));
    }
#else
    if (!isColorEnabled) {
        std::cout << "\033[0m";
        return;
    }
    switch (color) {
        case Color::GREY:    std::cout << "\033[90m"; break;
        case Color::BLUE:    std::cout << "\033[94m"; break;
        case Color::GREEN:   std::cout << "\033[92m"; break;
        case Color::CYAN:    std::cout << "\033[96m"; break;
        case Color::RED:     std::cout << "\033[91m"; break;
        case Color::MAGENTA: std::cout << "\033[95m"; break;
        case Color::YELLOW:  std::cout << "\033[93m"; break;
        default:             std::cout << "\033[0m";  break;
    }
#endif
}

} // namespace DuoEscape
