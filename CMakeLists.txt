cmake_minimum_required(VERSION 3.30)
project(Public_transit_search)

set(CMAKE_CXX_STANDARD 20)

add_executable(Public_transit_search main.cpp
        GraphTest.cpp
        RoutePlanner.cpp
        RoutePlanner.h
)
