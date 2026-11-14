## Reviews, Accessibility Optimization and What-if Simulation

### Module Overview

This module helps building administrators identify accessibility problems through visitor reviews and test possible building changes using graph simulations.

### Main Features

**1. Accessibility Review Management**
- Add visitor reviews.
- Store review descriptions, categories and severity.
- Track review verification.
- Save and load reviews using file handling.

**2. Review-based Optimization**
- Group reviews by building location.
- Calculate verified severity totals.
- Rank locations using a custom max heap.
- Identify locations requiring attention.

**3. What-if Simulation**
- Block building connections.
- Simulate elevator failure.
- Compare reachable locations before and after changes.
- Reset simulations without changing the original graph.

### DSA Concepts
- Linked Lists
- Graphs
- Breadth First Search
- Queues
- Max Heaps
- File Handling

### Compilation

```bash
g++ -std=c++17 main.cpp -o member3_app
```

### Current Limitations
- Uses sample building data.
- Priority scores are prototype values, not validated accessibility standards.
- Simulation reports reachability, not certified evacuation safety.
- Reviews are stored locally in a text file.
- The module is integrated with the shared building data defined in common/AccessWayData.h.
