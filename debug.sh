#!/bin/bash

set -e
qmake CONFIG+=debug Fishy.pro
make
cd build
./Fishy
cd ..
