#!/bin/bash

make distclean -s
qmake6 CONFIG+="debug NO_WEB" QMAKE_CXXFLAGS+=-w -spec linux-clang QMAKE_CXX="clazy" FastTrack.pro
export CLAZY_CHECKS="level2"
export CLAZY_EXPORT_FIXES=ON
export CLAZY_HEADER_FILTER="./src/" 
export CLAZY_IGNORE_DIRS="./test/*"

if [ "$1" = "fail" ]; then
  VAR=$(make -s 2>&1 >/dev/null | grep "warning")
  if [ -z "$VAR" ]
  then
        exit 0
  else
        exit 1
  fi
else
  make -s
fi
