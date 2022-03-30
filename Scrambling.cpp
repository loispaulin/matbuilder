#include <random>
#include "Scrambling.h"

static uint32_t hash3( uint32_t x ) {
    // finalizer from murmurhash3
    x ^= x >> 16;
    x *= 0x85ebca6bu;
    x ^= x >> 13;
    x *= 0xc2b2ae35u;
    x ^= x >> 16;
    return x;
}

/// Scramble an index
/// @param i the index to scramble
/// @param seed a seed
/// @param m the matrix size
/// @param base the base
/// @returns a new index
uint64_t scramble(uint64_t i, int seed, int m, int base)
{
    std::minstd_rand gen(seed);
    std::uniform_int_distribution<int> unif(0,base-1);
    uint64_t res = 0;
    uint64_t current = 1;
    for (int digit = 0; digit < m; ++digit){
        res += current * uint64_t((i%base + unif(gen))%base);
        current *= base;
        i /= base;
    }
    return res;
}


/// Owen Scramble an index
/// @param i the index to scramble
/// @param seed a seed
/// @param m the matrix size
/// @param base the base
/// @returns a new index
uint64_t owenScramble(uint64_t i, int seed, int m, int base){
    std::minstd_rand gen(hash3(seed));
    std::uniform_int_distribution<int> unif(0,base-1);
    std::uniform_int_distribution<int> nextSeed;
    uint64_t res = 0;
    //We start from strong digits
    uint64_t current = uint64_t(round(pow(base, m-1)));

    for (int pos = 0; pos < m; ++pos){
        int permut = unif(gen);
        int digit = (i / current) % base;
        //Apply permutation
        res += uint64_t((digit + permut) % base) * current;
        //Move to next node in Owen tree
        gen.seed(hash3(nextSeed(gen) + digit));
        //Move to next digit
        i -= digit * current;
        current /= uint64_t(base);
    }
    return res;
}