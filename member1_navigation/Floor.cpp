
#include "Floor.h"
#include <iostream>
using namespace std;

// Constructor
FloorManager::FloorManager() {
    head = NULL;
}

// Add a new floor
void FloorManager::addFloor(int buildingId, int floorNumber, string floorName) {

    FloorNode* curr = head;

    while (curr != NULL) {
        if (curr->buildingId == buildingId &&
            curr->floorNumber == floorNumber) {
            cout << "Floor already exists!\n";
            return;
        }

        curr = curr->next;
    }

    FloorNode* newNode = new FloorNode;

    newNode->buildingId = buildingId;
    newNode->floorNumber = floorNumber;
    newNode->floorName = floorName;
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

    cout << "Floor added successfully!\n";
}

// Search for a floor
bool FloorManager::searchFloor(int buildingId, int floorNumber) {

    FloorNode* curr = head;

    while (curr != NULL) {
        if (curr->buildingId == buildingId &&
            curr->floorNumber == floorNumber)
            return true;

        curr = curr->next;
    }

    return false;
}

// Display floors of a particular building
void FloorManager::displayFloors(int buildingId) {

    FloorNode* curr = head;
    bool found = false;

    cout << "\nFloors in Building " << buildingId << ":\n";

    while (curr != NULL) {

        if (curr->buildingId == buildingId) {
            cout << "Floor: " << curr->floorNumber
                 << " | Name: " << curr->floorName << endl;

            found = true;
        }

        curr = curr->next;
    }

    if (!found)
        cout << "No floors found for this building.\n";
}

// Delete a floor
void FloorManager::deleteFloor(int buildingId, int floorNumber) {

    FloorNode* curr = head;
    FloorNode* prev = NULL;

    while (curr != NULL) {

        if (curr->buildingId == buildingId &&
            curr->floorNumber == floorNumber)
            break;

        prev = curr;
        curr = curr->next;
    }

    if (curr == NULL) {
        cout << "Floor not found!\n";
        return;
    }

    if (prev == NULL)
        head = curr->next;
    else
        prev->next = curr->next;

    delete curr;

    cout << "Floor deleted successfully!\n";
}

// Destructor
FloorManager::~FloorManager() {

    FloorNode* curr = head;

    while (curr != NULL) {
        FloorNode* temp = curr;
        curr = curr->next;
        delete temp;
    }
}
