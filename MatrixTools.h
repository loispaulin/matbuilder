#pragma once

#include <fstream>
#include <string>
#include <vector>


/// Returns the index in 1D tab for position (\p row, \p col)
/// @param row the row index
/// @param col the column index
/// @param width matrix width
/// @returns the index in 1D tab for position (\p row, \p col)
int index(int row, int col, int width);


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
