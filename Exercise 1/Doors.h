#pragma once
class Doors {
    int doorNumber;
    bool isLocked;
    int requiredKeys;
    int destinationScreen;
    int requiredSwitchID;
    bool requiredSwitchState;

public:
    Doors(int num, int dest, int reqKeys, int switchID, bool switchState);
    bool canPlayerPass(int keysHeld, bool switchCurrentState) const;
    void setDoorNumber(int num);
};
   
