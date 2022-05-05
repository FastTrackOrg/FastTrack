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
#include <QHash>
#include <QHeaderView>
#include <QStandardPaths>
#include <QWidget>

namespace Ui {
class TrackingManager;
}

class TrackingManager : public QWidget {
  Q_OBJECT

 public:
  explicit TrackingManager(QWidget *parent = nullptr);
  TrackingManager(const TrackingManager &T) = delete;
  TrackingManager &operator=(const TrackingManager &T) = delete;
  TrackingManager &operator=(TrackingManager &&T) = delete;
  TrackingManager(TrackingManager &&T) = delete;
  ~TrackingManager();

 private:
  QString logPath;
  Ui::TrackingManager *ui;

 public slots:
  void addLogEntry(QHash<QString, QString> log);
  void appendToFile(const QString &path, const QHash<QString, QString> &line);
  void writeToFile(const QString &path, const QList<QHash<QString, QString>> &lines);
  void readFromFile(const QString &path);
};

#endif  // TRACKINGMANAGER_H
