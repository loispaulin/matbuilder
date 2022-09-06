#!/bin/bash

## Expecting the cmake build folder to be `build`
BUILDPATH=build/
PATHTOSCRIPTS=scripts/

## Generating matrices
${BUILDPATH}/matbuilder -i profiles/generic_projLDS.txt -o output-matrices.dat -t 30

## Generating pts
${BUILDPATH}/sampler -i output-matrices.dat -s 6 -p 3 -m 10 -n 2048 -o samples.dat

## Generate fig
${PATHTOSCRIPTS}/projections_grid.sh samples.dat 6 -t pdf
