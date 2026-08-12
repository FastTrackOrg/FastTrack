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

#ifndef INTERACTIVE_H
#define INTERACTIVE_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <QAction>
#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QFile>
#include <QFileDialog>
#include <QFuture>
#include <QFutureWatcher>
#include <QHash>
#include <QMainWindow>
#include <QMessageBox>
#include <QMetaType>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPainter>
#include <QResizeEvent>
#include <QScopedValueRollback>
#include <QScrollArea>
#include <QScrollBar>
#include <QSettings>
#include <QSharedPointer>
#include <QString>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QUrl>
#include <QWidget>
#include <QtConcurrent/QtConcurrentRun>
#include "autolevel.h"
#include "data.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "replay.h"
#include "tracking.h"
#include "videoreader.h"

using namespace std;
using namespace cv;

namespace Ui {
class Interactive;
}

class Interactive : public QMainWindow {
  Q_OBJECT

 public:
  explicit Interactive(QWidget *parent = nullptr);
  Interactive(const Interactive &T) = delete;
  Interactive &operator=(const Interactive &T) = delete;
  Interactive &operator=(Interactive &&T) = delete;
  Interactive(Interactive &&T) = delete;
  ~Interactive();
  void openFolder(QString path = QString());
  void setReplayVisible(bool visible);
  bool isReplayVisible() const;
  void setImageOptionsVisible(bool visible);
  bool imageOptionsVisible() const;
  void setTrackingOptionsVisible(bool visible);
  bool trackingOptionsVisible() const;
  void setControlOptionsVisible(bool visible);
  bool controlOptionsVisible() const;

 private slots:
  void display(int index, int scale = 0);
  void display(const QImage &image);
  void display(const cv::UMat &image, QImage::Format format = QImage::Format_RGB888);

  void getParameters();

  void previewTracking();
  void track();

  void computeBackground();
  void selectBackground();

  bool eventFilter(QObject *target, QEvent *event) override;
  void crop();
  void reset();

  void saveSettings();
  void loadParameters(const QString &path);
  void level();
  void setupParameterTabOrder();
  void setupParameterWidgets();
  void setupParameterTooltips();

 private:
  Ui::Interactive *ui;
  QLabel *counterLabel;
  bool replayVisible = false;
  QString memoryDir;                  /*!< Saves the path to the last opened folder in dialog. */
  QSize originalImageSize;            /*!< Size of the original image. */
  QSize cropedImageSize;              /*!< Size of the croped image. */
  QHash<QString, QString> parameters; /*!< Tracking parameters. */
  QString path;                       /*!< */
  QString backgroundPath;             /*!< Path to the background image. */
  QString dir;                        /*!< Path to the folder where the image sequence to display is stored. */
  Tracking *tracking;                 /*!< Tracking object. */
  UMat background;                    /*!< Background image. */
  bool isBackground;                  /*!< Is the background computed. */
  QPair<QPointF, QPointF> clicks;
  Rect roi;
  QPixmap resizedPix;
  vector<Point3i> colorMap;
  VideoReader *video;
  Replay *replay;
  bool videoStatus;
  bool displayInProgress = false;
  bool contourWarningOpen = false;
  int largeContourDisplayDecision = -1;

  QSettings *settingsFile;

 signals:
  void message(QString message);
  void log(QHash<QString, QString> log);
  void status(QString messsage);
  void replayVisibleChanged(bool visible);
};
#endif  // INTERACTIVE_H
