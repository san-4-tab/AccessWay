
#include <iostream>
#include "member1_navigation/Building.h"
#include "member1_navigation/Floor.h"
#include "member1_navigation/Location.h"
#include "member1_navigation/Graph.h"
#include "member1_navigation/Accessibility.h"
#include "member1_navigation/MinHeap.h"

using namespace std;

int main() {

    cout << "===== ACCESSWAY MEMBER 1 COMPLETE TEST =====\n";

    // 1. BUILDING TEST
    cout << "\n--- BUILDING TEST ---\n";

    BuildingManager bm;

    Building b1 = {101, "Main Building", 3};
    Building b2 = {102, "Library", 2};

    bm.addBuilding(b1);
    bm.addBuilding(b2);

    cout << "\nAll Buildings:\n";
    bm.displayBuildings();

    cout << "\nSearch Building 101:\n";
    bm.searchBuilding(101);

    // 2. FLOOR TEST
    cout << "\n--- FLOOR TEST ---\n";

    FloorManager fm;

    fm.addFloor(101, 0, "Ground Floor");
    fm.addFloor(101, 1, "First Floor");

    cout << "\nAll Floors:\n";
    fm.displayFloors(101);

    cout << "\nSearch Floor 0: "
         << fm.searchFloor(101, 0) << endl;

    cout << "\nDeleting Floor 1...\n";
    fm.deleteFloor(101, 1);

    cout << "\nFloors after deletion:\n";
    fm.displayFloors(101);

    // 3. LOCATION TEST
    cout << "\n--- LOCATION TEST ---\n";

    LocationManager lm;

    Location l1 = {1, 101, "Entrance", 0, ENTRANCE};
    Location l2 = {2, 101, "Corridor", 0, CORRIDOR};
    Location l3 = {3, 101, "Library", 1, ROOM};

    lm.addLocation(l1);
    lm.addLocation(l2);
    lm.addLocation(l3);

    cout << "\nAll Locations:\n";
    lm.displayLocations(101);

    cout << "\nLocations on Floor 0:\n";
    lm.displayFloorLocations(101, 0);

    cout << "\nSearch Location 2:\n";

    Location* foundLocation = lm.searchLocation(2);

    if (foundLocation != nullptr) {
        cout << "Found: " << foundLocation->locationName << endl;
    } else {
        cout << "Location not found.\n";
    }

    // 4. MIN HEAP TEST
    cout << "\n--- MIN HEAP TEST ---\n";

    MinHeap heap(10);

    heap.insert(1, 30);
    heap.insert(2, 10);
    heap.insert(3, 20);
    heap.insert(4, 5);

    cout << "\nHeap after insertion:\n";
    heap.display();

    cout << "\nIs heap empty? "
         << heap.isEmpty() << endl;

    cout << "Does heap contain location 2? "
         << heap.contains(2) << endl;

    cout << "\nDecrease key of location 1 to 3:\n";
    heap.decreaseKey(1, 3);
    heap.display();

    int locationId;
    float distance;

    cout << "\nExtract minimum:\n";
    heap.extractMin(locationId, distance);

    cout << "Extracted Location: "
         << locationId << endl;

    cout << "Extracted Distance: "
         << distance << endl;

    cout << "\nHeap after extraction:\n";
    heap.display();

    // 5. GRAPH TEST
    cout << "\n--- GRAPH TEST ---\n";

    Graph g;

    g.addVertex(l1);
    g.addVertex(l2);
    g.addVertex(l3);

    Connection c1 = {
        1, 101, 1, 2,
        10, 1.5, 0,
        false, false, false, true,
        true, true
    };

    Connection c2 = {
        2, 101, 2, 3,
        15, 1.5, 0,
        false, false, false, true,
        true, true
    };

    Connection c3 = {
        3, 101, 1, 3,
        40, 1.5, 0,
        false, false, false, true,
        true, true
    };

    g.addEdge(c1);
    g.addEdge(c2);
    g.addEdge(c3);

    cout << "\nGraph:\n";
    g.displayGraph();

    cout << "\nNumber of vertices: "
         << g.getVertexCount() << endl;

    cout << "\nNeighbours of Location 1:\n";
    g.displayNeighbours(1);

    cout << "\nBFS Traversal:\n";
    g.BFS(1);

    // 6. DIJKSTRA TEST
    cout << "\n--- DIJKSTRA TEST ---\n";

    Graph::RouteResult r =
        g.dijkstra(1, 3, GENERAL);

    if (r.found) {
        cout << "Shortest Route: ";

        for (int i = 0; i < r.pathSize; i++) {
            cout << r.path[i] << " ";
        }

        cout << "\nTotal Cost: " << r.totalCost << endl;
    } else {
        cout << "No route found.\n";
    }

    delete[] r.path;

    // 7. ALTERNATIVE ROUTE TEST
    cout << "\n--- ALTERNATIVE ROUTE TEST ---\n";

    Graph::RouteResult alt =
        g.findAlternativeRoute(1, 3, GENERAL, 1);

    if (alt.found) {
        cout << "Alternative Route: ";

        for (int i = 0; i < alt.pathSize; i++) {
            cout << alt.path[i] << " ";
        }

        cout << "\nAlternative Cost: "
             << alt.totalCost << endl;
    } else {
        cout << "No alternative route found.\n";
    }

    delete[] alt.path;

    // 8. GRAPH EDGE REMOVAL TEST
    cout << "\n--- EDGE REMOVAL TEST ---\n";

    cout << "Removing connection 3...\n";
    g.removeEdge(3);

    cout << "\nGraph after removing connection 3:\n";
    g.displayGraph();

    cout << "\nRemoving connection 2...\n";
    g.removeEdge(2);

    cout << "\nGraph after removing connection 2:\n";
    g.displayGraph();

    cout << "\nTrying route from 1 to 3:\n";

    Graph::RouteResult noRoute =
        g.dijkstra(1, 3, GENERAL);

    if (noRoute.found) {
        cout << "Route found.\n";
    } else {
        cout << "No route found, as expected.\n";
    }

    delete[] noRoute.path;

    // 9. ACCESSIBILITY TEST
    cout << "\n--- ACCESSIBILITY TEST ---\n";

    Accessibility ac;

    Connection narrow = {
        4, 101, 1, 2,
        10, 0.6, 0,
        false, false, false, true,
        true, true
    };

    cout << "\nNarrow Path:\n";

    cout << "Wheelchair allowed: "
         << ac.isEdgeAllowed(narrow, WHEELCHAIR) << endl;

    cout << "Crutches allowed: "
         << ac.isEdgeAllowed(narrow, CRUTCHES) << endl;

    Connection wide = {
        5, 101, 1, 2,
        10, 1.5, 0,
        false, false, false, true,
        true, true
    };

    cout << "\nWide Path:\n";

    cout << "Wheelchair allowed: "
         << ac.isEdgeAllowed(wide, WHEELCHAIR) << endl;

    Connection stairs = {
        6, 101, 1, 2,
        10, 1.5, 0,
        true, false, false, true,
        true, true
    };

    cout << "\nStairs:\n";

    cout << "Wheelchair allowed: "
         << ac.isEdgeAllowed(stairs, WHEELCHAIR) << endl;

    cout << "Limited mobility allowed: "
         << ac.isEdgeAllowed(stairs, LIMITED_MOBILITY) << endl;

    Connection door = {
        7, 101, 1, 2,
        10, 1.5, 0,
        false, false, false, false,
        true, true
    };

    cout << "\nAutomatic Door Test:\n";

    cout << "Hand disability penalty: "
         << ac.getAccessibilityPenalty(door, HAND_DISABILITY)
         << endl;

    Connection slope = {
        8, 101, 1, 2,
        10, 1.5, 10,
        false, true, false, true,
        true, true
    };

    cout << "\nSlope Warning Test:\n";
    ac.displayWarnings(slope, WHEELCHAIR);

    Connection unavailable = {
        9, 101, 1, 2,
        10, 1.5, 0,
        false, false, false, true,
        false, true
    };

    cout << "\nUnavailable Connection Test:\n";

    cout << "Connection allowed: "
         << ac.isEdgeAllowed(unavailable, GENERAL) << endl;

    // 10. DELETE TESTS
    cout << "\n--- DELETE TESTS ---\n";

    cout << "\nDeleting Location 2...\n";
    lm.deleteLocation(2);
    lm.displayLocations(101);

    cout << "\nDeleting Building 102...\n";
    bm.deleteBuilding(102);
    bm.displayBuildings();

    cout << "\n===== ALL TESTS COMPLETED =====\n";

    return 0;
}
