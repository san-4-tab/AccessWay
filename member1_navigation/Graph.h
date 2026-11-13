
#pragma once

#include "../common/AccessPulseData.h"
#include "MinHeap.h"
#include "Accessibility.h"

class Graph {
private:

    struct EdgeNode {
        Connection data;
        int neighbourLocationId;
        EdgeNode* next;
    };

    struct VertexNode {
        Location data;
        EdgeNode* edges;
        VertexNode* next;
    };

    VertexNode* head;
    int vertexCount;

    VertexNode* findVertex(int locationId);

public:

    struct RouteResult {
        bool found;
        int* path;
        int pathSize;
        float totalCost;
    };

    Graph();
    ~Graph();

    void addVertex(Location location);
    void addEdge(Connection connection);

    void removeEdge(int connectionId);

    void displayGraph();
    void displayNeighbours(int locationId);

    void BFS(int startLocationId);

    RouteResult dijkstra(
        int startLocationId,
        int destinationLocationId,
        MobilityProfile profile
    );

    RouteResult findAlternativeRoute(
        int startLocationId,
        int destinationLocationId,
        MobilityProfile profile,
        int blockedConnectionId
    );

    int getVertexCount();
};
