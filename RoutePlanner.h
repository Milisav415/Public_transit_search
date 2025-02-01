//
// Created by Gime on 01/02/2025.
//

#ifndef PUBLIC_TRANSIT_SEARCH_ROUTEPLANNER_H
#define PUBLIC_TRANSIT_SEARCH_ROUTEPLANNER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <deque>
#include <tuple>
#include <limits>

class RoutePlanner {
public:
    // A very large number used to represent infinity.
    static const int INF;

    // Structure representing an edge (connection between stations).
    struct Edge {
        int station;      // destination station index
        int vehicle;      // vehicle id used on this edge
        int travel_time;  // time cost to travel this edge
    };

    // Default constructor.
    RoutePlanner();

    // Load station names from an input stream.
    // Each nonempty line is considered one station.
    bool loadStations(std::istream& is);

    // Load vehicles (and their routes) from an input stream.
    // Each line should be formatted as:
    // VehicleName Station1 Station2 Time12 Station2 Station3 Time23 ...
    bool loadVehicles(std::istream& is);

    // Returns a pair where first is the total travel time for the fastest route,
    // and second is the path (vector of station indices) from start to goal.
    std::pair<int, std::vector<int>> getFastestRoute(const std::string& start_station,
                                                     const std::string& goal_station);

    // Returns a pair where first is the minimal number of vehicle transfers,
    // and second is the path (vector of station indices) from start to goal.
    std::pair<int, std::vector<int>> getMinimalTransfersRoute(const std::string& start_station,
                                                              const std::string& goal_station);

    // Returns the station names (to map indices to names).
    const std::vector<std::string>& getStationNames() const;

    // Export the graph in Graphviz DOT format to a file.
    // Returns true if successful.
    bool exportToDOT(const std::string& filename) const;

private:
    // Mapping from station name to index.
    std::unordered_map<std::string, int> station_id;
    // List of station names.
    std::vector<std::string> station_names;

    // Mapping from vehicle name to id.
    std::unordered_map<std::string, int> vehicle_ids;
    // List of vehicle names.
    std::vector<std::string> vehicle_names;

    // Adjacency list representing the graph.
    std::vector<std::vector<Edge>> adjacency;
};

#endif //PUBLIC_TRANSIT_SEARCH_ROUTEPLANNER_H
