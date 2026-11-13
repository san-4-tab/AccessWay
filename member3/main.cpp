#include <iostream>
#include <string>
#include <limits>

#include "ReviewManager.h"
#include "Graph.h"
#include "ReviewOptimizer.h"
#include "WhatIfSimulator.h"

using namespace std;

// Sample building data

BuildingData createSampleBuilding() {
    BuildingData data;

    Location l;

    l = {1, 1, "Main Entrance", 0, ENTRANCE};
    data.locations.push_back(l);

    l = {2, 1, "Main Corridor", 0, CORRIDOR};
    data.locations.push_back(l);

    l = {3, 1, "Elevator", 0, ELEVATOR};
    data.locations.push_back(l);

    l = {4, 1, "Room 101", 0, ROOM};
    data.locations.push_back(l);

    l = {5, 1, "Staircase", 0, STAIRCASE};
    data.locations.push_back(l);

    l = {6, 1, "First Floor Corridor", 1, CORRIDOR};
    data.locations.push_back(l);

    l = {7, 1, "Room 201", 1, ROOM};
    data.locations.push_back(l);

    l = {8, 1, "Emergency Exit", 0, EXIT};
    data.locations.push_back(l);

    Connection c;

    c = {1, 1, 1, 2, 10, 1.5, 0, false, true, false, true, true, true};
    data.connections.push_back(c);

    c = {2, 1, 2, 3, 5, 1.5, 0, false, false, true, true, true, true};
    data.connections.push_back(c);

    c = {3, 1, 3, 6, 8, 1.5, 0, false, false, true, true, true, true};
    data.connections.push_back(c);

    c = {4, 1, 6, 7, 6, 1.2, 0, false, false, false, true, true, true};
    data.connections.push_back(c);

    c = {5, 1, 2, 5, 4, 1.0, 0, true, false, false, false, true, true};
    data.connections.push_back(c);

    c = {6, 1, 5, 6, 9, 1.0, 0, true, false, false, false, true, true};
    data.connections.push_back(c);

    c = {7, 1, 2, 8, 12, 1.5, 0, false, true, false, true, true, true};
    data.connections.push_back(c);

    return data;
}


// Sample reviews
void addSampleReviews(ReviewManager& manager) {
    manager.addReview({
        1, 1, 5, 5,
        "Steep ramp",
        "Ramp is difficult for wheelchair users.",
        true
    });

    manager.addReview({
        2, 1, 3, 4,
        "Elevator failure",
        "Elevator stopped working repeatedly.",
        true
    });

    manager.addReview({
        3, 1, 5, 3,
        "Difficult ramp access",
        "Ramp entrance is difficult to access.",
        true
    });

    manager.addReview({
        4, 1, 2, 5,
        "Blocked corridor",
        "Corridor is frequently blocked.",
        true
    });

    manager.addReview({
        5, 1, 3, 5,
        "Frequent breakdown",
        "Elevator frequently becomes unavailable.",
        true
    });

    manager.addReview({
        6, 1, 7, 4,
        "Door accessibility",
        "Door is difficult to open.",
        false
    });
}

int main() {
    ReviewManager manager;
    ReviewOptimizer optimizer;

    BuildingData building = createSampleBuilding();
    WhatIfSimulator simulator(building);

    manager.loadFromFile();

    if (manager.getReviews(1).empty()) {
        addSampleReviews(manager);
        manager.saveToFile();
    }

    int choice;

    do {
        cout << "\n========== ACCESSWAY ==========\n";
        cout << "1. Add review\n";
        cout << "2. Display reviews\n";
        cout << "3. Verify a review\n";
        cout << "4. Generate priority ranking\n";
        cout << "5. Block a connection\n";
        cout << "6. Disable elevator\n";
        cout << "7. Compare accessibility\n";
        cout << "8. Display scenario\n";
        cout << "9. Reset scenario\n";
        cout << "0. Exit\n";

        cout << "\nEnter choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(
                numeric_limits<streamsize>::max(), '\n'
            );
            cout << "Invalid input.\n";
            continue;
        }

        if (choice == 1) {
            Review r;

            cout << "Review ID: ";
            cin >> r.reviewId;

            if (manager.reviewExists(r.reviewId)) {
                cout << "Review ID already exists.\n";
                continue;
            }

            r.buildingId = 1;

            cout << "Location ID: ";
            cin >> r.locationId;

            cout << "Severity (1-5): ";
            cin >> r.severity;

            if (r.severity < 1 || r.severity > 5) {
                cout << "Invalid severity.\n";
                continue;
            }

            cin.ignore(
                numeric_limits<streamsize>::max(), '\n'
            );

            cout << "Category: ";
            getline(cin, r.category);

            cout << "Description: ";
            getline(cin, r.description);

            r.verified = false;

            manager.addReview(r);
            manager.saveToFile();

            cout << "Review submitted and awaiting verification.\n";
        }
        else if (choice == 2) {
            manager.displayReviews();
        }
        else if (choice == 3) {
            int id;

            cout << "Enter review ID to verify: ";
            cin >> id;

            if (manager.verifyReview(id)) {
                manager.saveToFile();
                cout << "Review verified.\n";
            } else {
                cout << "Review not found.\n";
            }
        }
        else if (choice == 4) {
            optimizer.generateRanking(
                manager.getReviews(1), 1
            );
        }
        else if (choice == 5) {
            int id;

            cout << "Enter connection ID to block: ";
            cin >> id;

            simulator.blockConnection(id);
        }
        else if (choice == 6) {
            simulator.disableFacility(3);
        }
        else if (choice == 7) {
            simulator.compare(1, true);
        }
        else if (choice == 8) {
            simulator.displayScenario();
        }
        else if (choice == 9) {
            simulator.reset();
            cout << "Scenario reset successfully.\n";
        }
        else if (choice != 0) {
            cout << "Invalid choice.\n";
        }

    } while (choice != 0);

    manager.saveToFile();

    cout << "AccessWay closed.\n";

    return 0;
}
