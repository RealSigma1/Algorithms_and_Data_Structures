#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>

class HyperLogLog {
public:
    explicit HyperLogLog(int b, std::uint32_t seed = 0x9747b28c);

    void add(const std::string& element);

    double estimate() const;

    size_t exactCount() const {
        return exact_set_.size();
    }

    void reset();

private:
    int b_;
    std::size_t m_;
    std::vector<std::uint8_t> registers_;
    mutable double alpha_;
    std::uint32_t seed_;
    std::unordered_set<std::string> exact_set_;

    static int rho(std::uint32_t hash, int b);
};
