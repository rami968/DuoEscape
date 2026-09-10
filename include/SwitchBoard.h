#pragma once

#include <cstddef>
#include "Point.h"
#include "SwitchState.h"

class SwitchBoard {
public:
    static constexpr size_t MAX_SWITCHES = 10;

    struct SwitchEntry {
        int switchId = -1;
        Point location;
        SwitchState currentState = SwitchState::OFF;
    };

private:
    // Array of all registered switches
    SwitchEntry entries[MAX_SWITCHES]{};

    // Number of switches currently in use
    size_t switchCount = 0;

public:
    // Clears all switches from the board
    void clear();

    // Registers a new switch with id, position and initial state
    void registerSwitch(int id, const Point& pos, SwitchState initialState);

    // Returns pointer to switch at this position, or nullptr if none
    SwitchEntry* getSwitchAt(const Point& pos);

    // Const version of getSwitchAt
    const SwitchEntry* getSwitchAt(const Point& pos) const;

    // Returns pointer to switch with this id, or nullptr if not found
    SwitchEntry* getSwitchById(int id);

    // Const version of getSwitchById
    const SwitchEntry* getSwitchById(int id) const;

    // Returns current state of switch with this id, OFF if not found
    SwitchState getState(int id) const;

    // Sets state of switch with this id, returns true if found
    bool setState(int id, SwitchState state);

    // Toggles switch at this position, writes new state to outState
    bool toggleAt(const Point& pos, SwitchState& outState);

    // Returns number of registered switches
    size_t getRegisteredSwitchCount() const { return switchCount; }
};