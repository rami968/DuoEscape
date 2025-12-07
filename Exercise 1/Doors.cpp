#include "Doors.h"
#include "SwitchBoard.h"


Doors::Doors(int id, int destID, const Point& destPos, bool oneWay, bool openForever,
    const Point* keys, size_t keyCount,
    const SwitchRequirement* switches, size_t switchCount): 
    doorID(id),
    destinationScreenID(destID),
    destinationPosition(destPos),
    isOneWay(oneWay),
    isOpenForever(openForever)
{
    if (keys && keyCount > 0) {
        requiredKeyCount = (keyCount < MAX_REQUIRED_KEYS) ? keyCount : MAX_REQUIRED_KEYS;
        for (size_t i = 0; i < requiredKeyCount; ++i) {
            requiredKeyPos[i] = keys[i];
        }
    }

    if (switches && switchCount > 0) {
        requiredSwitchCount = (switchCount < MAX_REQUIRED_SWITCHES) ? switchCount : MAX_REQUIRED_SWITCHES;
        for (size_t i = 0; i < requiredSwitchCount; ++i) {
            switchRequirements[i] = switches[i];
        }
    }

    if (requiredKeyCount == 0 && requiredSwitchCount == 0) {
        isCurrentlyOpen = true;
    }
    else {
        isCurrentlyOpen = false;
    }
}

bool Doors::canPlayerPass(const std::vector<Point>& playerKeyPos,
    const SwitchBoard& switchBoard) const
{
    if (isCurrentlyOpen) {
        return true;
    }
    for (size_t i = 0; i < requiredKeyCount; ++i) {
        const Point& requiredPos = requiredKeyPos[i];
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

    for (size_t i = 0; i < requiredSwitchCount; ++i) {
        const SwitchRequirement& requirement = switchRequirements[i];
        if (switchBoard.getState(requirement.switchId) != requirement.requiredState) {
            return false;
        }
    }

    return true;
}

void Doors::openDoor(std::vector<Point>& playerKeyPos) {
    if (!isCurrentlyOpen) {
        isCurrentlyOpen = true;

        for (size_t i = 0; i < requiredKeyCount; ++i) {
            const Point& usedKeyPos = requiredKeyPos[i];
            for (size_t j = 0; j < playerKeyPos.size(); ++j) {
                if (playerKeyPos[j].getX() == usedKeyPos.getX() &&
                    playerKeyPos[j].getY() == usedKeyPos.getY()) {
                    if (j != playerKeyPos.size() - 1) {
                        playerKeyPos[j] = playerKeyPos.back();
                    }
                    playerKeyPos.pop_back();
                    break; 
                }
            }
        }
    }
}




