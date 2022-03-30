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
