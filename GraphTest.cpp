//
// Created by Gime on 16/12/2024.
//
#include <cassert>
#include <iostream>
#include <sstream>
#include "RoutePlanner.h"

using namespace std;

class GraphTest {
public:
    static void runTests() {
        testFastestRoute();
        testMinimalTransfers();
        testInvalidFormat();
        cout << "All tests passed!" << endl;
    }

private:
    // Test the fastest route calculation.
    static void testFastestRoute() {
        // Prepare station data.
        stringstream stations_stream;
        stations_stream << "A\nB\nC\nD\nE\nF\nG\nH\nI\nJ\n";

        // Prepare vehicles data.
        // Bus1: A -> B (3), B -> C (4), C -> D (2)
        // Tram1: C -> E (1), E -> F (5), F -> G (3)
        // Bus2: D -> H (10), H -> I (2), I -> J (4)
        // Train1: A -> G (15), H -> 7
        // (Make sure to use the correct format; for Train1: "Train1 A G 15 H 7")
        stringstream vehicles_stream;
        vehicles_stream << "Bus1 A B 3 B C 4 C D 2\n";
        vehicles_stream << "Tram1 C E 1 E F 5 F G 3\n";
        vehicles_stream << "Bus2 D H 10 H I 2 I J 4\n";
        vehicles_stream << "Train1 A G 15 H 7\n";

        RoutePlanner planner;
        bool ok = planner.loadStations(stations_stream);
        assert(ok);
        ok = planner.loadVehicles(vehicles_stream);
        assert(ok);

        // Test: Fastest route from A to J.
        auto [cost, path] = planner.getFastestRoute("A", "J");

        // Expected:
        // Bus1: A->B (3), B->C (4), C->D (2) = 9
        // Bus2: D->H (10), H->I (2), I->J (4) = 16
        // Total = 9 + 16 = 25 (this is the best option).
        assert(cost == 25);
        assert(!path.empty());
        const auto &stationNames = planner.getStationNames();
        assert(stationNames[path.front()] == "A");
        assert(stationNames[path.back()] == "J");
    }

    // Test the minimal transfers route calculation.
    static void testMinimalTransfers() {
        // Prepare station data.
        stringstream stations_stream;
        stations_stream << "A\nB\nC\nD\nE\nF\nG\nH\nI\nJ\n";

        // Prepare vehicles data (same as before).
        stringstream vehicles_stream;
        vehicles_stream << "Bus1 A B 3 B C 4 C D 2\n";
        vehicles_stream << "Tram1 C E 1 E F 5 F G 3\n";
        vehicles_stream << "Bus2 D H 10 H I 2 I J 4\n";
        vehicles_stream << "Train1 A G 15 H 7\n";

        RoutePlanner planner;
        bool ok = planner.loadStations(stations_stream);
        assert(ok);
        ok = planner.loadVehicles(vehicles_stream);
        assert(ok);

        // Test: Minimal transfers route from A to J.
        auto [transfers, path] = planner.getMinimalTransfersRoute("A", "J");

        // One likely solution:
        // Bus1 from A to D, then Bus2 from D to J,
        // which implies 1 transfer (switching vehicles).
        assert(transfers == 1);
        const auto &stationNames = planner.getStationNames();
        assert(stationNames[path.front()] == "A");
        assert(stationNames[path.back()] == "J");
    }

    // Test that an invalid vehicle format is correctly detected.
    static void testInvalidFormat() {
        // Prepare station data.
        stringstream stations_stream;
        stations_stream << "A\nB\nC\n";

        // Prepare an invalid vehicles line (missing a travel time).
        stringstream vehicles_stream;
        vehicles_stream << "Bus1 A B 3 B C\n";  // Second leg is missing the travel time.

        RoutePlanner planner;
        bool ok = planner.loadStations(stations_stream);
        assert(ok);

        // loadVehicles should return false.
        bool vehiclesOk = planner.loadVehicles(vehicles_stream);
        assert(!vehiclesOk);
    }
};

int main() {
    GraphTest::runTests();
    return 0;
}
