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
#include <cstring>
#include <array>
#include <iostream>
#include <vector>
#include <bitset>
#include <cassert>

#include "Scrambling.h"

class MatrixSampler {

public:

  //matrix
  uint64_t m_size;
  std::vector<int> m_m;
    uint64_t m_base;

  MatrixSampler();

  MatrixSampler(const std::vector<int>&mat, const uint64_t size, const uint64_t base);

  void init(const std::vector<int>&mat, const uint64_t size, const uint64_t base);

  const std::vector<int>& getMat() const;

  /// Returns the n-th sample (int version)
  /// @param n the index of sample to get
  uint64_t getInt(uint64_t n) const;

  /// Returns the n-th sample as if matrix was of size \p m
  /// @param n the index of sample to get
  /// @param m the fake matrix size
  uint64_t getIntSubMatrix(uint64_t n, uint64_t m) const;

  /// Returns the n-th sample (double version)
  /// @param n the index of sample to get
  double getDouble(uint64_t n) const;

  ///Casts an index int -> double
  /// @param n the index of sample to get
  double toDouble(uint64_t n) const;


  /// Returns the n-th scrambled sample
  /// @param n the index of sample to get
  uint64_t getScrambledInt(uint64_t n, int seed, int depth) const;

  /// Returns the n-th scrambled sample
  /// @param n the index of sample to get
  double getScrambledDouble(uint64_t n, int seed, int depth) const;

  friend std::ostream &operator<<(std::ostream &out, const MatrixSampler &sampler);

};

/// Returns the n-th sample (int version)
/// @param mat the matrix
/// @param size the matrix size
/// @param base the basis
/// @param n the index
/// @returns the n-th int sample
uint64_t getInt(const std::vector<int>&mat, const uint64_t size, const uint64_t base, uint64_t n);

/// Returns the n-th sample (double version)
/// @param mat the matrix
/// @param size the matrix size
/// @param base the basis
/// @param n the index
/// @returns the n-th double sample
double getDouble(const std::vector<int>&mat, const uint64_t size, const uint64_t base, uint64_t n);

/// Returns the n-th sample (int version)
/// @param mat the matrix
/// @param size the matrix size
/// @param base the basis
/// @param n the index
/// @returns the owen scrambled n-th int sample
uint64_t getScrambledInt(const std::vector<int>&mat, const uint64_t size, const uint64_t base, uint64_t n, int seed,
                         int depth);

/// Returns the n-th sample (double version)
/// @param mat the matrix
/// @param size the matrix size
/// @param base the basis
/// @param n the index
/// @returns the owen scrambled n-th double sample
double getScrambledDouble(const std::vector<int>& mat, const uint64_t size, const uint64_t base, uint64_t n, int seed,
                          int depth);
