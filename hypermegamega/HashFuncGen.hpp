#pragma once

#include <cstdint>
#include <string>

class HashFuncGen {
public:
    static uint32_t murmur3_32(const std::string& key,
                               uint32_t seed = 0x9747b28c);

    static uint32_t fnv1a_32(const std::string& key);
};