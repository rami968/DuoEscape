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
    std::vector<int> requiredKeyIDs;
    std::map<int, SwitchState> requiredSwitches;
    int destinationScreenID;      
    Point destinationPosition;    
    bool isOneWay;                

public:
    Doors(int id, int destID, const Point& destPos, bool oneWay, bool openForever,
        const std::vector<int>& keys, const std::map<int, SwitchState>& switches);

    bool canPlayerPass(const std::vector<int>& playerKeyIDs,
        const std::map<int, SwitchState>& screenSwitchStates) const;

    void openDoor(std::vector<int>& playerKeyIDs);

    int getDestinationScreenID() const { return destinationScreenID; }
    const Point& getDestinationPosition() { return destinationPosition; }
    char getDisplayChar() const { return isCurrentlyOpen ? ' ' : (char)('0' + doorID); }
    bool getIsOneWay() const { return isOneWay; }
    const std::vector<int>& getRequiredKeys() const { return requiredKeyIDs; }
};