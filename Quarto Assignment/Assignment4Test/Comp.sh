#!/bin/bash
cCode="$1.c";
intermediateCode="$1.ll";
intermediateCodeOptimized="$1.optimized.ll";
binaryCode="$1.optimized.bc";
clang -O0 -S -emit-llvm -c $cCode -o $intermediateCode;
vim $intermediateCode;
../../BUILD/bin/opt -p mem2reg $intermediateCode -o $binaryCode;
llvm-dis $binaryCode -o $intermediateCode;
../../BUILD/bin/opt -p loopfusionpass $intermediateCode -o $binaryCode;
llvm-dis $binaryCode -o $intermediateCodeOptimized;
if [ -e "$intermediateCodeOptimized" ]
then
    echo "Optimized files created!";
else
    echo "Error!";
fi