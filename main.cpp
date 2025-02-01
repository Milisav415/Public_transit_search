//#include <iostream>
//#include <fstream>
//#include <string>
//#include <vector>
//#include <unordered_map>
//#include <queue>
//#include <limits>
//#include <algorithm>
//#include <sstream>
//#include <tuple>
//#include <deque>
//
//const int INF = std::numeric_limits<int>::max();
//
//struct Edge {
//    int station;
//    int vehicle;
//    int travel_time;
//};
//
//int main() {
//    std::string stations_file = R"(C:\Users\jm190\CLionProjects\Public_transit_search\stations.txt)";
//    std::string vehicles_file = R"(C:\Users\jm190\CLionProjects\Public_transit_search\vehicles.txt)";
//
//    // Read stations
//    std::ifstream sf(stations_file);
//    if(!sf) {
//        std::cerr << "Cannot open stations file.\n";
//        return 1;
//    }
//    std::unordered_map<std::string, int> station_id;
//    std::vector<std::string> station_names;
//    {
//        std::string line;
//        int id = 0;
//        while(std::getline(sf, line)) {
//            if(!line.empty()) {
//                station_id[line] = id++;
//                station_names.push_back(line);
//            }
//        }
//    }
//    sf.close();
//
//    int N = (int)station_names.size();
//    std::vector<std::vector<Edge>> adjacency(N);
//
//    // Read vehicles
//    std::ifstream vf(vehicles_file);
//    if(!vf) {
//        std::cerr << "Cannot open vehicles file.\n";
//        return 1;
//    }
//    std::unordered_map<std::string, int> vehicle_ids;
//    std::vector<std::string> vehicle_names;
//    int vehicle_count = 0;
//
//    {
//        std::string line;
//        while(std::getline(vf, line)) {
//            if(line.empty()) continue;
//
//            // Format:
//            // VehicleName Station1 Station2 Time12 Station2 Station3 Time23 ...
//            std::stringstream ss(line);
//            std::string vehicle_name;
//            ss >> vehicle_name;
//
//            if(vehicle_ids.find(vehicle_name) == vehicle_ids.end()) {
//                vehicle_ids[vehicle_name] = vehicle_count++;
//                vehicle_names.push_back(vehicle_name);
//            }
//            int v_id = vehicle_ids[vehicle_name];
//
//            // We will parse station-time sets
//            // Essentially: (StationX StationY timeXY) repeated
//            std::string prev_station;
//            std::string cur_station;
//            int travel_time;
//
//            // We need to read triplets: StationX StationY time
//            // We'll first read the first station:
//            if(!(ss >> prev_station)) {
//                // No stations for this vehicle line?
//                continue;
//            }
//
//            while(true) {
//                if(!(ss >> cur_station)) break;
//                if(!(ss >> travel_time)) {
//                    std::cerr << "Invalid format in vehicles file for vehicle: " << vehicle_name << "\n";
//                    return 1;
//                }
//
//                if(station_id.find(prev_station) == station_id.end()) {
//                    std::cerr << "Unknown station: " << prev_station << "\n";
//                    return 1;
//                }
//                if(station_id.find(cur_station) == station_id.end()) {
//                    std::cerr << "Unknown station: " << cur_station << "\n";
//                    return 1;
//                }
//
//                int u = station_id[prev_station];
//                int w = station_id[cur_station];
//
//                // Add bidirectional edges
//                adjacency[u].push_back({w, v_id, travel_time});
//                adjacency[w].push_back({u, v_id, travel_time});
//
//                prev_station = cur_station;
//            }
//        }
//    }
//    vf.close();
//
//    // Query
//    std::string start_station, end_station;
//    std::cout << "Enter start station: ";
//    std::cin >> start_station;
//    std::cout << "Enter end station: ";
//    std::cin >> end_station;
//
//    if(station_id.find(start_station) == station_id.end() ||
//       station_id.find(end_station) == station_id.end()) {
//        std::cerr << "Invalid stations.\n";
//        return 1;
//    }
//
//    int start = station_id[start_station];
//    int goal = station_id[end_station];
//
//    // 1. Find the fastest route by travel time using Dijkstra
//    {
//        std::vector<int> dist(N, INF);
//        std::vector<int> parent(N, -1);
//        dist[start] = 0;
//
//        // Min-heap for (distance, node)
//        std::priority_queue<std::pair<int,int>, std::vector<std::pair<int,int>>, std::greater<>> pq;
//        pq.push({0, start});
//
//        while(!pq.empty()) {
//            auto [cdist, u] = pq.top(); pq.pop();
//            if(cdist > dist[u]) continue;
//
//            for (auto &edge : adjacency[u]) {
//                int w = edge.station;
//                int cost = edge.travel_time;
//                if(dist[w] > dist[u] + cost) {
//                    dist[w] = dist[u] + cost;
//                    parent[w] = u;
//                    pq.push({dist[w], w});
//                }
//            }
//        }
//
//        if(dist[goal] == INF) {
//            std::cout << "No route found by travel time.\n";
//        } else {
//            std::cout << "Fastest route by travel time: " << dist[goal] << " units.\n";
//            // Reconstruct path
//            std::vector<int> path;
//            for (int cur = goal; cur != -1; cur = parent[cur]) {
//                path.push_back(cur);
//            }
//            std::reverse(path.begin(), path.end());
//            std::cout << "Path: ";
//            for (auto s : path) std::cout << station_names[s] << " ";
//            std::cout << "\n";
//        }
//    }
//
//    // 2. Route with the fewest vehicle changes (same logic as before, ignoring travel times)
//    {
//        int V = vehicle_count;
//        int start_vehicle = V; // represents no vehicle chosen yet
//        std::vector<std::vector<int>> dist_transfers(N, std::vector<int>(V+1, INF));
//        std::vector<std::vector<std::pair<int,int>>> parent_transfers(N, std::vector<std::pair<int,int>> (V+1, {-1,-1}));
//        dist_transfers[start][start_vehicle] = 0;
//
//        std::deque<std::tuple<int,int,int>> dq;
//        dq.push_back({start, start_vehicle, 0});
//
//        while(!dq.empty()) {
//            auto [u, v_id, cur_cost] = dq.front(); dq.pop_front();
//            if(cur_cost > dist_transfers[u][v_id]) continue;
//
//            for(auto &edge : adjacency[u]) {
//                int w = edge.station;
//                int w_vid = edge.vehicle;
//                int add_cost = 0;
//                if(v_id != V && v_id != w_vid) {
//                    add_cost = 1;
//                }
//
//                int new_cost = cur_cost + add_cost;
//                int &best = dist_transfers[w][w_vid];
//                if(new_cost < best) {
//                    best = new_cost;
//                    parent_transfers[w][w_vid] = {u, v_id};
//                    if(add_cost == 0) dq.push_front({w, w_vid, new_cost});
//                    else dq.push_back({w, w_vid, new_cost});
//                }
//            }
//        }
//
//        int min_transfers = INF;
//        int end_vehicle = -1;
//        for(int vid = 0; vid <= V; vid++) {
//            if(dist_transfers[goal][vid] < min_transfers) {
//                min_transfers = dist_transfers[goal][vid];
//                end_vehicle = vid;
//            }
//        }
//
//        if(min_transfers == INF) {
//            std::cout << "No route found for minimal transfers.\n";
//        } else {
//            std::cout << "Fewest vehicle changes: " << min_transfers << "\n";
//            // Reconstruct path
//            std::vector<int> path;
//            int cur_station = goal;
//            int cur_vehicle = end_vehicle;
//            while(!(cur_station == start && cur_vehicle == start_vehicle)) {
//                path.push_back(cur_station);
//                auto p = parent_transfers[cur_station][cur_vehicle];
//                cur_station = p.first;
//                cur_vehicle = p.second;
//            }
//            path.push_back(start);
//            std::reverse(path.begin(), path.end());
//            std::cout << "Path: ";
//            for (auto s : path) std::cout << station_names[s] << " ";
//            std::cout << "\n";
//        }
//    }
//
//    return 0;
//}
