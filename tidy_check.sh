#!/bin/bash

# This script can't be run on the outer FastTrack.pro because it triggers header include errors with clang-tidy
make distclean -s
cd src/
qmake6 CONFIG+="debug NO_WEB" QMAKE_CXXFLAGS+=-w -spec linux-clang FastTrack.pro
bear -- make -s
find . -wholename "./*cpp" -not -path "./build*/*" -not -path "./Hungarian*" | xargs clang-tidy -header-filter=./*.h -p ./
