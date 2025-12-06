#include "SwitchBoard.h"


void SwitchBoard::clear() {
    pads.clear();
    states.clear();
}

void SwitchBoard::registerSwitch(int id, const Point& pos, SwitchState initialState) {
    pads.push_back(SwitchPad{ id, pos });
    states[id] = initialState;
}

SwitchBoard::SwitchPad* SwitchBoard::getSwitchAt(const Point& pos) {
    for (auto& pad : pads) {
        if (pad.position.getX() == pos.getX() && pad.position.getY() == pos.getY()) {
            return &pad;
        }
    }
    return nullptr;
}

const SwitchBoard::SwitchPad* SwitchBoard::getSwitchAt(const Point& pos) const {
    for (const auto& pad : pads) {
        if (pad.position.getX() == pos.getX() && pad.position.getY() == pos.getY()) {
            return &pad;
        }
    }
    return nullptr;
}

SwitchBoard::SwitchPad* SwitchBoard::getSwitchById(int id) {
    for (auto& pad : pads) {
        if (pad.id == id) {
            return &pad;
        }
    }
    return nullptr;
}

const SwitchBoard::SwitchPad* SwitchBoard::getSwitchById(int id) const {
    for (const auto& pad : pads) {
        if (pad.id == id) {
            return &pad;
        }
    }
    return nullptr;
}

SwitchState SwitchBoard::getState(int id) const {
    auto it = states.find(id);
    if (it != states.end()) {
        return it->second;
    }
    return SwitchState::OFF;
}

bool SwitchBoard::setState(int id, SwitchState state) {
    auto it = states.find(id);
    if (it == states.end()) {
        return false;
    }
    it->second = state;
    return true;
}

bool SwitchBoard::toggleAt(const Point& pos, SwitchState& outState) {
    SwitchPad* pad = getSwitchAt(pos);
    if (!pad) {
        return false;
    }
    SwitchState current = getState(pad->id);
    SwitchState next = (current == SwitchState::ON) ? SwitchState::OFF : SwitchState::ON;
    setState(pad->id, next);
    outState = next;
    return true;
}