#include "HyperLogLogAvg.hpp"
#include <cstdint>

static std::uint32_t mix_seed(std::uint32_t x) {
    x ^= x >> 16;
    x *= 0x7feb352du;
    x ^= x >> 15;
    x *= 0x846ca68bu;
    x ^= x >> 16;
    return x;
}

HyperLogLogAvg::HyperLogLogAvg(int b, std::size_t k, std::uint32_t base_seed) {
    sketches_.reserve(k);
    for (std::size_t i = 0; i < k; ++i) {
        std::uint32_t s = mix_seed(base_seed + static_cast<std::uint32_t>(i * 0x9e3779b9u));
        sketches_.emplace_back(b, s);
    }
}

void HyperLogLogAvg::add(const std::string& element) {
    for (auto& h : sketches_) {
        h.add(element);
    }
}

double HyperLogLogAvg::estimateMean() const {
    double sum = 0.0;
    for (const auto& h : sketches_) {
        sum += h.estimate();
    }
    return sum / static_cast<double>(sketches_.size());
}

std::size_t HyperLogLogAvg::exactCount() const {
    if (sketches_.empty())
        return 0;
    return sketches_[0].exactCount();
}

void HyperLogLogAvg::reset() {
    for (auto& h : sketches_) {
        h.reset();
    }
}
