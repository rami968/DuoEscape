#define NOMINMAX
#include "screen.h"
#include "Doors.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include "MapChar.h"
#include "ColorManager.h"

// Converts a switch state to the character shown on the map
char screen::switchStateToChar(SwitchState state) {
    return (state == SwitchState::ON ? static_cast<char>(MapChar::SwitchOff) : static_cast<char>(MapChar::SwitchOn));
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
void screen::draw(bool isColorEnabled) const {
    cls();
    gotoxy(0, 0);
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            char ch = mapData[y][x];
            // If this cell is dark and torch is not lit, draw space
            if (darkMask[y][x] && !torchLit) {
                ColorManager::setTextColor(ColorManager::DEFAULT, isColorEnabled);
                cout << static_cast<char>(MapChar::Empty);
            }
            else {
                if (ch == static_cast<char>(MapChar::Wall) || ch == static_cast<char>(MapChar::SoftWall))
                    ColorManager::setTextColor(ColorManager::GREY, isColorEnabled);
                else if (ch == static_cast<char>(MapChar::Player1) || ch == static_cast<char>(MapChar::Player2))
                    ColorManager::setTextColor(ColorManager::GREEN, isColorEnabled);
                else if (ch == static_cast<char>(MapChar::Obstacle) || ch == static_cast<char>(MapChar::Spring))
                    ColorManager::setTextColor(ColorManager::YELLOW, isColorEnabled);
                else if (ch == static_cast<char>(MapChar::BombItem))
                    ColorManager::setTextColor(ColorManager::RED, isColorEnabled);
                else if (ch == static_cast<char>(MapChar::Star))
                    ColorManager::setTextColor(ColorManager::MAGENTA, isColorEnabled);
                else if (ch == static_cast<char>(MapChar::SwitchOn) || ch == static_cast<char>(MapChar::SwitchOff))
                    ColorManager::setTextColor(ColorManager::CYAN, isColorEnabled);
                else if (ch == static_cast<char>(MapChar::Key) || ch == static_cast<char>(MapChar::Torch))
                    ColorManager::setTextColor(ColorManager::YELLOW, isColorEnabled);
                else if (ch >= '0' && ch <= '9')
                    ColorManager::setTextColor(ColorManager::BLUE, isColorEnabled);
                else
                    ColorManager::setTextColor(ColorManager::DEFAULT, isColorEnabled);
                cout << ch;
            }
        }
        if (y < MAX_Y - 1) {
            cout << endl;
        }
    }
    ColorManager::setTextColor(ColorManager::DEFAULT, isColorEnabled);
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

// Initialize static member
std::map<int, Riddle> screen::globalRiddles;

bool screen::loadRiddlesFromFile(const std::string& filename, std::vector<std::string>& outErrors) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		outErrors.push_back("Error: Could not open riddle file: " + filename);
		return false;
	}
	std::string line;
	int lineNum = 0;
	while (std::getline(file, line)) {
		lineNum++;
		if (line.empty()) continue;
		std::stringstream ss(line);
		std::string segment;
		std::vector<std::string> parts;
		
		// Split by '|'
		while (std::getline(ss, segment, '|')) {
			parts.push_back(segment);
		}

		if (parts.size() >= 3) {
			try {
				int id = std::stoi(parts[0]);
				std::string question = parts[1];
				std::vector<std::string> answers;
				for (size_t i = 2; i < parts.size(); ++i) {
					answers.push_back(parts[i]);
				}
				// Store in cache with placeholder position
				globalRiddles.emplace(id, Riddle(id, Point(0, 0, 0, 0, '?'), question, answers));
			} catch (...) {
				outErrors.push_back("Warning: Invalid riddle format at line " + std::to_string(lineNum) + " in " + filename);
			}
		} else {
             outErrors.push_back("Warning: Insufficient riddle data (needs ID|Question|Answer) at line " + std::to_string(lineNum) + " in " + filename);
        }
	}
	return true;
}

void screen::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        errors.push_back("Critical Error: Could not open map file: " + filename);
        // Fill safe default map
        for (int y = 0; y < MAX_Y; ++y) {
             for (int x = 0; x < MAX_X; ++x) mapData[y][x] = static_cast<char>(MapChar::Empty);
             mapData[y][MAX_X] = '\0';
        }
        return;
    }

    // Read map data (first 25 lines)
    std::string line;
    for (int y = 0; y < MAX_Y; ++y) {
        if (std::getline(file, line)) {
             size_t len = std::min(line.length(), (size_t)MAX_X);
             std::copy(line.begin(), line.begin() + len, mapData[y]);
             for (size_t i = len; i < MAX_X; ++i) {
                 mapData[y][i] = static_cast<char>(MapChar::Empty);
             }
             mapData[y][MAX_X] = '\0';
        } else {
             for (int i = 0; i < MAX_X; ++i) mapData[y][i] = static_cast<char>(MapChar::Empty);
             mapData[y][MAX_X] = '\0';
             // Not necessarily an error, maybe short file, but let's warn if it looks very short?
             // Actually many text editors might not add 25 lines. Defaulting to space is fine "overcoming problem".
        }
    }

    // Read optional sections
    riddles.clear();
    while (std::getline(file, line)) {
        if (line == "[RIDDLES]") {
            while (std::getline(file, line) && !line.empty()) {
                // Parse: X Y ID
                std::stringstream ss(line);
                int x, y, id;
                if (ss >> x >> y >> id) {
					// Instantiate riddle from cache if exists
					auto it = globalRiddles.find(id);
					if (it != globalRiddles.end()) {
						riddles.push_back(Riddle(id, Point(x, y, 0, 0, '?'), it->second.getQuestion(), it->second.getCorrectAnswers()));
					} else {
                        errors.push_back("Warning: Map " + filename + " references unknown Riddle ID: " + std::to_string(id));
                    }
                }
            }
        }
    }
}

// Initializes screen data
void screen::initScreenData(int id) {
    doors.clear();
    switchBoard.clear();
    clearDarkMask();
    torchLit = false;
    currentScreenID = id;
    resetArmedBombs();
    valid = true;
    loadError.clear();

    // Load data from file associated with this screen
    if (!fileName.empty()) {
        loadFromFile(fileName);
    }

  
    if (!findLegendPosition()) {
        errors.push_back(loadError);
        return;
    }

    if (hasLegend) {
        // Validate that player start positions are not inside the legend area
        for (int y = legendY; y < legendY + LEGEND_TOTAL_H; ++y) {
            for (int x = 0; x < MAX_X; ++x) {
                char c = mapData[y][x];
                if (c == static_cast<char>(MapChar::Player1) ||
                    c == static_cast<char>(MapChar::Player2)) {

                    valid = false;
                    loadError = "Critical Error: Player start is inside the legend area in file: " + fileName;
                    errors.push_back(loadError);
                    return;
                }
            }
        }
    }

    // Collect initial positions of all keys on this screen
    initialKeyPositions.clear();
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            if (mapData[y][x] == static_cast<char>(MapChar::Key)) {
                initialKeyPositions.emplace_back(x, y, 0, 0, static_cast<char>(MapChar::Key));
            }
        }
    }
	// Setup for screen 0
    if (currentScreenID == 0) {
        // Register switches for this screen
        registerSwitch(0, Point(6, 10, 0, 0, static_cast<char>(MapChar::SwitchOn)), SwitchState::OFF);
        registerSwitch(1, Point(20, 10, 0, 0, static_cast<char>(MapChar::SwitchOn)), SwitchState::OFF);

        // Switch requirements for door 3
        const Doors::SwitchRequirement doorSwitchReq[] = {
            {0, SwitchState::ON},
            {1, SwitchState::ON}
        };
        const size_t doorSwitchReqCount = sizeof(doorSwitchReq) / sizeof(doorSwitchReq[0]);

        // Springs
        springs.clear();
        springs.emplace_back(Spring({ Point(78, 16, 0, 0, static_cast<char>(MapChar::Spring)), Point(77, 16, 0, 0, static_cast<char>(MapChar::Spring)), Point(76, 16, 0, 0, static_cast<char>(MapChar::Spring)), Point(75, 16, 0, 0, static_cast<char>(MapChar::Spring)), Point(74, 16, 0, 0, static_cast<char>(MapChar::Spring)) }, Direction::LEFT, this));
        springs.emplace_back(Spring({ Point(14, 19, 0, 0, static_cast<char>(MapChar::Spring)), Point(14, 18, 0, 0, static_cast<char>(MapChar::Spring)) }, Direction::UP, this));
        
        // Key positions for doors on this screen
        std::vector<Point> keyPositions1 = { Point(32, 19, 0, 0, static_cast<char>(MapChar::Key)) };
        std::vector<Point> keyPositions2 = { Point(3, 16, 0, 0, static_cast<char>(MapChar::Key)), Point(1, 8, 0, 0, static_cast<char>(MapChar::Key)) };
        std::vector<Point> keyPositions3 = { Point(47, 13, 0, 0, static_cast<char>(MapChar::Key)), Point(54, 7, 0, 0, static_cast<char>(MapChar::Key)) };

        // Create doors for this screen
        doors.emplace_back(1, 0, Point(9, 19, 0, 0, static_cast<char>(MapChar::Empty)),
            keyPositions1, 1, nullptr, 0);
        doors.emplace_back(2, 0, Point(35, 11, 0, 0, static_cast<char>(MapChar::Empty)),
            keyPositions2, 2, nullptr, 0);
        doors.emplace_back(3, 1, Point(1, 23, 0, 0, static_cast<char>(MapChar::Empty)),
            keyPositions3, 2,
            doorSwitchReq, doorSwitchReqCount);

        // Obstacles
        obstacles.clear();
        obstacles.emplace_back(Obstacle({ Point(2, 12, 0, 0, static_cast<char>(MapChar::Obstacle))}, this));
        obstacles.emplace_back(Obstacle({ Point(5, 14, 0, 0, static_cast<char>(MapChar::Obstacle)), Point(5, 13, 0, 0, static_cast<char>(MapChar::Obstacle)) }, this));
        obstacles.emplace_back(Obstacle({ Point(14, 14, 0, 0, static_cast<char>(MapChar::Obstacle)), Point(14, 15, 0, 0, static_cast<char>(MapChar::Obstacle)) }, this));
        obstacles.emplace_back(Obstacle({ Point(31, 16, 0, 0, static_cast<char>(MapChar::Obstacle)), Point(32, 16, 0, 0, static_cast<char>(MapChar::Obstacle)), Point(33, 16, 0, 0, static_cast<char>(MapChar::Obstacle)) }, this));

        // Mark dark area for torch effect
        markDarkArea(50, 8, 75, 20);
    }
    // Setup for screen 1 (Map 2)
    else if (currentScreenID == 1) {

        // Register switches for this screen
        registerSwitch(2, Point(48, 21, 0, 0, static_cast<char>(MapChar::SwitchOn)), SwitchState::OFF);
        registerSwitch(3, Point(52, 15, 0, 0, static_cast<char>(MapChar::SwitchOn)), SwitchState::OFF);

        // Switch requirements for door 5
        const Doors::SwitchRequirement doorSwitchReq[] = {
            {2, SwitchState::ON},
            {3, SwitchState::ON}
        };
        const size_t doorSwitchReqCount = sizeof(doorSwitchReq) / sizeof(doorSwitchReq[0]);
        // Key positions for doors on this screen
        std::vector<Point> keyPositions4 = { Point(63, 9, 0, 0, static_cast<char>(MapChar::Key)) };
        std::vector<Point> keyPositions5 = { Point(33, 9, 0, 0, static_cast<char>(MapChar::Key)) };

        // Create doors for this screen
        doors.emplace_back(3, 0, Point(78, 14, 0, 0, static_cast<char>(MapChar::Empty)),
            std::vector<Point>{}, 0, nullptr, 0);
        doors.emplace_back(4, 1, Point(70,15,0, 0, static_cast<char>(MapChar::Empty)),
            keyPositions4, 1, nullptr, 0);
        doors.emplace_back(5, 2, Point(39, 23, 0, 0, static_cast<char>(MapChar::Empty)),
            keyPositions5, 1,
            doorSwitchReq, doorSwitchReqCount);

        // Springs
        springs.clear();
        springs.emplace_back(Spring({ Point(1, 6, 0, 0, static_cast<char>(MapChar::Spring)), Point(2, 6, 0, 0, static_cast<char>(MapChar::Spring)), Point(3, 6, 0, 0, static_cast<char>(MapChar::Spring)), Point(4, 6, 0, 0, static_cast<char>(MapChar::Spring)) }, Direction::RIGHT, this));
        springs.emplace_back(Spring({ Point(10, 23, 0, 0, static_cast<char>(MapChar::Spring)), Point(11, 23, 0, 0, static_cast<char>(MapChar::Spring)), Point(12, 23, 0, 0, static_cast<char>(MapChar::Spring)), Point(13, 23, 0, 0, static_cast<char>(MapChar::Spring)) }, Direction::RIGHT, this));

        // Obstacles
        obstacles.clear();
        obstacles.emplace_back(Obstacle({ Point(50, 13, 0, 0, static_cast<char>(MapChar::Obstacle)), Point(51, 13, 0, 0, static_cast<char>(MapChar::Obstacle)) }, this));
        obstacles.emplace_back(Obstacle({ Point(46, 23, 0, 0, static_cast<char>(MapChar::Obstacle)), Point(47, 23, 0, 0, static_cast<char>(MapChar::Obstacle)), Point(48, 23, 0, 0, static_cast<char>(MapChar::Obstacle)), Point(49, 23, 0, 0, static_cast<char>(MapChar::Obstacle)) }, this));

        // Mark dark area
        markDarkArea(5, 22, 78, 1);
    }
    // Setup for screen 2 (Map 3)
    else if (currentScreenID == 2) {
        
        // Register switches for this screen
        registerSwitch(4, Point(57, 23, 0, 0, static_cast<char>(MapChar::SwitchOn)), SwitchState::OFF);
        registerSwitch(5, Point(20, 23, 0, 0, static_cast<char>(MapChar::SwitchOn)), SwitchState::OFF);

        // Switch requirements for door 6 & 7
        const Doors::SwitchRequirement door6SwitchReq[] = {
            {5, SwitchState::ON},          
        };
        const Doors::SwitchRequirement door7SwitchReq[] = {
            {4, SwitchState::ON},
        };

        const size_t doorSwitchReqCount = sizeof(door6SwitchReq) / sizeof(door6SwitchReq[0]);
        // Key positions for doors on this screen
        std::vector<Point> keyPositions8 = { Point(72, 10, 0, 0, static_cast<char>(MapChar::Key)), Point(7, 10, 0, 0, static_cast<char>(MapChar::Key)) };

        // Create doors for this screen
        doors.emplace_back(5, 1, Point(78, 7, 0, 0, static_cast<char>(MapChar::Empty)),
            std::vector<Point>{}, 0, nullptr, 0);
        doors.emplace_back(6, 2, Point(75, 22, 0, 0, static_cast<char>(MapChar::Empty)),
            std::vector<Point>{}, 0, door6SwitchReq, doorSwitchReqCount);
        doors.emplace_back(7, 2, Point(4, 22, 0, 0, static_cast<char>(MapChar::Empty)),
            std::vector<Point>{}, 0, door7SwitchReq, doorSwitchReqCount);
        doors.emplace_back(8, 3, Point(1, 23, 0, 0, static_cast<char>(MapChar::Empty)),
            keyPositions8, 2, nullptr, 0);

        
        
        // Springs
        springs.clear();
        springs.emplace_back(Spring({ Point(1, 5, 0, 0, static_cast<char>(MapChar::Spring)), Point(2, 5, 0, 0, static_cast<char>(MapChar::Spring)), Point(3, 5, 0, 0, static_cast<char>(MapChar::Spring)), Point(4, 5, 0, 0, static_cast<char>(MapChar::Spring)) }, Direction::RIGHT, this));
        springs.emplace_back(Spring({ Point(78, 5, 0, 0, static_cast<char>(MapChar::Spring)), Point(77, 5, 0, 0, static_cast<char>(MapChar::Spring)), Point(76, 5, 0, 0, static_cast<char>(MapChar::Spring)), Point(75, 5, 0, 0, static_cast<char>(MapChar::Spring)) }, Direction::LEFT, this));
        springs.emplace_back(Spring({ Point(1, 23, 0, 0, static_cast<char>(MapChar::Spring)), Point(1, 22, 0, 0, static_cast<char>(MapChar::Spring)), Point(1, 21, 0, 0, static_cast<char>(MapChar::Spring)) }, Direction::UP, this));
        springs.emplace_back(Spring({ Point(78, 23, 0, 0, static_cast<char>(MapChar::Spring)), Point(78, 22, 0, 0, static_cast<char>(MapChar::Spring)), Point(78, 21, 0, 0, static_cast<char>(MapChar::Spring)) }, Direction::UP, this));

        // Obstacles
        obstacles.clear();
        obstacles.emplace_back(Obstacle({ Point(6, 5, 0, 0, static_cast<char>(MapChar::Obstacle)), Point(7, 5, 0, 0, static_cast<char>(MapChar::Obstacle)), Point(8, 5, 0, 0, static_cast<char>(MapChar::Obstacle)) }, this));
        obstacles.emplace_back(Obstacle({ Point(71, 5, 0, 0, static_cast<char>(MapChar::Obstacle)), Point(72, 5, 0, 0, static_cast<char>(MapChar::Obstacle)), Point(73, 5, 0, 0, static_cast<char>(MapChar::Obstacle)) }, this));
        obstacles.emplace_back(Obstacle({ Point(75, 15, 0, 0, static_cast<char>(MapChar::Obstacle)) }, this));
        obstacles.emplace_back(Obstacle({ Point(4, 15, 0, 0, static_cast<char>(MapChar::Obstacle)) }, this));

    }
    // Setup for final screen
    else {
        // Just map data, mostly static
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

// Returns pointer to spring at this position, or nullptr if none
Spring* screen::getSpringByPosition(const Point& p) {
	for (auto& spring : springs) {
		for (const auto& pos : spring.getPositions()) {
			if (pos.getX() == p.getX() && pos.getY() == p.getY()) {
				return &spring;
			}
		}
	}
	return nullptr;

}

// Returns pointer to obstacle at this position, or nullptr if none
Obstacle* screen::getObstacleByPosition(const Point& p) {
    for (auto& obstacle : obstacles) {
        for (const auto& pos : obstacle.getPositions()) {
            if (pos.getX() == p.getX() && pos.getY() == p.getY()) {
                return &obstacle;
            }
        }
    }
    return nullptr;

}

// Sets the character in the map at this position
void screen::setCharAt(const Point& pos, char ch)
{
    if (ch == static_cast<char>(MapChar::Empty)) {
        SwitchBoard::SwitchEntry* sw = switchBoard.getSwitchAt(pos);
        if (sw != nullptr) {
            mapData[pos.getY()][pos.getX()] = switchStateToChar(sw->currentState);
            return;
        }
    }
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

// Resets all bombs to unarmed state
void screen::resetArmedBombs() {
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            bombIsArmed[y][x] = false;
        }
    }
}

// Marks a bomb as armed or unarmed at the given position
void screen::markBombAsArmedAt(const Point& p, bool isArmed) {
    const int x = p.getX();
    const int y = p.getY();
    if (x < 0 || x >= MAX_X || y < 0 || y >= MAX_Y) {
        return;
    }
    bombIsArmed[y][x] = isArmed;
}

// Returns true if a bomb is armed at the given position
bool screen::isBombArmedAt(const Point& p) const {
    const int x = p.getX();
    const int y = p.getY();
    if (x < 0 || x >= MAX_X || y < 0 || y >= MAX_Y) {
        return false;
    }
    return bombIsArmed[y][x];
}

// Clears the legend area in the map
void screen::clearLegendAreaInMap() {
    if (!hasLegend) return;  
    for (int y = legendY; y < legendY + LEGEND_TOTAL_H; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            mapData[y][x] = static_cast<char>(MapChar::Empty);  
        }
    }
}

// Finds the legend position in the map
bool screen::findLegendPosition()
{
    hasLegend = false;
    legendX = 0;  
    legendY = -1;

    int foundCount = 0;
    int foundX = -1;
    int foundY = -1;

    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            if (mapData[y][x] == static_cast<char>(MapChar::Legend)) {
                ++foundCount;
                foundX = x;
                foundY = y;
                mapData[y][x] = static_cast<char>(MapChar::Empty);
            }
        }
    }

    // Found no legend
    if (foundCount == 0) {
        if (currentScreenID < 3) {
            valid = false;
            loadError = "Invalid screen: missing 'L' legend marker.";
            loadError += " (file: " + fileName + ")";
            return false;
        }
        hasLegend = false;
        return true;
    }

    // Found multiple legends
    if (foundCount > 1) {
        valid = false;
        loadError = "Invalid screen: multiple 'L' legend markers were found.";
        loadError += " (file: " + fileName + ")";
        return false;
    }

    // Adjust legendY so it starts at the row ABOVE the 'L' (the top frame)
    // 'L' is at Row 1. Frame starts at Row 0.
    legendY = foundY - 1;
    legendX = 0;

    const int legendBottomY = legendY + LEGEND_TOTAL_H - 1;
    
    // Bounds check
    if (legendY < 0 || legendBottomY >= MAX_Y) {
        valid = false;
        loadError = "Invalid legend position: 'L' at row Y=" + std::to_string(foundY) +
            ". Legend height is " + std::to_string(LEGEND_TOTAL_H) +
            ", so it needs rows Y=" + std::to_string(foundY) + ".." +
            std::to_string(legendBottomY) + " but screen rows are 0.." + std::to_string(MAX_Y - 1) + ".";
        loadError += " (file: " + fileName + ")";
        return false;
    }

    hasLegend = true;
    return true;
}

// Returns true if the given position is in the legend area
bool screen::isInLegendArea(int x, int y) const {
    if (!hasLegend) return false;
    if (x < 0 || x >= MAX_X) return false;
    if (y < 0 || y >= MAX_Y) return false;
    return (y >= legendY && y < legendY + LEGEND_TOTAL_H);
}

// Returns true if the given position is clear
bool screen::isClear(const Point& p, bool canPassSpring) {
    char ch = getCharAt(p);
    if (ch == static_cast<char>(MapChar::Empty) || isKey(p) || isTorch(p) || isSwitchOn(p) || isSwitchOff(p) || isRiddle(p) || isObstacle(p)) {
        return true;
    }
	if (canPassSpring && isSpring(p)) {
		return true;
	}
    return false; 
}

