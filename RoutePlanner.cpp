//
// Created by Gime on 01/02/2025.
//

#include "RoutePlanner.h"
#include <iostream>
#include <sstream>
#include <string>
#include <queue>
#include <algorithm>
#include <fstream>

// Use the standard namespace for convenience.
using namespace std;

const int RoutePlanner::INF = numeric_limits<int>::max();

RoutePlanner::RoutePlanner() = default; // Nothing to initiate

bool RoutePlanner::loadStations(istream& is) {
    string line;
    int id = 0;
    while(getline(is, line)) {
        if(line.empty())
            continue;
        station_id[line] = id++;
        station_names.push_back(line);
    }
    // Resize the adjacency list to the number of stations.
    adjacency.resize(station_names.size());
    return true;
}

bool RoutePlanner::loadVehicles(istream& is) {
    string line;
    int vehicle_count = 0;
    while(getline(is, line)) {
        if(line.empty())
            continue;

        // Expected format for each line:
        // VehicleName Station1 Station2 Time12 Station2 Station3 Time23 ...
        istringstream ss(line);
        string vehicle_name;
        ss >> vehicle_name;

        if(vehicle_ids.find(vehicle_name) == vehicle_ids.end()) {
            vehicle_ids[vehicle_name] = vehicle_count++;
            vehicle_names.push_back(vehicle_name);
        }
        int v_id = vehicle_ids[vehicle_name];

        string prev_station, cur_station;
        int travel_time;

        // Read the first station.
        if(!(ss >> prev_station))
            continue;

        // Process subsequent tokens: destination station and travel time.
        while (true) {
            if (!(ss >> cur_station))
                break;
            if (!(ss >> travel_time)) {
                std::cerr << "Invalid format in vehicles input for vehicle: " << vehicle_name << "\n";
                return false;
            }
            if (station_id.find(prev_station) == station_id.end()) {
                std::cerr << "Unknown station: " << prev_station << "\n";
                return false;
            }
            if (station_id.find(cur_station) == station_id.end()) {
                std::cerr << "Unknown station: " << cur_station << "\n";
                return false;
            }
            int u = station_id[prev_station];
            int w = station_id[cur_station];

            // Add bidirectional edges.
            adjacency[u].push_back({w, v_id, travel_time});
            adjacency[w].push_back({u, v_id, travel_time});

            // The next leg starts at the current destination.
            prev_station = cur_station;
        }
    }
    return true;
}

pair<int, vector<int>> RoutePlanner::getFastestRoute(const string& start_station,
                                                     const string& goal_station) {
    int N = station_names.size();
    vector<int> dist(N, INF);
    vector<int> parent(N, -1);
    int start = station_id[start_station];
    int goal = station_id[goal_station];
    dist[start] = 0;

    // Min-heap: (distance, station)
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;
    pq.push({0, start});

    while(!pq.empty()) {
        auto [cdist, u] = pq.top();
        pq.pop();
        if(cdist > dist[u])
            continue;
        for (auto &edge : adjacency[u]) {
            int w = edge.station;
            int cost = edge.travel_time;
            if(dist[w] > dist[u] + cost) {
                dist[w] = dist[u] + cost;
                parent[w] = u;
                pq.push({dist[w], w});
            }
        }
    }

    if(dist[goal] == INF)
        return {INF, {}};

    // Reconstruct the path.
    vector<int> path;
    for (int cur = goal; cur != -1; cur = parent[cur])
        path.push_back(cur);
    reverse(path.begin(), path.end());
    return {dist[goal], path};
}

pair<int, vector<int>> RoutePlanner::getMinimalTransfersRoute(const string& start_station,
                                                              const string& goal_station) {
    int N = station_names.size();
    int V = vehicle_ids.size();
    int start = station_id[start_station];
    int goal = station_id[goal_station];
    int start_vehicle = V;  // Special state: no vehicle chosen yet.
    vector<vector<int>> dist_transfers(N, vector<int>(V+1, INF));
    vector<vector<pair<int,int>>> parent_transfers(N, vector<pair<int,int>>(V+1, {-1,-1}));
    dist_transfers[start][start_vehicle] = 0;

    deque<tuple<int,int,int>> dq;
    dq.push_back({start, start_vehicle, 0});

    while(!dq.empty()) {
        auto [u, v_id, cur_cost] = dq.front();
        dq.pop_front();
        if(cur_cost > dist_transfers[u][v_id])
            continue;
        for(auto &edge : adjacency[u]) {
            int w = edge.station;
            int w_vid = edge.vehicle;
            int add_cost = (v_id != V && v_id != w_vid) ? 1 : 0;
            int new_cost = cur_cost + add_cost;
            if(new_cost < dist_transfers[w][w_vid]) {
                dist_transfers[w][w_vid] = new_cost;
                parent_transfers[w][w_vid] = {u, v_id};
                if(add_cost == 0)
                    dq.push_front({w, w_vid, new_cost});
                else
                    dq.push_back({w, w_vid, new_cost});
            }
        }
    }

    int min_transfers = INF;
    int end_vehicle = -1;
    for (int vid = 0; vid <= V; vid++) {
        if(dist_transfers[goal][vid] < min_transfers) {
            min_transfers = dist_transfers[goal][vid];
            end_vehicle = vid;
        }
    }
    if(min_transfers == INF)
        return {INF, {}};

    // Reconstruct the path.
    vector<int> path;
    int cur_station = goal;
    int cur_vehicle = end_vehicle;
    while(!(cur_station == start && cur_vehicle == start_vehicle)) {
        path.push_back(cur_station);
        auto p = parent_transfers[cur_station][cur_vehicle];
        cur_station = p.first;
        cur_vehicle = p.second;
    }
    path.push_back(start);
    reverse(path.begin(), path.end());
    return {min_transfers, path};
}

bool RoutePlanner::exportToDOT(const string& filename) const {
    ofstream dot(filename);
    if (!dot) {
        cerr << "Error opening " << filename << " for writing.\n";
        return false;
    }

    dot << "graph G {\n";
    dot << "  node [shape=ellipse, fontsize=10];\n";

    // Output nodes with station names as labels.
    for (size_t i = 0; i < station_names.size(); i++) {
        dot << "  " << i << " [label=\"" << station_names[i] << "\"];\n";
    }

    // To avoid duplicate edges, output an edge only if u < v.
    for (size_t u = 0; u < adjacency.size(); u++) {
        for (const auto &edge : adjacency[u]) {
            int v = edge.station;
            if (u < static_cast<size_t>(v)) {
                // Get vehicle name (if available).
                std::string vehicle_label = (edge.vehicle < (int)vehicle_names.size())
                                            ? vehicle_names[edge.vehicle]
                                            : "Unknown";
                dot << "  " << u << " -- " << v << " [label=\""
                    << vehicle_label << " (" << edge.travel_time << ")\"];\n";
            }
        }
    }
    dot << "}\n";
    dot.close();
    return true;
}

const std::vector<std::string>& RoutePlanner::getStationNames() const {
    return station_names;
}