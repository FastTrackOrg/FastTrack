#!/bin/bash

if [ "$1" = "clean" ]; then
  make distclean
  echo "cleaning"
fi

if [ "$1" = "" ]; then
  echo "release, debug, profile, ci, cli"
fi

if [ "$1" = "release" ]; then
  make distclean
  set -e
  qmake6 CONFIG+=release FastTrack.pro
  make
  make check
  make clean
  src/build/fasttrack
fi

if [ "$1" = "no_web" ]; then
  make distclean
  set -e
  qmake6 CONFIG+="release NO_WEB" FastTrack.pro
  make
  make check
  make clean
  src/build/fasttrack
fi

if [ "$1" = "debug" ]; then
  set -e
  qmake6 CONFIG+=debug FastTrack.pro
  make
  make check
  src/build/fasttrack
fi

if [ "$1" = "profile" ]; then
  set -e
  qmake6 QMAKE_CXXFLAGS+=-pg QMAKE_LFLAGS+=-pg CONFIG+=debug FastTrack.pro
  make
  make check
  src/build/fasttrack
  gprof src/build/fasttrack src/build/gmon.out > profiling.txt
fi

if [ "$1" = "ci" ]; then
  make distclean
  set -e
  qmake6 CONFIG+="release NO_WEB" src/FastTrack.pro
  make
  make clean
  cd build
fi

if [ "$1" = "cli" ]; then
  make distclean
  set -e
  qmake6 CONFIG+=release src/FastTrack-Cli.pro
  make
  make check
  make clean
  cd build_cli
fi
