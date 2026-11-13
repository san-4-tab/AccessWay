
#pragma once

#include "../common/AccessWayData.h"

class FloorManager {
private:
    struct FloorNode {
        int buildingId;
        int floorNumber;
        string floorName;
        FloorNode* next;
    };

    FloorNode* head;

public:
    FloorManager();
    ~FloorManager();

    void addFloor(int buildingId, int floorNumber, string floorName);
    bool searchFloor(int buildingId, int floorNumber);
    void displayFloors(int buildingId);
    void deleteFloor(int buildingId, int floorNumber);
};
