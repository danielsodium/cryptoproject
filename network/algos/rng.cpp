#include "rng.h"
#include <ctime>
#include <cstdint>

long RNG::generate() {
    static uint64_t seed = time(nullptr); //Using current time as seed

    //Constants
    const uint64_t a = 1664525;
    const uint64_t c = 1013904223;
    const uint64_t m = (1ULL << 32); // 2^32

    seed = (a * seed + c) % m;

    return static_cast<long>(seed);
}
