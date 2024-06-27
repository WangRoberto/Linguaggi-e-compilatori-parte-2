#!/bin/bash
codiceC="$1.c";
codiceIntermedio="$1.ll";
codiceBinary="$1.optimized.bc";
clang -O0 -S -emit-llvm -c $codiceC -o $codiceIntermedio;
vim $codiceIntermedio;
../../BUILD/bin/opt -p mem2reg $codiceIntermedio -o $codiceBinary;
llvm-dis $codiceBinary -o $codiceIntermedio;
#../../BUILD/bin/opt -p loopfusionpass $codiceIntermedio -o $codiceBinary;