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
    SwitchEntry entries[MAX_SWITCHES]{};
    size_t switchCount = 0;

public:
    void clear();
    void registerSwitch(int id, const Point& pos, SwitchState initialState);
    SwitchEntry* getSwitchAt(const Point& pos);
    const SwitchEntry* getSwitchAt(const Point& pos) const;
    SwitchEntry* getSwitchById(int id);
    const SwitchEntry* getSwitchById(int id) const;
    SwitchState getState(int id) const;
    bool setState(int id, SwitchState state);
    bool toggleAt(const Point& pos, SwitchState& outState);
    size_t getRegisteredSwitchCount() const { return switchCount; }
};