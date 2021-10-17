#!/bin/bash

if [ "$1" = "full" ]; then
  set -e
  qmake6 CONFIG+=released src/FastTrack.pro
  make clean
  make
  cd build
  ./fasttrack
  cd ..
  make clean
fi

if [ "$1" = "no_web" ]; then
  set -e
  qmake6 CONFIG+="released NO_WEB" src/FastTrack.pro
  make clean
  make
  cd build
  ./fasttrack
  cd ..
  make clean
fi

if [ "$1" = "partial" ]; then
  set -e
  qmake6 CONFIG+=released src/FastTrack.pro
  make
  cd build
  ./fasttrack
  cd ..
fi

if [ "$1" = "debug" ]; then
  set -e
  qmake6 CONFIG+=debug src/FastTrack.pro
  make
  cd build
  ./fasttrack
  cd ..
fi


if [ "$1" = "profile" ]; then
  set -e
  qmake6 QMAKE_CXXFLAGS+=-pg QMAKE_LFLAGS+=-pg CONFIG+=debug src/FastTrack.pro
  make
  cd build
  ./fasttrack
  cd ..
  gprof build/FastTrack build/gmon.out > profiling.txt
fi

if [ "$1" = "ci" ]; then
  set -e
  qmake6 CONFIG+=released src/FastTrack.pro
  make clean
  make
  make clean
  cd build
fi

if [ "$1" = "cli" ]; then
  set -e
  qmake6 CONFIG+=released src/FastTrack-Cli.pro
  make clean
  make
  make clean
  cd build_cli
fi
