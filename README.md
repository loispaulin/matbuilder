 # MatBuilder


 This folder contains both the source code of MatBuilder and profiles used in the paper (tested on linux and MacOS).


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


TODO

## License

The software uses Wojciech Jarosz's [Galois++](https://github.com/wkjarosz/galois) for computations on Galois fields.



```
MIT License

Copyright (c) 2022 CNRS

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
```
