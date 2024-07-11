#!/bin/bash
intermediateCode="$1.ll";
binaryCode="$1.optimized.bc";
optimizedCode="$1.optimized.ll";
rm $intermediateCode $binaryCode $optimizedCode;
echo "IR and Optimized files deleted!";