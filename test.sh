#!/bin/bash

set -e
trap 'rm -r ../dataSet/images/Groundtruth/Tracking_Result_Copy/' ERR

export QT_QPA_PLATFORM=offscreen

if [ "$1" = "unix" ]; then
  qmake6 FastTrack.pro CONFIG+="NO_WEB"
  make clean
  make check
  python -m pytest test/accuracyTest.py
fi
if [ "$1" = "win" ]; then
  qmake6 FastTrack.pro -spec win32-g++ CONFIG+="NO_WEB"
  mingw32-make clean
  mingw32-make
  mingw32-make check
  python -m pytest test/accuracyTest.py
fi
if [ "$1" = "" ]; then
  echo "Please specify platform (win or unix)"
fi

