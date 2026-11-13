
#include <iostream>

#include "common/AccessWayData.h"
#include "common/EmergencyData.h"

#include "member1_navigation/Graph.h"
#include "EmergencyManager.h"
#include "member3/WhatIfSimulator.h"

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

    if (navigationLoadPass &&
    emergencyLoadPass &&
    navigationPass &&
    navigationBlockPass &&
    emergencyPass &&
    emergencyBlockPass &&
    simulationPass) {

        cout << "\nALL BACKEND INTEGRATION TESTS PASSED!\n";
        return 0;
    }

    cout << "\nBACKEND INTEGRATION TEST FAILED.\n";
    return 1;
}
