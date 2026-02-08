#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "HyperLogLog.hpp"

class HyperLogLogAvg {
public:
    HyperLogLogAvg(int b, std::size_t k, std::uint32_t base_seed = 0x9747b28c);

    void add(const std::string& element);

    double estimateMean() const;

    std::size_t exactCount() const;

    void reset();

private:
    std::vector<HyperLogLog> sketches_;
};
