#pragma once

#include <random>
#include <string>
#include <vector>

class RandomStreamGen {
public:
    RandomStreamGen(unsigned int seed = std::random_device{}());

    std::string generateElement();

    std::vector<std::string> generateStream(size_t n);

private:
    std::mt19937 rng_;
    const std::string charset_ =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-";
};