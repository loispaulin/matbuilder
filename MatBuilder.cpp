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
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <galois++/field.h>
#include "CLI11.hpp"
#include "cplexMatrices.h"
#include "MatrixTools.h"

using namespace std;

class Constraint{
public:
    enum Type {Stratified, Net, PropA, PropAprime};
    Type type;
    std::vector<int> dimensions;
    bool weak = false;
    double weight;
    int start = -1;
    int end = std::numeric_limits<int>::max();
    int max_unbalance = std::numeric_limits<int>::max();

    void add(const std::vector<vector<int>>& C, int fullSize, int m, const Galois::Field& gf, std::vector<int> &subdets,
             IloEnv& env, IloNumVarArray& vars, IloNumVarArray &ks, const std::vector<vector<int>>& matIndices,
             IloConstraintArray& c, IloNumExpr& obj, IloNumVarArray& weakvar) const;
    bool check(const std::vector<vector<int>>& C, int fullSize, int m, const Galois::Field& gf) const;
    string tostring() const;
};

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
        string left_part;
        getline(sline, left_part, '=');
        if (!sline.eof()) {
            if (left_part[0] == 's') {
                sline >> s;
            } else if (left_part[0] == 'm') {
                sline >> m;
            } else if (left_part[0] == 'b' || left_part[0] == 'p') {
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
                sline >> c;
                if (c == 'u'){
                    sline >> cons.max_unbalance;
                } else {
                    sline.putback(c);
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

int main (int argc, const char** argv)
{
    CLI::App app{"Cplex Matrices"};

    string filename;
    app.add_option("-i", filename, "Input file name")->required();
    string outfile;
    app.add_option("-o", outfile, "Output file name");
    int fullSize = -1;
    app.add_option("-m", fullSize, "Override file matrix size");
    int nbTrials = 50;
    app.add_option("-n,--nbTrials", nbTrials, "Number of tentative (def:50)");
    int nbBacktrack = 15;
    app.add_option("--nbBacktrack", nbBacktrack, "Number of backtracks before starting again (def:15)");
    int s = -1;
    app.add_option("-s", s, "Override file number of dimensions");
    bool check = false;
    app.add_flag("--check", check, "Check properties (unit test)");
    int b = -1;
    app.add_option("-b", b, "Override matrices basis");
    int nbThreads = 0;
    app.add_option("--threads", nbThreads, "Number of threads to use (def: all avalaible)");
    double tolerance_ratio = 0.01;
    app.add_option("--tolerance", tolerance_ratio, "Error tolerance on objective value (def: 0.01)");
    double timeout = pow(10.,10.);
    app.add_option("-t, --timeout", timeout, "Maximum time for each cplex solve (def: 10^10 s)");
    int seed = 133742;
    app.add_option("--seed", seed, "Program seed");
    bool no_seed = false;
    app.add_flag("--no-seed", no_seed, "Disables seed objective in optimizer");
    bool dbg_flag = false;
    app.add_flag("--debug", dbg_flag, "Toggles debug outputs");
    bool header = false;
    app.add_flag("--header", header, "Writes profile as comments at the beginning of matrix file");

    CLI11_PARSE(app, argc, argv);

    int tmpFullSize;
    int tmpBasis;
    int tmpS;
    bool sequence;
    ifstream in(filename);
    if (in.fail()){
        cerr << "Error: Could not open input file" << endl;
        return -1;
    }
    vector<Constraint> constraints;
    parseFile(in, constraints, tmpS, tmpFullSize, tmpBasis);
    if (fullSize == -1) fullSize = tmpFullSize;
    if (s == -1) s = tmpS;
    if (b == -1) b = tmpBasis;
    Galois::Field gf(b);

    mt19937_64 gen(seed);

    vector<vector<int> > C(s, vector<int>(fullSize*fullSize));
    bool failed = true;
    int countFail = 0;

    auto clock_start = std::chrono::steady_clock::now();

    do {
        int greedyFail = 0;
        failed = false;
        bool goingBack;
        int lastM = -1;
        for (int m = 1; m <= fullSize; ++m) {
            goingBack = false;
            try {
                cerr << "m = " << m << endl;
                vector<vector<int> > matIndices(s, vector<int>(m));
                vector<int> subdets(m);

                IloEnv env;
                IloModel model(env);
                IloNumVarArray vars(env);
                IloNumVarArray ks(env);
                IloConstraintArray c(env);
                IloNumExpr weakObj(env);
                IloNumVarArray weakVars(env);

                initVar(env, s, m, b, vars, matIndices);

                if (dbg_flag) {
                    env.out() << endl << "m = " << m << endl;
                    env.out() << vars << endl;
                }

                weakObj += 0;
                for (const auto &cons: constraints) {
                    cons.add(C, fullSize, m, b, subdets, env, vars, ks, matIndices, c, weakObj, weakVars);
                }

                IloNumExpr randomObj = randomObjective(gen, vars, env, b);
                if (no_seed){
                    model.add(IloMinimize(env, 1000 * weakObj + 0));
                } else {
                    model.add(IloMinimize(env, 1000 * weakObj + randomObj + 0));
                }

                if (dbg_flag) {
                    for (int i = 0; i < c.getSize(); ++i) {
                        env.out() << c[i] << endl;
                    }
                }

                model.add(c);
                IloCplex cplex(model);
                if (!dbg_flag)
                    cplex.setParam(IloCplex::Param::MIP::Display, 0);
                cplex.setParam(IloCplex::Param::ParamDisplay, 0);
                cplex.setParam(IloCplex::Param::Threads, nbThreads);
                cplex.setParam(IloCplex::Param::MIP::Tolerances::MIPGap, tolerance_ratio);
                cplex.setParam(IloCplex::TiLim, timeout);

                // Optimize the problem and obtain solution.
                if (!cplex.solve()) {
                    throw "Failed to solve LP for m = " + to_string(m);
                }

                IloNumArray vals(env);
                cplex.getValues(vals, vars);
                if (dbg_flag) {
                    env.out() << "Objective: " << cplex.getObjective();
                    env.out() << "Solution status = " << cplex.getStatus() << endl;
                    env.out() << "Solution value  = " << cplex.getObjValue() << endl;
                    env.out() << "Variables\t\t= " << vars << endl;
                    env.out() << "Values\t\t\t= " << vals << endl;
                    env.out() << "Weak constraints = " << endl;
                    int total = 0;
                    for (int i = 0; i < weakVars.getSize(); ++i) {
                        int val = IloRound(cplex.getValue(weakVars[i]));
                        env.out() << "\t" << weakVars[i] << " = " << val << endl;
                        total += val;
                    }
                    env.out() << "Total weak constraints = " << total << " / " << weakVars.getSize() << endl;
                }

                for (int i = 0; i < s; ++i) {
                    for (int j = 0; j < m; ++j) {
                        C[i][index(j, m - 1, fullSize)] = IloRound(vals[matIndices[i][j]]);
                    }
                }

                env.end();
            } catch (string &error) {
                cerr << error << endl;
                if (m != lastM){
                    greedyFail = 0;
                }
                lastM = m;
                m -= 2;
                greedyFail += 1;
                goingBack = true;
                if (greedyFail > nbBacktrack) {
                    failed = true;
                    countFail += 1;
                    cerr << "Failed too many times. Restarting research" << endl;
                    break;
                } else {
                    cerr << "Backtracking (" << greedyFail << "/" << nbBacktrack << ")" << endl;
                }
            }
          auto end = std::chrono::steady_clock::now();
          auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - clock_start);
          std::cout << "===done===> m " << m<<" "<<elapsed.count() << " milliseconds." << std::endl;
        }
    } while (countFail < nbTrials && failed);


    if (check) {
        bool total = true;
        for (int m = 1; m <= fullSize; ++m) {
            for (const auto &cons: constraints) {
                bool test = cons.check(C, fullSize, m, b);
                if (!test){
                    cerr << "Failed check:m=" << m << " : " << cons.tostring() << endl;
                }
                total = total && test;
            }
        }
        if (total) {
            cout << "All properties are verified" << endl;
        }
    }

    if (outfile==""){
        writeMatrices(cout, fullSize, C, true);
    } else{
        ofstream out(outfile);
        if (header){
            ifstream file(filename);
            string line;
            while (getline(file, line)){
                out << "# " << line << endl;
            }
        }
        writeMatrices(out, fullSize, C, true);
        out.close();
    }

    return 0;
}  // END main
