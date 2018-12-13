#!/bin/bash

set -e
qmake QMAKE_CXXFLAGS+=-pg QMAKE_LFLAGS+=-pg CONFIG+=debug
make
cd build
./Fishy
cd ..
gprof build/Fishy build/gmon.out > profiling.txt
