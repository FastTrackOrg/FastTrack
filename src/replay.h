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

#ifndef REPLAY_H
#define REPLAY_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QHash>
#include <QImage>
#include <QLCDNumber>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <QMouseEvent>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QSettings>
#include <QSlider>
#include <QSpinBox>
#include <QString>
#include <QTableWidgetItem>
#include <QThread>
#include <QTimer>
#include <QUndoStack>
#include <QWidget>
#include <fstream>
#include <opencv2/core/ocl.hpp>
#include <string>
#include <vector>
#include "annotation.h"
#include "data.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "videoreader.h"
using namespace std;
using namespace cv;

namespace Ui {
class Replay;
}

class Replay : public QMainWindow {
  Q_OBJECT

 public:
  explicit Replay(QWidget *parent, VideoReader *videoReader);
  Replay(const Replay &T) = delete;
  Replay &operator=(const Replay &T) = delete;
  Replay &operator=(Replay &&T) = delete;
  Replay(Replay &&T) = delete;
  ~Replay();
  QHash<QString, QVariant> displayParameters() const;
  void setDisplayParameters(const QHash<QString, QVariant> &parameters);
  void setAnnotationText(const QString &text);
  void findAnnotation(const QString &text);
  void nextAnnotation();
  void previousAnnotation();
  Data *trackingData;
  Annotation *annotation;

 private:
  Ui::Replay *ui;
  QShortcut *deletedFrameFocus; /*!< Keyboard shortcut to next frame. */
  QSettings *settingsFile;

  QUndoStack *commandStack;
  QAction *undoAction;
  QAction *redoAction;

  QComboBox *object1Replay;
  QComboBox *object2Replay;
  QSpinBox *deletedFrameNumber;

  QString memoryDir; /*!< Saves the path of the last opened folder.*/

  vector<Point3i> colorMap;   /*!< RGB color map to display each object in one color. */
  QList<int> occlusionEvents; /*!< Index of each occlusion event in the replayed images sequence. */
  int replayNumberObject;     /*!< Number of objects tracked in the replayed images sequence. */
  int maxIndex;               /*!< Number of images. */
  bool isReplayable;          /*!< True if user input is an images sequences that can be played. */
  int replayFps;              /*!< Frame rate value at which a new image is displayed in autoplay mode in the replay. */
  int autoPlayerIndex;        /*!< Index of the image displayed in autoplay mode in the replay. */
  bool object;                /*!< Alternatively true or false to associate either object A or object B at each click of the user in the ui->replayDisplay. */
  QSize resizedFrame;         /*!< Width and height of displayed QPixmap to accomodate window size changment. */
  QSize originalImageSize;    /*!< Width and height of the original image in the images sequence. */
  int currentIndex;           /*!< Current image index. */
  QList<int> ids;
  VideoReader *video;
  QHash<QString, QVariant> displayState{{QStringLiteral("ellipse"), 0}, {QStringLiteral("arrow"), 0}, {QStringLiteral("trace"), false}, {QStringLiteral("traceLength"), 10}, {QStringLiteral("numbers"), true}, {QStringLiteral("size"), 2}, {QStringLiteral("fps"), 25}};
  int informationObject1 = 0;
  int informationObject2 = 0;

 public slots:

  void openTrackingDir();
  void loadReplay(const QString &dir);
  void loadTrackingDir(const QString &dir);
  void loadFrame(int frameIndex);
  bool eventFilter(QObject *target, QEvent *event) override;
  void updateInformation(int objectId, int imageIndex, int table);
  void correctTracking();
  void nextOcclusionEvent();
  void previousOcclusionEvent();
  void saveTrackedMovie();
  void sliderConnection(const int index);
  void clear();

 signals:
  void opened(bool);
  void annotationTextChanged(const QString &text);
  void informationChanged(int table, const QList<QString> &values);
  void frameRequested(int frame);
};

#endif  // REPLAY_H
