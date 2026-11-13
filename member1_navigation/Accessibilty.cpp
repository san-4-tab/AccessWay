
#include "Accessibility.h"
#include <iostream>
using namespace std;

// Check whether a connection is suitable
bool Accessibility::isEdgeAllowed(
    const Connection& c,
    MobilityProfile profile
) {

    if (!c.available || c.distance < 0)
        return false;

    switch (profile) {

        case WHEELCHAIR:

            if (c.width < 0.9)
                return false;

            if (c.hasStairs && !c.hasRamp && !c.hasElevator)
                return false;

            break;

        case CRUTCHES:

            if (c.width < 0.7)
                return false;

            break;

        case LIMITED_MOBILITY:

            if (c.hasStairs && !c.hasRamp && !c.hasElevator)
                return false;

            break;

        case HAND_DISABILITY:

            break;

        case GENERAL:

            break;
    }

    return true;
}

// Calculate additional accessibility cost
float Accessibility::getAccessibilityPenalty(
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

// Calculate total route cost
float Accessibility::getRouteCost(
    const Connection& c,
    MobilityProfile profile
) {

    if (!isEdgeAllowed(c, profile))
        return -1;

    return c.distance +
           getAccessibilityPenalty(c, profile);
}

// Display accessibility warnings
void Accessibility::displayWarnings(
    const Connection& c,
    MobilityProfile profile
) {

    bool warningFound = false;

    cout << "\nAccessibility Warnings:\n";

    if (!c.available) {
        cout << "- This connection is currently unavailable.\n";
        warningFound = true;
    }

    if (c.width < 0.9 && profile == WHEELCHAIR) {
        cout << "- Path may be too narrow for wheelchair users.\n";
        warningFound = true;
    }

    if (c.hasStairs && !c.hasRamp && !c.hasElevator) {
        cout << "- Stairs are present without a ramp or elevator.\n";
        warningFound = true;
    }

    if (!c.automaticDoor && profile == HAND_DISABILITY) {
        cout << "- Door is not automatic.\n";
        warningFound = true;
    }

    if (c.slope > 8.0) {
        cout << "- Path has a steep slope.\n";
        warningFound = true;
    }

    if (!warningFound)
        cout << "No major accessibility warnings.\n";
}

