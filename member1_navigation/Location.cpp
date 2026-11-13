
#include "Location.h"
#include <iostream>
using namespace std;

// Constructor
LocationManager::LocationManager() {
    head = NULL;
}

// Add a new location
void LocationManager::addLocation(Location l) {

    LocationNode* curr = head;

    // Check for duplicate location ID
    while (curr != NULL) {
        if (curr->data.locationId == l.locationId) {
            cout << "Location ID already exists!\n";
            return;
        }

        curr = curr->next;
    }

    LocationNode* newNode = new LocationNode;

    newNode->data = l;
    newNode->next = NULL;

    if (head == NULL) {
        head = newNode;
    }
    else {
        curr = head;

        while (curr->next != NULL)
            curr = curr->next;

        curr->next = newNode;
    }

    cout << "Location added successfully!\n";
}

// Search location using ID
Location* LocationManager::searchLocation(int locationId) {

    LocationNode* curr = head;

    while (curr != NULL) {

        if (curr->data.locationId == locationId)
            return &curr->data;

        curr = curr->next;
    }

    return NULL;
}

// Display all locations in a building
void LocationManager::displayLocations(int buildingId) {

    LocationNode* curr = head;
    bool found = false;

    cout << "\nLocations in Building " << buildingId << ":\n";

    while (curr != NULL) {

        if (curr->data.buildingId == buildingId) {

            cout << "Location ID: " << curr->data.locationId
                 << " | Name: " << curr->data.locationName
                 << " | Floor: " << curr->data.floor
                 << " | Type: " << curr->data.type << endl;

            found = true;
        }

        curr = curr->next;
    }

    if (!found)
        cout << "No locations found.\n";
}

// Display locations on a particular floor
void LocationManager::displayFloorLocations(int buildingId, int floor) {

    LocationNode* curr = head;
    bool found = false;

    cout << "\nLocations on Floor " << floor
         << " of Building " << buildingId << ":\n";

    while (curr != NULL) {

        if (curr->data.buildingId == buildingId &&
            curr->data.floor == floor) {

            cout << "ID: " << curr->data.locationId
                 << " | Name: " << curr->data.locationName
                 << " | Type: " << curr->data.type << endl;

            found = true;
        }

        curr = curr->next;
    }

    if (!found)
        cout << "No locations found on this floor.\n";
}

// Delete a location
void LocationManager::deleteLocation(int locationId) {

    LocationNode* curr = head;
    LocationNode* prev = NULL;

    while (curr != NULL) {

        if (curr->data.locationId == locationId)
            break;

        prev = curr;
        curr = curr->next;
    }

    if (curr == NULL) {
        cout << "Location not found!\n";
        return;
    }

    if (prev == NULL)
        head = curr->next;
    else
        prev->next = curr->next;

    delete curr;

    cout << "Location deleted successfully!\n";
}

// Destructor
LocationManager::~LocationManager() {

    LocationNode* curr = head;

    while (curr != NULL) {

        LocationNode* temp = curr;
        curr = curr->next;

        delete temp;
    }
}
