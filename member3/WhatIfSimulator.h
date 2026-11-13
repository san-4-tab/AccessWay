
#ifndef ACCESSWAY_WHAT_IF_SIMULATOR_H
#define ACCESSWAY_WHAT_IF_SIMULATOR_H

#include <iostream>
#include "Graph.h"

using namespace std;

class WhatIfSimulator {
private:
    SimulationGraph originalGraph;
    SimulationGraph scenarioGraph;

    const char* getProfileName(MobilityProfile profile) const {
        switch (profile) {
            case WHEELCHAIR: return "Wheelchair";
            case CRUTCHES: return "Crutches";
            case HAND_DISABILITY: return "Hand Disability";
            case LIMITED_MOBILITY: return "Limited Mobility";
            case GENERAL: return "General";
            default: return "Unknown";
        }
    }

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

    // Compare accessibility for any mobility profile.
    void compare(int startId, MobilityProfile profile) const {
        int before =
            originalGraph.countReachable(startId, profile);

        int after =
            scenarioGraph.countReachable(startId, profile);

        cout << "\n======= ACCESSIBILITY COMPARISON =======\n";
        cout << "Mobility profile: " << getProfileName(profile) << "\n";
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

    // Maintain compatibility with existing true/false calls.
    void compare(int startId, bool wheelchair) const {
        compare(
            startId,
            wheelchair ? WHEELCHAIR : GENERAL
        );
    }

    void displayScenario() const {
        scenarioGraph.displayConnections();
    }
};

#endif
