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
AutoLevel::AutoLevel(const string &path, const UMat &background, const QHash<QString, QString> &parameters) : m_path{path}, m_background{background}, m_parameters{parameters}, m_endImage{200} {
  // Save precedent analysis if exist
  VideoReader video(m_path);
  if (!video.open()) {
    throw std::runtime_error("Can't open video " + m_path);
  }
  QFileInfo savingInfo(QString::fromStdString(m_path));
  QString savingFilename = savingInfo.baseName();
  m_savedPath = savingInfo.absolutePath();
  if (video.isSequence()) {
    m_savedPath.append(QStringLiteral("/Tracking_Result"));
  }
  else {
    m_savedPath.append(QStringLiteral("/Tracking_Result_") + savingFilename);
  }
  QDir r;
  r.rename(m_savedPath, m_savedPath + "tmp");
}

/**
 * @brief Levels the tracking parameters.
 * @return Map containing the levelled parameters.
 */
QHash<QString, double> AutoLevel::level() {
  try {
    srand(static_cast<unsigned int>(time(NULL)));
    double stdAngle = static_cast<double>(rand() % 360 + 1);
    double stdDist = static_cast<double>(rand() % 500 + 1);
    double stdArea = static_cast<double>(rand() % 500 + 1);
    double stdPerimeter = static_cast<double>(rand() % 500 + 1);
    int spot = m_parameters.value(QStringLiteral("spot")).toInt();
    if (spot == 0) {
      m_spotSuffix = QStringLiteral("Head");
    }
    else if (spot == 1) {
      m_spotSuffix = QStringLiteral("Tail");
    }
    else if (spot == 2) {
      m_spotSuffix = QStringLiteral("Body");
    }
    int counter = 0;
    while (abs(stdAngle - m_parameters.value(QStringLiteral("normAngle")).toDouble()) > 1E-3 && abs(stdDist - m_parameters.value(QStringLiteral("normDist")).toDouble()) > 1E-3 && abs(stdArea - m_parameters.value(QStringLiteral("normArea")).toDouble()) > 1E-3 && abs(stdPerimeter - m_parameters.value(QStringLiteral("normPerim")).toDouble()) > 1E-3) {
      m_parameters.insert(QStringLiteral("normAngle"), QString::number(stdAngle));
      m_parameters.insert(QStringLiteral("normDist"), QString::number(stdDist));
      m_parameters.insert(QStringLiteral("normArea"), QString::number(stdArea));
      m_parameters.insert(QStringLiteral("normPerim"), QString::number(stdPerimeter));
      Tracking tracking = Tracking(m_path, m_background, 0, m_endImage);
      tracking.updatingParameters(m_parameters);
      tracking.startProcess();
      Data data(m_savedPath + QDir::separator());
      stdAngle = 180 * computeStdAngle(data) / M_PI;
      stdDist = computeStdDistance(data);
      stdArea = computeStdArea(data);
      stdPerimeter = computeStdPerimeter(data);
      // This construction only work because tracking data are not changed
      // therefore no call to Data.save() are triggered at destruction
      // at out of scope on a non-existing tracking.txt already deleted.
      QDir(m_savedPath).removeRecursively();
      counter++;
      if (counter > 100) {
        throw std::runtime_error("Level not converging");
      }
    }
    QHash<QString, double> levelParameters;
    levelParameters.insert(QStringLiteral("normAngle"), stdAngle);
    levelParameters.insert(QStringLiteral("normDist"), stdDist);
    levelParameters.insert(QStringLiteral("normArea"), stdArea);
    levelParameters.insert(QStringLiteral("normPerim"), stdPerimeter);
    levelParameters.insert(QStringLiteral("nIterations"), double(counter));
    emit levelParametersChanged(levelParameters);
    emit finished();
    return levelParameters;
  }
  catch (const std::exception &e) {
    emit forceFinished(QString::fromStdString(e.what()));
    QHash<QString, double> levelParameters;
    return levelParameters;
  }
}

/**
 * @brief Compute the std from a vector.
 * @param[in] data Distribution.
 * @return Std.
 */
double AutoLevel::stdev(const QList<double> &vect) {
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
  QList<double> angle;
  QList<int> ids = data.getId(0, data.maxFrameIndex);
  for (const int &a : ids) {
    QHash<QString, QList<double>> i = data.getDataId(a);
    QList<double> angleDiff(i.value("t" + m_spotSuffix).size());
    QList<double> ang = i.value("t" + m_spotSuffix);
    std::adjacent_difference(ang.begin(), ang.end(), angleDiff.begin(), [](double a, double b) { return Tracking::angleDifference(b, a); });
    QList<double> tDiff(i.value(QStringLiteral("imageNumber")).size());
    QList<double> image = i.value(QStringLiteral("imageNumber"));
    std::adjacent_difference(image.begin(), image.end(), tDiff.begin());
    std::transform(angleDiff.begin(), angleDiff.end(), tDiff.begin(), angleDiff.begin(), std::divides<double>{});  // Divide the distance by the time to account for time gaps
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
  QList<double> distance;
  QList<int> ids = data.getId(0, data.maxFrameIndex);
  for (const int &a : ids) {
    QHash<QString, QList<double>> i = data.getDataId(a);
    QList<double> xDiff(i.value("x" + m_spotSuffix).size());
    QList<double> x = i.value("x" + m_spotSuffix);
    std::adjacent_difference(x.begin(), x.end(), xDiff.begin());
    QList<double> yDiff(i.value("y" + m_spotSuffix).size());
    QList<double> y = i.value("y" + m_spotSuffix);
    std::adjacent_difference(y.begin(), y.end(), yDiff.begin());
    QList<double> tDiff(i.value(QStringLiteral("imageNumber")).size());
    QList<double> image = i.value(QStringLiteral("imageNumber"));
    std::adjacent_difference(image.begin(), image.end(), tDiff.begin());
    QList<double> dist(xDiff.size());
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
  QList<double> area;
  QList<int> ids = data.getId(0, data.maxFrameIndex);
  for (const int &a : ids) {
    QHash<QString, QList<double>> i = data.getDataId(a);
    QList<double> areaDiff(i.value(QStringLiteral("areaBody")).size());
    QList<double> ar = i.value(QStringLiteral("areaBody"));
    std::adjacent_difference(ar.begin(), ar.end(), areaDiff.begin());
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
  QList<double> perimeter;
  QList<int> ids = data.getId(0, data.maxFrameIndex);
  for (const int &a : ids) {
    QHash<QString, QList<double>> i = data.getDataId(a);
    QList<double> perimDiff(i.value(QStringLiteral("perimeterBody")).size());
    QList<double> perim = i.value(QStringLiteral("perimeterBody"));
    std::adjacent_difference(perim.begin(), perim.end(), perimDiff.begin());
    perimDiff.removeFirst();
    perimeter.append(perimDiff);
  }
  return stdev(perimeter);
}

AutoLevel::~AutoLevel() {
  QDir r;
  r.rename(m_savedPath + "tmp", m_savedPath);
}
