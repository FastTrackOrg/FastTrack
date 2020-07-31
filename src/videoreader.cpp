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

#include "videoreader.h"

/**
 * @class VideoReader
 *
 * @brief This class is intended to abstract the opening of a video, it can load image sequence and video with the same public API
 *
 * @author Benjamin Gallois
 *
 * @version $Revision: 5.0 $
 *
 * Contact: benjamin.gallois@fasttrack.sh
 *
 */

/**
  * @brief Construct the VideoReader object from a path to a file that can be either an image from  an image sequence or a movie file.
  * @param[in] path Path to a video or image file.
*/
VideoReader::VideoReader(const string &path) {
  string normPath;
  std::set<string> imageExtensions{".pgm", ".png", ".jpeg", ".jpg", ".tiff", ".tif", ".bmp", ".dib", ".jpe", ".jp2", ".webp", ".pbm", ".ppm", ".sr", ".ras", ".tif"};
  if (imageExtensions.count(filesystem::path(path).extension()) > 0) {
    m_isSequence = true;
    string name = filesystem::path(path).filename();
    regex e("\\D\\d+\\D");
    smatch match;
    if (std::regex_search(name, match, e)) {
      string pattern = string("%0") + to_string(match.length(0) - 2) + "d";
      name.replace(match.position(0) + 1, match.length(0) - 2, pattern);
      normPath = (filesystem::path(path).parent_path() / name).string();
    }
  }
  else {
    m_isSequence = false;
    normPath = path;
  }
  open(normPath);
}

/**
  * @brief Get the image at selected index, always one channel.
  * @param[in] index Index of the image.
	* @param[in] destination UMat to store the image.
*/
void VideoReader::getImage(int index, UMat &destination) {
  set(CAP_PROP_POS_FRAMES, index);
  read(destination);
  if (destination.channels() >= 3) {
    cvtColor(destination, destination, COLOR_BGR2GRAY);
  }
}

/**
  * @brief Get the image at selected index, always one channel.
  * @param[in] index Index of the image.
	* @param[in] destination Mat to store the image.
*/
void VideoReader::getImage(int index, Mat &destination) {
  set(CAP_PROP_POS_FRAMES, index);
  read(destination);
  if (destination.channels() >= 3) {
    cvtColor(destination, destination, COLOR_BGR2GRAY);
  }
}

/**
  * @brief Get the total number of images in the video.
  * @return total number of images.
*/
unsigned int VideoReader::getImageCount() {
  return get(CAP_PROP_FRAME_COUNT);
}

/**
  * @brief Is the file is an image sequence.
  * @return True if an image sequence, false otherwise.
*/
bool VideoReader::isSequence() {
  return m_isSequence;
}
