# AccessWay

**An accessibility-focused building navigation and emergency assistance system built using C++ and Data Structures and Algorithms.**

AccessWay helps users navigate buildings, identify accessible routes, report accessibility problems, and coordinate emergency assistance.

## Project Modules

### Member 1 – Navigation
- Building, floor, location, and connection management.
- Graph-based route finding using Dijkstra's algorithm.
- Accessibility-aware navigation.
- Alternative route handling when connections are blocked.

### Member 2 – Emergency Management
- Emergency evacuation route planning.
- Accessible evacuation based on mobility profiles.
- Blocked connection and unavailable facility management.
- Volunteer registration and availability tracking.
- Help request creation, prioritization, and automatic assignment.
- Persistence of volunteers, requests, and emergency status.

### Member 3 – Reviews and What-if Simulation
- Accessibility review management.
- Review validation and persistence.
- Location priority ranking using a max heap.
- What-if simulations for blocked connections and facility failures.
- Reachability analysis using graph traversal.

## Shared Building Data

The three backend modules use a common building data structure defined in:

`common/AccessWayData.h`

This allows navigation, emergency management, and simulation to work with the same building locations and connections.

## Technologies and DSA Concepts

- Language: C++
- Graphs and graph traversal
- Dijkstra's shortest-path algorithm
- Breadth-First Search (BFS)
- Queues and priority queues
- Linked lists
- Hash tables
- Max heaps
- File handling

## Backend Testing

GitHub Actions automatically builds and tests the backend modules when changes are pushed to the `main` branch.

The repository includes these checks:

1. Member 1 C++ Build Check
2. Member 2 C++ Build Check
3. Member 3 C++ Build Check
4. AccessWay Backend Integration Check

The integration test verifies shared building data, navigation, emergency routing, reverse-direction routes, blocked paths, simulation reachability, review validation, priority ranking, and heap behavior.

## Local Compilation

The integrated backend can be compiled from the repository root using:

```bash
g++ -std=c++17 -Wall -Wextra -I. \
  integration_test.cpp \
  member1_navigation/Graph.cpp \
  member1_navigation/Accessibility.cpp \
  member1_navigation/MinHeap.cpp \
  EmergencyManager.cpp \
  EvacuationManager.cpp \
  VolunteerManager.cpp \
  HelpRequestManager.cpp \
  -o integration_test
```

Run the integration test:

```bash
./integration_test
```

## Project Status

The three backend modules have been integrated using shared building data. Their individual build checks and the backend integration workflow are passing.

**Note:** This is an academic prototype. Route recommendations and priority scores are not a substitute for certified accessibility assessments or professional emergency evacuation procedures.
