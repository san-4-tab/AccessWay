
#include "MinHeap.h"
#include <iostream>
using namespace std;

// Constructor
MinHeap::MinHeap(int size) {

    capacity = size;
    heapSize = 0;

    heapArray = new HeapNode[capacity];
}

// Swap two heap nodes
static void swapNodes(
    int& id1, float& d1,
    int& id2, float& d2
) {
    int tempId = id1;
    float tempDistance = d1;

    id1 = id2;
    d1 = d2;

    id2 = tempId;
    d2 = tempDistance;
}

// Find location in heap
int MinHeap::findLocation(int locationId) {

    for (int i = 0; i < heapSize; i++) {

        if (heapArray[i].locationId == locationId)
            return i;
    }

    return -1;
}

// Check if location exists
bool MinHeap::contains(int locationId) {

    return findLocation(locationId) != -1;
}

// Move node upwards
void MinHeap::heapifyUp(int index) {

    while (index > 0) {

        int parent = (index - 1) / 2;

        if (heapArray[parent].distance <=
            heapArray[index].distance)
            break;

        HeapNode temp = heapArray[parent];

        heapArray[parent] = heapArray[index];
        heapArray[index] = temp;

        index = parent;
    }
}

// Move node downwards
void MinHeap::heapifyDown(int index) {

    while (true) {

        int left = 2 * index + 1;
        int right = 2 * index + 2;

        int smallest = index;

        if (left < heapSize &&
            heapArray[left].distance <
            heapArray[smallest].distance)
            smallest = left;

        if (right < heapSize &&
            heapArray[right].distance <
            heapArray[smallest].distance)
            smallest = right;

        if (smallest == index)
            break;

        HeapNode temp = heapArray[index];

        heapArray[index] = heapArray[smallest];
        heapArray[smallest] = temp;

        index = smallest;
    }
}

// Insert a new location
void MinHeap::insert(int locationId, float distance) {

    if (heapSize == capacity) {
        cout << "Heap is full!\n";
        return;
    }

    if (contains(locationId)) {
        cout << "Location already exists in heap!\n";
        return;
    }

    int index = heapSize;

    heapArray[index].locationId = locationId;
    heapArray[index].distance = distance;

    heapSize++;

    heapifyUp(index);
}

// Extract minimum element
void MinHeap::extractMin(
    int& locationId,
    float& distance
) {

    if (heapSize == 0) {

        cout << "Heap is empty!\n";

        locationId = -1;
        distance = -1;

        return;
    }

    locationId = heapArray[0].locationId;
    distance = heapArray[0].distance;

    heapArray[0] = heapArray[heapSize - 1];

    heapSize--;

    if (heapSize > 0)
        heapifyDown(0);
}

// Decrease distance value
void MinHeap::decreaseKey(
    int locationId,
    float newDistance
) {

    int index = findLocation(locationId);

    if (index == -1) {
        cout << "Location not found in heap!\n";
        return;
    }

    if (newDistance > heapArray[index].distance) {
        cout << "New distance is greater than current distance!\n";
        return;
    }

    heapArray[index].distance = newDistance;

    heapifyUp(index);
}

// Check if heap is empty
bool MinHeap::isEmpty() {

    return heapSize == 0;
}

// Display heap
void MinHeap::display() {

    if (heapSize == 0) {
        cout << "Heap is empty!\n";
        return;
    }

    cout << "\nMin Heap:\n";

    for (int i = 0; i < heapSize; i++) {

        cout << "Location: "
             << heapArray[i].locationId
             << " | Distance: "
             << heapArray[i].distance
             << endl;
    }
}

// Destructor
MinHeap::~MinHeap() {

    delete[] heapArray;
}
