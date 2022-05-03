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

#include "MatrixSamplerClass.h"
#include "Scrambling.h"

MatrixSampler::MatrixSampler() {}

MatrixSampler::MatrixSampler(const std::vector<int>&mat, const uint64_t size, const uint64_t base) {
        m_m = mat;
        m_size = size;
        m_base = base;
}

void MatrixSampler::init(const std::vector<int>&mat, const uint64_t size, const uint64_t base) {
    m_m = mat;
    m_size = size;
    m_base = base;
}

const std::vector<int>& MatrixSampler::getMat() const{
    return m_m;
}

/// Returns the n-th sample (int version)
/// @param n the index of sample to get
uint64_t MatrixSampler::getInt(uint64_t n) const {
    assert(m_size <= 64);
    std::array<uint64_t, 64> digits;
    uint64_t current = 1;
    for (int i = 0; i < m_size; ++i){
        digits[i] = (n / current) % m_base;
        current *= m_base;
    }
    uint64_t result = 0;
    for (int row = 0; row < m_size; ++row){
        //Radical inverse is encoded in current
        current /= m_base;
        uint64_t total = 0;
        for (int i = 0; i < m_size; ++i){
            total += digits[i] * uint64_t(m_m[row * m_size + i]);
        }
        result += (total % m_base) * current;
    }
    return result;
}

/// Returns the n-th sample as if matrix was of size \p m
/// @param n the index of sample to get
/// @param m the fake matrix size
uint64_t MatrixSampler::getIntSubMatrix(uint64_t n, uint64_t m) const {
    return getInt(n) / uint64_t(round(pow(double(m_base), double(m_size-m))));
}

/// Returns the n-th sample (double version)
/// @param n the index of sample to get
double MatrixSampler::getDouble(uint64_t n) const {
    uint64_t res = getInt(n);
    return double(res) / pow(double(m_base), m_size);
}

///Casts an index int -> double
/// @param n the index of sample to get
double MatrixSampler::toDouble(uint64_t n) const {
    return double(n) / pow(double(m_base), m_size);
}


/// Returns the n-th scrambled sample
/// @param n the index of sample to get
uint64_t MatrixSampler::getScrambledInt(uint64_t n, int seed, int depth) const {
    uint64_t i = getInt(n) * uint64_t(pow(m_base, depth - m_size));
    return owenScramble(i, seed, depth, m_base);
}

/// Returns the n-th scrambled sample
/// @param n the index of sample to get
double MatrixSampler::getScrambledDouble(uint64_t n, int seed, int depth) const {
    return getScrambledInt(n, seed, depth) / pow(double(m_base), depth);
}

std::ostream &operator<<(std::ostream &out, const MatrixSampler &sampler) {

    for (int i = 0; i < sampler.m_size; ++i) {
        for (int j = int(sampler.m_size)-1; j >= 0; --j){
            out << sampler.m_m[j + i * sampler.m_size] << " " ;
        }
        out << std::endl;
    }

    return out;
}


/// Returns the n-th sample (int version)
/// @param mat the matrix
/// @param size the matrix size
/// @param base the basis
/// @param n the index
/// @returns the n-th int sample
uint64_t getInt(const std::vector<int>&mat, const uint64_t size, const uint64_t base, uint64_t n) {
    assert(size <= 64);
    std::array<uint64_t, 64> digits;
    uint64_t current = 1;
    for (int i = 0; i < size; ++i){
        digits[i] = (n / current) % base;
        current *= base;
    }
    uint64_t result = 0;
    for (int row = 0; row < size; ++row){
        //Radical inverse is encoded in current
        current /= base;
        uint64_t total = 0;
        for (int i = 0; i < size; ++i){
            total += digits[i] * uint64_t(mat[row * size + i]);
        }
        result += (total % base) * current;
    }
    return result;
}

/// Returns the n-th sample (double version)
/// @param mat the matrix
/// @param size the matrix size
/// @param base the basis
/// @param n the index
/// @returns the n-th double sample
double getDouble(const std::vector<int>&mat, const uint64_t size, const uint64_t base, uint64_t n) {
    uint64_t res = getInt(mat, size, base, n);
    return double(res) / pow(double(base), double(size));
}

/// Returns the n-th sample (int version)
/// @param mat the matrix
/// @param size the matrix size
/// @param base the basis
/// @param n the index
/// @returns the owen scrambled n-th int sample
uint64_t getScrambledInt(const std::vector<int>&mat, const uint64_t size, const uint64_t base, uint64_t n, int seed,
                         int depth) {
    uint64_t i = getInt(mat, size, base, n) * uint64_t(pow(base, depth - size));
    return owenScramble(i, seed, depth, base);
}

/// Returns the n-th sample (double version)
/// @param mat the matrix
/// @param size the matrix size
/// @param base the basis
/// @param n the index
/// @returns the owen scrambled n-th double sample
double getScrambledDouble(const std::vector<int>& mat, const uint64_t size, const uint64_t base, uint64_t n, int seed,
                          int depth) {
    return getScrambledInt(mat, size, base, n, seed, depth) / pow(double(base), depth);
}
