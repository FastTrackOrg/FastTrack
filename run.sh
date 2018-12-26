#!/bin/bash

if [ "$1" = "full" ]; then
  set -e
  qmake CONFIG+=released src/Fishy.pro
  make clean
  make
  cd build
  ./Fishy
  cd ..
  make clean
fi

if [ "$1" = "partial" ]; then
  set -e
  qmake CONFIG+=released src/Fishy.pro
  make
  cd build
  ./Fishy
  cd ..
fi

if [ "$1" = "debug" ]; then
  set -e
  qmake CONFIG+=debug src/Fishy.pro
  make
  cd build
  ./Fishy
  cd ..
fi


if [ "$1" = "profile" ]; then
  set -e
  qmake QMAKE_CXXFLAGS+=-pg QMAKE_LFLAGS+=-pg CONFIG+=debug src/Fishy.pro
  make
  cd build
  ./Fishy
  cd ..
  gprof build/Fishy build/gmon.out > profiling.txt
fi
