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
#ifndef AUTOLEVEL_H
#define AUTOLEVEL_H

#include <QHash>
#include <QMap>
#include <algorithm>
#include <functional>
#include <opencv2/core/types.hpp>
#include <string>
#include "data.h"
#include "tracking.h"
#include "videoreader.h"

using namespace std;

class AutoLevel : public QObject {
  Q_OBJECT

  QString m_savedPath;                 /*!< Old tracking analysis */
  string m_path;                       /*!< Path to video file/image sequence. */
  QString m_spotSuffix;                /*!< Spot to track. */
  UMat m_background;                   /*!< Path to video file/image sequence. */
  QMap<QString, QString> m_parameters; /*!< Optimal ending image index. */
  int m_endImage;                      /*!< Optimal ending image index. */

  double computeStdAngle(const Data &data);
  double computeStdDistance(const Data &data);
  double computeStdArea(const Data &data);
  double computeStdPerimeter(const Data &data);

 public:
  explicit AutoLevel(QWidget *parent = nullptr) : QObject(parent){};
  ~AutoLevel();
  AutoLevel(const string &path, const UMat &background, const QMap<QString, QString> &parameters);
  AutoLevel(const AutoLevel &T) = delete;
  AutoLevel &operator=(const AutoLevel &T) = delete;
  AutoLevel &operator=(AutoLevel &&T) = delete;
  AutoLevel(AutoLevel &&T) = delete;
  static double stdev(const QList<double> &vect);

 public slots:
  QMap<QString, double> level();

 signals:
  void forceFinished(QString message);
  void levelParametersChanged(QMap<QString, double>);
  void finished();
};

#endif
