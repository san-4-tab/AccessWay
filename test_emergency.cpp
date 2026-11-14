// test_emergency.cpp  (Member 2) - stand-alone test, no other member's code needed.
// Build (run inside member2_emergency/):
//   g++ -std=c++11 -o test_emergency test_emergency.cpp EmergencyManager.cpp EvacuationManager.cpp VolunteerManager.cpp HelpRequestManager.cpp
#include "EmergencyManager.h"
#include <cctype>
#include <iostream>
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

// Test invalid help-request rejection.
bool invalidRequestChecksPass =
    !em.createHelpRequest(makeRequest(0, 6, "General")) &&
    !em.createHelpRequest(makeRequest(10, 0, "General")) &&
    !em.createHelpRequest(makeRequest(11, 6, "General|Invalid"));

cout << "Invalid help requests rejected: "
     << (invalidRequestChecksPass ? "PASS" : "FAIL") << "\n";

em.createHelpRequest(makeRequest(1, 6, "General"));
    em.createHelpRequest(makeRequest(2, 2, "Medical help"));    // higher priority -> served first
    em.createHelpRequest(makeRequest(3, 4, "Wheelchair"));
    em.displayRequests(1);

    int handled = -1;
    em.processNextRequest(1, &handled);
    cout << "\nMost urgent request handled first: #" << handled << "\n";
    em.processNextRequest(1, &handled);
    cout << "Next request handled: #" << handled << "\n";
    // Verify an assigned volunteer cannot be manually marked available.
HelpRequest* activeRequest =
    em.getHelpRequestManager().findRequest(1, 2);

bool manualReleaseBlocked = false;

if (activeRequest && activeRequest->assignedVolunteerId != -1) {
    manualReleaseBlocked =
        !em.setVolunteerAvailability(
            1, activeRequest->assignedVolunteerId, true
        );
}

cout << "Manual release of busy volunteer blocked: "
     << (manualReleaseBlocked ? "PASS" : "FAIL") << "\n";
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
    cout << "saveAll: " << em.saveAll("data") << "\n";
    EmergencyManager em2;
    em2.loadSampleBuilding(1);
    cout << "loadAll: " << em2.loadAll("data") << "\n";
    HelpRequest* loadedRequest =
    em2.getHelpRequestManager().findRequest(1, 2);

bool persistenceAvailabilityPass = false;

if (loadedRequest &&
    (loadedRequest->status == ACCEPTED ||
     loadedRequest->status == IN_PROGRESS) &&
    loadedRequest->assignedVolunteerId != -1) {

    Volunteer* loadedVolunteer =
        em2.getVolunteerManager().findVolunteer(
            1, loadedRequest->assignedVolunteerId
        );

    persistenceAvailabilityPass =
        loadedVolunteer && !loadedVolunteer->available;
}

cout << "Active volunteer remains unavailable after loading: "
     << (persistenceAvailabilityPass ? "PASS" : "FAIL") << "\n";

em2.displayVolunteers(1);
em2.displayRequests(1);
em2.printEmergencyReport(1);
// Test that an unreachable high-priority request
// does not prevent a reachable lower-priority request.

EmergencyManager queueTest;
queueTest.loadSampleBuilding(1);

queueTest.registerVolunteer(
    makeVolunteer(10, "Queue Tester", "General", 1)
);

queueTest.createHelpRequest(
    makeRequest(1, 999, "Emergency")
);

queueTest.createHelpRequest(
    makeRequest(2, 1, "General")
);

int queueHandledId = -1;

bool queueProcessingPass =
    queueTest.processNextRequest(1, &queueHandledId);

HelpRequest* urgentRequest =
    queueTest.getHelpRequestManager().findRequest(1, 1);

HelpRequest* reachableRequest =
    queueTest.getHelpRequestManager().findRequest(1, 2);

bool queueFallbackPass =
    queueProcessingPass &&
    queueHandledId == 2 &&
    urgentRequest &&
    urgentRequest->status == PENDING &&
    reachableRequest &&
    reachableRequest->status == ACCEPTED;

cout << "Queue skips unreachable urgent request: "
     << (queueFallbackPass ? "PASS" : "FAIL") << "\n";
// Make GitHub Actions fail if either test fails.
if (!invalidRequestChecksPass ||
    !manualReleaseBlocked ||
    !persistenceAvailabilityPass) {
    cout << "\nVOLUNTEER CONSISTENCY TEST FAILED.\n";
    return 1;
}

cout << "\nVOLUNTEER CONSISTENCY TEST PASSED.\n";
return 0;
}
