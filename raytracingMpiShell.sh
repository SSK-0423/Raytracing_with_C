#!/bin/bash

for i in {1..1}
do
    mpic++ -O1 $1.cpp raytracing_lib.cpp mymath.cpp myPng.cpp log.cpp -lpng -o $1 && mpirun -np $2 ./$1
done