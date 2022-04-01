#pragma once
/*
MIT License

Copyright (c) 2022 CNRS

Loïs Paulin, David Coeurjolly, Jean-Claude Iehl, Nicolas Bonneel, Alexander Keller, Victor Ostromoukhov
"MatBuilder: Mastering Sampling Uniformity Over Projections", ACM Transactions on Graphics (Proceedings of SIGGRAPH), August 2022.


Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
