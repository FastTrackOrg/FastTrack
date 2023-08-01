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

Replay::Replay(QWidget* parent, bool standalone, Timeline* slider, VideoReader* videoReader) : QMainWindow(parent),
                                                                                               ui(new Ui::Replay),
                                                                                               isStandalone(standalone),
                                                                                               settingsFile(new QSettings(QStringLiteral("FastTrack"), QStringLiteral("FastTrackOrg"), this)) {
  ui->setupUi(this);
  ui->replayDisplay->setAttribute(Qt::WA_Hover);

  // Loads settings
  settingsFile->beginGroup(QStringLiteral("replay"));
  restoreState(settingsFile->value(QStringLiteral("windowState")).toByteArray());

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

  QIcon img;
  if (isStandalone) {
    img = QIcon(":/assets/buttons/openImage.png");
    QAction* openAction = ui->toolBar->addAction(img, tr("&Open"));
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Open a tracked movie"));
    connect(openAction, &QAction::triggered, this, &Replay::openReplay);
  }

  img = QIcon(":/assets/buttons/open.png");
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
  undoAction->setStatusTip(tr("Undo"));
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
  redoAction->setStatusTip(tr("Redo"));
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
      updateInformation(static_cast<int>(id), ui->replaySlider->value(), ui->infoTableObject1);
      object1Replay->setStyleSheet("QComboBox { background-color: rgb(" + QString::number(colorMap[id].x) + "," + QString::number(colorMap[id].y) + "," + QString::number(colorMap[id].z) + "); }");
    }
  });
  ui->toolBar->addWidget(object1Replay);

  img = QIcon(":/assets/buttons/replace.png");
  QAction* swapAction = ui->toolBar->addAction(img, tr("&Swap"));
  swapAction->setStatusTip(tr("Swap the two objects"));
  connect(swapAction, &QAction::triggered, this, &Replay::correctTracking);

  object2Replay = new QComboBox(this);
  object2Replay->setEditable(true);
  object2Replay->setInsertPolicy(QComboBox::NoInsert);
  object2Replay->setStatusTip(tr("Second selected object"));
  connect(object2Replay, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
    if (object2Replay->count() != 0) {
      int id = object2Replay->itemText(index).toInt();
      updateInformation(static_cast<int>(id), ui->replaySlider->value(), ui->infoTableObject2);
      object2Replay->setStyleSheet("QComboBox { background-color: rgb(" + QString::number(colorMap[id].x) + "," + QString::number(colorMap[id].y) + "," + QString::number(colorMap[id].z) + "); }");
    }
  });
  ui->toolBar->addWidget(object2Replay);

  img = QIcon(":/assets/buttons/deleteOne.png");
  QAction* deleteOneAction = ui->toolBar->addAction(img, tr("&Delete"));
  deleteOneAction->setShortcut(QKeySequence(QStringLiteral("f")));
  deleteOneAction->setStatusTip(tr("Delete the object on this frame"));
  connect(deleteOneAction, &QAction::triggered, this, [this]() {
    if (isReplayable) {
      DeleteData* del = new DeleteData(object2Replay->currentText().toInt(), ui->replaySlider->value(), ui->replaySlider->value(), trackingData);
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
  deleteAction->setStatusTip(tr("Delete the object from this frame on the selected number of frames"));
  connect(deleteAction, &QAction::triggered, this, [this]() {
    if (isReplayable) {
      DeleteData* del = new DeleteData(object2Replay->currentText().toInt(), ui->replaySlider->value(), ui->replaySlider->value() + deletedFrameNumber->value() - 1, trackingData);
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
  deletedFrameNumber->setStatusTip(tr("Number of frames where to delete the selected object"));
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

  img = QIcon(":/assets/buttons/annotate.png");
  QAction* annotAction = ui->annotationDock->toggleViewAction();
  annotAction->setIcon(img);
  annotAction->setStatusTip(tr("Annotation"));
  ui->toolBar->addAction(annotAction);

  img = QIcon(":/assets/buttons/info.png");
  QAction* optionAction = ui->infoDock->toggleViewAction();
  optionAction->setIcon(img);
  optionAction->setStatusTip(tr("Display Options"));
  ui->toolBar->addAction(optionAction);

  img = QIcon(":/assets/buttons/option.png");
  QAction* infoAction = ui->optionDock->toggleViewAction();
  infoAction->setIcon(img);
  infoAction->setStatusTip(tr("Information"));
  ui->toolBar->addAction(infoAction);

  // Install event filters
  ui->replayDisplay->installEventFilter(this);

  isReplayable = false;
  ui->ellipseBox->addItems({"Head + Tail", "Head", "Tail", "Body", "None"});
  connect(ui->ellipseBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, [this]() {
    loadFrame(currentIndex);
  });
  ui->arrowBox->addItems({"Head + Tail", "Head", "Tail", "Body", "None"});
  connect(ui->arrowBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, [this]() {
    loadFrame(currentIndex);
  });

  connect(ui->replayTrace, &QCheckBox::stateChanged, this, [this]() {
    loadFrame(currentIndex);
  });
  connect(ui->replayTraceLength, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [this]() {
    loadFrame(currentIndex);
  });

  connect(ui->replaySize, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [this]() {
    loadFrame(currentIndex);
  });

  // Info tables
  connect(ui->infoTableObject1, &QTableWidget::cellClicked, this, [this](int row, int col) {
    if (row == 1 && col == 0) {
      ui->replaySlider->setValue(ui->infoTableObject1->item(row, col)->text().toInt());
    }
  });
  connect(ui->infoTableObject2, &QTableWidget::cellClicked, this, [this](int row, int col) {
    if (row == 1 && col == 0) {
      ui->replaySlider->setValue(ui->infoTableObject2->item(row, col)->text().toInt());
    }
  });

  // Annotation object
  annotation = new Annotation();
  // Load annotation file
  connect(annotation, &Annotation::annotationText, ui->annotation, &QTextEdit::setPlainText);
  connect(ui->annotation, &QTextEdit::textChanged, annotation, [this]() {
    int index = ui->replaySlider->currentValue();
    QString text = ui->annotation->toPlainText();
    annotation->write(index, text);
  });
  connect(ui->findLine, &QLineEdit::textEdited, annotation, &Annotation::find);
  connect(ui->findNext, &QPushButton::pressed, annotation, [this]() {
    int index = annotation->next();
    ui->replaySlider->setValue(index);
  });
  connect(ui->findPrev, &QPushButton::pressed, annotation, [this]() {
    int index = annotation->prev();
    ui->replaySlider->setValue(index);
  });

  trackingData = new Data();

  // In not standalone mode, the user need to manually connect the slider to the loadFrame
  // signal to avoid double connections and increase performance.
  if (!standalone) {
    delete ui->controls;
    ui->replaySlider = slider;
    video = videoReader;
  }
  // If standalone create video reader and timeline connections
  else {
    video = new VideoReader();
    connect(ui->replaySlider, &Timeline::valueChanged, this, &Replay::sliderConnection);
  }
}

void Replay::sliderConnection(const int index) {
  if (!ui->replaySlider->isAutoplay && !trackingData->isEmpty) {
    updateInformation(ui->infoTableObject1->item(0, 0)->text().toInt(), index, ui->infoTableObject1);
    updateInformation(ui->infoTableObject2->item(0, 0)->text().toInt(), index, ui->infoTableObject2);
    deletedFrameNumber->setMaximum(maxIndex - index);
    deletedFrameNumber->setValue(maxIndex - index);
    annotation->read(index);
  }
  loadFrame(index);
}

Replay::~Replay() {
  settingsFile->setValue(QStringLiteral("windowState"), saveState());
  delete ui;
  delete trackingData;
  delete annotation;
  if (isStandalone) {
    delete video;
  }
}

/**
 * @brief Opens a dialogue to select a folder.
 */
void Replay::openReplay() {
  QString dir = QFileDialog::getOpenFileName(this, tr("Open video file or image from an image sequence"), memoryDir);
  QApplication::setOverrideCursor(Qt::WaitCursor);
  loadReplay(dir);
  QApplication::restoreOverrideCursor();
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

  ui->replaySlider->setValue(0);
  commandStack->clear();
  occlusionEvents.clear();
  object1Replay->clear();
  object2Replay->clear();
  ui->replayDisplay->clear();
  ui->annotation->clear();
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
    if (isStandalone) {
      video->open(dir.toStdString());
    }
    ui->replaySlider->setMinimum(0);
    ui->replaySlider->setMaximum(static_cast<int>(video->getImageCount()) - 1);
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

    // Block the signal to not overwrite the first annonation at ui setup
    ui->annotation->blockSignals(true);
    ui->replaySlider->setValue(1);  // To force the change
    ui->replaySlider->setValue(0);
    ui->replayDisplay->fitToView();
    ui->annotation->blockSignals(false);
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
      int scale = ui->replaySize->value();
      QList<QHash<QString, double>> dataImage = trackingData->getData(frameIndex);
      for (const QHash<QString, double>& coordinate : dataImage) {
        int id = static_cast<int>(coordinate.value(QStringLiteral("id")));

        object1Replay->addItem(QString::number(id));

        if (ui->ellipseBox->currentIndex() != 4) {
          switch (ui->ellipseBox->currentIndex()) {
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

        if (ui->arrowBox->currentIndex() != 4) {
          switch (ui->arrowBox->currentIndex()) {
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

        if (ui->replayNumbers->isChecked()) {
          cv::putText(frame, to_string(id), Point(static_cast<int>(coordinate.value(QStringLiteral("xHead")) + coordinate.value(QStringLiteral("headMajorAxisLength")) * cos(coordinate.value(QStringLiteral("tHead")))), static_cast<int>(coordinate.value(QStringLiteral("yHead")) - coordinate.value(QStringLiteral("headMajorAxisLength")) * sin(coordinate.value(QStringLiteral("tHead"))))), cv::FONT_HERSHEY_SIMPLEX, double(scale) * 0.5, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_8);
        }

        if (ui->replayTrace->isChecked()) {
          vector<Point> memory;
          QList<QHash<QString, double>> coordinate = trackingData->getData(frameIndex - ui->replayTraceLength->value(), frameIndex + 1, id);
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
        int frameIndex = ui->replaySlider->value();
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
void Replay::updateInformation(int objectId, int imageIndex, QTableWidget* table) {
  QHash<QString, double> infoData = trackingData->getData(imageIndex, objectId);
  table->item(0, 0)->setText(QString::number(objectId));
  table->item(1, 0)->setText(QString::number(trackingData->getObjectInformation(objectId)));
  table->item(2, 0)->setText(QString::number(infoData.value(QStringLiteral("areaBody"))));
  table->item(3, 0)->setText(QString::number(infoData.value(QStringLiteral("perimeterBody"))));
  table->item(4, 0)->setText(QString::number(infoData.value(QStringLiteral("bodyExcentricity"))));
}

/**
 * @brief Gets the index of the two selected objects, the start index, swaps the data from the start index to the end, and saves the new tracking data. Triggered when ui->swapButton is pressed or a right-click event is registered inside the replayDisplay.
 */
void Replay::correctTracking() {
  if (isReplayable) {
    // Swaps the data
    int firstObject = object1Replay->currentText().toInt();
    int secondObject = object2Replay->currentText().toInt();
    int start = ui->replaySlider->value();
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
    int current = ui->replaySlider->value();
    int nextOcclusion = *std::upper_bound(occlusionEvents.begin(), occlusionEvents.end(), current);
    ui->replaySlider->setValue(nextOcclusion);
  }
}

/**
 * @brief Finds and displays the previous occlusion event on the ui->replayDisplay. Triggered when ui->previousReplay is pressed.
 */
void Replay::previousOcclusionEvent() {
  if (!occlusionEvents.isEmpty()) {
    int current = ui->replaySlider->value();
    int previousOcclusion = occlusionEvents.at(static_cast<int>(std::upper_bound(occlusionEvents.begin(), occlusionEvents.end(), current) - occlusionEvents.begin() - 2));
    ui->replaySlider->setValue(previousOcclusion);
  }
}

/**
 * @brief Saves the tracked movie in .avi. Triggered when ui->previousReplay is pressed.
 */
void Replay::saveTrackedMovie() {
  // If tracking data are available, gets the display settings and saves the movie in the
  // selected folder
  if (isReplayable) {
    QString savePath = QFileDialog::getSaveFileName(this, tr("Save File"), QStringLiteral("/home/save.avi"), tr("Videos (*.avi)"));
    cv::VideoWriter outputVideo(savePath.toStdString(), cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), ui->replayFps->value(), Size(originalImageSize.width(), originalImageSize.height()));
    int scale = ui->replaySize->value();

    for (int frameIndex = 0; frameIndex < static_cast<int>(video->getImageCount()); frameIndex++) {
      Mat frame;
      video->getImage(frameIndex, frame);
      cvtColor(frame, frame, COLOR_GRAY2BGR);
      // Takes the tracking data corresponding to the replayed frame and parse data to display
      // arrows on tracked objects.
      // Takes the tracking data corresponding to the replayed frame and parse data to display
      QList<int> idList = trackingData->getId(frameIndex);
      for (auto const& a : idList) {
        QHash<QString, double> coordinate = trackingData->getData(frameIndex, a);
        int id = a;

        object1Replay->addItem(QString::number(id));
        object2Replay->addItem(QString::number(id));

        if (ui->ellipseBox->currentIndex() != 4) {
          switch (ui->ellipseBox->currentIndex()) {
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

        if (ui->arrowBox->currentIndex() != 4) {
          switch (ui->arrowBox->currentIndex()) {
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

        if (ui->replayNumbers->isChecked()) {
          cv::putText(frame, to_string(id), Point(static_cast<int>(coordinate.value(QStringLiteral("xHead")) + coordinate.value(QStringLiteral("headMajorAxisLength")) * cos(coordinate.value(QStringLiteral("tHead")))), static_cast<int>(coordinate.value(QStringLiteral("yHead")) - coordinate.value(QStringLiteral("headMajorAxisLength")) * sin(coordinate.value(QStringLiteral("tHead"))))), cv::FONT_HERSHEY_SIMPLEX, double(scale) * 0.5, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA);
        }

        if (ui->replayTrace->isChecked()) {
          vector<Point> memory;
          for (int j = frameIndex - ui->replayTraceLength->value(); j < frameIndex; j++) {
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
      ui->replaySlider->setValue(static_cast<int>(frameIndex));
    }
    outputVideo.release();
  }
}
