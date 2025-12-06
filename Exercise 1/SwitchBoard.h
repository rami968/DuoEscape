#pragma once

#include <vector>
#include <map>
#include "Point.h"
#include "SwitchState.h"

class SwitchBoard {
public:
    struct SwitchPad {
        int id = -1;
        Point position;
    };

private:
    std::vector<SwitchPad> pads;
    std::map<int, SwitchState> states;

public:
    void clear();
    void registerSwitch(int id, const Point& pos, SwitchState initialState);
    SwitchPad* getSwitchAt(const Point& pos);
    const SwitchPad* getSwitchAt(const Point& pos) const;
    SwitchPad* getSwitchById(int id);
    const SwitchPad* getSwitchById(int id) const;
    SwitchState getState(int id) const;
    bool setState(int id, SwitchState state);
    bool toggleAt(const Point& pos, SwitchState& outState);
    const std::map<int, SwitchState>& getStates() const { return states; }
};