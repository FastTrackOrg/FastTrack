#!/bin/bash

set -e
cd Test
qmake Test.pro
make clean
make
cd build
./Test
cd ../..

qmake src/FastTrack-Cli.pro
make clean
make
pytest Test/accuracyTest.py
