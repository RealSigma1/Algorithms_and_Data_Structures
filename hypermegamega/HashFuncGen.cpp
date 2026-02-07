#include "HashFuncGen.hpp"

uint32_t HashFuncGen::murmur3_32(const std::string& key, uint32_t seed) {
    const uint8_t* data = reinterpret_cast<const uint8_t*>(key.data());
    size_t len = key.size();
    uint32_t h = seed;
    uint32_t c1 = 0xcc9e2d51;
    uint32_t c2 = 0x1b873593;
    size_t i = 0;

    while (len >= 4) {
        uint32_t k = static_cast<uint32_t>(data[i]) |
                     static_cast<uint32_t>(data[i + 1]) << 8 |
                     static_cast<uint32_t>(data[i + 2]) << 16 |
                     static_cast<uint32_t>(data[i + 3]) << 24;

        k *= c1;
        k = (k << 15) | (k >> 17);
        k *= c2;

        h ^= k;
        h = (h << 13) | (h >> 19);
        h = h * 5 + 0xe6546b64;

        i += 4;
        len -= 4;
    }

    uint32_t k = 0;
    switch (len) {
    case 3:
        k ^= static_cast<uint32_t>(data[i + 2]) << 16;
        [[fallthrough]];
    case 2:
        k ^= static_cast<uint32_t>(data[i + 1]) << 8;
        [[fallthrough]];
    case 1:
        k ^= static_cast<uint32_t>(data[i]);
        k *= c1;
        k = (k << 15) | (k >> 17);
        k *= c2;
        h ^= k;
    }

    h ^= static_cast<uint32_t>(key.size());
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;

    return h;
}

uint32_t HashFuncGen::fnv1a_32(const std::string& key) {
    uint32_t hash = 2166136261u;
    for (char c : key) {
        hash ^= static_cast<uint8_t>(c);
        hash *= 16777619u;
    }
    return hash;
}