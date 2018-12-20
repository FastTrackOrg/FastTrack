#!/bin/bash

set -e
qmake CONFIG+=released Fishy.pro
make clean
make
cd build
./Fishy
cd ..
make clean
