#!/bin/bash

set -e
trap 'rm -r ../dataSet/images/Groundtruth/Tracking_Result_Copy/' ERR

export QT_QPA_PLATFORM=offscreen

if [ "$1" = "unix" ]; then
  mkdir -p build
  cd build
  cmake -DCMAKE_BUILD_TYPE=Release ../
  make clean
  make
  make check
  cd ..
  python -m pytest test/accuracyTest.py
fi
if [ "$1" = "win" ]; then
  mkdir -p build
  cd build
  cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ../
  mingw32-make
  mingw32-make check
  cd ..
  python -m pytest test/accuracyTest.py
fi
if [ "$1" = "" ]; then
  echo "Please specify platform (win or unix)"
fi
