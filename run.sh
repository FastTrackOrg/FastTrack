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
  mkdir -p build
  cd build
  cmake -DCMAKE_BUILD_TYPE=Release ..
  make
  bin/fasttrack
fi

if [ "$1" = "debug" ]; then
  set -e
  mkdir -p build
  cd build
  cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-Wall -Wextra -g -Wconversion" ..
  make
  make check
  bin/fasttrack
fi

if [ "$1" = "profile" ]; then
  set -e
  mkdir -p build
  cd build
  cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-pg" -DCMAKE_EXE_LINKER_FLAGS="-pg" ..
  make
  make check
  bin/fasttrack
  gprof bin/fasttrack bin/gmon.out > profiling.txt
fi

if [ "$1" = "ci" ]; then
  set -e
  mkdir -p build
  cd build
  cmake -DCMAKE_BUILD_TYPE=Release -DSKIP_TEST=ON ../
  make
fi

if [ "$1" = "ci_clang" ]; then
  make distclean
  set -e
  mkdir -p build
  cd build
  cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release -DSKIP_TEST=ON ../
  make
fi

if [ "$1" = "build_linux" ]; then
    act -W .github/workflows/build_linux.yml --rm --pull --artifact-server-path "$PWD/.artifacts"
fi

if [ "$1" = "build_win" ]; then
    act -W .github/workflows/build_win.yml --rm --pull --artifact-server-path "$PWD/.artifacts"
fi

if [ "$1" = "build_mac" ]; then
    act -W .github/workflows/build_mac.yml --rm --pull --artifact-server-path "$PWD/.artifacts"
fi
