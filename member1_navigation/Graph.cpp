
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
