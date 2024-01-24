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


void ProjectiveProperty(int m, const std::vector<int>& dims, const std::vector<std::vector<int>>& matrices, 
                        int matrix_m, 
                        const std::vector<std::vector<int>>& matIndices, 
                        IloEnv& env, IloNumVarArray& vars, IloConstraintArray& c);

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

bool checkProjective(const std::vector<std::vector<int>>& C, int m, int fullM, const std::vector<std::vector<int>>& matrices, int matrix_m, const std::vector<int>& dimensions);

/// Initializes variable to generate a new column for \p s matrices of size \p m in base \p b
void initVar(IloEnv& env, int s, int m, int b, IloNumVarArray& var, std::vector<std::vector<int>>& matIndices);

/// Generates a random objective value for each variable in \p vars
IloNumExpr randomObjective(std::mt19937_64& gen, IloNumVarArray& vars, IloEnv& env, int b);
