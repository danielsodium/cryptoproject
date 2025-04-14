#include "rng.h"
#include <random>
#include <chrono>

long RNG::generate() {
    std::mt19937_64 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    return rng() % 1000000000;
}
