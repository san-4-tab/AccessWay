
#pragma once

#include "../common/AccessPulseData.h"

class LocationManager {
private:
    struct LocationNode {
        Location data;
        LocationNode* next;
    };

    LocationNode* head;

public:
    LocationManager();
    ~LocationManager();

    void addLocation(Location l);
    Location* searchLocation(int locationId);
    void displayLocations(int buildingId);
    void displayFloorLocations(int buildingId, int floor);
    void deleteLocation(int locationId);
};
