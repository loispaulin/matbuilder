#pragma once
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
