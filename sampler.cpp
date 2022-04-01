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
#include <math.h>
#include <vector>
#include <cstring>

#include <iostream>
#include <fstream>
#include <iomanip>
#include "MatrixTools.h"
#include "MatrixSamplerClass.h"
#include "Scrambling.h"
#include "CLI11.hpp"

using namespace std;
int main(int argc, char** argv)
{
  //Call parameters handling
  CLI::App app{"matBuiler sampler"};

  int nDims = 6;
  app.add_option("-s,--nDims", nDims, "number of dimensions to generate, default: " + std::to_string(nDims));
  int npts = 9*9*9*9;
  app.add_option("-n,--npts", npts, "number of points to generate, default: " + std::to_string(npts));
  int seed = 133742;
  app.add_option("--seed", seed, "Random number generator seed (for the scrambling). default: " + std::to_string(seed));
  string input_matrices;
  app.add_option("-i,--idv", input_matrices, "input matrices initialisation (ascii file), default: " +input_matrices)->required();
  int m = 8;
  app.add_option("-m,--matrixSize", m,"input matrix size, default: " + std::to_string(m));
  int depth = -1;
  app.add_option("--depth", depth,"scrambling depth (equals matrix size by default)");
  int base = 3;
  app.add_option("-p,--base", base, "Matrix base, default: " + std::to_string(base));
  bool owen_permut_flag = false;
  app.add_flag("--owen", owen_permut_flag,"apply Owen permutation on output points, default: " + std::to_string(owen_permut_flag));
  int nbReal = 1;
  app.add_option("--nbReal", nbReal, "number of realizations of the sampler (for the scrambling), default: " + std::to_string(nbReal));
  std::string output_fname = "out.dat";
  app.add_option("-o,--output", output_fname,"output samples filename, default: " + output_fname);
  bool dbg_flag = false;
  app.add_option("--dbg", dbg_flag, "dbg_flag, default: " + std::to_string(dbg_flag));
  CLI11_PARSE(app, argc, argv)

    if (depth == -1)
      depth = m;

  ifstream in(input_matrices);
  if (in.fail()) {
    cerr << "Error: Could not open input file: " << input_matrices << endl;
    return -1;
  }

  ofstream out(output_fname);
  if (out.fail()) {
    cerr << "Error: Could not open output file: " << output_fname << endl;
    return -1;
  }
  out << setprecision(16);

  std::vector<std::vector<int> > Bs(nDims, std::vector<int>(m*m));
  std::vector<std::vector<int> > Cs(nDims, std::vector<int>(m*m));

  readMatrices(in, m, nDims, Cs);

  if(dbg_flag) {
    writeMatrices(std::cout,m,Cs,true);
  }

  minstd_rand gen(seed);
  uniform_int_distribution<int> unif;
  for (int real = 0; real < nbReal; ++real) {
    int real_seed = unif(gen);
    for (int indpt = 0; indpt < npts; ++indpt) {
      for (int inddim = 0; inddim < nDims; ++inddim) {
        double pos;
        if (owen_permut_flag){
          pos = getScrambledDouble(Cs[inddim], m, base, indpt, real_seed + inddim, depth);
        } else {
          pos = getDouble(Cs[inddim], m, base, indpt);
        }
        out << pos << " ";
        if(dbg_flag) cout << " " << pos << " | ";
      }
      out << endl;
      if(dbg_flag) cout << endl;
    }
    if (real != nbReal-1) out << "#" << endl;
  }
  out.close();

}
