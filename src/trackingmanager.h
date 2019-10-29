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

#ifndef TRACKINGMANAGER_H
#define TRACKINGMANAGER_H

#include <QDataStream>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QHeaderView>
#include <QMap>
#include <QStandardPaths>
#include <QWidget>

namespace Ui {
class TrackingManager;
}

class TrackingManager : public QWidget {
  Q_OBJECT

 public:
  explicit TrackingManager(QWidget *parent = nullptr);
  ~TrackingManager();

 private:
  QString logPath;
  Ui::TrackingManager *ui;

 public slots:
  void addLogEntry(QMap<QString, QString> log);
  void appendToFile(QString path, QMap<QString, QString> line);
  void writeToFile(QString path, QList<QMap<QString, QString>> lines);
  void readFromFile(QString path);
};

#endif  // TRACKINGMANAGER_H
