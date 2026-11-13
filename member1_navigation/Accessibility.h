
#pragma once

#include "../common/AccessWayData.h"

class Accessibility {
public:

    bool isEdgeAllowed(
        const Connection& connection,
        MobilityProfile profile
    );

    float getAccessibilityPenalty(
        const Connection& connection,
        MobilityProfile profile
    );

    float getRouteCost(
        const Connection& connection,
        MobilityProfile profile
    );

    void displayWarnings(
        const Connection& connection,
        MobilityProfile profile
    );
};
