#!/bin/bash

mpicc -o go MPICustomScatterFunc.c -O3 -lmpi -lm

if [[ $? == "0" ]]; then
        mpirun -np 16 go
fi