// EvacuationManager.h  (Member 2)
// Emergency routing on the building graph.
// DSA: graph (adjacency list), BFS (queue), Dijkstra (priority queue / min-heap),
//      hash tables (id -> location / connection).
#ifndef EVACUATION_MANAGER_H
#define EVACUATION_MANAGER_H

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "common/AccessWayData.h"
#include "common/EmergencyData.h"


// Structured result (nothing is only printed - callers get data back)
struct RouteResult {
    bool found;
    int buildingId;
    int startLocationId;
    int destinationLocationId;
    bool usedRefugeArea;          // true when no exit was reachable
    float totalDistance;
    vector<int> path;             // location ids from start to destination
    vector<string> pathNames;     // same path, readable names
    string message;
};
typedef RouteResult EvacuationResult;

enum RouteMode {
    MODE_EMERGENCY,   // only emergencyApproved connections, no elevators
    MODE_NORMAL       // every available connection (used for volunteer directions)
};

class EvacuationManager {
private:
    struct BuildingGraph {
        unordered_map<int, Location> locations;
        unordered_map<int, Connection> connections;
        unordered_map<int, vector<int> > adjacency;   // locationId -> outgoing connectionIds
        unordered_set<int> unavailableLocations;
    };

    unordered_map<int, BuildingGraph> graphs;          // buildingId -> graph

   bool isUsable(const BuildingGraph& g, const Connection& c,
              int targetLocationId,
              MobilityProfile profile, RouteMode mode) const;
    void dijkstra(const BuildingGraph& g, int start, MobilityProfile profile, RouteMode mode,
                  unordered_map<int, float>& dist, unordered_map<int, int>& parent) const;
    void buildPath(const BuildingGraph& g, const unordered_map<int, int>& parent,
                   int start, int target, RouteResult& result) const;
    static bool isExitType(LocationType t);

public:
    // ---- building data (from Member 1 later, dummy data for now) ----
    bool addLocation(const Location& location);
    bool addConnection(const Connection& connection);
bool loadBuildingData(const BuildingData& data);
    bool hasBuilding(int buildingId) const;
    bool hasLocation(int buildingId, int locationId) const;
    string getLocationName(int buildingId, int locationId) const;

    // ---- blocked paths / unavailable facilities ----
    bool blockConnection(int buildingId, int connectionId);
    bool restoreConnection(int buildingId, int connectionId);
    bool setLocationAvailability(int buildingId, int locationId, bool available);
    vector<int> getBlockedConnections(int buildingId) const;
    vector<int> getUnavailableLocations(int buildingId) const;

    // ---- routing ----
    // nearest exit; if none reachable, nearest refuge area
    RouteResult findEvacuationRoute(int buildingId, int startLocationId,
                                    MobilityProfile profile = GENERAL) const;
    // normal shortest route (volunteer -> visitor directions)
    RouteResult findRoute(int buildingId, int fromLocationId, int toLocationId,
                          MobilityProfile profile = GENERAL) const;

    // ---- graph analysis ----
    vector<int> findReachableLocations(int buildingId, int startLocationId,
                                       MobilityProfile profile = GENERAL,
                                       RouteMode mode = MODE_NORMAL) const;   // BFS
    vector<int> getSafeLocations(int buildingId) const;      // usable exits + refuge areas
    vector<int> findTrappedLocations(int buildingId,
                                     MobilityProfile profile = GENERAL) const; // reverse BFS from exits

    void displayRoute(const RouteResult& route) const;

    // emergency_status.txt (blocked connections + unavailable locations)
    bool saveStatus(const string& filename) const;
    bool loadStatus(const string& filename);
};

#endif
