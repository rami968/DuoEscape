#pragma once

#include <iostream>
#include "Point.h"
#include "Doors.h"
#include "SwitchState.h"
#include "SwitchBoard.h"
#include "Riddle.h"
#include "Spring.h"
#include "Obstacle.h"
#include <vector>
#include <string>
#include <map>
#include "MapChar.h"

using std::cout;
using std::endl;

class screen {
public:
	enum { MAX_X = 80, MAX_Y = 25 };

private:
	int currentScreenID = 0;
	char mapData[MAX_Y][MAX_X + 1];
	std::vector<Doors> doors;
	std::vector<Riddle> riddles;
	std::vector<Spring> springs;
	std::vector<Obstacle> obstacles;
	std::vector<Point> initialKeyPositions;
	SwitchBoard switchBoard;
	bool torchLit = false;
	bool darkMask[MAX_Y][MAX_X] = { false };
	void clearDarkMask();
	static char switchStateToChar(SwitchState state);
	bool bombIsArmed[MAX_Y][MAX_X]{};
	int legendX = 1;
	int legendY = -1;
	bool hasLegend = false;
	bool valid = true;
	std::string loadError;


public:

	screen(int id, const std::string& fileName) : currentScreenID(id), fileName(fileName)
	{
		initScreenData(id);
	}
	
	const std::vector<std::string>& getErrors() const { return errors; }
	
	// Check if riddles are loaded, if not load them and return success/fail
	static bool validRiddlesLoaded() { return !globalRiddles.empty(); }
	static bool loadRiddlesFromFile(const std::string& filename, std::vector<std::string>& outErrors);

private:
	std::string fileName;
	std::vector<std::string> errors;
	
	void loadFromFile(const std::string& filename);
	
	// Global riddle cache: ID -> Riddle Logic (Question/Answers)
	// We'll store Riddle objects here but Position will be irrelevant/placeholder in this cache.
	static std::map<int, Riddle> globalRiddles;

public:
	char getCharAt(const Point& p) const {
		return mapData[p.getY()][p.getX()];
	}

	Doors* getDoorByChar(char doorChar);
	Riddle* getRiddleByPosition(const Point& p);
	Spring* getSpringByPosition(const Point& p);
	Obstacle* getObstacleByPosition(const Point& p);
	SwitchBoard::SwitchEntry* getSwitchAt(const Point& pos);
	SwitchState getSwitchState(int id) const;
	const SwitchBoard& getSwitchBoard() const { return switchBoard; }
	void toggleSwitchAt(const Point& pos);
	void setSwitchState(int id, SwitchState state);
	void registerSwitch(int id, const Point& pos, SwitchState initialState);
	void initScreenData(int ScreenID);
	void draw(bool isColorEnabled) const;
	bool isWall(const Point& p) const {
		char ch = getCharAt(p);
		return (ch == static_cast<char>(MapChar::Wall) || ch == static_cast<char>(MapChar::SoftWall));
	}
	bool isDoor(const Point& p) const {
		return isdigit(getCharAt(p));
	}
	bool isSwitchOff(const Point& p) const {
		return getCharAt(p) == static_cast<char>(MapChar::SwitchOff);
	}
	bool isSwitchOn(const Point& p) const {
		return getCharAt(p) == static_cast<char>(MapChar::SwitchOn);
	}
	bool isKey(const Point& p) const {
		return getCharAt(p) == static_cast<char>(MapChar::Key);
	}
	bool isTorch(const Point& p) const {
		return getCharAt(p) == static_cast<char>(MapChar::Torch);
	}
	bool isRiddle(const Point& p) const {
		return getCharAt(p) == static_cast<char>(MapChar::Riddle);
	}
	bool isBomb(const Point& p) const {
		return getCharAt(p) == static_cast<char>(MapChar::BombItem) && !isBombArmedAt(p);
	}
	bool isSpring(const Point& p) const {
		return getCharAt(p) == static_cast<char>(MapChar::Spring);
	}
	bool isObstacle(const Point& p) const {
		return getCharAt(p) == static_cast<char>(MapChar::Obstacle);
	}
	bool isPlayer(const Point& p) const {
		char ch = getCharAt(p);
		return ch == static_cast<char>(MapChar::Player1) || ch == static_cast<char>(MapChar::Player2);
	}
	void setCharAt(const Point& pos, char ch);
	void markDarkArea(int x1, int y1, int x2, int y2);
	void setTorchLit(bool lit);
	bool isTorchLit() const { return torchLit; }
	const Point* findOriginalKeyID(const Point& keyID) const;
	int getCurrentScreenID() const { return currentScreenID; }
	void resetArmedBombs();
	void markBombAsArmedAt(const Point& p, bool isArmed);
	bool isBombArmedAt(const Point& p) const;
	bool findLegendPosition();  
	bool hasLegendArea() const { return hasLegend; }
	int getLegendX() const { return legendX; }
	int getLegendY() const { return legendY; }
	bool isValid() const { return valid; }
	const std::string& getLoadError() const { return loadError; }
	void clearLegendAreaInMap();
	bool isInLegendArea(int x, int y) const;
	static constexpr int LEGEND_TOTAL_H = 5;
	bool isClear(const Point& p, bool canPassSpring);
	std::vector<Obstacle>& getObstacles() { return obstacles; }
};