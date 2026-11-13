
#ifndef ACCESSPULSE_DATA_H
#define ACCESSPULSE_DATA_H

#include <string>
using namespace std;

// Mobility profiles
enum MobilityProfile {
    WHEELCHAIR,
    CRUTCHES,
    HAND_DISABILITY,
    LIMITED_MOBILITY,
    GENERAL
};

// Types of locations in a building
enum LocationType {
    ENTRANCE,
    ROOM,
    CORRIDOR,
    ELEVATOR,
    STAIRCASE,
    RAMP,
    EXIT,
    REFUGE_AREA,
    WASHROOM,
    JUNCTION
};

// Building information
struct Building {
    int buildingId;
    string buildingName;
    int totalFloors;
};

// Building location
struct Location {
    int locationId;
    int buildingId;
    string locationName;
    int floor;
    LocationType type;
};

// Connection between two locations
struct Connection {
    int connectionId;
    int buildingId;
    int fromLocationId;
    int toLocationId;

    float distance;
    float width;
    float slope;

    bool hasStairs;
    bool hasRamp;
    bool hasElevator;
    bool automaticDoor;

    bool available;
    bool emergencyApproved;
};

// Visitor review
struct Review {
    int reviewId;
    int buildingId;
    int locationId;

    int severity;
    string category;
    string description;

    bool verified;
};

// What-if simulation changes
enum ChangeType {
    BLOCK_CONNECTION,
    RESTORE_CONNECTION,
    DISABLE_FACILITY,
    ENABLE_FACILITY
};

struct SimulationChange {
    int changeId;
    int buildingId;
    ChangeType type;
    int connectionId;
    int locationId;
};

#endif
