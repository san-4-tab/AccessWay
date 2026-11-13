// test_emergency.cpp  (Member 2) - stand-alone test, no other member's code needed.
// Build (run inside member2_emergency/):
//   g++ -std=c++11 -o test_emergency test_emergency.cpp EmergencyManager.cpp EvacuationManager.cpp VolunteerManager.cpp HelpRequestManager.cpp
#include "EmergencyManager.h"
#include <cctype>
static Volunteer makeVolunteer(int id, const char* name, const char* type, int loc) {
    Volunteer v; v.volunteerId = id; v.buildingId = 1; v.volunteerName = name;
    v.assistanceType = type; v.available = true; v.currentLocationId = loc;
    return v;
}
static HelpRequest makeRequest(int id, int loc, const char* type) {
    HelpRequest r; r.requestId = id; r.buildingId = 1; r.visitorLocationId = loc;
    r.assignedVolunteerId = -1; r.helpType = type; r.status = PENDING;
    return r;
}

int main() {
    EmergencyManager em;
    em.loadSampleBuilding(1);

    cout << "===== 1. EVACUATION =====\n";
    em.startEmergency(1);
    cout << "\nVisitor in Room 201 (general):\n";
    em.getEvacuationManager().displayRoute(em.findEvacuationRoute(1, 7));
    cout << "\nVisitor in Room 201 (wheelchair - no stairs/elevator):\n";
    em.getEvacuationManager().displayRoute(em.findEvacuationRoute(1, 7, WHEELCHAIR));

    cout << "\nBlock Main Corridor -> Emergency Exit (connection 13); evacuate from Room 101:\n";
    em.blockConnection(1, 13);
    em.getEvacuationManager().displayRoute(em.findEvacuationRoute(1, 4));
    em.setFacilityAvailability(1, 3, false);        // elevator out of service
    em.printEmergencyReport(1);
    em.restoreConnection(1, 13);
    em.setFacilityAvailability(1, 3, true);
    em.endEmergency(1);

    cout << "\n===== 2. VOLUNTEERS & HELP REQUESTS =====\n";
    em.registerVolunteer(makeVolunteer(1, "Asha", "Wheelchair", 1));
    em.registerVolunteer(makeVolunteer(2, "Ravi", "General", 4));
    em.registerVolunteer(makeVolunteer(3, "Meena", "Wheelchair", 7));
    cout << "Duplicate volunteer rejected: " << (!em.registerVolunteer(makeVolunteer(1, "X", "General", 1)) ? "yes" : "NO") << "\n";
    em.displayVolunteers(1);

    em.createHelpRequest(makeRequest(1, 6, "General"));
    em.createHelpRequest(makeRequest(2, 2, "Medical help"));    // higher priority -> served first
    em.createHelpRequest(makeRequest(3, 4, "Wheelchair"));
    em.displayRequests(1);

    int handled = -1;
    em.processNextRequest(1, &handled);
    cout << "\nMost urgent request handled first: #" << handled << "\n";
    em.processNextRequest(1, &handled);
    cout << "Next request handled: #" << handled << "\n";
    em.displayRequests(1);

    cout << "\nDirections for request #3:\n";
    em.getEvacuationManager().displayRoute(em.getVolunteerDirections(1, 3));

    cout << "\nStatus flow for request #3:\n";
    cout << "COMPLETED directly (should fail): " << em.updateRequestStatus(1, 3, COMPLETED) << "\n";
    cout << "IN_PROGRESS: " << em.updateRequestStatus(1, 3, IN_PROGRESS) << "\n";
    cout << "COMPLETED  : " << em.updateRequestStatus(1, 3, COMPLETED) << "\n";
    em.displayRequests(1);
    em.displayVolunteers(1);

    cout << "\n===== 3. FILE PERSISTENCE =====\n";
    em.blockConnection(1, 13);
    cout << "saveAll: " << em.saveAll("../data") << "\n";
    EmergencyManager em2;
    em2.loadSampleBuilding(1);
    cout << "loadAll: " << em2.loadAll("../data") << "\n";
    em2.displayVolunteers(1);
    em2.displayRequests(1);
    em2.printEmergencyReport(1);
    return 0;
}
