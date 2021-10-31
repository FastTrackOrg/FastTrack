#!/bin/bash

set -e
trap 'rm -r ../dataSet/images/Groundtruth/Tracking_Result_Copy/' ERR

cd Test
qmake6 Test.pro
make clean
make
cd build
./Test
cd ../..

qmake6 src/FastTrack-Cli.pro
make clean
make
pytest Test/accuracyTest.py
