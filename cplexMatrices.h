#pragma once
/*
MIT License

Copyright (c) 2021 CNRS

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



#include <vector>
#include <random>
#include <iostream>
#include <string>
#include <galois++/field.h>
#include <ilcplex/ilocplex.h>

/// Computes the linear constraints to add a new column to matrices in \p C in order to check M_k
/// @param C Matrices
/// @param fullSize Size of matrix storage (to use for striding)
/// @param m Size of the matrices to generate < \p fullSize
/// @param k number of lines to take from each matrix
/// @param gf Galois field to make computations in
/// @param subdets Memory for subdets for considered free variables
/// @param env Concert solver environement
/// @param vars Solver variable array containing matrices new columns variables
/// @param matIndices For each matrix a table containing its variables indices in \p vars
/// @param c set of constraints to add new constraint to
void constraintMk(const std::vector<const int*>& C, int fullSize, int m, const std::vector<int>& k, const Galois::Field& gf,
                  std::vector<int> &subdets, IloEnv& env, IloNumVarArray& vars, IloNumVarArray &ks,
                  const std::vector<const int*>& matIndices, IloConstraintArray& c, IloNumExpr& obj, bool weak,
                  double weight, IloNumVarArray& weakvar, const std::string& label="");

/// Adds to \p c constraints for all s matrices in \p C to have the (0,m,s)-net property
/// @param C Matrices
/// @param fullSize Size of matrix storage (to use for striding)
/// @param m Size of the matrices to generate < \p fullSize
/// @param gf Galois field to make computations in
/// @param subdets Memory for subdets for considered free variables
/// @param env Concert solver environement
/// @param vars Solver variable array containing matrices new columns variables
/// @param matIndices For each matrix a table containing its variables indices in \p vars
/// @param c set of constraints to add new constraint to
void zeronetProperty(const std::vector<const int*>& C, int fullSize, int m, const Galois::Field& gf, std::vector<int> &subdets,
                     IloEnv& env, IloNumVarArray& vars, IloNumVarArray &ks, const std::vector<const int*>& matIndices,
                     IloConstraintArray& c, IloNumExpr& obj, bool weak, double weight, IloNumVarArray& weakvar,
                     int max_unbalance, const std::string& label="");


/// Computes the linear constraints to add a new column to \p C2 in order to check M_k considering \p C1 is fully known
/// @param C Matrices
/// @param fullSize Size of matrix storage (to use for striding)
/// @param m Size of the matrices to generate < \p fullSize
/// @param k Number of line from \p C1 to take
/// @param gf Galois field to make computations in
/// @param subdets Output subdets for considered free variables
/// @param env Concert solver environement
/// @param vars Solver variable array containing matrices new columns variables
/// @param mat1Indices Indices of C1 variables in \p vars
/// @param mat1Indices Indices of C2 variables in \p vars
/// @param c set of constraints to add new constraint to
void stratifiedProperty(const std::vector<const int*>& C, int fullSize, int m, const Galois::Field& gf, std::vector<int> &subdets,
                        IloEnv& env, IloNumVarArray& vars, IloNumVarArray &ks,
                        const std::vector<const int*>& matIndices, IloConstraintArray& c, IloNumExpr& obj, bool weak,
                        double weight, IloNumVarArray& weakvar, const std::string& label="");


int getMdet(const std::vector<const int*>& C, int fullsize, int m, const std::vector<int>& k, const Galois::Field& gf);

/// Tests whether the s matrices \p C are (0,m,s)-net
/// @param C Matrices
/// @param fullSize Size of matrix storage (to use for striding)
/// @param m Size of the matrices to generate < \p fullSize
/// @param k Number of line from \p C1 to take
/// @param gf Galois field to make computations in
bool checkzeronet(const std::vector<const int*>& C, int fullSize, int m, int max_unbalance, const Galois::Field& gf);

/// Tests whether matrices \p C are stratified
/// @param C Matrices
/// @param fullSize Size of matrix storage (to use for striding)
/// @param m Size of the matrices to generate < \p fullSize
/// @param k Number of line from \p C1 to take
/// @param gf Galois field to make computations in
bool checkStratified(const std::vector<const int*>& C, int fullSize, int m, const Galois::Field& gf);

/// Initializes variable to generate a new column for \p s matrices of size \p m in base \p b
void initVar(IloEnv& env, int s, int m, int b, IloNumVarArray& var, std::vector<std::vector<int>>& matIndices);

/// Generates a random objective value for each variable in \p vars
IloNumExpr randomObjective(std::mt19937_64& gen, IloNumVarArray& vars, IloEnv& env, int b);
