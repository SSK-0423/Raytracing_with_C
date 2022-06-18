#!/bin/bash

rm -f "log.txt"
for i in {1..10}
do
    mpic++ -O1 $1.cpp raytracing_lib.cpp mymath.cpp myPng.cpp log.cpp -lpng -o $1 && mpirun -np $3 ./$1 $2
done

eog result.png