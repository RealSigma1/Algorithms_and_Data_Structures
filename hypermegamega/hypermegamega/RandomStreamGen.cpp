#include "RandomStreamGen.hpp"
#include <algorithm>
#include <iterator>

RandomStreamGen::RandomStreamGen(unsigned int seed)
    : rng_(seed) {
}

std::string RandomStreamGen::generateElement() {
    static std::uniform_int_distribution<size_t> len_dist(1, 30);
    size_t len = len_dist(rng_);

    std::string element(len, ' ');
    std::uniform_int_distribution<size_t> char_dist(0, charset_.size() - 1);
    for (size_t i = 0; i < len; ++i) {
        element[i] = charset_[char_dist(rng_)];
    }
    return element;
}

std::vector<std::string> RandomStreamGen::generateStream(size_t n) {
    std::vector<std::string> stream;
    stream.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        stream.push_back(generateElement());
    }
    return stream;
}