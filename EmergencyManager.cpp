// EmergencyManager.cpp  (Member 2)
#include "EmergencyManager.h"

EmergencyManager::EmergencyManager() : helpRequests(volunteers) {}

// ------------------------------------------------------------ building data
bool EmergencyManager::addLocation(const Location& l) { return evacuation.addLocation(l); }
bool EmergencyManager::addConnection(const Connection& c) { return evacuation.addConnection(c); }

// Dummy data = the shared sample dataset (College Block A) so Member 2 can test alone.
// Additions for Member 2 testing: location 9 (refuge area), and a 2<->4 link for Room 101.
void EmergencyManager::loadSampleBuilding(int b) {
    struct L { int id; const char* name; int floor; LocationType type; };
    L locs[] = {
        {1, "Main Entrance", 0, ENTRANCE}, {2, "Main Corridor", 0, CORRIDOR},
        {3, "Elevator", 0, ELEVATOR},      {4, "Room 101", 0, ROOM},
        {5, "Staircase", 0, STAIRCASE},    {6, "Corridor 2", 1, CORRIDOR},
        {7, "Room 201", 1, ROOM},          {8, "Emergency Exit", 0, EXIT},
        {9, "Refuge Area F1", 1, REFUGE_AREA}
    };
    for (size_t i = 0; i < sizeof(locs) / sizeof(locs[0]); i++) {
        Location loc;
        loc.locationId = locs[i].id; loc.buildingId = b; loc.locationName = locs[i].name;
        loc.floor = locs[i].floor; loc.type = locs[i].type;
        addLocation(loc);
    }

    struct C { int from, to; float dist; bool stairs, elevator; };
    C conns[] = {
        {1, 2, 10, false, false}, {2, 3, 5, false, true},  {3, 6, 8, false, true},
        {6, 7, 6, false, false},  {2, 5, 4, false, false}, {5, 6, 9, true, false},
        {2, 8, 12, false, false}, {6, 9, 3, false, false}, {2, 4, 6, false, false}
    };
    int id = 1;
    for (size_t i = 0; i < sizeof(conns) / sizeof(conns[0]); i++) {
        for (int dir = 0; dir < 2; dir++) {                  // two-way = two connections
            Connection c;
            c.connectionId = id++; c.buildingId = b;
            c.fromLocationId = dir == 0 ? conns[i].from : conns[i].to;
            c.toLocationId = dir == 0 ? conns[i].to : conns[i].from;
            c.distance = conns[i].dist; c.width = 1.2f; c.slope = 0;
            c.hasStairs = conns[i].stairs; c.hasRamp = false;
            c.hasElevator = conns[i].elevator; c.automaticDoor = false;
            c.available = true;
            c.emergencyApproved = !conns[i].elevator;         // elevators are never emergency routes
            addConnection(c);
        }
    }
}

// -------------------------------------------------------------- emergency
void EmergencyManager::startEmergency(int b) { emergencyActive[b] = true; }
void EmergencyManager::endEmergency(int b) { emergencyActive[b] = false; }

bool EmergencyManager::isEmergencyActive(int b) const {
    auto it = emergencyActive.find(b);
    return it != emergencyActive.end() && it->second;
}

RouteResult EmergencyManager::findEvacuationRoute(int b, int start, MobilityProfile profile) const {
    return evacuation.findEvacuationRoute(b, start, profile);
}

bool EmergencyManager::blockConnection(int b, int id) { return evacuation.blockConnection(b, id); }
bool EmergencyManager::restoreConnection(int b, int id) { return evacuation.restoreConnection(b, id); }

bool EmergencyManager::setFacilityAvailability(int b, int loc, bool available) {
    return evacuation.setLocationAvailability(b, loc, available);
}

void EmergencyManager::printEmergencyReport(int b) const {
    cout << "\n===== Emergency Report: Building " << b << " ("
         << (isEmergencyActive(b) ? "EMERGENCY ACTIVE" : "normal") << ") =====\n";

    vector<int> blocked = evacuation.getBlockedConnections(b);
    cout << "Blocked connections   : ";
    if (blocked.empty()) cout << "none";
    for (size_t i = 0; i < blocked.size(); i++) cout << blocked[i] << " ";

    vector<int> unavailable = evacuation.getUnavailableLocations(b);
    cout << "\nUnavailable facilities: ";
    if (unavailable.empty()) cout << "none";
    for (size_t i = 0; i < unavailable.size(); i++)
        cout << evacuation.getLocationName(b, unavailable[i]) << "  ";

    vector<int> safe = evacuation.getSafeLocations(b);
    cout << "\nSafe exits / refuges  : ";
    for (size_t i = 0; i < safe.size(); i++) cout << evacuation.getLocationName(b, safe[i]) << "  ";

    vector<int> trapped = evacuation.findTrappedLocations(b);
    cout << "\nTrapped locations     : ";
    if (trapped.empty()) cout << "none";
    for (size_t i = 0; i < trapped.size(); i++) cout << evacuation.getLocationName(b, trapped[i]) << "  ";
    cout << "\n";
}

// -------------------------------------------------------------- volunteers
bool EmergencyManager::registerVolunteer(const Volunteer& v) { return volunteers.registerVolunteer(v); }

bool EmergencyManager::setVolunteerAvailability(int b, int id, bool available) {
    return volunteers.setAvailability(b, id, available);
}

void EmergencyManager::displayVolunteers(int b) const { volunteers.displayVolunteers(b); }

// ----------------------------------------------------------- help requests
bool EmergencyManager::createHelpRequest(const HelpRequest& r) { return helpRequests.createHelpRequest(r); }

bool EmergencyManager::assignVolunteer(int b, int requestId, int volunteerId) {
    return helpRequests.assignVolunteer(b, requestId, volunteerId);
}

bool EmergencyManager::autoAssignRequest(int b, int requestId) {
    return helpRequests.autoAssignRequest(b, requestId, evacuation);
}

bool EmergencyManager::processNextRequest(int b, int* handled) {
    return helpRequests.processNextRequest(b, evacuation, handled);
}

bool EmergencyManager::updateRequestStatus(int b, int requestId, RequestStatus status) {
    return helpRequests.updateRequestStatus(b, requestId, status);
}

void EmergencyManager::displayRequests(int b) const { helpRequests.displayRequests(b); }

RouteResult EmergencyManager::getVolunteerDirections(int b, int requestId) {
    RouteResult none;
    none.found = false; none.buildingId = b; none.startLocationId = -1;
    none.destinationLocationId = -1; none.usedRefugeArea = false; none.totalDistance = 0;

    HelpRequest* r = helpRequests.findRequest(b, requestId);
    if (!r || r->assignedVolunteerId == -1) { none.message = "Request has no assigned volunteer."; return none; }
    Volunteer* v = volunteers.findVolunteer(b, r->assignedVolunteerId);
    if (!v) { none.message = "Volunteer not found."; return none; }
    return evacuation.findRoute(b, v->currentLocationId, r->visitorLocationId);
}

// -------------------------------------------------------------- persistence
bool EmergencyManager::saveAll(const string& folder) const {
    bool ok = volunteers.saveToFile(folder + "/volunteers.txt");
    ok = helpRequests.saveToFile(folder + "/requests.txt") && ok;
    ok = evacuation.saveStatus(folder + "/emergency_status.txt") && ok;
    return ok;
}

bool EmergencyManager::loadAll(const string& folder) {
    bool ok = volunteers.loadFromFile(folder + "/volunteers.txt");
    ok = helpRequests.loadFromFile(folder + "/requests.txt") && ok;
    ok = evacuation.loadStatus(folder + "/emergency_status.txt") && ok;
    return ok;
}
