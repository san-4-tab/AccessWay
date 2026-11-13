#ifndef ACCESSWAY_EMERGENCY_DATA_H
#define ACCESSWAY_EMERGENCY_DATA_H

#include <string>
using namespace std;

struct Volunteer {
    int volunteerId;
    int buildingId;
    string volunteerName;
    string assistanceType;
    bool available;
    int currentLocationId;
};

enum RequestStatus {
    PENDING,
    ACCEPTED,
    IN_PROGRESS,
    COMPLETED,
    CANCELLED
};

struct HelpRequest {
    int requestId;
    int buildingId;
    int visitorLocationId;
    int assignedVolunteerId;
    string helpType;
    RequestStatus status;
};

#endif
