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

#include "replay.h"
#include "ui_replay.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace cv;
using namespace std;

/**
 * @class Replay
 *
 * @brief
 *
 * @author Benjamin Gallois
 *
 * @version $Revision: 4.1 $
 *
 * Contact: gallois.benjamin08@gmail.com
 *
 */

Replay::Replay(QWidget* parent, VideoReader* videoReader) : QMainWindow(parent),
                                                            ui(new Ui::Replay),
                                                            settingsFile(new QSettings(QStringLiteral("FastTrack"), QStringLiteral("FastTrackOrg"), this)) {
  ui->setupUi(this);
  ui->replayDisplay->setAttribute(Qt::WA_Hover);

  // Generates a color map.
  int a, b, c;
  srand((unsigned int)time(NULL));
  for (int j = 0; j < 90000; ++j) {
    a = rand() % 255;
    b = rand() % 255;
    c = rand() % 255;
    colorMap.push_back(Point3i(a, b, c));
  }

  currentIndex = 0;

  QIcon img = QIcon(":/assets/buttons/open.png");
  QAction* openTrackingDirAction = ui->toolBar->addAction(img, tr("&Open Tracking_Result directory"));
  openTrackingDirAction->setStatusTip(tr("Open an analysis folder"));
  connect(openTrackingDirAction, &QAction::triggered, this, &Replay::openTrackingDir);
  connect(this, &Replay::opened, openTrackingDirAction, &QAction::setEnabled);
  openTrackingDirAction->setEnabled(false);

  img = QIcon(":/assets/buttons/refresh.png");
  QAction* refreshAction = ui->toolBar->addAction(img, tr("&Refresh"));
  refreshAction->setStatusTip(tr("Reload the latest tracking analysis"));
  connect(refreshAction, &QAction::triggered, this, [this]() {
    loadReplay(memoryDir);
  });

  img = QIcon(":/assets/buttons/save.png");
  QAction* exportAction = ui->toolBar->addAction(img, tr("&Export"));
  exportAction->setStatusTip(tr("Export the tracked movie"));
  connect(exportAction, &QAction::triggered, this, &Replay::saveTrackedMovie);

  commandStack = new QUndoStack(this);
  img = QIcon(":/assets/buttons/undo.png");
  QAction* undoAction = commandStack->createUndoAction(this, tr("&Undo"));
  undoAction->setIcon(img);
  undoAction->setShortcuts(QKeySequence::Undo);
  undoAction->setStatusTip(tr("Undo (Ctrl+Z)"));
  undoAction->setToolTip(tr("Undo (Ctrl+Z)"));
  connect(undoAction, &QAction::triggered, this, [this]() {
    object2Replay->clear();
    ids = trackingData->getId(0, static_cast<int>(video->getImageCount()));
    std::sort(ids.begin(), ids.end());
    for (auto const& a : ids) {
      object2Replay->addItem(QString::number(a));
    }
    loadFrame(currentIndex);
  });
  ui->toolBar->addAction(undoAction);

  img = QIcon(":/assets/buttons/redo.png");
  QAction* redoAction = commandStack->createRedoAction(this, tr("&Redo"));
  redoAction->setIcon(img);
  redoAction->setShortcuts(QKeySequence::Redo);
  redoAction->setStatusTip(tr("Redo (Ctrl+Y)"));
  redoAction->setToolTip(tr("Redo (Ctrl+Y)"));
  connect(redoAction, &QAction::triggered, this, [this]() {
    object2Replay->clear();
    ids = trackingData->getId(0, static_cast<int>(video->getImageCount()));
    std::sort(ids.begin(), ids.end());
    for (auto const& a : ids) {
      object2Replay->addItem(QString::number(a));
    }
    loadFrame(currentIndex);
  });
  ui->toolBar->addAction(redoAction);

  ui->toolBar->addSeparator();

  object1Replay = new QComboBox(this);
  object1Replay->setEditable(true);
  object1Replay->setInsertPolicy(QComboBox::NoInsert);
  object1Replay->setStatusTip(tr("First selected object"));
  connect(object1Replay, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
    if (object1Replay->count() != 0) {
      int id = object1Replay->itemText(index).toInt();
      updateInformation(static_cast<int>(id), currentIndex, 1);
      object1Replay->setStyleSheet("QComboBox { background-color: rgb(" + QString::number(colorMap[id].x) + "," + QString::number(colorMap[id].y) + "," + QString::number(colorMap[id].z) + "); }");
    }
  });
  ui->toolBar->addWidget(object1Replay);

  img = QIcon(":/assets/buttons/replace.png");
  QAction* swapAction = ui->toolBar->addAction(img, tr("&Swap"));
  swapAction->setShortcut(QKeySequence(QStringLiteral("S")));
  swapAction->setStatusTip(tr("Swap the two objects (S)"));
  swapAction->setToolTip(tr("Swap the two objects (S)"));
  connect(swapAction, &QAction::triggered, this, &Replay::correctTracking);

  object2Replay = new QComboBox(this);
  object2Replay->setEditable(true);
  object2Replay->setInsertPolicy(QComboBox::NoInsert);
  object2Replay->setStatusTip(tr("Second selected object"));
  connect(object2Replay, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
    if (object2Replay->count() != 0) {
      int id = object2Replay->itemText(index).toInt();
      updateInformation(static_cast<int>(id), currentIndex, 2);
      object2Replay->setStyleSheet("QComboBox { background-color: rgb(" + QString::number(colorMap[id].x) + "," + QString::number(colorMap[id].y) + "," + QString::number(colorMap[id].z) + "); }");
    }
  });
  ui->toolBar->addWidget(object2Replay);

  img = QIcon(":/assets/buttons/deleteOne.png");
  QAction* deleteOneAction = ui->toolBar->addAction(img, tr("&Delete"));
  deleteOneAction->setShortcut(QKeySequence(QStringLiteral("f")));
  deleteOneAction->setStatusTip(tr("Delete the object on this frame (F)"));
  deleteOneAction->setToolTip(tr("Delete the object on this frame (F)"));
  connect(deleteOneAction, &QAction::triggered, this, [this]() {
    if (isReplayable) {
      DeleteData* del = new DeleteData(object2Replay->currentText().toInt(), currentIndex, currentIndex, trackingData);
      commandStack->push(del);
      ids = trackingData->getId(0, static_cast<int>(video->getImageCount()));
      object2Replay->clear();
      for (auto const& a : ids) {
        object2Replay->addItem(QString::number(a));
      }
      loadFrame(currentIndex);
    }
  });

  img = QIcon(":/assets/buttons/delete.png");
  QAction* deleteAction = ui->toolBar->addAction(img, tr("&Delete"));
  deleteAction->setShortcut(QKeySequence(tr("G")));
  deleteAction->setStatusTip(tr("Delete the object from this frame on the selected number of frames (G)"));
  deleteAction->setToolTip(tr("Delete the object from this frame on the selected number of frames (G)"));
  connect(deleteAction, &QAction::triggered, this, [this]() {
    if (isReplayable) {
      DeleteData* del = new DeleteData(object2Replay->currentText().toInt(), currentIndex, currentIndex + deletedFrameNumber->value() - 1, trackingData);
      commandStack->push(del);
      ids = trackingData->getId(0, static_cast<int>(video->getImageCount()));
      object2Replay->clear();
      for (auto const& a : ids) {
        object2Replay->addItem(QString::number(a));
      }
      loadFrame(currentIndex);
    }
  });

  deletedFrameNumber = new QSpinBox(this);
  deletedFrameNumber->setStatusTip(tr("Number of frames where to delete the selected object (C to focus)"));
  deletedFrameNumber->setToolTip(tr("Number of frames where to delete the selected object (C to focus)"));
  deletedFrameFocus = new QShortcut(QKeySequence(QStringLiteral("c")), this);
  connect(deletedFrameFocus, &QShortcut::activated, deletedFrameNumber, static_cast<void (QSpinBox::*)(void)>(&QSpinBox::setFocus));
  connect(deletedFrameFocus, &QShortcut::activated, deletedFrameNumber, &QSpinBox::selectAll);

  ui->toolBar->addWidget(deletedFrameNumber);

  img = QIcon(":/assets/buttons/previous.png");
  QAction* previousAction = ui->toolBar->addAction(img, tr("&Previous"));
  previousAction->setStatusTip(tr("Previous occlusion"));
  connect(previousAction, &QAction::triggered, this, &Replay::previousOcclusionEvent);

  img = QIcon(":/assets/buttons/next.png");
  QAction* nextAction = ui->toolBar->addAction(img, tr("&Next"));
  nextAction->setStatusTip(tr("Next occlusion"));
  connect(nextAction, &QAction::triggered, this, &Replay::nextOcclusionEvent);

  img = QIcon(":/assets/buttons/help.png");
  QAction* helpAction = ui->toolBar->addAction(img, tr("&Help"));
  helpAction->setStatusTip(tr("Help"));
  connect(helpAction, &QAction::triggered, this, [this]() {
    QMessageBox helpBox(this);
    helpBox.setIconPixmap(QPixmap(QStringLiteral(":/assets/buttons/helpImg.png")));
    helpBox.exec();
  });

  ui->toolBar->addSeparator();

  // Install event filters
  ui->replayDisplay->installEventFilter(this);

  isReplayable = false;

  // Annotation object
  annotation = new Annotation(this);
  // Load annotation file
  connect(annotation, &Annotation::annotationText, this, &Replay::annotationTextChanged);

  trackingData = new Data();

  video = videoReader;
}

QHash<QString, QVariant> Replay::displayParameters() const {
  return displayState;
}

void Replay::setDisplayParameters(const QHash<QString, QVariant>& parameters) {
  displayState = parameters;
  loadFrame(currentIndex);
}

void Replay::setAnnotationText(const QString& text) {
  annotation->write(currentIndex, text);
}

void Replay::findAnnotation(const QString& text) {
  annotation->find(text);
}

void Replay::nextAnnotation() {
  emit frameRequested(annotation->next());
}

void Replay::previousAnnotation() {
  emit frameRequested(annotation->prev());
}

void Replay::sliderConnection(const int index) {
  currentIndex = index;
  if (!trackingData->isEmpty) {
    updateInformation(informationObject1, index, 1);
    updateInformation(informationObject2, index, 2);
    deletedFrameNumber->setMaximum(maxIndex - index);
    deletedFrameNumber->setValue(maxIndex - index);
    annotation->read(index);
  }
  loadFrame(index);
}

Replay::~Replay() {
  delete ui;
  delete trackingData;
  delete annotation;
}

/**
 * @brief Opens a dialogue to select a Tracking_Result dir, necessitate a video already opened and matching tracking results.
 */
void Replay::openTrackingDir() {
  QString dir = QFileDialog::getExistingDirectory(this, tr("Open Tracking_Result_* Directory"), memoryDir, QFileDialog::ShowDirsOnly);
  QApplication::setOverrideCursor(Qt::WaitCursor);
  loadTrackingDir(dir);
  QApplication::restoreOverrideCursor();
}

/**
 * @brief Clears replay data.
 */
void Replay::clear() {
  annotation->clear();
  trackingData->clear();

  currentIndex = 0;
  emit frameRequested(0);
  commandStack->clear();
  occlusionEvents.clear();
  object1Replay->clear();
  object2Replay->clear();
  ui->replayDisplay->clear();
  emit annotationTextChanged(QString());
  object = true;
  isReplayable = false;
  emit opened(isReplayable);
}

/**
 * @brief Loads a video/images sequence and the last analysis performed.
 * @arg[in] dir Path to a video or image of an images sequence.
 */
void Replay::loadReplay(const QString& dir) {
  // This function will detect from an inputed path to a directory the image sequence and the tracking data.
  // The last tracking data from the folder Tracking_Result is automatically loaded if found.
  // If the user explicitly select another Tracking_Result folder, these data are loaded.
  // Delete existing data

  QApplication::setOverrideCursor(Qt::WaitCursor);
  clear();
  if (!dir.length()) {
    memoryDir.clear();
    return;
  }

  try {
    // Gets the paths to all the frames in the folder and puts it in a vector.
    // Setups the ui by setting maximum and minimum of the slider bar.
    maxIndex = static_cast<int>(video->getImageCount());

    Mat frame;
    video->getImage(0, frame);
    cvtColor(frame, frame, COLOR_GRAY2RGB);
    originalImageSize.setWidth(frame.cols);
    originalImageSize.setHeight(frame.rows);
    deletedFrameNumber->setRange(1, static_cast<int>(video->getImageCount()));
    deletedFrameNumber->setValue(static_cast<int>(video->getImageCount()));
    if (video->isOpened()) {
      isReplayable = true;
    }

    memoryDir = dir;
    loadTrackingDir(dir);

    emit frameRequested(1);  // Force a frame refresh after loading.
    emit frameRequested(0);
    ui->replayDisplay->fitToView();
    emit opened(isReplayable);
  }
  catch (const std::exception& e) {
    qWarning() << QString::fromStdString(e.what()) << " occurs opening " << dir;
    isReplayable = false;
    memoryDir.clear();
    QMessageBox msgBox;
    msgBox.setText(tr("No file found."));
    msgBox.exec();
  }
  QApplication::restoreOverrideCursor();
}

/**
 * @brief Loads a tracking analysis folder from a video file.
 * @arg[in] dir Path to a video or image of an images sequence.
 */
void Replay::loadTrackingDir(const QString& dir) {
  if (!dir.length()) return;

  QString trackingDir;
  QFileInfo savingInfo(dir);
  // If the dir is the Tracking_Result directory
  if (savingInfo.isDir()) {
    trackingDir = dir + QDir::separator();
  }
  // If the dir is the video file
  else if (savingInfo.isFile()) {
    QString savingFilename = savingInfo.baseName();
    QString savingPath = savingInfo.absolutePath();
    trackingDir = savingPath;
    if (video->isSequence()) {
      trackingDir.append(QStringLiteral("/Tracking_Result") + QDir::separator());
    }
    else {
      trackingDir.append(QStringLiteral("/Tracking_Result_") + savingFilename + QDir::separator());
    }
  }

  trackingData->setPath(trackingDir);
  ids = trackingData->getId(0, static_cast<int>(video->getImageCount()));
  for (auto const& a : ids) {
    object2Replay->addItem(QString::number(a));
  }

  // Load annotation file
  annotation->setPath(trackingDir);
}

/**
 * @brief Displays the image and the tracking data in the ui->displayReplay. Triggered when the ui->replaySlider value is changed.
 */
void Replay::loadFrame(int frameIndex) {
  try {
    if (!isReplayable) {
      return;
    }

    currentIndex = frameIndex;
    object1Replay->clear();

    UMat frame;
    if (!video->getImage(frameIndex, frame)) {
      return;
    }
    cvtColor(frame, frame, COLOR_GRAY2BGR);

    if (!trackingData->isEmpty) {
      // Takes the tracking data corresponding to the replayed frame and parse data to display
      int scale = displayState.value(QStringLiteral("size")).toInt();
      QList<QHash<QString, double>> dataImage = trackingData->getData(frameIndex);
      for (const QHash<QString, double>& coordinate : dataImage) {
        int id = static_cast<int>(coordinate.value(QStringLiteral("id")));

        object1Replay->addItem(QString::number(id));

        if (displayState.value(QStringLiteral("ellipse")).toInt() != 4) {
          switch (displayState.value(QStringLiteral("ellipse")).toInt()) {
            case 0:  // Head + Tail
              cv::ellipse(frame, Point(static_cast<int>(coordinate.value(QStringLiteral("xHead"))), static_cast<int>(coordinate.value(QStringLiteral("yHead")))), Size(static_cast<int>(coordinate.value(QStringLiteral("headMajorAxisLength"))), static_cast<int>(coordinate.value(QStringLiteral("headMinorAxisLength")))), 180 - (coordinate.value(QStringLiteral("tHead")) * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_8);
              cv::ellipse(frame, Point(static_cast<int>(coordinate.value(QStringLiteral("xTail"))), static_cast<int>(coordinate.value(QStringLiteral("yTail")))), Size(static_cast<int>(coordinate.value(QStringLiteral("tailMajorAxisLength"))), static_cast<int>(coordinate.value(QStringLiteral("tailMinorAxisLength")))), 180 - (coordinate.value(QStringLiteral("tTail")) * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_8);
              break;

            case 1:  // Head
              cv::ellipse(frame, Point(static_cast<int>(coordinate.value(QStringLiteral("xHead"))), static_cast<int>(coordinate.value(QStringLiteral("yHead")))), Size(static_cast<int>(coordinate.value(QStringLiteral("headMajorAxisLength"))), static_cast<int>(coordinate.value(QStringLiteral("headMinorAxisLength")))), 180 - (coordinate.value(QStringLiteral("tHead")) * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_8);
              break;

            case 2:  // Tail
              cv::ellipse(frame, Point(static_cast<int>(coordinate.value(QStringLiteral("xTail"))), static_cast<int>(coordinate.value(QStringLiteral("yTail")))), Size(static_cast<int>(coordinate.value(QStringLiteral("tailMajorAxisLength"))), static_cast<int>(coordinate.value(QStringLiteral("tailMinorAxisLength")))), 180 - (coordinate.value(QStringLiteral("tTail")) * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_8);
              break;

            case 3:  // Body
              cv::ellipse(frame, Point(static_cast<int>(coordinate.value(QStringLiteral("xBody"))), static_cast<int>(coordinate.value(QStringLiteral("yBody")))), Size(static_cast<int>(coordinate.value(QStringLiteral("bodyMajorAxisLength"))), static_cast<int>(coordinate.value(QStringLiteral("bodyMinorAxisLength")))), 180 - (coordinate.value(QStringLiteral("tBody")) * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_8);
              break;
          }
        }

        if (displayState.value(QStringLiteral("arrow")).toInt() != 4) {
          switch (displayState.value(QStringLiteral("arrow")).toInt()) {
            case 0:
              cv::arrowedLine(frame, Point(static_cast<int>(coordinate.value(QStringLiteral("xHead"))), static_cast<int>(coordinate.value(QStringLiteral("yHead")))), Point(static_cast<int>(coordinate.value(QStringLiteral("xHead")) + coordinate.value(QStringLiteral("headMajorAxisLength")) * cos(coordinate.value(QStringLiteral("tHead")))), static_cast<int>(coordinate.value(QStringLiteral("yHead")) - coordinate.value(QStringLiteral("headMajorAxisLength")) * sin(coordinate.value(QStringLiteral("tHead"))))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_8, 0, double(scale) / 10);
              cv::arrowedLine(frame, Point(static_cast<int>(coordinate.value(QStringLiteral("xTail"))), static_cast<int>(coordinate.value(QStringLiteral("yTail")))), Point(static_cast<int>(coordinate.value(QStringLiteral("xTail")) + coordinate.value(QStringLiteral("tailMajorAxisLength")) * cos(coordinate.value(QStringLiteral("tTail")))), static_cast<int>(coordinate.value(QStringLiteral("yTail")) - coordinate.value(QStringLiteral("tailMajorAxisLength")) * sin(coordinate.value(QStringLiteral("tTail"))))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_8, 0, double(scale) / 10);
              break;

            case 1:
              cv::arrowedLine(frame, Point(static_cast<int>(coordinate.value(QStringLiteral("xHead"))), static_cast<int>(coordinate.value(QStringLiteral("yHead")))), Point(static_cast<int>(coordinate.value(QStringLiteral("xHead")) + coordinate.value(QStringLiteral("headMajorAxisLength")) * cos(coordinate.value(QStringLiteral("tHead")))), static_cast<int>(coordinate.value(QStringLiteral("yHead")) - coordinate.value(QStringLiteral("headMajorAxisLength")) * sin(coordinate.value(QStringLiteral("tHead"))))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_8, 0, double(scale) / 10);
              break;

            case 2:
              cv::arrowedLine(frame, Point(static_cast<int>(coordinate.value(QStringLiteral("xTail"))), static_cast<int>(coordinate.value(QStringLiteral("yTail")))), Point(static_cast<int>(coordinate.value(QStringLiteral("xTail")) + coordinate.value(QStringLiteral("tailMajorAxisLength")) * cos(coordinate.value(QStringLiteral("tTail")))), static_cast<int>(coordinate.value(QStringLiteral("yTail")) - coordinate.value(QStringLiteral("tailMajorAxisLength")) * sin(coordinate.value(QStringLiteral("tTail"))))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_8, 0, double(scale) / 10);
              break;

            case 3:
              cv::arrowedLine(frame, Point(static_cast<int>(coordinate.value(QStringLiteral("xBody"))), static_cast<int>(coordinate.value(QStringLiteral("yBody")))), Point(static_cast<int>(coordinate.value(QStringLiteral("xBody")) + coordinate.value(QStringLiteral("bodyMajorAxisLength")) * cos(coordinate.value(QStringLiteral("tBody")))), static_cast<int>(coordinate.value(QStringLiteral("yBody")) - coordinate.value(QStringLiteral("bodyMajorAxisLength")) * sin(coordinate.value(QStringLiteral("tBody"))))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_8, 0, double(scale) / 10);
              break;
          }
        }

        if (displayState.value(QStringLiteral("numbers")).toBool()) {
          cv::putText(frame, to_string(id), Point(static_cast<int>(coordinate.value(QStringLiteral("xHead")) + coordinate.value(QStringLiteral("headMajorAxisLength")) * cos(coordinate.value(QStringLiteral("tHead")))), static_cast<int>(coordinate.value(QStringLiteral("yHead")) - coordinate.value(QStringLiteral("headMajorAxisLength")) * sin(coordinate.value(QStringLiteral("tHead"))))), cv::FONT_HERSHEY_SIMPLEX, double(scale) * 0.5, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_8);
        }

        if (displayState.value(QStringLiteral("trace")).toBool()) {
          vector<Point> memory;
          QList<QHash<QString, double>> coordinate = trackingData->getData(frameIndex - displayState.value(QStringLiteral("traceLength")).toInt(), frameIndex + 1, id);
          for (auto const& a : coordinate) {
            memory.push_back(Point(static_cast<int>(a.value(QStringLiteral("xBody"))), static_cast<int>(a.value(QStringLiteral("yBody")))));
          }
          cv::polylines(frame, memory, false, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_8);
        }
      }
    }

    QImage qimage(frame.u->data, frame.cols, frame.rows, static_cast<int>(frame.step), QImage::Format_RGB888);
    ui->replayDisplay->setImage(qimage);
  }
  catch (const std::exception& e) {
    qWarning() << QString::fromStdString(e.what()) << " occurs at image " << frameIndex << " display";
  }
  catch (...) {
    qWarning() << "Unknown error occurs at image " << frameIndex << " display";
  }
}

/**
 * @brief Manages all the mouse input in the display.
 * @param[in] target Target widget to apply the filter.
 * @param[in] event Describes the mouse event.
 */
bool Replay::eventFilter(QObject* target, QEvent* event) {
  // Event filter for the display
  if (target == ui->replayDisplay) {
    // Mouse click event
    if (event->type() == QEvent::MouseButtonPress) {
      QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);

      // Left click to select an object
      if (mouseEvent->buttons() == Qt::LeftButton && isReplayable) {
        // Finds user click coordinate
        QPointF click = ui->replayDisplay->mapToScene(mouseEvent->pos());
        double xTop = click.x();
        double yTop = click.y();

        // Finds the id of the closest object
        int frameIndex = currentIndex;
        QList<int> idList = trackingData->getId(frameIndex);

        if (!idList.isEmpty()) {
          QList<double> distance;
          for (auto const& a : idList) {
            QHash<QString, double> coordinate = trackingData->getData(frameIndex, a);
            distance.append(pow(coordinate.value(QStringLiteral("xBody")) - xTop, 2) + pow(coordinate.value(QStringLiteral("yBody")) - yTop, 2));
          }

          // Finds the minimal distance and updates the UI
          int min = idList.at(static_cast<int>(std::min_element(distance.begin(), distance.end()) - distance.begin()));
          if (object) {
            object1Replay->setCurrentIndex(object1Replay->findText(QString::number(min)));
            object = false;
          }
          else {
            object2Replay->setCurrentIndex(object2Replay->findText(QString::number(min)));
            object = true;
          }
        }
      }

      // Right click event
      else if (mouseEvent->buttons() == Qt::RightButton && isReplayable) {
        correctTracking();
        object1Replay->setStyleSheet(QStringLiteral("QComboBox { background-color: white; }"));
        object2Replay->setStyleSheet(QStringLiteral("QComboBox { background-color: white; }"));
      }
    }
  }
  return QWidget::eventFilter(target, event);
}

/**
 * @brief Update the information of an object inside a table widget.
 * @param[in] objectId The id of the object to display the data.
 * @param[in] imageIndex The index of the image where to extracts the data.
 * @param[in] table Pointer to a QTableWidget where to display the data.
 */
void Replay::updateInformation(int objectId, int imageIndex, int table) {
  QHash<QString, double> infoData = trackingData->getData(imageIndex, objectId);
  if (table == 1) {
    informationObject1 = objectId;
  }
  else {
    informationObject2 = objectId;
  }
  emit informationChanged(table,
                          {QString::number(objectId),
                           QString::number(trackingData->getObjectInformation(objectId)),
                           QString::number(infoData.value(QStringLiteral("areaBody"))),
                           QString::number(infoData.value(QStringLiteral("perimeterBody"))),
                           QString::number(infoData.value(QStringLiteral("bodyExcentricity")))});
}

/**
 * @brief Gets the index of the two selected objects, the start index, swaps the data from the start index to the end, and saves the new tracking data. Triggered when ui->swapButton is pressed or a right-click event is registered inside the replayDisplay.
 */
void Replay::correctTracking() {
  if (isReplayable) {
    // Swaps the data
    int firstObject = object1Replay->currentText().toInt();
    int secondObject = object2Replay->currentText().toInt();
    int start = currentIndex;
    SwapData* swap = new SwapData(firstObject, secondObject, start, trackingData);
    commandStack->push(swap);
    loadFrame(currentIndex);
  }
}

/**
 * @brief Finds and displays the next occlusion event on the ui->replayDisplay. Triggered when ui->nextReplay is pressed.
 */
void Replay::nextOcclusionEvent() {
  if (!occlusionEvents.isEmpty()) {
    int current = currentIndex;
    int nextOcclusion = *std::upper_bound(occlusionEvents.begin(), occlusionEvents.end(), current);
    emit frameRequested(nextOcclusion);
  }
}

/**
 * @brief Finds and displays the previous occlusion event on the ui->replayDisplay. Triggered when ui->previousReplay is pressed.
 */
void Replay::previousOcclusionEvent() {
  if (!occlusionEvents.isEmpty()) {
    int current = currentIndex;
    int previousOcclusion = occlusionEvents.at(static_cast<int>(std::upper_bound(occlusionEvents.begin(), occlusionEvents.end(), current) - occlusionEvents.begin() - 2));
    emit frameRequested(previousOcclusion);
  }
}

/**
 * @brief Saves the tracked movie in .avi. Triggered when ui->previousReplay is pressed.
 */
void Replay::saveTrackedMovie() {
  // If tracking data are available, gets the display settings and saves the movie in the
  // selected folder
  if (isReplayable) {
    QString savePath = QFileDialog::getSaveFileName(this, tr("Save File"), QStringLiteral("/home/save.mp4"), tr("Videos (*.mp4)"));
    cv::VideoWriter outputVideo(savePath.toStdString(), CAP_FFMPEG, cv::VideoWriter::fourcc('a', 'v', 'c', '1'), displayState.value(QStringLiteral("fps")).toInt(), Size(originalImageSize.width(), originalImageSize.height()));
    int scale = displayState.value(QStringLiteral("size")).toInt();

    QApplication::setOverrideCursor(Qt::WaitCursor);
    this->setEnabled(false);
    for (int frameIndex = 0; frameIndex < static_cast<int>(video->getImageCount()); frameIndex++) {
      Mat frame;
      video->getImage(frameIndex, frame);
      cvtColor(frame, frame, COLOR_GRAY2BGR);
      // Takes the tracking data corresponding to the replayed frame and parse data to display
      // arrows on tracked objects.
      QList<int> idList = trackingData->getId(frameIndex);
      for (auto const& a : idList) {
        QHash<QString, double> coordinate = trackingData->getData(frameIndex, a);
        int id = a;

        object1Replay->addItem(QString::number(id));
        object2Replay->addItem(QString::number(id));

        if (displayState.value(QStringLiteral("ellipse")).toInt() != 4) {
          switch (displayState.value(QStringLiteral("ellipse")).toInt()) {
            case 0:  // Head + Tail
              cv::ellipse(frame, Point(static_cast<int>(coordinate.value(QStringLiteral("xHead"))), static_cast<int>(coordinate.value(QStringLiteral("yHead")))), Size(static_cast<int>(coordinate.value(QStringLiteral("headMajorAxisLength"))), static_cast<int>(coordinate.value(QStringLiteral("headMinorAxisLength")))), 180 - (coordinate.value(QStringLiteral("tHead")) * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, 8);
              cv::ellipse(frame, Point(static_cast<int>(coordinate.value(QStringLiteral("xTail"))), static_cast<int>(coordinate.value(QStringLiteral("yTail")))), Size(static_cast<int>(coordinate.value(QStringLiteral("tailMajorAxisLength"))), static_cast<int>(coordinate.value(QStringLiteral("tailMinorAxisLength")))), 180 - (coordinate.value(QStringLiteral("tTail")) * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA);
              break;

            case 1:  // Head
              cv::ellipse(frame, Point(static_cast<int>(coordinate.value(QStringLiteral("xHead"))), static_cast<int>(coordinate.value(QStringLiteral("yHead")))), Size(static_cast<int>(coordinate.value(QStringLiteral("headMajorAxisLength"))), static_cast<int>(coordinate.value(QStringLiteral("headMinorAxisLength")))), 180 - (coordinate.value(QStringLiteral("tHead")) * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, 8);
              break;

            case 2:  // Tail
              cv::ellipse(frame, Point(static_cast<int>(coordinate.value(QStringLiteral("xTail"))), static_cast<int>(coordinate.value(QStringLiteral("yTail")))), Size(static_cast<int>(coordinate.value(QStringLiteral("tailMajorAxisLength"))), static_cast<int>(coordinate.value(QStringLiteral("tailMinorAxisLength")))), 180 - (coordinate.value(QStringLiteral("tTail")) * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA);
              break;

            case 3:  // Body
              cv::ellipse(frame, Point(static_cast<int>(coordinate.value(QStringLiteral("xBody"))), static_cast<int>(coordinate.value(QStringLiteral("yBody")))), Size(static_cast<int>(coordinate.value(QStringLiteral("bodyMajorAxisLength"))), static_cast<int>(coordinate.value(QStringLiteral("bodyMinorAxisLength")))), 180 - (coordinate.value(QStringLiteral("tBody")) * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, 8);
              break;
          }
        }

        if (displayState.value(QStringLiteral("arrow")).toInt() != 4) {
          switch (displayState.value(QStringLiteral("arrow")).toInt()) {
            case 0:
              cv::arrowedLine(frame, Point(static_cast<int>(coordinate.value(QStringLiteral("xHead"))), static_cast<int>(coordinate.value(QStringLiteral("yHead")))), Point(static_cast<int>(coordinate.value(QStringLiteral("xHead")) + coordinate.value(QStringLiteral("headMajorAxisLength")) * cos(coordinate.value(QStringLiteral("tHead")))), static_cast<int>(coordinate.value(QStringLiteral("yHead")) - coordinate.value(QStringLiteral("headMajorAxisLength")) * sin(coordinate.value(QStringLiteral("tHead"))))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA, 0, double(scale) / 10);
              cv::arrowedLine(frame, Point(static_cast<int>(coordinate.value(QStringLiteral("xTail"))), static_cast<int>(coordinate.value(QStringLiteral("yTail")))), Point(static_cast<int>(coordinate.value(QStringLiteral("xTail")) + coordinate.value(QStringLiteral("tailMajorAxisLength")) * cos(coordinate.value(QStringLiteral("tTail")))), static_cast<int>(coordinate.value(QStringLiteral("yTail")) - coordinate.value(QStringLiteral("tailMajorAxisLength")) * sin(coordinate.value(QStringLiteral("tTail"))))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA, 0, double(scale) / 10);
              break;

            case 1:
              cv::arrowedLine(frame, Point(static_cast<int>(coordinate.value(QStringLiteral("xHead"))), static_cast<int>(coordinate.value(QStringLiteral("yHead")))), Point(static_cast<int>(coordinate.value(QStringLiteral("xHead")) + coordinate.value(QStringLiteral("headMajorAxisLength")) * cos(coordinate.value(QStringLiteral("tHead")))), static_cast<int>(coordinate.value(QStringLiteral("yHead")) - coordinate.value(QStringLiteral("headMajorAxisLength")) * sin(coordinate.value(QStringLiteral("tHead"))))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA, 0, double(scale) / 10);
              break;

            case 2:
              cv::arrowedLine(frame, Point(static_cast<int>(coordinate.value(QStringLiteral("xTail"))), static_cast<int>(coordinate.value(QStringLiteral("yTail")))), Point(static_cast<int>(coordinate.value(QStringLiteral("xTail")) + coordinate.value(QStringLiteral("tailMajorAxisLength")) * cos(coordinate.value(QStringLiteral("tTail")))), static_cast<int>(coordinate.value(QStringLiteral("yTail")) - coordinate.value(QStringLiteral("tailMajorAxisLength")) * sin(coordinate.value(QStringLiteral("tTail"))))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA, 0, double(scale) / 10);
              break;

            case 3:
              cv::arrowedLine(frame, Point(static_cast<int>(coordinate.value(QStringLiteral("xBody"))), static_cast<int>(coordinate.value(QStringLiteral("yBody")))), Point(static_cast<int>(coordinate.value(QStringLiteral("xBody")) + coordinate.value(QStringLiteral("bodyMajorAxisLength")) * cos(coordinate.value(QStringLiteral("tBody")))), static_cast<int>(coordinate.value(QStringLiteral("yBody")) - coordinate.value(QStringLiteral("bodyMajorAxisLength")) * sin(coordinate.value(QStringLiteral("tBody"))))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA, 0, double(scale) / 10);
              break;
          }
        }

        if (displayState.value(QStringLiteral("numbers")).toBool()) {
          cv::putText(frame, to_string(id), Point(static_cast<int>(coordinate.value(QStringLiteral("xHead")) + coordinate.value(QStringLiteral("headMajorAxisLength")) * cos(coordinate.value(QStringLiteral("tHead")))), static_cast<int>(coordinate.value(QStringLiteral("yHead")) - coordinate.value(QStringLiteral("headMajorAxisLength")) * sin(coordinate.value(QStringLiteral("tHead"))))), cv::FONT_HERSHEY_SIMPLEX, double(scale) * 0.5, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA);
        }

        if (displayState.value(QStringLiteral("trace")).toBool()) {
          vector<Point> memory;
          for (int j = frameIndex - displayState.value(QStringLiteral("traceLength")).toInt(); j < frameIndex; j++) {
            if (j > 0) {
              QHash<QString, double> coordinate = trackingData->getData(j, a);
              if (coordinate.contains(QStringLiteral("xBody"))) {
                memory.push_back(Point(static_cast<int>(coordinate.value(QStringLiteral("xBody"))), static_cast<int>(coordinate.value(QStringLiteral("yBody")))));
              }
            }
          }
          cv::polylines(frame, memory, false, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA);
        }
      }
      outputVideo.write(frame);
    }
    outputVideo.release();
    QApplication::restoreOverrideCursor();
    this->setEnabled(true);
  }
}
