
#include <iostream>

#include "common/AccessWayData.h"
#include "common/EmergencyData.h"

#include "member1_navigation/Graph.h"
#include "EmergencyManager.h"
#include "member3/WhatIfSimulator.h"
#include "member3/ReviewManager.h"
#include "member3/ReviewOptimizer.h"
#include <sstream>
using namespace std;

int main() {

    cout << "\n===== ACCESSWAY BACKEND INTEGRATION TEST =====\n";

    // 1. SHARED BUILDING DATA

    Location entrance = {
        1, 1, "Starting Location", 0, ROOM
    };

    Location exitLocation = {
        2, 1, "Emergency Exit", 0, EXIT
    };

    Connection path = {
        1, 1, 1, 2,
        10, 1.5, 0,
        false, true, false, true,
        true, true
    };
    // Create one common dataset for all three modules
BuildingData building;

building.locations.push_back(entrance);
building.locations.push_back(exitLocation);
building.connections.push_back(path);

    // 2. MEMBER 1 - NAVIGATION

    Graph navigation;

   bool navigationLoadPass =
    navigation.loadBuildingData(building);

cout << (navigationLoadPass
    ? "PASS: Common data loaded into navigation.\n"
    : "FAIL: Navigation data loading failed.\n");

    Graph::RouteResult normalRoute =
        navigation.dijkstra(1, 2, GENERAL);

    cout << "\n--- MEMBER 1: NAVIGATION ---\n";

    bool navigationPass = normalRoute.found;

    cout << (navigationPass
        ? "PASS: Normal route found.\n"
        : "FAIL: Normal route not found.\n");

    delete[] normalRoute.path;

    navigation.removeEdge(1);

    Graph::RouteResult blockedRoute =
        navigation.dijkstra(1, 2, GENERAL);

    bool navigationBlockPass = !blockedRoute.found;

    cout << (navigationBlockPass
        ? "PASS: Blocked route unavailable.\n"
        : "FAIL: Blocked route still exists.\n");

    delete[] blockedRoute.path;

    // 3. MEMBER 2 - EMERGENCY MANAGEMENT

    EmergencyManager emergency;

    bool emergencyLoadPass =
    emergency.loadBuildingData(building);

cout << (emergencyLoadPass
    ? "PASS: Common data loaded into emergency management.\n"
    : "FAIL: Emergency data loading failed.\n");

    RouteResult evacuationRoute =
        emergency.findEvacuationRoute(1, 1, GENERAL);

    cout << "\n--- MEMBER 2: EMERGENCY ---\n";

    bool emergencyPass = evacuationRoute.found;
// Verify evacuation graph works in both directions.

RouteResult reverseRoute =
    emergency.getEvacuationManager().findRoute(
        1, 2, 1, GENERAL
    );

vector<int> reverseReachable =
    emergency.getEvacuationManager().findReachableLocations(
        1, 2, GENERAL, MODE_NORMAL
    );

bool reverseRoutePass =
    reverseRoute.found &&
    reverseRoute.path.size() == 2 &&
    reverseRoute.path[0] == 2 &&
    reverseRoute.path[1] == 1;

bool reverseReachabilityPass =
    reverseReachable.size() == 2 &&
    reverseReachable[0] == 1 &&
    reverseReachable[1] == 2;

cout << (reverseRoutePass
    ? "PASS: Reverse evacuation route found (2 to 1).\n"
    : "FAIL: Reverse evacuation route failed.\n");

cout << (reverseReachabilityPass
    ? "PASS: Reverse BFS reaches both locations.\n"
    : "FAIL: Reverse BFS test failed.\n");
    cout << (emergencyPass
        ? "PASS: Evacuation route found.\n"
        : "FAIL: Evacuation route not found.\n");

    bool blockApplied = emergency.blockConnection(1, 1);

    RouteResult emergencyBlockedRoute =
        emergency.findEvacuationRoute(1, 1, GENERAL);

    bool emergencyBlockPass =
        blockApplied && !emergencyBlockedRoute.found;

    cout << (emergencyBlockPass
        ? "PASS: No evacuation route after blocking connection.\n"
        : "FAIL: Emergency blocked-path test failed.\n");

    // 4. MEMBER 3 - WHAT-IF SIMULATION

    
    WhatIfSimulator simulator(building);

    cout << "\n--- MEMBER 3: SIMULATION ---\n";

    cout << "Original scenario:\n";
    simulator.compare(1, false);

    simulator.blockConnection(1);

    cout << "\nAfter blocking connection:\n";
    simulator.compare(1, false);

    // Verify simulation reachability automatically

    SimulationGraph simulationCheck(building);

    int before = simulationCheck.countReachable(1, false);

    simulationCheck.blockConnection(1);

    int after = simulationCheck.countReachable(1, false);

    bool simulationPass = (before == 2 && after == 1);

    cout << (simulationPass
        ? "PASS: Accessibility decreases after blocking connection.\n"
        : "FAIL: Simulation reachability test failed.\n");
    // 5. MEMBER 3 - REVIEW VALIDATION

    cout << "\n--- MEMBER 3: REVIEW VALIDATION ---\n";

    ReviewManager reviewManager;
    reviewManager.setBuildingData(building);

    Review review1 = {
        101, 1, 1, 5,
        "Ramp issue",
        "Ramp is too steep.",
        true
    };

    Review review2 = {
        102, 1, 1, 4,
        "Door issue",
        "Door is difficult to open.",
        true
    };

    Review invalidReview = {
        103, 1, 99, 5,
        "Invalid location",
        "This location does not exist.",
        true
    };

    Review duplicateReview = {
        101, 1, 2, 3,
        "Duplicate",
        "This review ID already exists.",
        true
    };

    reviewManager.addReview(review1);
    reviewManager.addReview(review2);
    reviewManager.addReview(invalidReview);
    reviewManager.addReview(duplicateReview);

    vector<Review> storedReviews =
        reviewManager.getReviews(1);

    bool reviewValidationPass =
        storedReviews.size() == 2;

    cout << (reviewValidationPass
        ? "PASS: Valid reviews accepted; invalid and duplicate reviews rejected.\n"
        : "FAIL: Review validation test failed.\n");


    // 6. MEMBER 3 - PRIORITY RANKING

    cout << "\n--- MEMBER 3: PRIORITY RANKING ---\n";

    ReviewOptimizer reviewOptimizer;

    ostringstream rankingOutput;
    streambuf* oldOutput = cout.rdbuf(rankingOutput.rdbuf());

    reviewOptimizer.generateRanking(storedReviews, 1);

    cout.rdbuf(oldOutput);

    bool rankingPass =
        rankingOutput.str().find("Priority score: 11")
        != string::npos;

    cout << (rankingPass
        ? "PASS: Review priority score calculated correctly.\n"
        : "FAIL: Review priority calculation failed.\n");


    // 7. MEMBER 3 - MAX HEAP EMPTY AND ORDER TEST

    MaxHeap testHeap;

    ProblemSummary lowPriority = {1, 1, 1, 5, 5};
    ProblemSummary highPriority = {2, 1, 1, 10, 10};

    testHeap.push(lowPriority);
    testHeap.push(highPriority);

    ProblemSummary first = testHeap.pop();
    ProblemSummary second = testHeap.pop();
    ProblemSummary emptyResult = testHeap.pop();

    bool heapPass =
        first.locationId == 2 &&
        second.locationId == 1 &&
        testHeap.empty() &&
        emptyResult.locationId == 0;

    cout << (heapPass
        ? "PASS: Max heap ordering and empty-pop safety verified.\n"
        : "FAIL: Max heap test failed.\n");
    
    // 5. FINAL INTEGRATION RESULT

    cout << "\n===== FINAL TEST RESULTS =====\n";

    cout << "Member 1 normal navigation: "
         << (navigationPass ? "PASS" : "FAIL") << "\n";

    cout << "Member 1 blocked navigation: "
         << (navigationBlockPass ? "PASS" : "FAIL") << "\n";

    cout << "Member 2 emergency route: "
         << (emergencyPass ? "PASS" : "FAIL") << "\n";

    cout << "Member 2 blocked emergency: "
         << (emergencyBlockPass ? "PASS" : "FAIL") << "\n";

    cout << "Member 3 what-if simulation: "
         << (simulationPass ? "PASS" : "FAIL") << "\n";
     cout << "Member 3 review validation: "
     << (reviewValidationPass ? "PASS" : "FAIL") << "\n";

cout << "Member 3 priority ranking: "
     << (rankingPass ? "PASS" : "FAIL") << "\n";

cout << "Member 3 max heap: "
     << (heapPass ? "PASS" : "FAIL") << "\n";

    if (navigationLoadPass &&
    emergencyLoadPass &&
    navigationPass &&
    navigationBlockPass &&
    emergencyPass &&
    emergencyBlockPass &&
    simulationPass && reviewValidationPass &&
    rankingPass &&
    heapPass ) {

        cout << "\nALL BACKEND INTEGRATION TESTS PASSED!\n";
        return 0;
    }

    cout << "\nBACKEND INTEGRATION TEST FAILED.\n";
    return 1;
}
