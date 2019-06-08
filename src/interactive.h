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

#ifdef ENABLE_DEVTOOL
#include <quazip/JlCompress.h>
#endif
#include <QWidget>
#include <QAction>
#include <QDir>
#include <QDirIterator>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QFile>
#include <QFileDialog>
#include <QFuture>
#include <QMainWindow>
#include <QMap>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPainter>
#include <QScrollArea>
#include <QScrollBar>
#include <QString>
#include <QSettings>
#include <QTableWidget>
#include <QResizeEvent>
#include <QWidget>
#include <QTableWidgetItem>
#include <QUrl>
#include <QtConcurrent/QtConcurrentRun>
#include "data.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "tracking.h"
#include "replay.h"

using namespace std;
using namespace cv;

namespace Ui {
class Interactive;
}

class Interactive : public QMainWindow {
  Q_OBJECT

 public:
  explicit Interactive(QWidget *parent = nullptr);
  ~Interactive();

 private slots:
  void openFolder();

  void display(int index);
  void display(QImage image);
  void display(UMat image);

  void zoomIn();
  void zoomOut();

  void getParameters();

  void previewTracking();
  void track();

  void computeBackground();
  void selectBackground();

  bool eventFilter(QObject *target, QEvent *event);
  void crop();
  void reset();

  void loadSettings();
  void saveSettings();
  
  void resizeEvent(QResizeEvent *event);

#ifdef ENABLE_DEVTOOL
  void benchmark();
  void benchmarkAnalysis(QMap<QString, QString>);
#endif

 private:
  Ui::Interactive *ui;
  QSettings *settingsFile;
  int currentLayout;
  QMap<QString, QString> settings;
  QLabel *counterLabel;
  QString memoryDir;                 /*!< Saves the path to the last opened folder in dialog. */
  vector<String> framePath;          /*!< Path to all the image of the image sequence to display. */
  QSize resizedFrame;                /*!< Size of the resized image in the display QWidget. */
  QSize originalImageSize;           /*!< Size of the original image. */
  QSize cropedImageSize;             /*!< Size of the croped image. */
  QMap<QString, QString> parameters; /*!< Tracking parameters. */
  QString path;                      /*!< */
  QString backgroundPath;            /*!< Path to the background image. */
  QString dir;                       /*!< Path to the folder where the image sequence to display is stored. */
  Tracking *tracking;                /*!< Tracking object. */
  UMat background;                   /*!< Background image. */
  bool isBackground;                 /*!< Is the background computed. */
  QPair<QPoint, QPoint> clicks;
  QPointF panReferenceClick;
  QPointF zoomReferencePosition;
  Rect roi;
  QPixmap resizedPix;
  Data *trackingData;
  vector<Point3f> colorMap;
  double currentZoom;
  Replay *replay;

#ifdef ENABLE_DEVTOOL
  QVector<int> benchmarkTime;
  int benchmarkCount;
#endif

 signals:
  void message(QString message);
};
#endif  // INTERACTIVE_H
