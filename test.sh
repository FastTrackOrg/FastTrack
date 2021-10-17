#!/bin/bash

set -e
trap 'rm -r ../dataSet/images/Groundtruth/Tracking_Result_Copy/' ERR

cd Test
cp -r dataSet/images/Groundtruth/Tracking_Result/ dataSet/images/Groundtruth/Tracking_Result_Copy/
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

rm -r Test/dataSet/images/Groundtruth/Tracking_Result_Copy/
