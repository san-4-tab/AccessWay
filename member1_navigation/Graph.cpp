
#include "Graph.h"
#include <iostream>
using namespace std;

// Constructor
Graph::Graph() {
    head = NULL;
    vertexCount = 0;
}

// Find a vertex
Graph::VertexNode* Graph::findVertex(int locationId) {

    VertexNode* curr = head;

    while (curr != NULL) {

        if (curr->data.locationId == locationId)
            return curr;

        curr = curr->next;
    }

    return NULL;
}

// Add a location as a vertex
void Graph::addVertex(Location location) {

    if (findVertex(location.locationId) != NULL) {
        cout << "Location already exists in graph!\n";
        return;
    }

    VertexNode* newNode = new VertexNode;

    newNode->data = location;
    newNode->edges = NULL;
    newNode->next = head;

    head = newNode;
    vertexCount++;

    cout << "Vertex added successfully!\n";
}

// Add an undirected edge
void Graph::addEdge(Connection connection) {

    VertexNode* from = findVertex(connection.fromLocationId);
    VertexNode* to = findVertex(connection.toLocationId);

    if (from == NULL || to == NULL) {
        cout << "Both locations must exist before connecting!\n";
        return;
    }

    EdgeNode* e1 = new EdgeNode;

    e1->data = connection;
    e1->neighbourLocationId = connection.toLocationId;
    e1->next = from->edges;

    from->edges = e1;

    EdgeNode* e2 = new EdgeNode;

    e2->data = connection;
    e2->neighbourLocationId = connection.fromLocationId;
    e2->next = to->edges;

    to->edges = e2;

    cout << "Connection added successfully!\n";
}

// Remove a connection
void Graph::removeEdge(int connectionId) {

    bool found = false;

    VertexNode* vertex = head;

    while (vertex != NULL) {

        EdgeNode* curr = vertex->edges;
        EdgeNode* prev = NULL;

        while (curr != NULL) {

            if (curr->data.connectionId == connectionId) {

                EdgeNode* temp = curr;

                if (prev == NULL)
                    vertex->edges = curr->next;
                else
                    prev->next = curr->next;

                curr = curr->next;

                delete temp;
                found = true;
            }
            else {
                prev = curr;
                curr = curr->next;
            }
        }

        vertex = vertex->next;
    }

    if (found)
        cout << "Connection removed successfully!\n";
    else
        cout << "Connection not found!\n";
}

// Display complete graph
void Graph::displayGraph() {

    VertexNode* vertex = head;

    cout << "\nACCESSWAY GRAPH\n";

    while (vertex != NULL) {

        cout << "\nLocation " << vertex->data.locationId << " -> ";

        EdgeNode* edge = vertex->edges;

        while (edge != NULL) {

            cout << edge->neighbourLocationId
                 << " (Distance: " << edge->data.distance << ") ";

            edge = edge->next;
        }

        vertex = vertex->next;
    }

    cout << endl;
}

// Display neighbours
void Graph::displayNeighbours(int locationId) {

    VertexNode* vertex = findVertex(locationId);

    if (vertex == NULL) {
        cout << "Location not found!\n";
        return;
    }

    cout << "\nNeighbours of " << locationId << ":\n";

    EdgeNode* edge = vertex->edges;

    while (edge != NULL) {

        cout << edge->neighbourLocationId
             << " | Distance: " << edge->data.distance
             << " | Available: "
             << (edge->data.available ? "Yes" : "No")
             << endl;

        edge = edge->next;
    }
}

// Breadth First Search
void Graph::BFS(int startLocationId) {

    if (findVertex(startLocationId) == NULL) {
        cout << "Starting location not found!\n";
        return;
    }

    int* queue = new int[vertexCount];
    int* visited = new int[vertexCount];

    for (int i = 0; i < vertexCount; i++)
        visited[i] = 0;

    int front = 0;
    int rear = 0;

    queue[rear++] = startLocationId;

    VertexNode* start = findVertex(startLocationId);

    int index = 0;
    VertexNode* tempVertex = head;

    while (tempVertex != NULL) {
        if (tempVertex->data.locationId == startLocationId)
            break;

        index++;
        tempVertex = tempVertex->next;
    }

    visited[index] = 1;

    cout << "\nBFS Traversal: ";

    while (front < rear) {

        int currentId = queue[front++];

        cout << currentId << " ";

        VertexNode* current = findVertex(currentId);

        EdgeNode* edge = current->edges;

        while (edge != NULL) {

            if (edge->data.available) {

                int neighbourId = edge->neighbourLocationId;

                int i = 0;
                VertexNode* v = head;

                while (v != NULL &&
                       v->data.locationId != neighbourId) {
                    v = v->next;
                    i++;
                }

                if (v != NULL && visited[i] == 0) {

                    visited[i] = 1;
                    queue[rear++] = neighbourId;
                }
            }

            edge = edge->next;
        }
    }

    cout << endl;

    delete[] queue;
    delete[] visited;
}

// Get number of vertices
int Graph::getVertexCount() {
    return vertexCount;
}
Graph::RouteResult Graph::dijkstra(
    int startLocationId,
    int destinationLocationId,
    MobilityProfile profile
) {

    RouteResult result;
    result.found = false;
    result.path = NULL;
    result.pathSize = 0;
    result.totalCost = -1;

    if (findVertex(startLocationId) == NULL ||
        findVertex(destinationLocationId) == NULL) {

        cout << "Starting or destination location not found!\n";
        return result;
    }

    if (startLocationId == destinationLocationId) {

        result.path = new int[1];
        result.path[0] = startLocationId;
        result.pathSize = 1;
        result.totalCost = 0;
        result.found = true;

        return result;
    }

    int n = vertexCount;

    int* ids = new int[n];
    float* dist = new float[n];
    int* parent = new int[n];
    bool* visited = new bool[n];

    VertexNode* vertex = head;

    int k = 0;

    while (vertex != NULL) {

        ids[k] = vertex->data.locationId;
        dist[k] = 1e9;
        parent[k] = -1;
        visited[k] = false;

        k++;
        vertex = vertex->next;
    }

    int startIndex = -1;
    int destIndex = -1;

    for (int i = 0; i < n; i++) {

        if (ids[i] == startLocationId)
            startIndex = i;

        if (ids[i] == destinationLocationId)
            destIndex = i;
    }

    dist[startIndex] = 0;

    MinHeap heap(n);
    heap.insert(startLocationId, 0);

    Accessibility access;

    while (!heap.isEmpty()) {

        int currentId;
        float currentDistance;

        heap.extractMin(currentId, currentDistance);

        int currentIndex = -1;

        for (int i = 0; i < n; i++) {
            if (ids[i] == currentId) {
                currentIndex = i;
                break;
            }
        }

        if (currentIndex == -1 || visited[currentIndex])
            continue;

        visited[currentIndex] = true;

        if (currentId == destinationLocationId)
            break;

        VertexNode* current = findVertex(currentId);

        EdgeNode* edge = current->edges;

        while (edge != NULL) {

            int neighbourId = edge->neighbourLocationId;
            int neighbourIndex = -1;

            for (int i = 0; i < n; i++) {
                if (ids[i] == neighbourId) {
                    neighbourIndex = i;
                    break;
                }
            }

            if (neighbourIndex != -1 &&
                !visited[neighbourIndex]) {

                float cost = access.getRouteCost(
                    edge->data, profile
                );

                if (cost >= 0) {

                    float newDistance =
                        dist[currentIndex] + cost;

                    if (newDistance < dist[neighbourIndex]) {

                        dist[neighbourIndex] = newDistance;
                        parent[neighbourIndex] = currentId;

                        if (heap.contains(neighbourId))
                            heap.decreaseKey(neighbourId, newDistance);
                        else
                            heap.insert(neighbourId, newDistance);
                    }
                }
            }

            edge = edge->next;
        }
    }

    if (dist[destIndex] != 1e9) {

        int* reversePath = new int[n];
        int count = 0;

        int currentId = destinationLocationId;

        while (currentId != -1 && count < n) {

            reversePath[count++] = currentId;

            int index = -1;

            for (int i = 0; i < n; i++) {
                if (ids[i] == currentId) {
                    index = i;
                    break;
                }
            }

            if (index == -1)
                break;

            currentId = parent[index];
        }

        result.path = new int[count];

        for (int i = 0; i < count; i++)
            result.path[i] = reversePath[count - 1 - i];

        result.pathSize = count;
        result.totalCost = dist[destIndex];
        result.found = true;

        delete[] reversePath;
    }

    delete[] ids;
    delete[] dist;
    delete[] parent;
    delete[] visited;

    return result;
}
// Destructor
Graph::~Graph() {

    VertexNode* vertex = head;

    while (vertex != NULL) {

        EdgeNode* edge = vertex->edges;

        while (edge != NULL) {

            EdgeNode* tempEdge = edge;
            edge = edge->next;

            delete tempEdge;
        }

        VertexNode* tempVertex = vertex;
        vertex = vertex->next;

        delete tempVertex;
    }
}
