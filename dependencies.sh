#!/bin/bash

if [ "$1" = "opencv" ]; then
  set -e
  git clone https://github.com/opencv/opencv
  cd opencv
  mkdir build
  cd build
  cmake -D WITH_FFMPEG=ON -D WITH_V4L=ON -D WITH_TBB=ON -D WITH_OPENMP=ON -D WITH_IPP=ON -D CMAKE_BUILD_TYPE=RELEASE -D BUILD_EXAMPLES=OFF -D WITH_NVCUVID=ON -D WITH_CUDA=ON -D BUILD_DOCS=OFF -D BUILD_PERF_TESTS=OFF -D BUILD_TESTS=OFF -D WITH_CSTRIPES=ON -D WITH_OPENCL=OFF -D BUILD_opencv_gapi=OFF -D CMAKE_INSTALL_PREFIX=/usr/local/ ..
  make -j8
  sudo make install
  cd ../..
  rm -rf opencv
  sudo ldconfig
fi

if [ "$1" = "opencvmac" ]; then
  set -e
  git clone https://github.com/opencv/opencv
  cd opencv
  mkdir build
  cd build
  cmake -D WITH_FFMPEG=ON -D WITH_V4L=ON -D WITH_TBB=ON -D WITH_OPENMP=ON -D WITH_IPP=ON -D CMAKE_BUILD_TYPE=RELEASE -D BUILD_EXAMPLES=OFF -D WITH_NVCUVID=ON -D WITH_CUDA=ON -D BUILD_DOCS=OFF -D BUILD_PERF_TESTS=OFF -D BUILD_TESTS=OFF -D WITH_CSTRIPES=ON -D WITH_OPENCL=OFF -D BUILD_opencv_gapi=OFF -D CMAKE_INSTALL_PREFIX=/usr/local/ ..
  make -j8
  sudo make install
  cd ../..
  rm -rf opencv
fi
