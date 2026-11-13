// AccessPulseData.h  -- FROZEN shared data format. Do not edit alone.
#ifndef ACCESSPULSE_DATA_H
#define ACCESSPULSE_DATA_H

#include <iostream>
#include <string>
using namespace std;

struct Building {
    int buildingId;
    string buildingName;
    int totalFloors;
};

enum LocationType {
    ENTRANCE, ROOM, CORRIDOR, ELEVATOR, STAIRCASE,
    RAMP, EXIT, REFUGE_AREA, WASHROOM, JUNCTION
};

struct Location {
    int locationId;
    int buildingId;
    string locationName;
    int floor;
    LocationType type;
};

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

enum MobilityProfile {
    WHEELCHAIR, CRUTCHES, HAND_DISABILITY, LIMITED_MOBILITY, GENERAL
};

struct Review {
    int reviewId;
    int buildingId;
    int locationId;
    int severity; // 1 to 5
    string category;
    string description;
    bool verified;
};

struct Volunteer {
    int volunteerId;
    int buildingId;
    string volunteerName;
    string assistanceType;
    bool available;
    int currentLocationId;
};

enum RequestStatus { PENDING, ACCEPTED, IN_PROGRESS, COMPLETED, CANCELLED };

struct HelpRequest {
    int requestId;
    int buildingId;
    int visitorLocationId;
    int assignedVolunteerId;

    string helpType;
    RequestStatus status;
};

enum ChangeType {
    BLOCK_CONNECTION, RESTORE_CONNECTION, DISABLE_FACILITY,
    ENABLE_FACILITY, ADD_CONNECTION
};

struct SimulationChange {
    int changeId;
    int buildingId;
    ChangeType type;
    int connectionId;
    int locationId;
};

#endif
