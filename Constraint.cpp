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
#include <iostream>
#include "Constraint.h"
#include "cplexMatrices.h"

using namespace std;

void Constraint::add(const std::vector<vector<int>> &C, int fullSize, int m, const Galois::Field& gf, std::vector<int> &subdets,
                     IloEnv &env, IloNumVarArray &vars, IloNumVarArray &ks, const std::vector<vector<int>> &matIndices,
                     IloConstraintArray &c, IloNumExpr& obj, IloNumVarArray& weakvar)
const {
    if (m < start || m > end)
        return;
    vector<const int*> selC(dimensions.size());
    vector<const int*> selInd(dimensions.size());
    string label;
    if (weak){
        label += "weak";
    }
    for (int i = 0; i < dimensions.size(); ++i){
        selC[i] = C[dimensions[i]].data();
        selInd[i] = matIndices[dimensions[i]].data();
        label += to_string(dimensions[i]);
    }
    switch (type) {
        case Net:
            zeronetProperty(selC, fullSize, m, gf, subdets, env, vars, ks, selInd, c, obj, weak, weight,
                            weakvar, max_unbalance, label);
            break;
        case Stratified:
            stratifiedProperty(selC, fullSize, m, gf, subdets, env, vars, ks, selInd, c, obj, weak, weight, weakvar, label);
            break;
        case PropA:
            label += "A";
            if (m == dimensions.size())
                stratifiedProperty(selC, fullSize, m, gf, subdets, env, vars, ks, selInd, c, obj, weak, weight, weakvar, label);
            break;
        case PropAprime:
            label += "A'";
            if (m == 2 * dimensions.size())
                stratifiedProperty(selC, fullSize, m, gf, subdets, env, vars, ks, selInd, c, obj, weak, weight, weakvar, label);
            break;
    }
}

bool Constraint::check(const std::vector<vector<int>> &C, int fullSize, int m, const Galois::Field& gf) const {

    if (weak){
        return true;
    }
    if (m < start || m > end){
        return true;
    }
    vector<const int*> selC(dimensions.size());
    for (int i = 0; i < dimensions.size(); ++i){
        selC[i] = C[dimensions[i]].data();
    }
    switch (type) {
        case Net:
            return checkzeronet(selC, fullSize, m, max_unbalance, gf);
            break;
        case Stratified:
            return checkStratified(selC, fullSize, m, gf);
            break;
        case PropA:
            return m != dimensions.size() || checkStratified(selC, fullSize, m, gf);
            break;
        case PropAprime:
            return m != 2*dimensions.size() || checkStratified(selC, fullSize, m, gf);
            break;
    }
    return false;
}

string Constraint::tostring() const {
    string name;
    switch (type) {
        case Net:
            name += "(0,m,s)-net ";
            break;
        case Stratified:
            name += "Stratified ";
            break;
        case PropA:
            name += "PropA ";
            break;
        case PropAprime:
            name += "PropAprime ";
            break;
    }
    for (int v : dimensions){
        name += to_string(v);
    }
    return name;
}

void parseFile(istream& in, vector<Constraint>& constraints, int& s, int& m, int& b){
    constraints.resize(0);
    string mode;
    string type;
    string line;
    while (getline(in, line)) {
        istringstream sline(line);
        char letter;
        sline >> letter;
        if (letter == '#') {
            continue;
        }
        sline.putback(letter);
        string full_left_part;
        getline(sline, full_left_part, '=');
        if (!sline.eof()) {
            string left_part;
            istringstream left_ss(full_left_part);
            left_ss >> left_part;
            if (left_part == "s") {
                sline >> s;
            } else if (left_part == "m") {
                sline >> m;
            } else if (left_part == "b" || left_part == "p") {
                sline >> b;
            }
        } else {
            sline = istringstream(line);
            constraints.emplace_back();
            auto &cons = constraints.back();
            sline >> type;
            bool loop = true;
            do {
                if (type == "from") {
                    sline >> cons.start;
                    sline >> type;
                } else if (type == "to") {
                    sline >> cons.end;
                    sline >> type;
                } else if (type == "weak") {
                    cons.weak = true;
                    sline >> cons.weight;
                    sline >> type;
                } else {
                    loop = false;
                }
            } while (loop);
            if (type == "net") {
                cons.type = Constraint::Type::Net;
                char c;
                for (int i = 0; i < 2; ++i){
                    sline >> c;
                    if (c == 'u'){
                        sline >> cons.max_unbalance;
                    } else {
                        sline.putback(c);
                        break;
                    }
                }
            } else if (type == "stratified") {
                cons.type = Constraint::Type::Stratified;
            } else if (type == "propA") {
                cons.type = Constraint::Type::PropA;
            } else if (type == "propAprime") {
                cons.type = Constraint::Type::PropAprime;
            } else {
                cerr << "Error Parsing: Unknown constraint type " << type << endl;
                constraints.pop_back();
            }
            int d;

            while (sline >> d) {
                cons.dimensions.push_back(d);
            }
        }

    }
}