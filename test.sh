#!/bin/bash

set -e
cd Test
qmake Test.pro
make clean
make
cd build
./Test
cd ..
