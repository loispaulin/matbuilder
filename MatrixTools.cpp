#include <fstream>
#include <string>
#include <vector>
#include "MatrixTools.h"


/// Returns the index in 1D tab for position (\p row, \p col)
/// @param row the row index
/// @param col the column index
/// @param width matrix width
/// @returns the index in 1D tab for position (\p row, \p col)
int index(int row, int col, int width){
    return row * width + col;
}



/// Writes a matrix \p B on \p out
/// @param out the output stream
/// @param m the matrix size
/// @param B the matrix
void writeMatrix(std::ostream& out, int m, const std::vector<int>& B){
    for (int i = 0; i < m; ++i){
        for (int j = 0; j < m; ++j){
            out << B[index(i, j, m)] << " ";
        }
        out << std::endl;
    }
}

/// Writes multiple matrices \p B on \p out
/// @param out the output stream
/// @param m the matrices size
/// @param B the matrices
/// @param spacing flag to toggle additional white line between matrices
void writeMatrices(std::ostream& out, int m, const std::vector<std::vector<int>>& B, bool spacing){
    for (const std::vector<int>& b : B){
        writeMatrix(out, m, b);
        if (spacing) out << std::endl;
    }
}


/// Reads a matrix \p B from \p in
/// @param in the input stream
/// @param m the matrix size
/// @param B the matrix
void readMatrix(std::istream& in, int m, std::vector<int>& B){
  for (int row = 0; row < m; ++row) {
    for (int col = 0; col < m; ++col) {
      in >> B[index(row, col, m)];
    }
  }
}


/// Reads matrices \p B from \p in
/// @param in the input stream
/// @param m the matrices size
/// @param B the matrices
void readMatrices(std::istream& in, int m, int s, std::vector<std::vector<int>>& B){
  int sampler = 0;
  while(sampler < s && !in.eof()){
    char c;
    in >> c;
    if (c == '#') {
      std::string line;
      getline(in, line);
      continue;
    }
    in.putback(c);
    readMatrix(in, m, B[sampler]);
    sampler += 1;
  }
}

/// Initializes multiple MatrixSampler from stream \p in containing B style cascaded matrices
/// Use \p cStyle to true to skip matrices sequential multiplication
/// @param in the input stream
/// @param m the matrix size
/// @param nDims the number of sampler to initialize
/// @param b the basis of matrices
/// @param Cs the output samplers
/// @param cStyle toggles off multiplication
void initSamplersFromStream(std::istream& in, int m, int nDims, int b, std::vector<MatrixSampler>& Cs, bool cStyle) {
  std::vector<int> C(m*m);
  Cs.reserve(nDims);
  
  if (cStyle){
    //Read C and push back_sampler
    int sampler = 0;
    while(sampler < nDims && !in.eof()){
      char c;
      in >> c;
      if (c == '#') {
        std::string line;
        getline(in, line);
        continue;
      }
      in.putback(c);
      readMatrix(in, m, C);
      Cs.emplace_back(C, m, b);
      sampler += 1;
    }
  } else {
    std::vector<int> B(m*m);
    std::vector<int> prev(m*m);
    for (int i =0; i < m; ++i) {
      for (int j = 0; j < m; ++j) {
        prev[index(i,j,m)] = 0;
      }
    }
    for (int i =0; i < m; ++i){
      prev[index(i,i,m)] = 1;
    }
    int sampler = 0;
    while(sampler < nDims && !in.eof()){
      char c;
      in >> c;
      if (c == '#') {
        std::string line;
        getline(in, line);
        continue;
      }
      in.putback(c);
      readMatrix(in, m, B);
      matmult(B, prev, C, m, b);
      Cs.emplace_back(C, m, b);
      prev = C;
      sampler += 1;
    }
  }
}

/// Computes newC = B . prevC
/// @param m the matrix size
/// @param base the basis of matrices
/// @param B the B matrix
/// @param prevC the previous C matrix
/// @param newC the new C matrix
void nextC(int m, int base, const std::vector<int>& B, const std::vector<int>& prevC, std::vector<int>& newC){
  matmult(B, prevC, newC, m, base);
}


/// Transforms B matrices in corresponding C matrices
/// C must be a vector of pointers on m*m allocated ints
/// @param m the matrix size
/// @param base the basis of matrices
/// @param B the B matrices
/// @param C output C matrices
void B2C(int m, int base, const std::vector<std::vector<int>>& B, std::vector<std::vector<int>>& C){
  C[0] = B[0];
  for (size_t i = 1; i < B.size(); ++i){
    matmult(B[i], C[i-1], C[i], m, base);
  }
}
