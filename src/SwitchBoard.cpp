#include "SwitchBoard.h"

// Clears all switches from the board
void SwitchBoard::clear() {
    switchCount = 0;
    for (auto& entry : entries) {
        entry = SwitchEntry{};
    }
}

// Registers a new switch with id, position and initial state
void SwitchBoard::registerSwitch(int id, const Point& pos, SwitchState initialState) {
    if (switchCount >= MAX_SWITCHES) {
        return;
    }
    SwitchEntry& slot = entries[switchCount++];
    slot.switchId = id;
    slot.location = pos;
    slot.currentState = initialState;
}

// Returns a pointer to the switch at this position, or nullptr if none
SwitchBoard::SwitchEntry* SwitchBoard::getSwitchAt(const Point& pos) {
    for (size_t i = 0; i < switchCount; ++i) {
        SwitchEntry& entry = entries[i];
        if (entry.location.getX() == pos.getX() && entry.location.getY() == pos.getY()) {
            return &entry;
        }
    }
    return nullptr;
}

// Const version of getSwitchAt, cannot modify the switch
const SwitchBoard::SwitchEntry* SwitchBoard::getSwitchAt(const Point& pos) const {
    for (size_t i = 0; i < switchCount; ++i) {
        const SwitchEntry& entry = entries[i];
        if (entry.location.getX() == pos.getX() && entry.location.getY() == pos.getY()) {
            return &entry;
        }
    }
    return nullptr;
}

// Returns a pointer to the switch with this id, or nullptr if not found
SwitchBoard::SwitchEntry* SwitchBoard::getSwitchById(int id) {
    for (size_t i = 0; i < switchCount; ++i) {
        SwitchEntry& entry = entries[i];
        if (entry.switchId == id) {
            return &entry;
        }
    }
    return nullptr;
}

// Const version of getSwitchById, cannot modify the switch
const SwitchBoard::SwitchEntry* SwitchBoard::getSwitchById(int id) const {
    for (size_t i = 0; i < switchCount; ++i) {
        const SwitchEntry& entry = entries[i];
        if (entry.switchId == id) {
            return &entry;
        }
    }
    return nullptr;
}

// Returns the current state of the switch with this id, OFF if not found
SwitchState SwitchBoard::getState(int id) const {
    const SwitchEntry* entry = getSwitchById(id);
    return entry ? entry->currentState : SwitchState::OFF;
}

// Sets the state of the switch with this id, returns true if found
bool SwitchBoard::setState(int id, SwitchState state) {
    SwitchEntry* entry = getSwitchById(id);
    if (!entry) {
        return false;
    }
    entry->currentState = state;
    return true;
}

// Toggles the switch at this position and returns the new state in outState
bool SwitchBoard::toggleAt(const Point& pos, SwitchState& outState) {
    SwitchEntry* entry = getSwitchAt(pos);
    if (!entry) {
        return false;
    }
    SwitchState next = (entry->currentState == SwitchState::ON) ? SwitchState::OFF : SwitchState::ON;
    entry->currentState = next;
    outState = next;
    return true;
}
