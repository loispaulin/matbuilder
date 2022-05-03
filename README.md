 # MatBuilder

Source code of the paper [MatBuilder: Mastering Sampling Uniformity Over Projections](https://perso.liris.cnrs.fr/lpaulin/Publications/paulin2022.html), Loïs Paulin, Nicolas Bonneel, David Coeurjolly, Jean-Claude Iehl, Alexander Keller, Victor Ostromoukhov, ACM Trans. on Graphics, SIGGRAPH 2022.

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

 This folder contains both the source code of MatBuilder and profiles used in the paper (tested on linux and MacOS).
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


TODO



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
