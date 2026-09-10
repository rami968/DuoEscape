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
    std::vector<Point> requiredKeyPos;
    size_t requiredKeyCount = 0;
    SwitchRequirement switchRequirements[MAX_REQUIRED_SWITCHES]{};
    size_t requiredSwitchCount = 0;
    int destinationScreenID;
    Point destinationPosition;
    static constexpr int EXTERNAL_DOOR_ID_1 = 3;
    static constexpr int EXTERNAL_DOOR_ID_2 = 5;

public:
    Doors(int id, int destID, const Point& destPos,
        std::vector<Point> keys, size_t keyCount,
        const SwitchRequirement* switches, size_t switchCount);

    bool canPlayerPass(const SwitchBoard& switchBoard) const;
    void openDoor();
    bool depositKey(const Point& keyPos);
    int getDestinationScreenID() const { return destinationScreenID; }
    const Point& getDestinationPosition() const { return destinationPosition; }
    char getDisplayChar() const { return (char)('0' + doorID); }
    size_t getRequiredKeyCount() const { return requiredKeyCount; }
    size_t getRequiredSwitchCount() const { return requiredSwitchCount; }
    int getDoorId() const { return doorID; }
    bool isExternalDoor() const {
        return doorID == EXTERNAL_DOOR_ID_1 || doorID == EXTERNAL_DOOR_ID_2;
    }
	bool isOpen() const { return isCurrentlyOpen; }
};