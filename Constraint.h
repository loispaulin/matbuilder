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
#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <ilcplex/ilocplex.h>
#include <galois++/field.h>

class Constraint{
public:
    enum Type {Stratified, Net, PropA, PropAprime, Proj};
    Type type;
    std::vector<int> dimensions;
    
    std::vector<std::vector<int>> matrices;
    unsigned int matrix_m;
    std::string filename;

    bool weak = false;
    double weight;
    int start = -1;
    int end = std::numeric_limits<int>::max();
    int max_unbalance = std::numeric_limits<int>::max();

    void set(std::vector<std::vector<int>>& C, int m) const;

    void add(const std::vector<std::vector<int>>& C, int fullSize, int m, const Galois::Field& gf, std::vector<int> &subdets,
             IloEnv& env, IloNumVarArray& vars, IloNumVarArray &ks, const std::vector<std::vector<int>>& matIndices,
             IloConstraintArray& c, IloNumExpr& obj, IloNumVarArray& weakvar) const;
    bool check(const std::vector<std::vector<int>>& C, int fullSize, int m, const Galois::Field& gf) const;
    std::string tostring() const;
};

void parseFile(std::istream& in, std::vector<Constraint>& constraints, int& s, int& m, int& b);