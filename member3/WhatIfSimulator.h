
#ifndef ACCESSWAY_WHAT_IF_SIMULATOR_H
#define ACCESSWAY_WHAT_IF_SIMULATOR_H

#include <iostream>
#include "Graph.h"

using namespace std;

class WhatIfSimulator {
private:
    Graph originalGraph;
    Graph scenarioGraph;

public:
    WhatIfSimulator(const BuildingData& data)
        : originalGraph(data), scenarioGraph(data) {}

    void reset() {
        scenarioGraph = originalGraph;
    }

    void blockConnection(int id) {
        scenarioGraph.blockConnection(id);
        cout << "Scenario: connection blocked.\n";
    }

    void restoreConnection(int id) {
        scenarioGraph.restoreConnection(id);
        cout << "Scenario: connection restored.\n";
    }

    void disableFacility(int id) {
        scenarioGraph.disableFacility(id);
        cout << "Scenario: facility disabled.\n";
    }

    void compare(int startId, bool wheelchair) const {
        int before =
            originalGraph.countReachable(startId, wheelchair);

        int after =
            scenarioGraph.countReachable(startId, wheelchair);

        cout << "\n======= ACCESSIBILITY COMPARISON =======\n";

        cout << "Before: " << before << " reachable locations\n";
        cout << "After: " << after << " reachable locations\n";
        cout << "Difference: " << after - before << "\n";

        if (after < before)
            cout << "Accessibility has decreased.\n";
        else if (after > before)
            cout << "Accessibility has improved.\n";
        else
            cout << "No change in reachability.\n";
    }

    void displayScenario() const {
        scenarioGraph.displayConnections();
    }
};

#endif
