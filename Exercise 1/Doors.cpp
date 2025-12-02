#include "Doors.h"
#include <iostream>


Doors::Doors(int id, int destID, const Point& destPos, bool oneWay, bool openForever,
    const std::vector<Point>& keys, const std::map<int, SwitchState>& switches): 
    doorID(id),
    destinationScreenID(destID),
    destinationPosition(destPos),
    isOneWay(oneWay),
    isOpenForever(openForever),
    requiredKeyPos(keys),
    requiredSwitches(switches)
{
    if (requiredKeyPos.empty() && requiredSwitches.empty()) {
        isCurrentlyOpen = true;
    }
    else {
        isCurrentlyOpen = false;
    }
}

bool Doors::canPlayerPass(const std::vector<Point>& playerKeyPos,
    const std::map<int, SwitchState>& screenSwitchStates) const
{
    if (isCurrentlyOpen && isOpenForever) {
        return true;
    }
    for (const Point& requiredPos : requiredKeyPos) {
        bool keyFoundInInventory = false;
        for (const Point& heldKeyPos : playerKeyPos) {
            if (heldKeyPos.getX() == requiredPos.getX() &&
                heldKeyPos.getY() == requiredPos.getY()) {
                keyFoundInInventory = true;
                break;
            }
        }
        
        if (!keyFoundInInventory) {
            return false; 
        }
    }

    for (const auto& pair : requiredSwitches) {
        int requiredSwitchID = pair.first;
        SwitchState requiredState = pair.second;

        auto it = screenSwitchStates.find(requiredSwitchID);

        if (it == screenSwitchStates.end() || it->second != requiredState) {
            return false;
        }
    }

    return true;
}

void Doors::openDoor(std::vector<Point>& playerKeyPos) {
    if (!isCurrentlyOpen) {
        isCurrentlyOpen = true;

        for (const Point& usedKeyPos : requiredKeyPos) {
            for (size_t i = 0; i < playerKeyPos.size(); ++i) {
                if (playerKeyPos[i].getX() == usedKeyPos.getX() &&
                    playerKeyPos[i].getY() == usedKeyPos.getY()) {
                    if (i != playerKeyPos.size() - 1) {
                        playerKeyPos[i] = playerKeyPos.back();
                    }
                    playerKeyPos.pop_back();
                    break; 
                }
            }
        }
    }
}




