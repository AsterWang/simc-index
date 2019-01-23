#!/bin/sh
rm R.*
make clean
make
./create R 5000 4 1000
./gendata 100 4 | ./insert R
#./gendata 97 4 | ./insert R
#./gendata 97 4 | ./insert R
./stats R
./dump R
