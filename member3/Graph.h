
#ifndef ACCESSWAY_GRAPH_H
#define ACCESSWAY_GRAPH_H

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

struct Location {
    int locationId;
    string name;
    int floor;
};

struct Connection {
    int connectionId;
    int fromLocationId;
    int toLocationId;
    float distance;
    bool available;
    bool hasStairs;
};

struct BuildingData {
    vector<Location> locations;
    vector<Connection> connections;
};

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

    int countReachable(int startId, bool wheelchair) const {
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
                if (c.fromLocationId != currentId)
                    continue;

                if (!c.available)
                    continue;

                if (wheelchair && c.hasStairs)
                    continue;

                int next = findLocation(c.toLocationId);

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
