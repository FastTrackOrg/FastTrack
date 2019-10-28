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

#ifndef OPENVIDEO_H
#define OPENVIDEO_H

#include <stdio.h>
#include <QAbstractItemView>
#include <QDebug>
#include <QDialog>
#include <QFileDialog>
#include <QFuture>
#include <QMessageBox>
#include <QProgressBar>
#include <QtConcurrent/QtConcurrentRun>
#include <iostream>
#include <opencv2/core.hpp>
#include <ostream>
#include <string>
#include "opencv2/opencv.hpp"

namespace Ui {
class OpenVideo;
}

class OpenVideo : public QDialog {
  Q_OBJECT

 public:
  explicit OpenVideo(QWidget *parent = nullptr);
  ~OpenVideo();

 private slots:

  void selectVideo();
  void createImageSequence(QString path, QProgressBar *bar);

 private:
  Ui::OpenVideo *ui;
};

#endif  // OPENVIDEO_H
