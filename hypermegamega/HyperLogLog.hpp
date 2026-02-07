#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>

class HyperLogLog {
public:
    explicit HyperLogLog(int b);

    void add(const std::string& element);

    double estimate() const;

    size_t exactCount() const {
        return exact_set_.size();
    }

    void reset();

private:
    int b_;
    size_t m_;
    std::vector<uint8_t> registers_;
    mutable double alpha_;
    std::unordered_set<std::string> exact_set_;

    static int rho(uint32_t hash, int b);
};