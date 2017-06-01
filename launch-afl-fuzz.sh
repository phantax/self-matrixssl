#!/bin/bash

if [[ "$1" != "" ]]; then
    DIR=$1
else
    DIR=output
fi

export AFL_SKIP_CPUFREQ=1
/home/walz/Code/afl-2.40b/afl-fuzz -d -i input/ -o $DIR ./self-matrixssl -s

