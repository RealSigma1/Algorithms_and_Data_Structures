#include "HyperLogLog.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include "HashFuncGen.hpp"

static int clz32(std::uint32_t x) {
    if (x == 0)
        return 32;
    int r = 0;
    while ((x & 0x80000000u) == 0u) {
        ++r;
        x <<= 1;
    }
    return r;
}

HyperLogLog::HyperLogLog(int b)
    : b_(b)
    , m_(1ULL << b)
    , registers_(m_, 0) {
    if (b < 4 || b > 16) {
        throw std::invalid_argument("b must be in [4, 16]");
    }

    if (m_ == 16)
        alpha_ = 0.673;
    else if (m_ == 32)
        alpha_ = 0.697;
    else if (m_ == 64)
        alpha_ = 0.709;
    else
        alpha_ = 0.7213 / (1.0 + 1.079 / static_cast<double>(m_));
}

int HyperLogLog::rho(std::uint32_t hash, int b) {
    std::uint32_t w = hash << b;
    if (w == 0)
        return 32 - b + 1;
    return clz32(w) + 1;
}

void HyperLogLog::add(const std::string& element) {
    exact_set_.insert(element);

    std::uint32_t hash = HashFuncGen::murmur3_32(element);
    std::uint32_t index = hash >> (32 - b_);
    int r = rho(hash, b_);
    if (r > registers_[index]) {
        registers_[index] = static_cast<std::uint8_t>(r);
    }
}

double HyperLogLog::estimate() const {
    double sum = 0.0;
    int zero_registers = 0;

    for (std::uint8_t r : registers_) {
        sum += 1.0 / static_cast<double>(1ULL << r);
        if (r == 0)
            ++zero_registers;
    }

    double E = alpha_ * static_cast<double>(m_) * static_cast<double>(m_) / sum;

    if (E <= 2.5 * static_cast<double>(m_)) {
        if (zero_registers != 0) {
            E = static_cast<double>(m_) *
                std::log(static_cast<double>(m_) /
                         static_cast<double>(zero_registers));
        }
    } else if (E > static_cast<double>((1ULL << 32)) / 30.0) {
        E = -static_cast<double>(1ULL << 32) *
            std::log(1.0 - E / static_cast<double>(1ULL << 32));
    }

    return E;
}

void HyperLogLog::reset() {
    std::fill(registers_.begin(), registers_.end(), 0);
    exact_set_.clear();
}
