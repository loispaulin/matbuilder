 # MatBuilder

 Demo code for "MatBuilder: Mastering Sampling Uniformity Over Projections" (submission id: 168)

 This folder contains both the source code of MatBuilder and profiles used in the paper (tested on linux and MacOS).

 To build the code, you would need an install of the CPLEX Optimization Studio (free for academics,). Once CPLEX as been installed:

```
mkdir build
cd build
cmake ..
make
```

Then you can run the solver using:

```
./MatBuilder -i profiles/generic_net.txt -o output-matrices.dat
```

(`-h` to get the list of options).
