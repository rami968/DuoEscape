#pragma once
#include <iostream>
#include "Point.h"
#include "Doors.h"
#include "SwitchState.h"
#include "SwitchBoard.h"
#include <vector>
#include <map>

using std::cout, std::endl;

class screen {
public:
	enum { MAX_X = 80, MAX_Y = 25 };
private:
	int currentScreenID = 0;
	char mapData[MAX_Y][MAX_X + 1];
	std::vector<Doors> doors;
	SwitchBoard switchBoard;
	bool torchLit = false;
	bool darkMask[MAX_Y][MAX_X] = { false };
	void clearDarkMask();

public:
	screen(int id = 0) { initScreenData(id); }
	char getCharAt(const Point& p) const {
		return mapData[p.getY()][p.getX()];
	}
	Doors* getDoorByChar(char doorChar);
	SwitchBoard::SwitchPad* getSwitchAt(const Point& pos);
	SwitchState getSwitchState(int id) const;
	const std::map<int, SwitchState>& getSwitchStates() const { return switchBoard.getStates(); }
	void toggleSwitchAt(const Point& pos);
	void setSwitchState(int id, SwitchState state);
	void registerSwitch(int id, const Point& pos, SwitchState initialState);
	void initScreenData(int ScreenID);
	void draw() const;
	bool isWall(const Point& p) const {
		return getCharAt(p) == 'W';
	}
	bool isDoor(const Point& p) const {
		return isdigit(getCharAt(p));
	}
	bool isSwitchOff(const Point& p) const {
		return getCharAt(p) == '/';
	}
	bool isSwitchOn(const Point& p) const {
		return getCharAt(p) == '\\';
	}
	bool isKey(const Point& p) const {
		return getCharAt(p) == 'K';
	}
	bool isTorch(const Point& p) const {
		return getCharAt(p) == '!';
	}
	bool isBomb(const Point& p) const {
		return getCharAt(p) == '@';
	}
	bool isRiddle(const Point& p) const {
		return getCharAt(p) == '?';
	}
	void setCharAt(const Point& pos, char ch);
	void markDarkArea(int x1, int y1, int x2, int y2);
	void setTorchLit(bool lit);
	bool isTorchLit() const { return torchLit; }
};
