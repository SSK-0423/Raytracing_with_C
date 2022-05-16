#!/bin/bash

mpic++ $1.cpp raytracing_lib.cpp mymath.cpp myPng.cpp log.cpp -lpng -o $1 && mpirun -np $2 ./$1