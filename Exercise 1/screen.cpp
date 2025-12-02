#include "screen.h"
#include "Doors.h"
#include "point.h"



void screen::draw() const {
	cls();
	gotoxy(0, 0);
	for (size_t i = 0; i < MAX_Y - 1; ++i) {
		cout << mapData[i] << endl;
	}
	cout << mapData[MAX_Y - 1];
	cout.flush();
}

void screen::initScreenData(int id) {
    currentScreenID = id;
    if (currentScreenID == 0) {
        char Screen1[MAX_Y][MAX_X + 1] = {
            //   01234567890123456789012345678901234567890123456789012345678901234567890123456789
        "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
        "W####                                                                  #    ###W", // 1
        "W####                                                                  #    ###W", // 2
        "W####                                                                  #    ###W", // 3
        "W####                                                                  #    ###W", // 4
        "W####                                                                  #    ###W", // 5
        "W####                                                                  #    ###W", // 6
        "W####                                                                  #    ###W", // 7
        "W####               WWWWWWWWW                                               ###W", // 8
        "W####               #########                                               ###W", // 9
        "W                                                                              W", // 10
        "W                                                                              W", // 11
        "W                                                                           ###W", // 12
        "W                                                                              W", // 13
        "W                                                                              1", // 14
        "W                                                                              W", // 15
        "W                                                                              W", // 16
        "W                                                                              W", // 17
        "W                                                                              W", // 18
        "W                                                                              W", // 19
        "W                                                                              W", // 20
        "W                                                                              W", // 21
        "W                                                                              W", // 22
        "W                                                                              W", // 23
        "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"  // 24
        };
        for (int i = 0; i < MAX_Y; ++i) {
            strcpy_s(mapData[i], MAX_X + 1, Screen1[i]);
        }
		doors.push_back(Doors(1, 1, Point(10, 14, 0, 0, ' '), false, false, {}, {}));
    }
    else if (currentScreenID == 1) {
        char Screen2[MAX_Y][MAX_X + 1] = {
        "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
        "W####                                                                  #    ###W", // 1
        "W####                                                                  #    ###W", // 2
        "W####                                                                  #    ###W", // 3
        "W####                                                                  #    ###W", // 4
        "W####                                                                  #    ###W", // 5
        "W####                                                                  #    ###W", // 6
        "W####                                                                  #    ###W", // 7
        "W####               WWWWWWWWW                                               ###W", // 8
        "W####               #########                                               ###W", // 9
        "W                                                                              W", // 10
        "W                                                                              W", // 11
        "W                                                                           ###W", // 12
        "W                                                                              W", // 13
        "W                                                                              W", // 14
        "W                                                                              W", // 15
        "W                                                                              W", // 16
        "W                                                                              W", // 17
        "W                                                                              W", // 18
        "W                                                                              W", // 19
        "W                                                                              W", // 20
        "W                                                                              W", // 21
        "W                                                                              W", // 22
        "W                                                                              W", // 23
        "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"  // 24
        };
		for (int i = 0; i < MAX_Y; ++i) {
            strcpy_s(mapData[i], MAX_X + 1, Screen2[i]);
		}
    }
    else {
        char EndScreen[MAX_Y][MAX_X + 1] = {
        "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
        "W####                                                                  #    ###W", // 1
        "W####                                                                  #    ###W", // 2
        "W####                                                                  #    ###W", // 3
        "W####                                                                  #    ###W", // 4
        "W####                                                                  #    ###W", // 5
        "W####                                                                  #    ###W", // 6
        "W####                                                                  #    ###W", // 7
        "W####               WWWWWWWWW                                               ###W", // 8
        "W####               #########                                               ###W", // 9
        "W                                                                              W", // 10
        "W                                                                              W", // 11
        "W                                                                           ###W", // 12
        "W                                                                              W", // 13
        "W                             the end                                          W", // 14
        "W                                                                              W", // 15
        "W                                                                              W", // 16
        "W                                                                              W", // 17
        "W                                                                              W", // 18
        "W                                                                              W", // 19
        "W                                                                              W", // 20
        "W                                                                              W", // 21
        "W                                                                              W", // 22
        "W                                                                              W", // 23
        "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"  // 24
        };
		for (int i = 0; i < MAX_Y; ++i) {
            strcpy_s(mapData[i], MAX_X + 1, EndScreen[i]);
		}
    }
}
Doors* screen::getDoorByChar(char doorChar) {
    for (auto& door : doors) {
        if (door.getDisplayChar() == doorChar) {
            return &door;
        }
    }
    return nullptr;
}

void screen:: setCharAt(const Point& pos, char ch)
{
    mapData[pos.getY()][pos.getX()] = ch;
}

