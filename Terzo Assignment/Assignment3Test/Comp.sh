#!/bin/bash
fileC="$1.c"
intermediateRappresentation="$1.ll";
optimizedIntermediateRappresentation="$1.optimized.ll";
binaryCode="$1.optimized.bc";
clang -O0 -S -emit-llvm -c $fileC -o $intermediateRappresentation;
vim $intermediateRappresentation;
../../BUILD/bin/opt -p "mem2reg" $intermediateRappresentation -o $binaryCode;
llvm-dis $binaryCode -o $optimizedIntermediateRappresentation;
../../BUILD/bin/opt -p "loopwalk" $optimizedIntermediateRappresentation -o $binaryCode;
if [ -e "$binaryCode" ]
then
    echo "Optimized files created!";
else
    echo "Error!";
fi
