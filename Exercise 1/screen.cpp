#include "screen.h"
#include "Doors.h"
#include "point.h"

namespace {
char switchStateToChar(SwitchState state) {
    return state == SwitchState::ON ? '\\' : '/';
}

SwitchState flippedState(SwitchState state) {
    return state == SwitchState::ON ? SwitchState::OFF : SwitchState::ON;
}
}



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
    doors.clear();
    switchBoard.clear();
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
        registerSwitch(0, Point(5, 10, 0, 0, '/'), SwitchState::OFF);
        registerSwitch(1, Point(20, 10, 0, 0, '/'), SwitchState::OFF);
        std::map<int, SwitchState> doorSwitchReq = {
            {0, SwitchState::ON},
            {1, SwitchState::ON}
        };
        doors.push_back(Doors(1, 1, Point(10, 14, 0, 0, ' '), false, false, {}, doorSwitchReq));
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
        registerSwitch(2, Point(30, 12, 0, 0, '/'), SwitchState::OFF);
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

SwitchBoard::SwitchPad* screen::getSwitchAt(const Point& pos) {
    return switchBoard.getSwitchAt(pos);
}

SwitchState screen::getSwitchState(int id) const {
    return switchBoard.getState(id);
}

void screen::setSwitchState(int id, SwitchState state) {
    if (!switchBoard.setState(id, state)) {
        return;
    }
    if (auto* sw = switchBoard.getSwitchById(id)) {
        setCharAt(sw->position, switchStateToChar(state));
    }
}

void screen::toggleSwitchAt(const Point& pos) {
    SwitchState newState;
    if (switchBoard.toggleAt(pos, newState)) {
        setCharAt(pos, switchStateToChar(newState));
    }
}

void screen::registerSwitch(int id, const Point& pos, SwitchState initialState) {
    switchBoard.registerSwitch(id, pos, initialState);
    setCharAt(pos, switchStateToChar(initialState));
}

