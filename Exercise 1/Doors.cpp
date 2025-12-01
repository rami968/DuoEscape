#include "Doors.h"
#include <iostream>


Doors::Doors(int id, int destID, const Point& destPos, bool oneWay, bool openForever,
    const std::vector<int>& keys, const std::map<int, SwitchState>& switches): 
    doorID(id),
    destinationScreenID(destID),
    destinationPosition(destPos),
    isOneWay(oneWay),
    isOpenForever(openForever),
    requiredKeyIDs(keys),
    requiredSwitches(switches)
{
    if (requiredKeyIDs.empty() && requiredSwitches.empty()) {
        isCurrentlyOpen = true;
    }
    else {
        isCurrentlyOpen = false;
    }
}

bool Doors::canPlayerPass(const std::vector<int>& playerKeyIDs,
    const std::map<int, SwitchState>& screenSwitchStates) const
{
    if (isCurrentlyOpen && isOpenForever) {
        return true;
    }
    for (int requiredID : requiredKeyIDs) {
        bool keyFoundInInventory = false;

        for (int heldKeyID : playerKeyIDs) {
            if (requiredID == heldKeyID) {
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

void Doors::openDoor(std::vector<int>& playerKeyIDs) {
    if (!isCurrentlyOpen) {
        isCurrentlyOpen = true;

        for (int usedKeyID : requiredKeyIDs) {
            for (size_t i = 0; i < playerKeyIDs.size(); ++i) {
                if (playerKeyIDs[i] == usedKeyID) {             
                    if (i != playerKeyIDs.size() - 1) {
                        playerKeyIDs[i] = playerKeyIDs.back(); 
                    }
                    playerKeyIDs.pop_back(); 
                    break; 
                }
            }
        }
    }
}

