#!/bin/bash

echo "--------------------"
echo "  逐次処理計測開始  "
echo "--------------------"
rm -f "64seq.txt" "128seq.txt" "256seq.txt" "512seq.txt" "1024seq.txt"
for i in {1..5}
do
    if [ ${i} -eq 1 ]; then
        size=64
    fi
    if [ ${i} -eq 2 ]; then
        size=128
    fi
    if [ ${i} -eq 3 ]; then
        size=256
    fi
    if [ ${i} -eq 4 ]; then
        size=512
    fi
    if [ ${i} -eq 5 ]; then
        size=1024
    fi

    for j in {1..10}
    do
        mpic++ -O1 $1.cpp raytracing_lib.cpp mymath.cpp myPng.cpp log.cpp -lpng -o $1 && mpirun -np 1 ./$1 ${size} "seq.txt" ${j}
    done
    echo ${size}"x"${size} "計測完了"
done

echo ""
echo "--------------------"
echo "  並列処理計測開始  "
echo "--------------------"
rm -f "64para.txt" "128para.txt" "256para.txt" "512para.txt" "1024para.txt"
for i in {1..5}
do
    if [ ${i} -eq 1 ]; then
        size=64
    fi
    if [ ${i} -eq 2 ]; then
        size=128
    fi
    if [ ${i} -eq 3 ]; then
        size=256
    fi
    if [ ${i} -eq 4 ]; then
        size=512
    fi
    if [ ${i} -eq 5 ]; then
        size=1024
    fi

    for j in {1..10}
    do
        mpic++ -O1 $1.cpp raytracing_lib.cpp mymath.cpp myPng.cpp log.cpp -lpng -o $1 && mpirun -np $2 ./$1 ${size} "para.txt" ${j}
    done
    echo ${size}"x"${size} "計測完了"
done
eog result.png