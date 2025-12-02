#pragma once

#include "Point.h" 
#include <vector>  
#include <map>     

enum class SwitchState {
    OFF,
    ON
};

class Doors {
    int doorID;
    bool isCurrentlyOpen = false; 
    bool isOpenForever;           
    std::vector<Point> requiredKeyPos;
    std::map<int, SwitchState> requiredSwitches;
    int destinationScreenID;      
    Point destinationPosition;    
    bool isOneWay;                

public:
    Doors(int id, int destID, const Point& destPos, bool oneWay, bool openForever,
        const std::vector<Point>& keys, const std::map<int, SwitchState>& switches);

    bool canPlayerPass(const std::vector<Point>& playerKeyPos,
        const std::map<int, SwitchState>& screenSwitchStates) const;

    void openDoor(std::vector<Point>& playerKeyPos);

    int getDestinationScreenID() const { return destinationScreenID; }
    const Point& getDestinationPosition() { return destinationPosition; }
    char getDisplayChar() const { return isCurrentlyOpen ? ' ' : (char)('0' + doorID); }
    bool getIsOneWay() const { return isOneWay; }
    const std::vector<Point>& getRequiredKeys() const { return requiredKeyPos; }
};