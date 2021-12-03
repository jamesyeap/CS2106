#!/bin/bash

gcc routine.c -o routine
gcc problematic_routine.c -o problematic_routine
gcc test.c -o test

./test