// EvacuationManager.cpp  (Member 2)
#include "EvacuationManager.h"
#include "VolunteerManager.h"   // splitLine()
#include <queue>
#include <functional>
#include <algorithm>
#include <fstream>
#include <limits>
#include <iostream>
// ------------------------------------------------------------------ data

// Keep accessibility penalties consistent with Member 1
static float getAccessibilityPenalty(
    const Connection& c,
    MobilityProfile profile
) {
    float penalty = 0;

    switch (profile) {
        case WHEELCHAIR:
            if (c.hasRamp)
                penalty += 3;
            break;

        case CRUTCHES:
            if (c.hasStairs)
                penalty += 10;
            if (c.width < 1.0)
                penalty += 5;
            break;

        case LIMITED_MOBILITY:
            if (c.hasStairs)
                penalty += 10;
            if (c.width < 1.0)
                penalty += 5;
            break;

        case HAND_DISABILITY:
            if (!c.automaticDoor)
                penalty += 10;
            break;

        case GENERAL:
            break;
    }

    return penalty;
}

bool EvacuationManager::addLocation(const Location& loc) {
    BuildingGraph& g = graphs[loc.buildingId];
    if (g.locations.count(loc.locationId)) return false;
    g.locations[loc.locationId] = loc;
    g.adjacency[loc.locationId];                       // make empty list
    return true;
}

bool EvacuationManager::addConnection(const Connection& c) {
    auto it = graphs.find(c.buildingId);

    if (it == graphs.end())
        return false;

    BuildingGraph& g = it->second;

    if (!g.locations.count(c.fromLocationId) ||
        !g.locations.count(c.toLocationId))
        return false;

    if (g.connections.count(c.connectionId))
        return false;

    g.connections[c.connectionId] = c;

    // Store the same physical connection at both endpoints.
    g.adjacency[c.fromLocationId].push_back(c.connectionId);
    g.adjacency[c.toLocationId].push_back(c.connectionId);

    return true;
}
// Load the common building dataset into emergency management
bool EvacuationManager::loadBuildingData(const BuildingData& data) {

    if (data.locations.empty())
        return false;

    int buildingId = data.locations[0].buildingId;

    BuildingGraph newGraph;

    // Load locations
    for (const Location& loc : data.locations) {

        if (loc.buildingId != buildingId ||
            loc.locationId <= 0)
            return false;

        if (newGraph.locations.count(loc.locationId))
            return false;

        newGraph.locations[loc.locationId] = loc;
        newGraph.adjacency[loc.locationId];
    }

    // Load connections
    for (const Connection& c : data.connections) {

        if (c.buildingId != buildingId ||
            c.connectionId <= 0 ||
            c.fromLocationId == c.toLocationId ||
            c.distance < 0)
            return false;

        if (!newGraph.locations.count(c.fromLocationId) ||
            !newGraph.locations.count(c.toLocationId))
            return false;

        if (newGraph.connections.count(c.connectionId))
            return false;

        newGraph.connections[c.connectionId] = c;

        // Both directions use the same connection ID.
        newGraph.adjacency[c.fromLocationId].push_back(
            c.connectionId
        );

        newGraph.adjacency[c.toLocationId].push_back(
            c.connectionId
        );
    }

    // Replace existing graph only after validation succeeds.
    graphs[buildingId] = newGraph;

    cout << "Common building data loaded into emergency graph.\n";

    return true;
}

bool EvacuationManager::hasBuilding(int b) const { return graphs.find(b) != graphs.end(); }

bool EvacuationManager::hasLocation(int b, int loc) const {
    auto it = graphs.find(b);
    return it != graphs.end() && it->second.locations.count(loc) > 0;
}

string EvacuationManager::getLocationName(int b, int loc) const {
    auto it = graphs.find(b);
    if (it == graphs.end()) return "?";
    auto l = it->second.locations.find(loc);
    return l == it->second.locations.end() ? "?" : l->second.locationName;
}

// ------------------------------------------------- blocked / unavailable
bool EvacuationManager::blockConnection(int b, int connectionId) {
    auto it = graphs.find(b);
    if (it == graphs.end()) return false;
    auto c = it->second.connections.find(connectionId);
    if (c == it->second.connections.end()) return false;
    c->second.available = false;
    return true;
}

bool EvacuationManager::restoreConnection(int b, int connectionId) {
    auto it = graphs.find(b);
    if (it == graphs.end()) return false;
    auto c = it->second.connections.find(connectionId);
    if (c == it->second.connections.end()) return false;
    c->second.available = true;
    return true;
}

bool EvacuationManager::setLocationAvailability(int b, int locationId, bool available) {
    auto it = graphs.find(b);
    if (it == graphs.end() || !it->second.locations.count(locationId)) return false;
    if (available) it->second.unavailableLocations.erase(locationId);
    else it->second.unavailableLocations.insert(locationId);
    return true;
}

vector<int> EvacuationManager::getBlockedConnections(int b) const {
    vector<int> result;
    auto it = graphs.find(b);
    if (it == graphs.end()) return result;
    for (auto& kv : it->second.connections)
        if (!kv.second.available) result.push_back(kv.first);
    sort(result.begin(), result.end());
    return result;
}

vector<int> EvacuationManager::getUnavailableLocations(int b) const {
    vector<int> result;
    auto it = graphs.find(b);
    if (it == graphs.end()) return result;
    result.assign(it->second.unavailableLocations.begin(), it->second.unavailableLocations.end());
    sort(result.begin(), result.end());
    return result;
}

// ---------------------------------------------------------------- helpers
bool EvacuationManager::isExitType(LocationType t) {
    return t == EXIT || t == ENTRANCE;
}

bool EvacuationManager::isUsable(
    const BuildingGraph& g,
    const Connection& c,
    int targetLocationId,
    MobilityProfile profile,
    RouteMode mode
) const {

    if (!c.available)
        return false;

    auto target = g.locations.find(targetLocationId);

    if (target == g.locations.end())
        return false;

    if (g.unavailableLocations.count(targetLocationId))
        return false;

    if (mode == MODE_EMERGENCY) {

        if (!c.emergencyApproved)
            return false;

        if (c.hasElevator ||
            target->second.type == ELEVATOR)
            return false;
    }

switch (profile) {

    case WHEELCHAIR:
        if (c.width < 0.9f)
            return false;

        if (c.hasStairs && !c.hasRamp && !c.hasElevator)
            return false;
        break;

    case CRUTCHES:
        if (c.width < 0.7f)
            return false;
        break;

    case LIMITED_MOBILITY:
        if (c.hasStairs && !c.hasRamp && !c.hasElevator)
            return false;
        break;

    case HAND_DISABILITY:
        // Non-automatic doors are allowed but receive a penalty
        break;

    case GENERAL:
        break;
}

    return true;
}

void EvacuationManager::dijkstra(const BuildingGraph& g, int start, MobilityProfile profile,
                                 RouteMode mode, unordered_map<int, float>& dist,
                                 unordered_map<int, int>& parent) const {
    typedef pair<float, int> Node;                                    // (distance, location)
    priority_queue<Node, vector<Node>, greater<Node> > pq;            // min-heap
    dist[start] = 0.0f;
    pq.push(Node(0.0f, start));

    while (!pq.empty()) {
        Node top = pq.top(); pq.pop();
        float d = top.first;
        int u = top.second;
        if (d > dist[u]) continue;                                    // stale entry

        auto adj = g.adjacency.find(u);
        if (adj == g.adjacency.end()) continue;
        for (size_t i = 0; i < adj->second.size(); i++) {

    const Connection& c =
        g.connections.at(adj->second[i]);

    // Find the opposite endpoint.
    int nextLocation =
        (c.fromLocationId == u)
        ? c.toLocationId
        : c.fromLocationId;

    if (!isUsable(g, c, nextLocation, profile, mode))
        continue;

    float nd = d + c.distance +
               getAccessibilityPenalty(c, profile);

    auto dv = dist.find(nextLocation);

    if (dv == dist.end() || nd < dv->second) {

        dist[nextLocation] = nd;
        parent[nextLocation] = u;

        pq.push(Node(nd, nextLocation));
    }
}
    }
}

void EvacuationManager::buildPath(const BuildingGraph& g, const unordered_map<int, int>& parent,
                                  int start, int target, RouteResult& result) const {
    vector<int> rev;
    int cur = target;
    rev.push_back(cur);
    while (cur != start) {
        cur = parent.at(cur);
        rev.push_back(cur);
    }
    result.path.assign(rev.rbegin(), rev.rend());
    result.pathNames.clear();
    for (size_t i = 0; i < result.path.size(); i++)
        result.pathNames.push_back(g.locations.at(result.path[i]).locationName);
}

// ---------------------------------------------------------------- routing
RouteResult EvacuationManager::findEvacuationRoute(int b, int start, MobilityProfile profile) const {
    RouteResult r;
    r.found = false; r.buildingId = b; r.startLocationId = start;
    r.destinationLocationId = -1; r.usedRefugeArea = false; r.totalDistance = 0;

    auto git = graphs.find(b);
    if (git == graphs.end()) { r.message = "Building not found."; return r; }
    const BuildingGraph& g = git->second;
    if (!g.locations.count(start)) { r.message = "Start location not found."; return r; }

    unordered_map<int, float> dist;
    unordered_map<int, int> parent;
    dijkstra(g, start, profile, MODE_EMERGENCY, dist, parent);

    int best = -1;
    float bestDist = numeric_limits<float>::max();
    // 1) nearest usable exit
    for (auto& kv : dist) {
        const Location& loc = g.locations.at(kv.first);
        if (!isExitType(loc.type) || g.unavailableLocations.count(kv.first)) continue;
        if (kv.second < bestDist || (kv.second == bestDist && kv.first < best)) {
            best = kv.first; bestDist = kv.second;
        }
    }
    // 2) otherwise nearest refuge area
    if (best == -1) {
        for (auto& kv : dist) {
            const Location& loc = g.locations.at(kv.first);
            if (loc.type != REFUGE_AREA || g.unavailableLocations.count(kv.first)) continue;
            if (kv.second < bestDist || (kv.second == bestDist && kv.first < best)) {
                best = kv.first; bestDist = kv.second;
            }
        }
        if (best != -1) r.usedRefugeArea = true;
    }

    if (best == -1) {
        r.message = "No exit or refuge area reachable. Call for assistance!";
        return r;
    }
    r.found = true;
    r.destinationLocationId = best;
    r.totalDistance = bestDist;
    buildPath(g, parent, start, best, r);
    r.message = r.usedRefugeArea ? "No exit reachable - go to the refuge area and wait for help."
                                 : "Evacuate through the nearest exit.";
    return r;
}

RouteResult EvacuationManager::findRoute(int b, int from, int to, MobilityProfile profile) const {
    RouteResult r;
    r.found = false; r.buildingId = b; r.startLocationId = from;
    r.destinationLocationId = to; r.usedRefugeArea = false; r.totalDistance = 0;

    auto git = graphs.find(b);
    if (git == graphs.end()) { r.message = "Building not found."; return r; }
    const BuildingGraph& g = git->second;
    if (!g.locations.count(from) || !g.locations.count(to)) { r.message = "Location not found."; return r; }

    unordered_map<int, float> dist;
    unordered_map<int, int> parent;
    dijkstra(g, from, profile, MODE_NORMAL, dist, parent);

    auto d = dist.find(to);
    if (d == dist.end()) { r.message = "No route available."; return r; }
    r.found = true;
    r.totalDistance = d->second;
    buildPath(g, parent, from, to, r);
    r.message = "Route found.";
    return r;
}

// ---------------------------------------------------------- graph analysis
vector<int> EvacuationManager::findReachableLocations(int b, int start, MobilityProfile profile,
                                                      RouteMode mode) const {
    vector<int> result;
    auto git = graphs.find(b);
    if (git == graphs.end() || !git->second.locations.count(start)) return result;
    const BuildingGraph& g = git->second;

    unordered_set<int> visited;
    queue<int> q;                                                      // BFS queue
    visited.insert(start);
    q.push(start);
    while (!q.empty()) {
        int u = q.front(); q.pop();
        result.push_back(u);
        auto adj = g.adjacency.find(u);
        if (adj == g.adjacency.end()) continue;
        for (size_t i = 0; i < adj->second.size(); i++) {

    const Connection& c =
        g.connections.at(adj->second[i]);

    int nextLocation =
        (c.fromLocationId == u)
        ? c.toLocationId
        : c.fromLocationId;

    if (!isUsable(g, c, nextLocation, profile, mode))
        continue;

    if (visited.count(nextLocation))
        continue;

    visited.insert(nextLocation);
    q.push(nextLocation);
}
        }
    }
    sort(result.begin(), result.end());
    return result;


vector<int> EvacuationManager::getSafeLocations(int b) const {
    vector<int> result;
    auto git = graphs.find(b);
    if (git == graphs.end()) return result;
    for (auto& kv : git->second.locations) {
        LocationType t = kv.second.type;
        if ((isExitType(t) || t == REFUGE_AREA) && !git->second.unavailableLocations.count(kv.first))
            result.push_back(kv.first);
    }
    sort(result.begin(), result.end());
    return result;
}

// Locations from which NO safe place can be reached (reverse BFS from every safe place).
vector<int> EvacuationManager::findTrappedLocations(
    int b,
    MobilityProfile profile
) const {

    vector<int> trapped;

    auto git = graphs.find(b);

    if (git == graphs.end())
        return trapped;

    const BuildingGraph& g = git->second;

    // Build reverse adjacency for every usable physical connection.
    unordered_map<int, vector<int> > reverse;

    for (auto& kv : g.connections) {

        const Connection& c = kv.second;

        if (isUsable(g, c, c.toLocationId,
                     profile, MODE_EMERGENCY)) {

            reverse[c.toLocationId].push_back(
                c.fromLocationId
            );
        }

        if (isUsable(g, c, c.fromLocationId,
                     profile, MODE_EMERGENCY)) {

            reverse[c.fromLocationId].push_back(
                c.toLocationId
            );
        }
    }

    unordered_set<int> safe;
    queue<int> q;

    vector<int> sources = getSafeLocations(b);

    for (size_t i = 0; i < sources.size(); i++) {

        safe.insert(sources[i]);
        q.push(sources[i]);
    }

    // Reverse BFS from every safe location.
    while (!q.empty()) {

        int u = q.front();
        q.pop();

        for (size_t i = 0; i < reverse[u].size(); i++) {

            int v = reverse[u][i];

            if (safe.insert(v).second)
                q.push(v);
        }
    }

    for (auto& kv : g.locations) {

        if (!safe.count(kv.first) &&
            kv.second.type != ELEVATOR) {

            trapped.push_back(kv.first);
        }
    }

    sort(trapped.begin(), trapped.end());

    return trapped;
}

void EvacuationManager::displayRoute(const RouteResult& route) const {
    cout << route.message << "\n";
    if (!route.found) return;
    cout << "Route: ";
    for (size_t i = 0; i < route.pathNames.size(); i++) {
        cout << route.pathNames[i];
        if (i + 1 < route.pathNames.size()) cout << " -> ";
    }
    cout << "  (" << route.totalDistance << " m)\n";
}

// ------------------------------------------------------------ persistence
bool EvacuationManager::saveStatus(const string& filename) const {
    ofstream out(filename.c_str());
    if (!out) return false;
    for (auto& g : graphs) {
        vector<int> blocked = getBlockedConnections(g.first);
        for (size_t i = 0; i < blocked.size(); i++)
            out << "BLOCKED_CONNECTION|" << g.first << '|' << blocked[i] << '\n';
        vector<int> unavailable = getUnavailableLocations(g.first);
        for (size_t i = 0; i < unavailable.size(); i++)
            out << "UNAVAILABLE_LOCATION|" << g.first << '|' << unavailable[i] << '\n';
    }
    return true;
}

bool EvacuationManager::loadStatus(const string& filename) {
    ifstream in(filename.c_str());
    if (!in) return false;
    string line;
    while (getline(in, line)) {
        vector<string> p = splitLine(line);
        if (p.size() < 3) continue;
        try {
            int b = stoi(p[1]), id = stoi(p[2]);
            if (p[0] == "BLOCKED_CONNECTION") blockConnection(b, id);
            else if (p[0] == "UNAVAILABLE_LOCATION") setLocationAvailability(b, id, false);
        } catch (...) {}
    }
    return true;
}
