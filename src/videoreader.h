/*
This file is part of Fast Track.

    FastTrack is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FastTrack is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FastTrack.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef VIDEOREADER_H
#define VIDEOREADER_H

#include <filesystem>
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <regex>
#include <set>
#include <string>

using namespace cv;
using namespace std;
namespace fs = std::filesystem;

class VideoReader : public VideoCapture {
  bool m_isSequence;

 public:
  VideoReader(const string &path);
  void getImage(int index, UMat &destination);
  void getImage(int index, Mat &destination);
  unsigned int getImageCount();
  bool isSequence();
};

#endif
