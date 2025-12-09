#include "screen.h"
#include "Doors.h"
#include "point.h"
#include <algorithm>

namespace {
char switchStateToChar(SwitchState state) {
    return state == SwitchState::ON ? '\\' : '/';
}

SwitchState flippedState(SwitchState state) {
    return state == SwitchState::ON ? SwitchState::OFF : SwitchState::ON;
}
}


void screen::clearDarkMask() {
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            darkMask[y][x] = false;
        }
    }
}

void screen::draw() const {
    cls();
    gotoxy(0, 0);
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            char ch = mapData[y][x];
            if (darkMask[y][x] && !torchLit) {
                cout << ' ';
            }
            else {
                cout << ch;
            }
        }
        if (y < MAX_Y - 1) {
            cout << endl;
        }
    }
    cout.flush();
}

void screen::initScreenData(int id) {
    doors.clear();
    switchBoard.clear();
    clearDarkMask();
    torchLit = false;
    currentScreenID = id;
    if (currentScreenID == 0) {
        char Screen1[MAX_Y][MAX_X + 1] = {
       //01234567890123456789012345678901234567890123456789012345678901234567890123456789
        "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
        "W              Player1:                W                Player2:               W", // 1
        "W Inventory:                           W Inventory:                            W", // 2
        "W Current Room:                        W Current Room:                         W", // 3
        "W                                      W                                       W", // 4
        "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 5
        "W                 W                                                            W", // 6
        "W                 W                                                            W", // 7
        "W                 W                                                            W", // 8
        "W                 W                                                            W", // 9
        "W                                                                              W", // 10
        "W                                                                              W", // 11
        "W                 W                                                            W", // 12
        "W                 W                                                           KW", // 13
        "W                 W                                                            3", // 14
        "WWWWWWWWWWWW      WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 15
        "W                                                                              W", // 16
        "W                                                                              W", // 17
        "W                                                                              W", // 18
        "W                              K                                               W", // 19
        "WWWWWWWWWWW WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW W", // 20
        "W            W                             W                                   W", // 21
        "W            W             W               W                                   W", // 22
        "W            ?             W                                                   W", // 23
        "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"  // 24
        };
        for (int i = 0; i < MAX_Y; ++i) {
            strcpy_s(mapData[i], MAX_X + 1, Screen1[i]);
        }
        registerSwitch(0, Point(5, 10, 0, 0, '/'), SwitchState::OFF);
        registerSwitch(1, Point(20, 10, 0, 0, '/'), SwitchState::OFF);
        riddles.clear();
		riddles.push_back(Riddle(0, Point(13, 23, 0, 0, '?'), "What has keys but can't open locks?", "Keyboard"));
        const Doors::SwitchRequirement doorSwitchReq[] = {
            {0, SwitchState::ON},
            {1, SwitchState::ON}
        };
        const size_t doorSwitchReqCount = sizeof(doorSwitchReq) / sizeof(doorSwitchReq[0]);
		std::vector<Point> keyPositions = { Point(31, 19, 0, 0, 'K'), Point(79, 13, 0, 0, 'K') };
        doors.emplace_back(3, 1, Point(10, 14, 0, 0, ' '), false, false,
            keyPositions , 2,
            doorSwitchReq, doorSwitchReqCount);
        setCharAt(Point(25, 11, 0, 0, ' '), '@');
        setCharAt(Point(55, 18, 0, 0, ' '), '@');
        setCharAt(Point(14, 12, 0, 0, ' '), '!');
        //for (int x = 18; x <= 26; ++x) {
            //setCharAt(Point(x, 10, 0, 0, '#'), '#');
        //}
        for (int x = 32; x <= 40; ++x) {
            setCharAt(Point(x, 19, 0, 0, '#'), '#');
        }
        //for (int y = 6; y <= 16; ++y) {
          //  setCharAt(Point(45, y, 0, 0, 'W'), 'W');
        //}
        markDarkArea(50, 8, 75, 20);
    }
    else if (currentScreenID == 1) {
        char Screen2[MAX_Y][MAX_X + 1] = {
        "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
        "W              Player1:                W                Player2:               W", // 1
        "W Inventory:                           W Inventory:                            W", // 2
        "W Current Room:                        W Current Room:                         W", // 3
        "W                                      W                                       W", // 4
        "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 5
        "W                                                                              W", // 6
        "W                                                                              W", // 7
        "W                                                                              W", // 8
        "W                                                                              W", // 9
        "W                                                                              W", // 10
        "W                                                                              W", // 11
        "W                                                                              W", // 12
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
        setCharAt(Point(40, 15, 0, 0, ' '), '@');
        setCharAt(Point(20, 8, 0, 0, ' '), '!');
        for (int x = 10; x <= 19; ++x) {
            setCharAt(Point(x, 6, 0, 0, '#'), '#');
        }
        for (int x = 25; x <= 34; ++x) {
            setCharAt(Point(x, 17, 0, 0, '#'), '#');
        }
        for (int y = 5; y <= 15; ++y) {
            setCharAt(Point(60, y, 0, 0, 'W'), 'W');
        }
        markDarkArea(15, 5, 35, 18);
    }
    else {
        char EndScreen[MAX_Y][MAX_X + 1] = {
        "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
        "W                                                                              W", // 1
        "W                                                                              W", // 2
        "W                                                                              W", // 3
        "W                                                                              W", // 4
        "W                                                                              W", // 5
        "W                                                                              W", // 6
        "W                                                                              W", // 7
        "W                                                                              W", // 8
        "W                                                                              W", // 9
        "W                                                                              W", // 10
        "W                                                                              W", // 11
        "W                                                                              W", // 12
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
            strcpy_s(mapData[i], MAX_X + 1, EndScreen[i]);
		}
		setCharAt(Point(30, 14, 0, 0, ' '), '!');
        for (int x = 35; x <= 45; ++x) {
            setCharAt(Point(x, 11, 0, 0, '#'), '#');
        }
        for (int x = 5; x <= 15; ++x) {
            setCharAt(Point(x, 20, 0, 0, '#'), '#');
        }
        for (int y = 8; y <= 18; ++y) {
            setCharAt(Point(50, y, 0, 0, 'W'), 'W');
        }
		markDarkArea(25, 10, 55, 20);
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

Riddle* screen::getRiddleByPosition(const Point& p) {
	for (auto& riddle : riddles) {
		if (riddle.getPosition().getX() == p.getX() &&
			riddle.getPosition().getY() == p.getY()) {
			return &riddle;
		}
	}
	return nullptr;
}

void screen:: setCharAt(const Point& pos, char ch)
{
    mapData[pos.getY()][pos.getX()] = ch;
}

SwitchBoard::SwitchEntry* screen::getSwitchAt(const Point& pos) {
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
        setCharAt(sw->location, switchStateToChar(state));
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

void screen::markDarkArea(int x1, int y1, int x2, int y2) {
    int left = std::max(0, std::min(x1, x2));
    int right = std::min(MAX_X - 1, std::max(x1, x2));
    int top = std::max(0, std::min(y1, y2));
    int bottom = std::min(MAX_Y - 1, std::max(y1, y2));
    for (int y = top; y <= bottom; ++y) {
        for (int x = left; x <= right; ++x) {
            darkMask[y][x] = true;
        }
    }
}

void screen::setTorchLit(bool lit) {
    torchLit = lit;
}


