 # MatBuilder: Mastering Sampling Uniformity Over Projections

Source code of the paper [MatBuilder: Mastering Sampling Uniformity Over Projections](https://projet.liris.cnrs.fr/matbuilder), Loïs Paulin, Nicolas Bonneel, David Coeurjolly, Jean-Claude Iehl, Alexander Keller, Victor Ostromoukhov, ACM Trans. on Graphics, SIGGRAPH 2022.

``` bibtex
@article{paulin2022,
      author = {Loïs Paulin and Nicolas Bonneel and David Coeurjolly and Jean-Claude Iehl and Alexander Keller and Victor Ostromoukhov},
      journal = {ACM Transactions on Graphics (Proceedings of SIGGRAPH)},
      month = {August},
      number = {xxx},
      title = {MatBuilder: Mastering Sampling Uniformity Over Projections},
      volume = {yyy},
      year = {2022}
}
```

 This project contains both the source code of MatBuilder and profiles used in the paper (tested on linux and MacOS).
 The software uses Wojciech Jarosz's [Galois++](https://github.com/wkjarosz/galois) library for computations on Galois fields.


## Building MatBuilder

 To build the code, you would need an install of the CPLEX Optimization Studio (free for academics,). Once CPLEX as been installed,
 you first need to verify the paths to the CPLEX headers and libraries (cf [CMakeLists.txt l25-35](https://github.com/loispaulin/matbuilder/blob/6b8474f16bfc26d2c82fcaf6bf55e544db6706e1/CMakeLists.txt#L25-L35)).
 Then, you can build the project, e.g.:

```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

Then you can run the solver using:

```
./MatBuilder -i profiles/generic_net.txt -o output-matrices.dat
```

(`-h` to get the list of options).


## Generating samples from the matrices


One the matrices have geen generated, samples can be retreive using the `sampler`tool: 

```
matBuiler sampler
Usage: ./sampler [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -s,--nDims INT              number of dimensions to generate, default: 6
  -n,--npts INT               number of points to generate, default: 6561
  --seed INT                  Random number generator seed (for the scrambling). default: 133742
  -i,--idv TEXT REQUIRED      input matrices initialisation (ascii file), default:
  -m,--matrixSize INT         input matrix size, default: 8
  --depth INT                 scrambling depth (equals matrix size by default)
  -p,--base INT               Matrix base, default: 3
  --owen                      apply Owen permutation on output points, default: 0
  --nbReal INT                number of realizations of the sampler (for the scrambling), default: 1
  -o,--output TEXT            output samples filename, default: out.dat
  --dbg UINT                  dbg_flag, default: 0```
```

For instance, to generate the first 16 samples from 8x8 (`-m 8`) matrices in base 3 (`-p 3`) and dimension 6 (`-s 6`):

```
 ./sampler -i output-matrices.dat -o samples.pts  -n 16 -p 3 -s 6 -m 8
```

Which returns for the generic profile:

``` 
0 0 0 0 0 0
0.3333333333333333 0.6666666666666666 0.6666666666666666 0.3333333333333333 0.3333333333333333 0.3333333333333333
0.6666666666666666 0.3333333333333333 0.3333333333333333 0.6666666666666666 0.6666666666666666 0.6666666666666666
0.5555555555555556 0.2222222222222222 0.5555555555555556 0.1111111111111111 0.4444444444444444 0.7777777777777778
0.8888888888888888 0.8888888888888888 0.2222222222222222 0.4444444444444444 0.7777777777777778 0.1111111111111111
0.2222222222222222 0.5555555555555556 0.8888888888888888 0.7777777777777778 0.1111111111111111 0.4444444444444444
0.7777777777777778 0.1111111111111111 0.7777777777777778 0.2222222222222222 0.8888888888888888 0.5555555555555556
0.1111111111111111 0.7777777777777778 0.4444444444444444 0.5555555555555556 0.2222222222222222 0.8888888888888888
0.4444444444444444 0.4444444444444444 0.1111111111111111 0.8888888888888888 0.5555555555555556 0.2222222222222222
0.8518518518518519 0.8518518518518519 0.03703703703703703 0.8148148148148148 0.4814814814814815 0.03703703703703703
0.1851851851851852 0.5185185185185185 0.7037037037037037 0.1481481481481481 0.8148148148148148 0.3703703703703703
0.5185185185185185 0.1851851851851852 0.3703703703703703 0.4814814814814815 0.1481481481481481 0.7037037037037037
0.07407407407407407 0.7407407407407407 0.5925925925925926 0.9259259259259259 0.9259259259259259 0.8148148148148148
0.4074074074074074 0.4074074074074074 0.2592592592592592 0.2592592592592592 0.2592592592592592 0.1481481481481481
0.7407407407407407 0.07407407407407407 0.9259259259259259 0.5925925925925926 0.5925925925925926 0.4814814814814815
0.6296296296296297 0.9629629629629629 0.8148148148148148 0.7037037037037037 0.03703703703703703 0.5925925925925926
```

## License


```
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
```
