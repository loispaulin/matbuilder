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

#include <fstream>
#include <string>
#include <vector>
#include "MatrixTools.h"
#include "MatrixSamplerClass.h"



/// Returns the index in 1D tab for position (\p row, \p col)
/// @param row the row index
/// @param col the column index
/// @param width matrix width
/// @returns the index in 1D tab for position (\p row, \p col)
int index(int row, int col, int width);



/// Performs \p a . \p b and stores it in \p res
/// @param a the left matrix
/// @param b the right matrix
/// @param res the result
/// @param m the matrices size
/// @param base the matrices base
void matmult(const std::vector<int>& a, const std::vector<int>& b, std::vector<int>& res, int m, int base);




/// Writes a matrix \p B on \p out
/// @param out the output stream
/// @param m the matrix size
/// @param B the matrix
void writeMatrix(std::ostream& out, int m, const std::vector<int>& B);

/// Writes multiple matrices \p B on \p out
/// @param out the output stream
/// @param m the matrices size
/// @param B the matrices
/// @param spacing flag to toggle additional white line between matrices
void writeMatrices(std::ostream& out, int m, const std::vector<std::vector<int>>& B, bool spacing=false);

/// Reads a matrix \p B from \p in
/// @param in the input stream
/// @param m the matrix size
/// @param B the matrix
void readMatrix(std::istream& in, int m, std::vector<int>& B);


/// Reads matrices \p B from \p in
/// @param in the input stream
/// @param m the matrices size
/// @param B the matrices
void readMatrices(std::istream& in, int m, int s, std::vector<std::vector<int>>& B);

/// Initializes multiple MatrixSampler from stream \p in containing B style cascaded matrices
/// Use \p cStyle to true to skip matrices sequential multiplication
/// @param in the input stream
/// @param m the matrix size
/// @param nDims the number of sampler to initialize
/// @param b the basis of matrices
/// @param Cs the output samplers
/// @param cStyle toggles off multiplication
void initSamplersFromStream(std::istream& in, int m, int nDims, int b, std::vector<MatrixSampler>& Cs, bool cStyle=false);

/// Computes newC = B . prevC
/// @param m the matrix size
/// @param base the basis of matrices
/// @param B the B matrix
/// @param prevC the previous C matrix
/// @param newC the new C matrix
void nextC(int m, int base, const std::vector<int>& B, const std::vector<int>& prevC, std::vector<int>& newC);

/// Transforms B matrices in corresponding C matrices
/// C must be a vector of pointers on m*m allocated ints
/// @param m the matrix size
/// @param base the basis of matrices
/// @param B the B matrices
/// @param C output C matrices
void B2C(int m, int base, const std::vector<std::vector<int>>& B, std::vector<std::vector<int>>& C);
