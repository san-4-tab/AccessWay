
#ifndef ACCESSWAY_GRAPH_H
#define ACCESSWAY_GRAPH_H

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include "../common/AccessWayData.h"
using namespace std;



class SimulationGraph  {

private:
    vector<Location> locations;
    vector<Connection> connections;

    int findLocation(int id) const {
        for (int i = 0; i < (int)locations.size(); i++) {
            if (locations[i].locationId == id)
                return i;
        }

        return -1;
    }

    bool isConnectionAllowed(
        const Connection& c,
        MobilityProfile profile
    ) const {

        if (!c.available || c.distance < 0)
            return false;

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
            case GENERAL:
                break;
        }

        return true;
    }


public:
    SimulationGraph() {}

    SimulationGraph(const BuildingData& data) {
        locations = data.locations;
        connections = data.connections;
    }

    void blockConnection(int id) {
        for (Connection& c : connections) {
            if (c.connectionId == id) {
                c.available = false;
                return;
            }
        }

        cout << "Connection not found.\n";
    }

    void restoreConnection(int id) {
        for (Connection& c : connections) {
            if (c.connectionId == id) {
                c.available = true;
                return;
            }
        }

        cout << "Connection not found.\n";
    }

    void disableFacility(int locationId) {
        bool found = false;

        for (Connection& c : connections) {
            if (c.fromLocationId == locationId ||
                c.toLocationId == locationId) {
                c.available = false;
                found = true;
            }
        }

        if (!found)
            cout << "No connections found for this facility.\n";
    }


    // Keep compatibility with existing true/false calls
    int countReachable(int startId, bool wheelchair) const {
        return countReachable(
            startId,
            wheelchair ? WHEELCHAIR : GENERAL
        );
    }

    // Count reachable locations for any mobility profile
    int countReachable(
        int startId,
        MobilityProfile profile
    ) const {

        int start = findLocation(startId);

        if (start == -1)
            return 0;

        vector<bool> visited(locations.size(), false);
        queue<int> q;

        visited[start] = true;
        q.push(start);

        int count = 0;

        while (!q.empty()) {

            int current = q.front();
            q.pop();

            count++;

            int currentId = locations[current].locationId;

            for (const Connection& c : connections) {

                if (!isConnectionAllowed(c, profile))
                    continue;

                int nextLocationId = -1;

                if (c.fromLocationId == currentId)
                    nextLocationId = c.toLocationId;
                else if (c.toLocationId == currentId)
                    nextLocationId = c.fromLocationId;
                else
                    continue;

                int next = findLocation(nextLocationId);

                if (next != -1 && !visited[next]) {
                    visited[next] = true;
                    q.push(next);
                }
            }
        }

        return count;
    }


    void displayConnections() const {
        for (const Connection& c : connections) {
            cout << c.connectionId << ": "
                 << c.fromLocationId << " -> "
                 << c.toLocationId
                 << " | Available: "
                 << (c.available ? "Yes" : "No")
                 << "\n";
        }
    }
};

#endif
