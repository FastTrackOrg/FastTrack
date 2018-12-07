#!/bin/bash

set -e
qmake CONFIG+=released Fishy.pro
make
cd build
./Fishy
cd ..
