// EmergencyManager.h  (Member 2)
// Single entry point of Member 2's module. Owns and connects:
//   EvacuationManager  - blocked paths, exits, refuge areas, evacuation routes
//   VolunteerManager   - volunteer registration / availability
//   HelpRequestManager - help requests and their status
#ifndef EMERGENCY_MANAGER_H
#define EMERGENCY_MANAGER_H

#include <string>
#include <unordered_map>
#include "../common/AccessWayData.h"
#include "EvacuationManager.h"
#include "VolunteerManager.h"
#include "HelpRequestManager.h"

class EmergencyManager {
private:
    EvacuationManager evacuation;
    VolunteerManager volunteers;                 // must be declared before helpRequests
    HelpRequestManager helpRequests;
    unordered_map<int, bool> emergencyActive;    // buildingId -> emergency mode

public:
    EmergencyManager();
    EmergencyManager(const EmergencyManager&) = delete;
    EmergencyManager& operator=(const EmergencyManager&) = delete;

    // ---- building data (replaced by Member 1's data during integration) ----
    bool addLocation(const Location& location);
    bool addConnection(const Connection& connection);
    void loadSampleBuilding(int buildingId = 1);     // "College Block A" dummy data

    // ---- emergency control ----
    void startEmergency(int buildingId);
    void endEmergency(int buildingId);
    bool isEmergencyActive(int buildingId) const;

    // agreed interface: findEvacuationRoute(buildingId, startLocationId)
    RouteResult findEvacuationRoute(int buildingId, int startLocationId,
                                    MobilityProfile profile = GENERAL) const;

    bool blockConnection(int buildingId, int connectionId);
    bool restoreConnection(int buildingId, int connectionId);
    bool setFacilityAvailability(int buildingId, int locationId, bool available);
    void printEmergencyReport(int buildingId) const;

    // ---- volunteers ----
    bool registerVolunteer(const Volunteer& volunteer);
    bool setVolunteerAvailability(int buildingId, int volunteerId, bool available);
    void displayVolunteers(int buildingId) const;

    // ---- help requests ----
    bool createHelpRequest(const HelpRequest& request);
    bool assignVolunteer(int buildingId, int requestId, int volunteerId);
    bool autoAssignRequest(int buildingId, int requestId);
    bool processNextRequest(int buildingId, int* handledRequestId = nullptr);
    bool updateRequestStatus(int buildingId, int requestId, RequestStatus status);
    void displayRequests(int buildingId) const;

    // route the assigned volunteer should follow to reach the visitor
    RouteResult getVolunteerDirections(int buildingId, int requestId);

    // ---- persistence (volunteers.txt, requests.txt, emergency_status.txt) ----
    bool saveAll(const string& dataFolder) const;
    bool loadAll(const string& dataFolder);

    // access for integration / tests
    EvacuationManager& getEvacuationManager() { return evacuation; }
    VolunteerManager& getVolunteerManager() { return volunteers; }
    HelpRequestManager& getHelpRequestManager() { return helpRequests; }
};

#endif
