#include "Doors.h"
#include "SwitchBoard.h"


Doors::Doors(int id, int destID, const Point& destPos, bool oneWay, bool openForever,
    std::vector<Point> keys, size_t keyCount,
    const SwitchRequirement* switches, size_t switchCount) :
    doorID(id),
    destinationScreenID(destID),
    destinationPosition(destPos),
    isOpenForever(openForever)
{
    if (keyCount > 0) {
        requiredKeyCount = (keyCount < MAX_REQUIRED_KEYS) ? keyCount : MAX_REQUIRED_KEYS;
        for (size_t i = 0; i < requiredKeyCount; ++i) {
            requiredKeyPos.push_back(keys[i]);
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

bool Doors::canPlayerPass(const SwitchBoard& switchBoard) const
{
    if (isCurrentlyOpen) {
        return true;
    }
    if (!requiredKeyPos.empty()) {
        return false;
    }
    for (size_t i = 0; i < requiredSwitchCount; ++i) {
        const SwitchRequirement& requirement = switchRequirements[i];
        if (switchBoard.getState(requirement.switchId) != requirement.requiredState) {
            return false;
        }
    }

    return true;
}

bool Doors::depositKey(const Point& keyPos) {
    if (isCurrentlyOpen) {
        return false;
    }
    auto requiredIt = requiredKeyPos.end();
    for (auto it = requiredKeyPos.begin(); it != requiredKeyPos.end(); ++it) {
        if (it->getX() == keyPos.getX() && it->getY() == keyPos.getY()) {
            requiredIt = it;
            break;
        }
    }
    if (requiredIt != requiredKeyPos.end()) {
        requiredKeyPos.erase(requiredIt);
        return true;
    }
    return false;
}

void Doors::openDoor() {
    if (!isCurrentlyOpen) {
        isCurrentlyOpen = true;
    }
}




