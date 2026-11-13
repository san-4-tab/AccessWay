
#include <iostream>

#include "common/AccessWayData.h"
#include "common/EmergencyData.h"

#include "member1_navigation/Graph.h"
#include "EmergencyManager.h"
#include "member3/WhatIfSimulator.h"

using namespace std;

int main() {

    cout << "\n===== ACCESSWAY BACKEND INTEGRATION TEST =====\n";

    // 1. Create ONE shared building dataset

    Location entrance = {
        1, 1, "Main Entrance", 0, ENTRANCE
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

    // 2. MEMBER 1 - Navigation

    Graph navigation;

    navigation.addVertex(entrance);
    navigation.addVertex(exitLocation);
    navigation.addEdge(path);

    Graph::RouteResult normalRoute =
        navigation.dijkstra(1, 2, GENERAL);

    cout << "\n--- MEMBER 1: NAVIGATION ---\n";

    if (normalRoute.found)
        cout << "Navigation route found.\n";
    else
        cout << "Navigation route failed.\n";

    delete[] normalRoute.path;
    // Test blocked connection
navigation.removeEdge(1);

Graph::RouteResult blockedRoute =
    navigation.dijkstra(1, 2, GENERAL);

cout << "\n--- BLOCKED ROUTE TEST ---\n";

if (!blockedRoute.found)
    cout << "PASS: No route available after blocking connection.\n";
else
    cout << "FAIL: Route still exists after blocking connection.\n";

delete[] blockedRoute.path;
    // 3. MEMBER 2 - Emergency Management

    EmergencyManager emergency;

    emergency.addLocation(entrance);
    emergency.addLocation(exitLocation);
    emergency.addConnection(path);

    RouteResult evacuationRoute =
        emergency.findEvacuationRoute(1, 1, GENERAL);

    cout << "\n--- MEMBER 2: EMERGENCY ---\n";

    if (evacuationRoute.found)
        cout << "Evacuation route found.\n";
    else
        cout << "Evacuation route failed.\n";
// Test emergency route after blocking the only connection
emergency.blockConnection(1, 1);

RouteResult emergencyBlockedRoute =
    emergency.findEvacuationRoute(1, 1, GENERAL);

cout << "\n--- EMERGENCY BLOCKED PATH TEST ---\n";

if (!emergencyBlockedRoute.found)
    cout << "PASS: Emergency route unavailable after blocking path.\n";
else
    cout << "FAIL: Emergency route still exists after blocking path.\n";
    // 4. MEMBER 3 - What-If Simulation

    BuildingData building;

    building.locations.push_back(entrance);
    building.locations.push_back(exitLocation);
    building.connections.push_back(path);

    WhatIfSimulator simulator(building);

    cout << "\n--- MEMBER 3: SIMULATION ---\n";

    cout << "Original scenario:\n";
    simulator.compare(1, false);

    simulator.blockConnection(1);

    cout << "\nAfter blocking connection:\n";
    simulator.compare(1, false);

    // 5. Final result

  if (normalRoute.found &&
    evacuationRoute.found &&
    !blockedRoute.found &&
    !emergencyBlockedRoute.found) {
        cout << "\nALL THREE MODULES CONNECTED SUCCESSFULLY!\n";
        return 0;
    }

    cout << "\nINTEGRATION TEST FAILED.\n";
    return 1;
}
