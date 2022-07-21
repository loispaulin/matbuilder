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
#include "cplexMatrices.h"

#include <vector>
#include <iostream>
#include <string>
#include <galois++/field.h>
#include <ilcplex/ilocplex.h>
#include "MatrixTools.h"
ILOSTLBEGIN

using namespace std;


/// Computes the determinant of \p C in \p gf
/// @param C matrix to compute determinant of
/// @param m matrix size
/// @param gf Galois field to make computation in
/// @param memory preallocated memory to do gaussian elemination in
int determinant(const vector<int>& C, int m, const Galois::Field& gf, vector<int>& memory){
    memory = C;
    int factorPermut = 1;
    for (int i = 0; i < m; ++i){
        int swapi = i;
        while (swapi < m && memory[index(swapi, i, m)] == 0){
            swapi += 1;
        }
        if (swapi >= m){
            return 0;
        } else if (swapi != i) {
            factorPermut *= -1;
            for (int k = i; k < m; ++k) {
                swap(memory[index(swapi, k, m)], memory[index(i, k, m)]);
            }
        }
        for (int j = i+1; j < m; ++j){
            if (memory[index(j,i,m)] != 0) {
                int factor = gf.neg[gf.times(gf.inv[memory[index(i, i, m)]], memory[index(j, i, m)])];
                for (int k = i; k < m; ++k) {
                    memory[index(j, k, m)] =
                        gf.plus(
                            memory[index(j, k, m)],
                            gf.times(
                                factor,
                                memory[index(i, k, m)]
                            )
                        );
                }
            }
        }
    }
    int res = 1;
    for (int i = 0; i < m; ++i){
        res = gf.times(res, memory[index(i,i,m)]);
    }
    if (factorPermut == -1){
        return gf.neg[res];
    } else {
        return res;
    }
}

/// Computes the linear constraints to add a new column to \p C2 in order to check M_k considering \p C1 is fully known
/// @param C Matrices
/// @param fullSize Size of matrix storage (to use for striding)
/// @param m Size of the matrices to generate < \p fullSize
/// @param k Number of line from each \p C to take
/// @param gf Galois field to make computations in
/// @param subdets Output subdets for considered free variables
void constraintMkSubdets (const vector<const int*>& C, int fullSize, int m, const vector<int>& k,
                          const Galois::Field& gf, vector<int> &subdets){
    vector<int> vecMat((m-1)*(m-1));
    vector<int> mem((m-1)*(m-1));
    int indMat = 0;
    int prevlines = 0;
    // skip first line to init first sub-matrix
    for (int row = 1; row < m; ++row){
        while (row - prevlines >= k[indMat]){
            prevlines += k[indMat];
            indMat += 1;
        }
        for (int col = 0; col < m-1; ++col){
            vecMat[index(row-1,col,m-1)] = C[indMat][index(row - prevlines, col, fullSize)];
        }
    }
    long long int factor = (int)pow(-1.0, m + 1);
    for (int i = 0; i < m; ++i){
        subdets[i] = 0;
    }
    indMat = 0;
    prevlines = 0;
    for (int row = 0; row < m; ++row){
        int idet = factor * determinant(vecMat, m-1, gf, mem);
        subdets[row] = idet;
        factor *= -1;
        while (row - prevlines == k[indMat]){
            prevlines += k[indMat];
            indMat += 1;
        }
        if (row != m-1) {
            for (int col = 0; col < m - 1; ++col) {
                vecMat[index(row, col, m-1)] = C[indMat][index(row - prevlines, col, fullSize)];
            }
        }
    }
    for (auto& w : subdets){
        w = (w + gf.q) % gf.q;
    }
}

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
void constraintMk(const vector<const int*>& C, int fullSize, int m, const vector<int>& k, const Galois::Field& gf,
                  vector<int> &subdets, IloEnv& env, IloNumVarArray& vars, IloNumVarArray &ks,
                  const vector<const int*>& matIndices, IloConstraintArray& c, IloNumExpr& obj, bool weak, double weight,
                  IloNumVarArray& weakvar, const string& label){
    //Get new constraint index
    int indC = int(c.getSize());

    // Name new constraint
    string name = "c_" + to_string(indC) + "_M_";
    for (auto v : k){
        name += to_string(v);
    }
    name += "_" + label;

    //Compute subdets
    constraintMkSubdets(C, fullSize, m, k, gf, subdets);

    //Write det formula
    IloNumExpr det(env);
    int indMat = 0;
    int prevlines = 0;
    for (int j = 0; j < m; ++j){
        while (j - prevlines >= k[indMat]){
            prevlines += k[indMat];
            indMat += 1;
        }
        det += subdets[j] * vars[matIndices[indMat][j - prevlines]];
    }
    // ki represents base gf.q modulo
    IloNumVar ki(env, 0, IloInfinity, ILOINT);
    string varname = "k_" + to_string(indC);
    ki.setName(varname.c_str());
    ks.add(ki);
    det -= ki * gf.q;
    //vars.add(ki);
    // Det must be non 0
    if (weak){
        if (weight >= 0) {
            IloNumVar x(env, 0, 1, ILOINT);
            varname = "x_" + name;
            x.setName(varname.c_str());
            weakvar.add(x);
            c.add(x <= det);
            c.add(det <= gf.q - 1);
            c[indC].setName(name.c_str());
            obj += -weight * x;
        } else {
            IloNumVar x(env, 0, 1, ILOINT);
            varname = "x_" + name;
            x.setName(varname.c_str());
            weakvar.add(x);
            c.add( x*gf.q >= det);
            c.add( det >= 0);
            c[indC].setName(name.c_str());
            obj += -weight * x;
        }
    } else {
        c.add(1 <= det <= gf.q-1);
        c[indC].setName(name.c_str());
    }
}


/// Enumerate next choice of different \p positions among [0..\p max]
/// @param positions the positions to be iterated on
/// @param max the maximum value of the range
bool advancePositions(vector<int>& positions, int max){
    int ind = 0;
    int end = int(positions.size()) - 1;
    // Find first position not packed to the end of range
    while(ind <= end && positions[end - ind] == max - ind){
        ind += 1;
    }
    // If none is found return false
    if (ind > end){
        return false;
    }
    // If found then advance it by one
    positions[end - ind] += 1;
    // And reset other positions after it
    for (int j = ind - 1; j >= 0; --j){
        positions[end - j] = positions[end - j - 1] + 1;
    }
    return true;
}


void positions2k(const vector<int>& positions, vector<int>& k, int& unbalance, int max){
    if (positions.empty()){
        k[0] = max + 1;
        unbalance = 0;
        return;
    }
    k[0] = positions[0];
    for (int i = 1; i < positions.size(); ++i){
        k[i] = positions[i] - positions[i-1] - 1;
    }
    k[positions.size()] = max - positions.back();
    int low = max;
    int high = 0;
    for (int v : k){
        if (v != 0){
            if (v > high)
                high = v;
            if (v < low)
                low = v;
        }
    }
    unbalance = high - low;
}

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
void zeronetProperty(const vector<const int*>& C, int fullSize, int m, const Galois::Field& gf, vector<int> &subdets,
                     IloEnv& env, IloNumVarArray& vars, IloNumVarArray &ks, const vector<const int*>& matIndices,
                     IloConstraintArray& c, IloNumExpr& obj, bool weak, double weight, IloNumVarArray& weakvar,
                     int max_unbalance, const string& label){
    int s = int(C.size());
    vector<int> k(s);
    vector<int> positions(s-1);
    for (int i = 0; i < s-1; ++i){
        positions[i] = i;
    }
    do{
        int unbalance;
        positions2k(positions, k, unbalance, m+s-2);
        if (unbalance <= max_unbalance) {
            constraintMk(C, fullSize, m, k, gf, subdets, env, vars, ks, matIndices, c, obj, weak, weight, weakvar,
                         label);
        }
    } while(advancePositions(positions, m+s-2));

}


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
void stratifiedProperty(const vector<const int*>& C, int fullSize, int m, const Galois::Field& gf, vector<int> &subdets,
                        IloEnv& env, IloNumVarArray& vars, IloNumVarArray &ks, const vector<const int*>& matIndices,
                        IloConstraintArray& c, IloNumExpr& obj, bool weak, double weight, IloNumVarArray& weakvar,
                        const string& label){
    int s = C.size();
    int unbalance = m%s;
    int nbLines = m/s;

    // positions contains the positions at which to add the extra lines to correct stratification unbalance
    vector<int> positions(unbalance);
    for (int i = 0; i < unbalance; ++i){
        positions[i] = i;
    }

    vector<int> k(s, nbLines);
    do {
        for (int v: positions) {
            k[v] += 1;
        }
        constraintMk(C, fullSize, m, k, gf, subdets, env, vars, ks, matIndices, c, obj, weak, weight, weakvar, label);
        for (int v: positions) {
            k[v] -= 1;
        }
    } while(advancePositions(positions, s-1));

}


int getMdet(const vector<const int*>& C, int fullsize, int m, const vector<int>& k, const Galois::Field& gf){
    vector<int> mat(m*m);
    vector<int> mem(m*m);
    int indMat = 0;
    int prevlines = 0;
    for (int row = 0; row < m; ++row){
        while(row - prevlines == k[indMat]){
            prevlines += k[indMat];
            indMat += 1;
        }
        for (int col = 0; col < m; ++col){
            mat[index(row,col,m)] = C[indMat][index(row - prevlines, col, fullsize)];
        }
    }
    return determinant(mat, m, gf, mem);
}

/// Tests whether the s matrices \p C are (0,m,s)-net
/// @param C Matrices
/// @param fullSize Size of matrix storage (to use for striding)
/// @param m Size of the matrices to generate < \p fullSize
/// @param k Number of line from \p C1 to take
/// @param gf Galois field to make computations in
bool checkzeronet(const vector<const int*>& C, int fullSize, int m, int max_unbalance, const Galois::Field& gf){
    int s = int(C.size());
    vector<int> k(s);
    vector<int> positions(s-1);
    for (int i = 0; i < s-1; ++i){
        positions[i] = i;
    }
    bool result = true;
    do{
        int unbalance;
        positions2k(positions, k, unbalance, m+s-2);
        if (unbalance <= max_unbalance) {
            bool mytest = (getMdet(C, fullSize, m, k, gf) != 0);
            result = result && mytest;
        }
    } while(advancePositions(positions, m+s-2));

    return result;
}

/// Tests whether matrices \p C are stratified
/// @param C Matrices
/// @param fullSize Size of matrix storage (to use for striding)
/// @param m Size of the matrices to generate < \p fullSize
/// @param k Number of line from \p C1 to take
/// @param gf Galois field to make computations in
bool checkStratified(const vector<const int*>& C, int fullSize, int m, const Galois::Field& gf){
    int s = C.size();
    int unbalance = m%s;
    int nbLines = m/s;

    // positions contains the positions at which to add the extra lines to correct stratification unbalance
    vector<int> positions(unbalance);
    for (int i = 0; i < unbalance; ++i){
        positions[i] = i;
    }

    vector<int> k(s, nbLines);
    bool result = true;
    do {
        for (int v: positions) {
            k[v] += 1;
        }
        result = result && (getMdet(C, fullSize, m, k, gf) != 0);
        for (int v: positions) {
            k[v] -= 1;
        }
    } while(advancePositions(positions, s-1));

    return result;

}

/// Initializes variable to generate a new column for \p s matrices of size \p m in base \p b
void initVar(IloEnv& env, int s, int m, int b, IloNumVarArray& var, vector<vector<int>>& matIndices){
    for (int j = 0; j < s; ++j){
        for (int i = 0; i < m; ++i){
            matIndices[j][i] = int(var.getSize());
            IloIntVar x(env, 0, b-1);
            string name = "x_{" + to_string(j) + "," + to_string(i) + "}";
            x.setName(name.c_str());
            var.add(x);
        }
    }
}

/// Generates a random objective value for each variable in \p vars
IloNumExpr randomObjective(mt19937_64& gen, IloNumVarArray& vars, IloEnv& env, int b){
    uniform_int_distribution<int> unif(0,b-1);
    IloNumExpr val(env, 0);
    for (int i = 0; i < int(vars.getSize()); ++i){
        val += IloAbs(vars[i] - unif(gen));
    }
    return val;
}
