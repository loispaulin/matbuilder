/*
Copyright 2022, CNRS

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
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
