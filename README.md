# Route Planner Project

This project is a C++ route planner that computes routes between stations using two methods:
- **Fastest Route:** Finds the route with the minimum total travel time using Dijkstra's algorithm.
- **Minimal Transfers Route:** Finds the route with the fewest vehicle changes using a 0–1 BFS variant.

In addition, the project can export the internal graph structure in [Graphviz DOT](https://graphviz.org/doc/info/lang.html) format for visualization.

## Features

- **Fastest Route Calculation:**  
  Uses Dijkstra's algorithm to compute the minimum travel time between two stations.

- **Minimal Transfers Route:**  
  Uses a 0–1 BFS variant to compute a route that minimizes the number of vehicle transfers.

- **Graph Visualization:**  
  Exports the graph as a DOT file, which can be converted into images (e.g., PNG) using Graphviz.

- **Modular Design:**  
  The project is organized into a `RoutePlanner` class (with separate header and implementation files) and a test harness (`GraphTest.cpp`) to validate functionality.

## File Structure

- **RoutePlanner.h:**  
  Contains the declaration of the `RoutePlanner` class and its member functions.

- **RoutePlanner.cpp:**  
  Contains the implementation of the `RoutePlanner` class, including methods for loading station and vehicle data, computing routes, and exporting the graph.

- **GraphTest.cpp:**  
  Provides a set of tests (using assertions) for validating the functionality of the `RoutePlanner` class. This file also demonstrates how to export the graph to a DOT file.

## Build Instructions

This project requires a C++ compiler that supports C++17. You can compile the project using the following command (assuming you are using `g++`):

```bash
g++ -std=c++17 RoutePlanner.cpp GraphTest.cpp -o route_planner
