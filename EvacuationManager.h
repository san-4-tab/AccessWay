
#ifndef EVACUATION_MANAGER_H
#define EVACUATION_MANAGER_H

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "common/AccessWayData.h"
#include "common/EmergencyData.h"

using namespace std;

// Structured result
struct RouteResult {
    bool found;
    int buildingId;
    int startLocationId;
    int destinationLocationId;
    bool usedRefugeArea;
    float totalDistance;
    vector<int> path;
    vector<string> pathNames;
    string message;
};

typedef RouteResult EvacuationResult;

enum RouteMode {
    MODE_EMERGENCY,
    MODE_NORMAL
};

class EvacuationManager {

private:

    struct BuildingGraph {
        unordered_map<int, Location> locations;
        unordered_map<int, Connection> connections;
        unordered_map<int, vector<int> > adjacency;
        unordered_set<int> unavailableLocations;
    };

    unordered_map<int, BuildingGraph> graphs;

    bool isUsable(
        const BuildingGraph& g,
        const Connection& c,
        int targetLocationId,
        MobilityProfile profile,
        RouteMode mode
    ) const;

    void dijkstra(
        const BuildingGraph& g,
        int start,
        MobilityProfile profile,
        RouteMode mode,
        unordered_map<int, float>& dist,
        unordered_map<int, int>& parent
    ) const;

    void buildPath(
        const BuildingGraph& g,
        const unordered_map<int, int>& parent,
        int start,
        int target,
        RouteResult& result
    ) const;

    static bool isExitType(LocationType t);

public:

    // Building data
    bool addLocation(const Location& location);
    bool addConnection(const Connection& connection);
    bool loadBuildingData(const BuildingData& data);

    bool hasBuilding(int buildingId) const;
    bool hasLocation(int buildingId, int locationId) const;
    string getLocationName(int buildingId, int locationId) const;

    // Blocked paths / unavailable facilities
    bool blockConnection(int buildingId, int connectionId);
    bool restoreConnection(int buildingId, int connectionId);

    bool setLocationAvailability(
        int buildingId,
        int locationId,
        bool available
    );

    vector<int> getBlockedConnections(int buildingId) const;
    vector<int> getUnavailableLocations(int buildingId) const;

    // Routing
    RouteResult findEvacuationRoute(
        int buildingId,
        int startLocationId,
        MobilityProfile profile = GENERAL
    ) const;

    RouteResult findRoute(
        int buildingId,
        int fromLocationId,
        int toLocationId,
        MobilityProfile profile = GENERAL
    ) const;

    // Graph analysis
    vector<int> findReachableLocations(
        int buildingId,
        int startLocationId,
        MobilityProfile profile = GENERAL,
        RouteMode mode = MODE_NORMAL
    ) const;

    vector<int> getSafeLocations(int buildingId) const;

    vector<int> findTrappedLocations(
        int buildingId,
        MobilityProfile profile = GENERAL
    ) const;

    void displayRoute(const RouteResult& route) const;

    // Persistence
    bool saveStatus(const string& filename) const;
    bool loadStatus(const string& filename);
};

#endif
