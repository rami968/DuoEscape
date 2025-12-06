#pragma once

#include "Point.h" 
#include "SwitchState.h" 
#include <vector>
#include <cstddef>

class SwitchBoard;

class Doors {
public:
    static constexpr size_t MAX_REQUIRED_KEYS = 4;
    static constexpr size_t MAX_REQUIRED_SWITCHES = 4;

    struct SwitchRequirement {
        int switchId = -1;
        SwitchState requiredState = SwitchState::OFF;
    };

private:
    int doorID;
    bool isCurrentlyOpen = false;
    bool isOpenForever;
    Point requiredKeyPos[MAX_REQUIRED_KEYS]{};
    size_t requiredKeyCount = 0;
    SwitchRequirement switchRequirements[MAX_REQUIRED_SWITCHES]{};
    size_t requiredSwitchCount = 0;
    int destinationScreenID;
    Point destinationPosition;
    bool isOneWay;

public:
    Doors(int id, int destID, const Point& destPos, bool oneWay, bool openForever,
        const Point* keys, size_t keyCount,
        const SwitchRequirement* switches, size_t switchCount);

    bool canPlayerPass(const std::vector<Point>& playerKeyPos,
        const SwitchBoard& switchBoard) const;

    void openDoor(std::vector<Point>& playerKeyPos);

    int getDestinationScreenID() const { return destinationScreenID; }
    const Point& getDestinationPosition() { return destinationPosition; }
    char getDisplayChar() const { return (char)('0' + doorID); }
    bool getIsOneWay() const { return isOneWay; }
    size_t getRequiredKeyCount() const { return requiredKeyCount; }
    size_t getRequiredSwitchCount() const { return requiredSwitchCount; }
};