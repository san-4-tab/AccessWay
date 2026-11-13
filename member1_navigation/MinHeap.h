
#pragma once

class MinHeap {

private:

    struct HeapNode {
        int locationId;
        float distance;
    };

    HeapNode* heapArray;

    int capacity;
    int heapSize;

    void heapifyUp(int index);
    void heapifyDown(int index);

    int findLocation(int locationId);

public:

    MinHeap(int size);
    ~MinHeap();

    void insert(int locationId, float distance);

    void extractMin(
        int& locationId,
        float& distance
    );

    void decreaseKey(
        int locationId,
        float newDistance
    );

    bool isEmpty();

    bool contains(int locationId);

    void display();
};
