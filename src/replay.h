#ifndef REPLAY_H
#define REPLAY_H

#include <stdlib.h>
#include <QCheckBox>
#include <QMainWindow>
#include <QAction>
#include <QComboBox>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QImage>
#include <QKeySequence>
#include <QLCDNumber>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMap>
#include <QMessageBox>
#include <QMouseEvent>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QSettings>
#include <QShortcut>
#include <QSlider>
#include <QSpinBox>
#include <QString>
#include <QTableWidgetItem>
#include <QThread>
#include <QTimer>
#include <QWidget>
#include <QUndoStack>
#include <ctime>
#include <fstream>
#include <opencv2/core/ocl.hpp>
#include <string>
#include <vector>
#include "data.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace std;
using namespace cv;

namespace Ui {
class Replay;
}

class Replay : public QMainWindow {
  Q_OBJECT

 public:
  explicit Replay(QWidget *parent = nullptr, bool standalone = true, QSlider* control = nullptr);
  ~Replay();

 private:
  Ui::Replay *ui;
  QShortcut *wShortcut; /*!< Keyboard shortcut to next occlusion. */
  QShortcut *qShortcut; /*!< Keyboard shortcut to previous frame. */
  QShortcut *aShortcut; /*!< Keyboard shortcut to previous frame. */
  QShortcut *dShortcut; /*!< Keyboard shortcut to next frame. */

  QUndoStack *commandStack;
  QAction *undoAction;
  QAction *redoAction;

  QComboBox *object1Replay;
  QComboBox *object2Replay;

  QString memoryDir; /*!< Saves the path of the last opened folder.*/

  Data *trackingData;

  vector<cv::String> replayFrames; /*!< Paths to each image of the images sequences to replay. */
  vector<Point3f> colorMap;        /*!< RGB color map to display each object in one color. */
  QVector<int> occlusionEvents;    /*!< Index of each occlusion event in the replayed images sequence. */
  int replayNumberObject;          /*!< Number of objects tracked in the replayed images sequence. */
  bool isReplayable;               /*!< True if user input is an images sequences that can be played. */
  QTimer *framerate;               /*!< Sets the time at which a new image is displayed in autoplay mode in the replay. */
  int replayFps;                   /*!< Frame rate value at which a new image is displayed in autoplay mode in the replay. */
  int autoPlayerIndex;             /*!< Index of the image displayed in autoplay mode in the replay. */
  bool object;                     /*!< Alternatively true or false to associate either object A or object B at each click of the user in the ui->replayDisplay. */
  QSize resizedFrame;              /*!< Width and height of displayed QPixmap to accomodate window size changment. */
  QSize originalImageSize;         /*!< Width and height of the original image in the images sequence. */
  double currentZoom;
  QPointF panReferenceClick;
  QPointF zoomReferencePosition;

 public slots:

  void openReplayFolder();
  void loadReplayFolder(QString dir);
  void loadFrame(int frameIndex);
  void zoomIn();
  void zoomOut();
  void toggleReplayPlay();
  bool eventFilter(QObject *target, QEvent *event);
  void correctTracking();
  void nextOcclusionEvent();
  void previousOcclusionEvent();
  void saveTrackedMovie();
};

#endif  // REPLAY_H
