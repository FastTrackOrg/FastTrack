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

#include "autolevel.h"

/**
 * @class Autoleveli
 *
 * @brief This class is entended to level the soft tracking parameters.
 *
 * @author Benjamin Gallois
 *
 * @version $Revision: 5.0 $
 *
 * Contact: gallois.benjamin08@gmail.com
 *
 */

/**
  * @brief Constructs the AutoLevel object.
  * @param[in] path Path to the movie to track.
  * @param[in] background Background image.
*/
AutoLevel::AutoLevel(string path, UMat background) {
}

AutoLevel::~AutoLevel() {
}

/**
  * @brief Levels the tracking parameters.
  * @return Map containing the levelled parameters.
*/
QMap<QString, double> AutoLevel::level() {
}

/**
  * @brief Compute the standard deviation of the angle distribution.
  * @param[in] data Tracking data.
  * @return Standard deviation.
*/
double AutoLevel::computeStdAngle(const Data &data) {
}

/**
  * @brief Compute the standard deviation of the distance distribution.
  * @param[in] data Tracking data.
  * @return Standard deviation.
*/
double AutoLevel::computeStdDistance(const Data &data) {
}

/**
  * @brief Compute the standard deviation of the area distribution.
  * @param[in] data Tracking data.
  * @return Standard deviation.
*/
double AutoLevel::computeStdArea(const Data &data) {
}

/**
  * @brief Compute the standard deviation of the angle distribution.
  * @param[in] data Tracking data.
  * @return Standard deviation.
*/
double AutoLevel::computeStdPerimeter(const Data &data) {
}
