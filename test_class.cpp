//
// Created by Gime on 16/12/2024.
//

#include "test_class.h"
#include <iostream>
#include <random>

test_class::test_class(){
    std::cout << "class test made!" << std::endl;
}

int test_class::getRandomNum() {
    // Create a random device
    std::random_device rd;

    // Seed the random number generator
    std::mt19937 gen(rd());

    // Define the range [min, max]
    int min = 1, max = 100;
    std::uniform_int_distribution<int> dist(min, max);

    // Generate a random integer in the range
    return dist(gen);
};
