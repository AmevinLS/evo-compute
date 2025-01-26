#pragma once

#include <random>
#include <vector>

int random_num(int start, int end) {
    std::random_device rd;  // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> d(start, end - 1); // define the range
    return d(gen);
}

int random_num(const std::vector<int> &weights) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<> d(weights.cbegin(), weights.cend());
    return d(gen);
}

int random_num(const std::vector<double> &weights) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<> d(weights.cbegin(), weights.cend());
    return d(gen);
}