
#include <iostream>
#include "member1_navigation/Building.h"
#include "member1_navigation/Floor.h"
#include "member1_navigation/Location.h"
#include "member1_navigation/Graph.h"
#include "member1_navigation/Accessibility.h"
using namespace std;

int main() {
    cout << "===== ACCESSWAY MEMBER 1 TEST =====\n";

    // Test Building Manager
    BuildingManager bm;

    Building b1 = {101, "Main Building", 3};
    Building b2 = {102, "Library", 2};

    bm.addBuilding(b1);
    bm.addBuilding(b2);

    cout << "\nBuildings:\n";
    bm.displayBuildings();

    // Test Floor Manager
    FloorManager fm;

    fm.addFloor(101, 0, "Ground Floor");
    fm.addFloor(101, 1, "First Floor");

    cout << "\nFloors:\n";
    fm.displayFloors(101);

    // Test Location Manager
    LocationManager lm;

    Location l1 = {1, 101, "Entrance", 0, ENTRANCE};
    Location l2 = {2, 101, "Corridor", 0, CORRIDOR};
    Location l3 = {3, 101, "Library", 1, ROOM};

    lm.addLocation(l1);
    lm.addLocation(l2);
    lm.addLocation(l3);

    cout << "\nLocations:\n";
    lm.displayLocations(101);

    // Test Graph
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

    g.addEdge(c1);
    g.addEdge(c2);

    Connection c3 = {
    3, 101, 1, 3,
    40, 1.5, 0,
    false, false, false, true,
    true, true
};

g.addEdge(c3);
    cout << "\nGraph:\n";
    g.displayGraph();

    cout << "\nBFS Traversal:\n";
    g.BFS(1);

    // Test Dijkstra
    cout << "\nShortest Route:\n";

    Graph::RouteResult r =
        g.dijkstra(1, 3, GENERAL);

    if (r.found) {
        cout << "Route: ";

        for (int i = 0; i < r.pathSize; i++) {
            cout << r.path[i] << " ";
        }

        cout << "\nTotal Cost: " << r.totalCost << endl;
    } else {
        cout << "No route found.\n";
    }

    delete[] r.path;
cout << "\nAlternative Route:\n";

Graph::RouteResult alt =
    g.findAlternativeRoute(1, 3, GENERAL, 1);

if (alt.found) {
    cout << "Alternative Route: ";

    for (int i = 0; i < alt.pathSize; i++) {
        cout << alt.path[i] << " ";
    }

    cout << "\nAlternative Cost: "
         << alt.totalCost << endl;
}
else {
    cout << "No alternative route found.\n";
}

delete[] alt.path;

    // Test Accessibility
cout << "\nAccessibility Test:\n";

Accessibility ac;

Connection narrow = {
    4, 101, 1, 2,
    10, 0.6, 0,
    false, false, false, true,
    true, true
};

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

cout << "Wheelchair allowed on wide path: "
     << ac.isEdgeAllowed(wide, WHEELCHAIR) << endl;

   

    Connection stairs = {
    6, 101, 1, 2,
    10, 1.5, 0,
    true, false, false, true,
    true, true
};

    cout << "Wheelchair allowed on stairs: "
     << ac.isEdgeAllowed(stairs, WHEELCHAIR) << endl;
    
 cout << "Limited mobility allowed on stairs: "
     << ac.isEdgeAllowed(stairs, LIMITED_MOBILITY) << endl;
    


// Test stairs accessibility
Connection stairs = {
    6, 101, 1, 2,
    10, 1.5, 0,
    true, false, false, true,
    true, true
};

cout << "Wheelchair allowed on stairs: "
     << ac.isEdgeAllowed(stairs, WHEELCHAIR) << endl;

cout << "Limited mobility allowed on stairs: "
     << ac.isEdgeAllowed(stairs, LIMITED_MOBILITY) << endl;
    cout << "\n===== TEST COMPLETED =====\n";

    return 0;
}
