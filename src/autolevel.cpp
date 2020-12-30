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

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * @class Autolevel
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
AutoLevel::AutoLevel(const string &path, const UMat &background, const QMap<QString, QString> &parameters) {
  m_path = path;
  m_background = background;
  m_parameters = parameters;
  m_endImage = 200;
}

AutoLevel::~AutoLevel() {
}

/**
  * @brief Levels the tracking parameters.
  * @return Map containing the levelled parameters.
*/
QMap<QString, double> AutoLevel::level() {
  try {
    VideoReader video(m_path);
    QFileInfo savingInfo(QString::fromStdString(m_path));
    QString savingFilename = savingInfo.baseName();
    QString directory = savingInfo.absolutePath();
    if (video.isSequence()) {
      directory.append(QString("/Tracking_Result") + QDir::separator());
    }
    else {
      directory.append(QString("/Tracking_Result_") + savingFilename + QDir::separator());
    }
    srand(static_cast<unsigned int>(time(NULL)));
    double stdAngle = static_cast<double>(rand() % 360 + 1);
    double stdDist = static_cast<double>(rand() % 500 + 1);
    double stdArea = static_cast<double>(rand() % 500 + 1);
    double stdPerimeter = static_cast<double>(rand() % 500 + 1);
    int spot = m_parameters.value("spot").toInt();
    if (spot == 0) {
      m_spotSuffix = "Head";
    }
    else if (spot == 1) {
      m_spotSuffix = "Tail";
    }
    else if (spot == 2) {
      m_spotSuffix = "Body";
    }
    int counter = 0;
    while (abs(stdAngle - m_parameters.value("normAngle").toDouble()) > 1E-3 && abs(stdDist - m_parameters.value("normDist").toDouble()) > 1E-3 && abs(stdArea - m_parameters.value("normArea").toDouble()) > 1E-3 && abs(stdPerimeter - m_parameters.value("normParam").toDouble()) > 1E-3) {
      m_parameters.insert("normAngle", QString::number(stdAngle));
      m_parameters.insert("normDist", QString::number(stdDist));
      m_parameters.insert("normArea", QString::number(stdArea));
      m_parameters.insert("normPerim", QString::number(stdPerimeter));
      Tracking tracking = Tracking(m_path, m_background, 0, m_endImage);
      tracking.updatingParameters(m_parameters);
      tracking.startProcess();
      Data data = Data(directory);
      stdAngle = 180 * computeStdAngle(data) / M_PI;
      stdDist = computeStdDistance(data);
      stdArea = computeStdArea(data);
      stdPerimeter = computeStdPerimeter(data);
      QDir(directory).removeRecursively();
      counter++;
      if (counter > 100) {
        throw 0;
      }
    }
    QMap<QString, double> levelParameters;
    levelParameters.insert("normAngle", stdAngle);
    levelParameters.insert("normDist", stdDist);
    levelParameters.insert("normArea", stdArea);
    levelParameters.insert("normPerim", stdPerimeter);
    levelParameters.insert("nIterations", double(counter));
    emit(levelParametersChanged(levelParameters));
    emit(finished());
    return levelParameters;
  }
  catch (...) {
    emit(forceFinished("Autoleveling not converging"));
    QMap<QString, double> levelParameters;
    return levelParameters;
  }
}

/**
  * @brief Compute the std from a vector.
  * @param[in] data Distribution.
  * @return Std.
*/
double AutoLevel::stdev(const QVector<double> &vect) {
  double mean = std::accumulate(vect.begin(), vect.end(), 0.0);
  mean /= double(vect.size());
  double std = std::accumulate(vect.begin(), vect.end(), 0.0, [&mean](double a, double b) { return a + (b - mean) * (b - mean); });
  std = std::pow(std / double(vect.size()), 0.5);
  return std;
}

/**
  * @brief Compute the standard deviation of the angle distribution.
  * @param[in] data Tracking data.
  * @return Standard deviation.
*/
double AutoLevel::computeStdAngle(const Data &data) {
  QVector<double> angle;
  QList<int> ids = data.getId(0, data.maxFrameIndex);
  for (const int &a : ids) {
    QMap<QString, QVector<double>> i = data.getDataId(a);
    QVector<double> angleDiff(i.value("t" + m_spotSuffix).size());
    std::adjacent_difference(i.value("t" + m_spotSuffix).begin(), i.value("t" + m_spotSuffix).end(), angleDiff.begin(), [](double a, double b) { return Tracking::angleDifference(b, a); });
    QVector<double> tDiff(i.value("imageNumber").size());
    std::adjacent_difference(i.value("imageNumber").begin(), i.value("imageNumber").end(), tDiff.begin());
    std::transform(angleDiff.begin(), angleDiff.end(), tDiff.begin(), angleDiff.begin(), std::divides<double>{});  // Divide the time distance by the time to account for time gaps
    angleDiff.removeFirst();
    angle.append(angleDiff);
  }
  return stdev(angle);
}

/**
  * @brief Compute the standard deviation of the distance distribution.
  * @param[in] data Tracking data.
  * @return Standard deviation.
*/
double AutoLevel::computeStdDistance(const Data &data) {
  QVector<double> distance;
  QList<int> ids = data.getId(0, data.maxFrameIndex);
  for (const int &a : ids) {
    QMap<QString, QVector<double>> i = data.getDataId(a);
    QVector<double> xDiff(i.value("x" + m_spotSuffix).size());
    std::adjacent_difference(i.value("x" + m_spotSuffix).begin(), i.value("x" + m_spotSuffix).end(), xDiff.begin());
    QVector<double> yDiff(i.value("y" + m_spotSuffix).size());
    std::adjacent_difference(i.value("y" + m_spotSuffix).begin(), i.value("y" + m_spotSuffix).end(), yDiff.begin());
    QVector<double> tDiff(i.value("imageNumber").size());
    std::adjacent_difference(i.value("imageNumber").begin(), i.value("imageNumber").end(), tDiff.begin());
    QVector<double> dist(xDiff.size());
    std::transform(xDiff.begin(), xDiff.end(), yDiff.begin(), dist.begin(), [](double a, double b) { return std::pow(std::pow(a, 2) + std::pow(b, 2), 0.5); });
    std::transform(dist.begin(), dist.end(), tDiff.begin(), dist.begin(), std::divides<double>{});  // Divide the time distance by the time to account for time gaps
    dist.removeFirst();
    distance.append(dist);
  }
  return stdev(distance);
}

/**
  * @brief Compute the standard deviation of the area distribution.
  * @param[in] data Tracking data.
  * @return Standard deviation.
*/
double AutoLevel::computeStdArea(const Data &data) {
  QVector<double> area;
  QList<int> ids = data.getId(0, data.maxFrameIndex);
  for (const int &a : ids) {
    QMap<QString, QVector<double>> i = data.getDataId(a);
    QVector<double> areaDiff(i.value("areaBody").size());
    std::adjacent_difference(i.value("areaBody").begin(), i.value("areaBody").end(), areaDiff.begin());
    areaDiff.removeFirst();
    area.append(areaDiff);
  }
  return stdev(area);
}

/**
  * @brief Compute the standard deviation of the angle distribution.
  * @param[in] data Tracking data.
  * @return Standard deviation.
*/
double AutoLevel::computeStdPerimeter(const Data &data) {
  QVector<double> perimeter;
  QList<int> ids = data.getId(0, data.maxFrameIndex);
  for (const int &a : ids) {
    QMap<QString, QVector<double>> i = data.getDataId(a);
    QVector<double> perimDiff(i.value("perimeterBody").size());
    std::adjacent_difference(i.value("perimeterBody").begin(), i.value("perimeterBody").end(), perimDiff.begin());
    perimDiff.removeFirst();
    perimeter.append(perimDiff);
  }
  return stdev(perimeter);
}
