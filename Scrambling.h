#pragma once

#include <random>

static
uint32_t hash3( uint32_t x );

/// Scramble an index
/// @param i the index to scramble
/// @param seed a seed
/// @param m the matrix size
/// @param base the base
/// @returns a new index
uint64_t scramble(uint64_t i, int seed, int m, int base);

/// Owen Scramble an index
/// @param i the index to scramble
/// @param seed a seed
/// @param m the matrix size
/// @param base the base
/// @returns a new index
uint64_t owenScramble(uint64_t i, int seed, int m, int base);