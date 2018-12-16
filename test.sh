#!/bin/bash

set -e
cd Test
qmake Test.pro
make
cd build
./Test
cd ..
