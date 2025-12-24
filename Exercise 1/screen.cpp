#include "screen.h"
#include "Doors.h"
#include "point.h"
#include <algorithm>

// Converts a switch state to the character shown on the map
char screen::switchStateToChar(SwitchState state) {
    return (state == SwitchState::ON ? '\\' : '/');
}

// Clears the darkMask so no tiles are marked as dark
void screen::clearDarkMask() {
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            darkMask[y][x] = false;
        }
    }
}

// Draws the whole screen to the console, with darkness effect if torch is off
void screen::draw() const {
    cls();
    gotoxy(0, 0);
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            char ch = mapData[y][x];
            // If this cell is dark and torch is not lit, draw space
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

// Finds the original key position by matching coordinates, returns nullptr if not found
const Point* screen::findOriginalKeyID(const Point& keyID) const {
    for (const auto& originalID : initialKeyPositions) {
        if (originalID.getX() == keyID.getX() && originalID.getY() == keyID.getY()) {
            return &originalID;
        }
    }
    return nullptr;
}

// Initializes map data, switches, doors, riddles and darkness for the given screen id
void screen::initScreenData(int id) {
    doors.clear();
    switchBoard.clear();
    clearDarkMask();
    torchLit = false;
    currentScreenID = id;

    // Setup for screen 0
    if (currentScreenID == 0) {
        char Screen1[MAX_Y][MAX_X + 1] = {
            //01234567890123456789012345678901234567890123456789012345678901234567890123456789
             "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
             "W              Player1:                W                Player2:               W", // 1
             "W Inventory:                           W Inventory:                            W", // 2
             "W Current Room:                        W Current Room:                         W", // 3
             "W                                      W                                       W", // 4
             "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 5
             "W                 W                W                W          W               W", // 6
             "W  WWWWWWWWWWWWW  W                W                W K W      WWWWWWWWWWWW    W", // 7
             "WK             W  W                W                WWWWW      WW         W    W", // 8
             "WWWWWWWWWWWWWWWW  W                W                           WW   WWWW  W    W", // 9
             "W                                  W                           WW   W  W  W    W", // 10
             "W                                  2                                W  W  W    W", // 11
             "W                 W                W         WWWWW             WWWWWW  W  W    W", // 12
             "W                 W                W         W K W             W       W  W    W", // 13
             "W                 W                W         W                 W       W       3", // 14
             "WWWWWWWWWWWW      WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 15
             "W  K     W                                                                     W", // 16
             "W        W                   WWW?WWW                                           W", // 17
             "W        W                   W     W                                           W", // 18
             "W        1                   W  K  W                                           W", // 19
             "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW  W", // 20
             "W            W                             W                                   W", // 21
             "W            W             W               W                                   W", // 22
             "W            ?             W                              !                    W", // 23
             "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"  // 24
        };

        // Collect initial positions of all keys on this screen
        initialKeyPositions.clear();
        for (int y = 0; y < MAX_Y; ++y) {
            for (int x = 0; x < MAX_X; ++x) {
                if (Screen1[y][x] == 'K') {
                    initialKeyPositions.emplace_back(x, y, 0, 0, 'K');
                }
            }
        }

        // Copy the screen layout into mapData
        for (int i = 0; i < MAX_Y; ++i) {
            strcpy_s(mapData[i], MAX_X + 1, Screen1[i]);
        }

        // Register switches for this screen
        registerSwitch(0, Point(5, 10, 0, 0, '/'), SwitchState::OFF);
        registerSwitch(1, Point(20, 10, 0, 0, '/'), SwitchState::OFF);

        // Setup riddles for this screen
        riddles.clear();
        riddles.push_back(Riddle(0, Point(13, 23, 0, 0, '?'), "What has keys but can't open locks?", "Keyboard"));
        riddles.push_back(Riddle(1, Point(32, 17, 0, 0, '?'), "What has Laces but does not wear clothes?", "Shoe"));

        // Switch requirements for door 3
        const Doors::SwitchRequirement doorSwitchReq[] = {
            {0, SwitchState::ON},
            {1, SwitchState::ON}
        };
        const size_t doorSwitchReqCount = sizeof(doorSwitchReq) / sizeof(doorSwitchReq[0]);

        // Key positions for doors on this screen
        std::vector<Point> keyPositions1 = { Point(32, 19, 0, 0, 'K') };
        std::vector<Point> keyPositions2 = { Point(3, 16, 0, 0, 'K'), Point(1, 8, 0, 0, 'K') };
        std::vector<Point> keyPositions3 = { Point(47, 13, 0, 0, 'K'), Point(54, 7, 0, 0, 'K') };

        // Create doors for this screen
        doors.emplace_back(1, 0, Point(9, 19, 0, 0, ' '), false, false,
            keyPositions1, 1, nullptr, 0);
        doors.emplace_back(2, 0, Point(35, 11, 0, 0, ' '), false, false,
            keyPositions2, 2, nullptr, 0);
        doors.emplace_back(3, 1, Point(1, 23, 0, 0, ' '), false, false,
            keyPositions3, 2,
            doorSwitchReq, doorSwitchReqCount);

        // Mark dark area for torch effect
        markDarkArea(50, 8, 75, 20);
    }
    // Setup for screen 1
    else if (currentScreenID == 1) {
        char Screen2[MAX_Y][MAX_X + 1] = {
            //01234567890123456789012345678901234567890123456789012345678901234567890123456789
             "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
             "W              Player1:                W                Player2:               W", // 1
             "W Inventory:                           W Inventory:                            W", // 2
             "W Current Room:                        W Current Room:                         W", // 3
             "W                                      W                                       W", // 4
             "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 5
             "W                                                                              W", // 6
             "W                                                                              5", // 7
             "W4WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 8
             "W           W                    K W                         W K               W", // 9
             "W           W            W      WWWW                         WWWW              W", // 10
             "W                        W                                                     W", // 11
             "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW?W", // 12
             "W                                                                              W", // 13
             "W                   WWWWWWWWWWWWWWWW              WWWW             W           W", // 14
             "W                 WWWWWWWWWWWWWWWWWWWW            W                W           W", // 15
             "W    WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 16
             "W                               W                  W                           W", // 17
             "W             W                 W                               W              W", // 18
             "W             W                                    W            W              W", // 19
             "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW?W", // 20
             "W                                             W                                W", // 21
             "W                                             WWWW                             W", // 22
             "3   !                                                                          W", // 23
             "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"  // 24
        };

        // Collect initial positions of all keys on this screen
        initialKeyPositions.clear();
        for (int y = 0; y < MAX_Y; ++y) {
            for (int x = 0; x < MAX_X; ++x) {
                if (Screen2[y][x] == 'K') {
                    initialKeyPositions.emplace_back(x, y, 0, 0, 'K');
                }
            }
        }

        // Copy the screen layout into mapData
        for (int i = 0; i < MAX_Y; ++i) {
            strcpy_s(mapData[i], MAX_X + 1, Screen2[i]);
        }

        // Register switches for this screen
        registerSwitch(2, Point(48, 21, 0, 0, '/'), SwitchState::OFF);
        registerSwitch(3, Point(52, 15, 0, 0, '/'), SwitchState::OFF);

        // Setup riddles for this screen
        riddles.clear();
        riddles.push_back(Riddle(2, Point(78, 20, 0, 0, '?'), "What has a Tongue but cannot speak?", "Wagon"));
        riddles.push_back(Riddle(3, Point(78, 12, 0, 0, '?'), "What has a Bed but never sleeps?", "River"));

        // Switch requirements for door 5
        const Doors::SwitchRequirement doorSwitchReq[] = {
            {2, SwitchState::ON},
            {3, SwitchState::ON}
        };
        const size_t doorSwitchReqCount = sizeof(doorSwitchReq) / sizeof(doorSwitchReq[0]);

        // Key positions for doors on this screen
        std::vector<Point> keyPositions4 = { Point(63, 9, 0, 0, 'K') };
        std::vector<Point> keyPositions5 = { Point(33, 9, 0, 0, 'K') };

        // Create doors for this screen
        doors.emplace_back(3, 0, Point(78, 14, 0, 0, ' '), false, false,
            std::vector<Point>{}, 0, nullptr, 0);
        doors.emplace_back(4, 1, Point(1, 8, 0, 0, ' '), false, false,
            keyPositions4, 1, nullptr, 0);
        doors.emplace_back(5, 2, Point(1, 23, 0, 0, ' '), false, false,
            keyPositions5, 1,
            doorSwitchReq, doorSwitchReqCount);

        // Mark dark area for torch effect
        markDarkArea(5, 22, 78, 1);
    }
    // Setup for final screen
    else {
        char EndScreen[MAX_Y][MAX_X + 1] = {
        "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
        "W                                                                              W", // 1
        "W                                                                              W", // 2
        "W                                                                              W", // 3
        "W                   WWWWWWWWWWWWW  W           W  WWWWWWWWWWW                  W", // 4
        "W                         W        W           W  W                            W", // 5
        "W                         W        W           W  W                            W", // 6
        "W                         W        WWWWWWWWWWWWW  WWWWWWWWWWW                  W", // 7
        "W                         W        W           W  W                            W", // 8
        "W                         W        W           W  W                            W", // 9
        "W                         W        W           W  WWWWWWWWWWW                  W", // 10
        "W                                                                              W", // 11
        "W                                                                              W", // 12
        "W                         WWWWWWWWWW  WW      W  WWWWWW                        W", // 13
        "W                         W           W W     W  W     W                       W", // 14
        "W                         W           W  W    W  W      W                      W", // 15
        "W                         WWWWWWWWWW  W   W   W  W       W                     W", // 16
        "W                         W           W    W  W  W      W                      W", // 17
        "W                         W           W     W W  W     W                       W", // 18
        "W                         WWWWWWWWWW  W      WW  WWWWWW                        W", // 19
        "W                                                                              W", // 20
        "W                                                                              W", // 21
        "W                                                                              W", // 22
        "W                  Press any key to return to the menu....                     W", // 23
        "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"  // 24
        };

        // Copy the end screen layout into mapData
        for (int i = 0; i < MAX_Y; ++i) {
            strcpy_s(mapData[i], MAX_X + 1, EndScreen[i]);
        }
    }
}

// Returns a pointer to the door that matches the given map character
Doors* screen::getDoorByChar(char doorChar) {
    for (auto& door : doors) {
        if (door.getDisplayChar() == doorChar) {
            return &door;
        }
    }
    return nullptr;
}

// Returns pointer to riddle at this position, or nullptr if none
Riddle* screen::getRiddleByPosition(const Point& p) {
    for (auto& riddle : riddles) {
        if (riddle.getPosition().getX() == p.getX() &&
            riddle.getPosition().getY() == p.getY()) {
            return &riddle;
        }
    }
    return nullptr;
}

// Sets the character in the map at this position
void screen::setCharAt(const Point& pos, char ch)
{
    mapData[pos.getY()][pos.getX()] = ch;
}

// Returns pointer to switch entry at this position from the switch board
SwitchBoard::SwitchEntry* screen::getSwitchAt(const Point& pos) {
    return switchBoard.getSwitchAt(pos);
}

// Returns current state of a switch by its id
SwitchState screen::getSwitchState(int id) const {
    return switchBoard.getState(id);
}

// Sets switch state by id and updates the symbol on the map
void screen::setSwitchState(int id, SwitchState state) {
    if (!switchBoard.setState(id, state)) {
        return;
    }
    if (auto* sw = switchBoard.getSwitchById(id)) {
        setCharAt(sw->location, switchStateToChar(state));
    }
}

// Toggles switch at a given position and updates the map symbol
void screen::toggleSwitchAt(const Point& pos) {
    SwitchState newState;
    if (switchBoard.toggleAt(pos, newState)) {
        setCharAt(pos, switchStateToChar(newState));
    }
}

// Registers a switch at the given position and draws its initial symbol
void screen::registerSwitch(int id, const Point& pos, SwitchState initialState) {
    switchBoard.registerSwitch(id, pos, initialState);
    setCharAt(pos, switchStateToChar(initialState));
}

// Marks a rectangular area as dark for the torch effect
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

// Turns the torch effect on or off
void screen::setTorchLit(bool lit) {
    torchLit = lit;
}




