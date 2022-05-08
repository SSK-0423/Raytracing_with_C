#!/bin/bash

read file

echo "clang++ "$file" raytracing_lib.cpp mymath.cpp myPng.cpp log.cpp -lpng"

echo "./a.out"

exit 0