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
#include <QActionGroup>
#include <QDateTime>
#include <QDesktopServices>
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
#include <QScrollArea>
#include <QScrollBar>
#include <QSettings>
#include <QSharedPointer>
#include <QStandardPaths>
#include <QString>
#include <QStyleFactory>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTimer>
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

 private slots:
  void openFolder();

  void display(int index, int scale = 0);
  void display(const QImage &image);
  void display(const cv::UMat &image, QImage::Format format = QImage::Format_RGB888);

  void zoomIn();
  void zoomOut();

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

 private:
  Ui::Interactive *ui;
  QLabel *counterLabel;
  QAction *replayAction;
  QString memoryDir;                  /*!< Saves the path to the last opened folder in dialog. */
  QSize resizedFrame;                 /*!< Size of the resized image in the display QWidget. */
  QSize originalImageSize;            /*!< Size of the original image. */
  QSize cropedImageSize;              /*!< Size of the croped image. */
  QHash<QString, QString> parameters; /*!< Tracking parameters. */
  QString path;                       /*!< */
  QString backgroundPath;             /*!< Path to the background image. */
  QString dir;                        /*!< Path to the folder where the image sequence to display is stored. */
  Tracking *tracking;                 /*!< Tracking object. */
  UMat background;                    /*!< Background image. */
  bool isBackground;                  /*!< Is the background computed. */
  QPair<QPoint, QPoint> clicks;
  QPointF panReferenceClick;
  QPointF zoomReferencePosition;
  Rect roi;
  QPixmap resizedPix;
  vector<Point3i> colorMap;
  double currentZoom;
  VideoReader *video;
  Replay *replay;
  bool videoStatus;

  QString style;
  QString color;
  bool isExpert; /*!< Is FastTrack is in expert mode with advanced capabilities. */
  QSettings *settingsFile;

 signals:
  void message(QString message);
  void log(QHash<QString, QString> log);
  void status(QString messsage);
  void modeChanged(bool isSimple);
};
#endif  // INTERACTIVE_H
