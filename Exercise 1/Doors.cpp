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
    // Copy required keys 
    if (keyCount > 0) {
        requiredKeyCount = (keyCount < MAX_REQUIRED_KEYS) ? keyCount : MAX_REQUIRED_KEYS;
        for (size_t i = 0; i < requiredKeyCount; ++i) {
            requiredKeyPos.push_back(keys[i]);
        }
    }

    // Copy required switches 
    if (switches && switchCount > 0) {
        requiredSwitchCount = (switchCount < MAX_REQUIRED_SWITCHES) ? switchCount : MAX_REQUIRED_SWITCHES;
        for (size_t i = 0; i < requiredSwitchCount; ++i) {
            switchRequirements[i] = switches[i];
        }
    }

    // If no keys and no switches are required – door starts open
    if (requiredKeyCount == 0 && requiredSwitchCount == 0) {
        isCurrentlyOpen = true;
    }
    else {
        isCurrentlyOpen = false;
    }
}

bool Doors::canPlayerPass(const SwitchBoard& switchBoard) const
{
    // Already open - player can always pass
    if (isCurrentlyOpen) {
        return true;
    }

    // If there are still required keys that were not deposited - cannot pass
    if (!requiredKeyPos.empty()) {
        return false;
    }

    // Check all switch requirements (must match requiredState)
    for (size_t i = 0; i < requiredSwitchCount; ++i) {
        const SwitchRequirement& requirement = switchRequirements[i];
        if (switchBoard.getState(requirement.switchId) != requirement.requiredState) {
            return false;
        }
    }

    return true;
}

bool Doors::depositKey(const Point& keyPos) {
    // If door is already open, no need to deposit keys
    if (isCurrentlyOpen) {
        return false;
    }

    // Look for matching key position in the requiredKeyPos list
    auto requiredIt = requiredKeyPos.end();
    for (auto it = requiredKeyPos.begin(); it != requiredKeyPos.end(); ++it) {
        if (it->getX() == keyPos.getX() && it->getY() == keyPos.getY()) {
            requiredIt = it;
            break;
        }
    }

    // If found – remove from required list 
    if (requiredIt != requiredKeyPos.end()) {
        requiredKeyPos.erase(requiredIt);
        return true;
    }
    return false;
}

void Doors::openDoor() {
    // Mark door as open 
    if (!isCurrentlyOpen) {
        isCurrentlyOpen = true;
    }
}
