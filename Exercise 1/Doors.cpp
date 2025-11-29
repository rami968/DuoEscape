#include "Doors.h"

bool Doors:: canPlayerPass(int keysHeld, bool switchCurrentState) const {
    bool hasKeys = keysHeld >= requiredKeys;

    bool switchConditionMet = (requiredSwitchID == 0) ||
        (switchCurrentState == requiredSwitchState);

    return !isLocked || (hasKeys && switchConditionMet);
};

Doors::Doors(int num, int dest, int reqKeys, int switchID, bool switchState)
	: doorNumber(num), destinationScreen(dest), requiredKeys(reqKeys),
	requiredSwitchID(switchID), requiredSwitchState(switchState) {
	isLocked = (requiredKeys > 0) || (requiredSwitchID != 0);
}

//void Doors::setDoorNumber(int num) {
//	switch (num) {
//	case 1:

//	}
//