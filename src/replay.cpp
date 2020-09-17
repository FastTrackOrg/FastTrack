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

Replay::Replay(QWidget* parent, bool standalone, Timeline* control) : QMainWindow(parent),
                                                                      ui(new Ui::Replay) {
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

  QIcon img = QIcon(":/assets/buttons/play.png");
  ui->playReplay->setIcon(img);
  ui->playReplay->setIconSize(QSize(ui->playReplay->width(), ui->playReplay->height()));

  img = QIcon(":/assets/buttons/open.png");
  QAction* openAction = new QAction(img, tr("&Open"), this);
  openAction->setShortcuts(QKeySequence::Open);
  openAction->setStatusTip(tr("Open a tracked movie"));
  connect(openAction, &QAction::triggered, this, &Replay::openReplayFolder);
  ui->toolBar->addAction(openAction);

  img = QIcon(":/assets/buttons/refresh.png");
  QAction* refreshAction = new QAction(img, tr("&Refresh"), this);
  refreshAction->setStatusTip(tr("Refresh a tracked movie"));
  connect(refreshAction, &QAction::triggered, [this]() {
    loadReplayFolder(memoryDir);
  });
  ui->toolBar->addAction(refreshAction);

  img = QIcon(":/assets/buttons/save.png");
  QAction* exportAction = new QAction(img, tr("&Export"), this);
  exportAction->setStatusTip(tr("Export the tracked movie"));
  connect(exportAction, &QAction::triggered, this, &Replay::saveTrackedMovie);
  ui->toolBar->addAction(exportAction);

  commandStack = new QUndoStack(this);
  img = QIcon(":/assets/buttons/undo.png");
  QAction* undoAction = commandStack->createUndoAction(this, tr("&Undo"));
  undoAction->setIcon(img);
  undoAction->setShortcuts(QKeySequence::Undo);
  undoAction->setStatusTip(tr("Undo"));
  connect(undoAction, &QAction::triggered, [this]() {
    object2Replay->clear();
    ids = trackingData->getId(0, video->getImageCount());
    std::sort(ids.begin(), ids.end());
    for (auto const& a : ids) {
      object2Replay->addItem(QString::number(a));
    }
    loadFrame(ui->replaySlider->value());
  });
  ui->toolBar->addAction(undoAction);

  img = QIcon(":/assets/buttons/redo.png");
  QAction* redoAction = commandStack->createRedoAction(this, tr("&Redo"));
  redoAction->setIcon(img);
  redoAction->setShortcuts(QKeySequence::Redo);
  redoAction->setStatusTip(tr("Redo"));
  connect(redoAction, &QAction::triggered, [this]() {
    object2Replay->clear();
    ids = trackingData->getId(0, video->getImageCount());
    std::sort(ids.begin(), ids.end());
    for (auto const& a : ids) {
      object2Replay->addItem(QString::number(a));
    }
    loadFrame(ui->replaySlider->value());
  });
  ui->toolBar->addAction(redoAction);

  ui->toolBar->addSeparator();

  object1Replay = new QComboBox(this);
  object1Replay->setEditable(true);
  object1Replay->setStatusTip(tr("First selected object"));
  connect(object1Replay, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
    if (object1Replay->count() != 0) {
      int id = object1Replay->itemText(index).toInt();
      updateInformation(static_cast<int>(id), ui->replaySlider->value(), ui->infoTableObject1);
      object1Replay->setStyleSheet("QComboBox { background-color: rgb(" + QString::number(colorMap[id].x) + "," + QString::number(colorMap[id].y) + "," + QString::number(colorMap[id].z) + "); }");
    }
  });
  ui->toolBar->addWidget(object1Replay);

  img = QIcon(":/assets/buttons/replace.png");
  QAction* swapAction = new QAction(img, tr("&Swap"), this);
  swapAction->setStatusTip(tr("Swap the two objects"));
  connect(swapAction, &QAction::triggered, this, &Replay::correctTracking);
  ui->toolBar->addAction(swapAction);

  object2Replay = new QComboBox(this);
  object2Replay->setEditable(true);
  object2Replay->setStatusTip(tr("Second selected object"));
  connect(object2Replay, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
    if (object2Replay->count() != 0) {
      int id = object2Replay->itemText(index).toInt();
      updateInformation(static_cast<int>(id), ui->replaySlider->value(), ui->infoTableObject2);
      object2Replay->setStyleSheet("QComboBox { background-color: rgb(" + QString::number(colorMap[id].x) + "," + QString::number(colorMap[id].y) + "," + QString::number(colorMap[id].z) + "); }");
    }
  });
  ui->toolBar->addWidget(object2Replay);

  img = QIcon(":/assets/buttons/deleteOne.png");
  QAction* deleteOneAction = new QAction(img, tr("&Delete"), this);
  deleteOneAction->setShortcut(QKeySequence("f"));
  deleteOneAction->setStatusTip(tr("Delete the object on this frame"));
  connect(deleteOneAction, &QAction::triggered, [this]() {
    if (isReplayable) {
      DeleteData* del = new DeleteData(object2Replay->currentText().toInt(), ui->replaySlider->value(), ui->replaySlider->value(), trackingData);
      commandStack->push(del);
      ids = trackingData->getId(0, video->getImageCount());
      object2Replay->clear();
      for (auto const& a : ids) {
        object2Replay->addItem(QString::number(a));
      }
      loadFrame(ui->replaySlider->value());
    }
  });
  ui->toolBar->addAction(deleteOneAction);

  img = QIcon(":/assets/buttons/delete.png");
  QAction* deleteAction = new QAction(img, tr("&Delete"), this);
  deleteAction->setShortcut(QKeySequence(tr("G")));
  deleteAction->setStatusTip(tr("Delete the object from this frame on the selected number of frames"));
  connect(deleteAction, &QAction::triggered, [this]() {
    if (isReplayable) {
      DeleteData* del = new DeleteData(object2Replay->currentText().toInt(), ui->replaySlider->value(), ui->replaySlider->value() + deletedFrameNumber->value() - 1, trackingData);
      commandStack->push(del);
      ids = trackingData->getId(0, video->getImageCount());
      object2Replay->clear();
      for (auto const& a : ids) {
        object2Replay->addItem(QString::number(a));
      }
      loadFrame(ui->replaySlider->value());
    }
  });
  ui->toolBar->addAction(deleteAction);

  deletedFrameNumber = new QSpinBox(this);
  deletedFrameNumber->setStatusTip(tr("Number of frames where to delete the selected object"));
  connect(ui->replaySlider, &Timeline::valueChanged, [this]() {
    deletedFrameNumber->setMaximum(video->getImageCount() - ui->replaySlider->value());
    deletedFrameNumber->setValue(video->getImageCount() - ui->replaySlider->value());
  });
  deletedFrameFocus = new QShortcut(QKeySequence("c"), this);
  connect(deletedFrameFocus, &QShortcut::activated, deletedFrameNumber, static_cast<void (QSpinBox::*)(void)>(&QSpinBox::setFocus));
  connect(deletedFrameFocus, &QShortcut::activated, deletedFrameNumber, &QSpinBox::selectAll);

  ui->toolBar->addWidget(deletedFrameNumber);

  img = QIcon(":/assets/buttons/previous.png");
  QAction* previousAction = new QAction(img, tr("&Previous"), this);
  previousAction->setStatusTip(tr("Previous occlusion"));
  connect(previousAction, &QAction::triggered, this, &Replay::previousOcclusionEvent);
  ui->toolBar->addAction(previousAction);

  img = QIcon(":/assets/buttons/next.png");
  QAction* nextAction = new QAction(img, tr("&Next"), this);
  nextAction->setStatusTip(tr("Next occlusion"));
  connect(nextAction, &QAction::triggered, this, &Replay::nextOcclusionEvent);
  ui->toolBar->addAction(nextAction);

  img = QIcon(":/assets/buttons/help.png");
  QAction* helpAction = new QAction(img, tr("&Help"), this);
  helpAction->setStatusTip(tr("Help"));
  connect(helpAction, &QAction::triggered, [this]() {
    QMessageBox helpBox(this);
    helpBox.setIconPixmap(QPixmap(":/assets/buttons/helpImg.png"));
    helpBox.exec();
  });
  ui->toolBar->addAction(helpAction);

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

  if (!standalone) {
    ui->controls->hide();
  }

  if (control) {
    ui->replaySlider = control;
  }

  // Keyboard shorcut
  // AZERTY keyboard shorcuts are set in the ui
  //QShortcut* wShortcut = new QShortcut(QKeySequence("w"), this);
  //connect(wShortcut, &QShortcut::activated, [this](nextAction) { nextAction->triggered(); });

  QShortcut* qShortcut = new QShortcut(QKeySequence("q"), this);
  connect(qShortcut, &QShortcut::activated, [this]() { ui->replaySlider->setValue(ui->replaySlider->value() - 1); });

  QShortcut* aShortcut = new QShortcut(QKeySequence("a"), this);
  connect(aShortcut, &QShortcut::activated, [this]() { ui->replaySlider->setValue(ui->replaySlider->value() - 1); });

  QShortcut* dShortcut = new QShortcut(QKeySequence("d"), this);
  connect(dShortcut, &QShortcut::activated, [this]() { ui->replaySlider->setValue(ui->replaySlider->value() + 1); });

  ui->replayDisplay->installEventFilter(this);
  ui->scrollArea->viewport()->installEventFilter(this);

  // Zoom
  connect(ui->scrollArea->verticalScrollBar(), &QScrollBar::rangeChanged, [this]() {
    QScrollBar* vertical = ui->scrollArea->verticalScrollBar();
    if (currentZoom > 0) {
      vertical->setValue(int(zoomReferencePosition.y() * 0.25 + vertical->value() * 1.25));
    }
    else {
      vertical->setValue(int(-zoomReferencePosition.y() * 0.25 + vertical->value() / 1.25));
    }
  });
  connect(ui->scrollArea->horizontalScrollBar(), &QScrollBar::rangeChanged, [this]() {
    QScrollBar* horizontal = ui->scrollArea->horizontalScrollBar();
    if (currentZoom > 0) {
      horizontal->setValue(int(zoomReferencePosition.x() * 0.25 + horizontal->value() * 1.25));
    }
    else {
      horizontal->setValue(int(-zoomReferencePosition.x() * 0.25 + horizontal->value() / 1.25));
    }
  });

  isReplayable = false;
  framerate = new QTimer();
  ui->ellipseBox->addItems({"Head + Tail", "Head", "Tail", "Body", "None"});
  connect(ui->ellipseBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), [this]() {
    loadFrame(ui->replaySlider->value());
  });
  ui->arrowBox->addItems({"Head + Tail", "Head", "Tail", "Body", "None"});
  connect(ui->arrowBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), [this]() {
    loadFrame(ui->replaySlider->value());
  });

  connect(ui->replayTrace, &QCheckBox::stateChanged, [this]() {
    loadFrame(ui->replaySlider->value());
  });
  connect(ui->replayTraceLength, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this]() {
    loadFrame(ui->replaySlider->value());
  });

  connect(ui->replaySize, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this]() {
    loadFrame(ui->replaySlider->value());
  });

  connect(ui->replaySlider, &Timeline::valueChanged, this, &Replay::loadFrame);

  connect(framerate, &QTimer::timeout, [this]() {
    ui->replaySlider->setValue(autoPlayerIndex);
    autoPlayerIndex++;
    if (autoPlayerIndex % int(video->getImageCount()) != autoPlayerIndex) {
      autoPlayerIndex = 0;
    }
  });
  connect(ui->replayFps, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this]() {
    if (isReplayable && ui->playReplay->isChecked()) {
      framerate->stop();
      framerate->start(1000 / ui->replayFps->value());
    }
  });
  connect(ui->playReplay, &QPushButton::clicked, this, &Replay::toggleReplayPlay);

  // Info tables
  connect(ui->infoTableObject1, &QTableWidget::cellClicked, [this](int row, int col) {
    if (row == 1 && col == 0) {
      ui->replaySlider->setValue(ui->infoTableObject1->item(row, col)->text().toInt());
    }
  });
  connect(ui->infoTableObject2, &QTableWidget::cellClicked, [this](int row, int col) {
    if (row == 1 && col == 0) {
      ui->replaySlider->setValue(ui->infoTableObject2->item(row, col)->text().toInt());
    }
  });

  annotation = new Annotation("");
  trackingData = new Data("");
  video = new VideoReader("");
}

Replay::~Replay() {
  delete ui;
}

/**
  * @brief Opens a dialogue to select a folder.
*/
void Replay::openReplayFolder() {
  QString dir = QFileDialog::getOpenFileName(this, tr("Open Directory"), memoryDir);
  loadReplayFolder(dir);
}

/**
  * @brief Loads a folder containing an image sequence and the tracking data if it exists. Triggerred when ui->pathButton is pressed.
  * @arg[in] dir Path to the folder where the image sequence is stored.
*/
void Replay::loadReplayFolder(QString dir) {
  // This function will detect from an inputed path to a directory the image sequence and the tracking data.
  // The last tracking data from the folder Tracking_Result is automatically loaded if found.
  // If the user explicitly select another Tracking_Result folder, these data are loaded.
  // Delete existing data

  if (!dir.length()) return;

  ui->replaySlider->setValue(0);
  commandStack->clear();
  occlusionEvents.clear();
  object1Replay->clear();
  object2Replay->clear();
  ui->replayDisplay->clear();
  ui->annotation->clear();
  framerate->stop();
  object = true;
  currentZoom = 1;
  memoryDir.clear();

  memoryDir = dir;

  try {
    // Gets the paths to all the frames in the folder and puts it in a vector.
    // Setups the ui by setting maximum and minimum of the slider bar.
    delete annotation;
    delete trackingData;
    delete video;
    video = new VideoReader(dir.toStdString());
    ui->replaySlider->setMinimum(0);
    ui->replaySlider->setMaximum(video->getImageCount() - 1);
    //Mat frame = imread(replayFrames[0], IMREAD_COLOR | IMREAD_ANYDEPTH);
    Mat frame;
    video->getImage(0, frame);
    cvtColor(frame, frame, COLOR_GRAY2RGB);
    originalImageSize.setWidth(frame.cols);
    originalImageSize.setHeight(frame.rows);
    deletedFrameNumber->setRange(1, video->getImageCount());
    deletedFrameNumber->setValue(video->getImageCount());
    if (video->isOpened()) {
      isReplayable = true;
    }

    QFileInfo savingInfo(dir);
    QString savingFilename = savingInfo.baseName();
    QString savingPath = savingInfo.absolutePath();
    QString trackingDir = savingPath;
    if (video->isSequence()) {
      trackingDir.append(QString("/Tracking_Result") + QDir::separator());
    }
    else {
      trackingDir.append(QString("/Tracking_Result_") + savingFilename + QDir::separator());
    }
    trackingData = new Data(trackingDir);
    ids = trackingData->getId(0, video->getImageCount());
    for (auto const& a : ids) {
      object2Replay->addItem(QString::number(a));
    }

    // Load annotation file
    annotation = new Annotation(trackingDir);
    connect(ui->replaySlider, &Timeline::valueChanged, annotation, &Annotation::read);
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

    // Information
    connect(ui->replaySlider, &Timeline::valueChanged, [this](int index) {
      updateInformation(ui->infoTableObject1->item(0, 0)->text().toInt(), index, ui->infoTableObject1);
      updateInformation(ui->infoTableObject2->item(0, 0)->text().toInt(), index, ui->infoTableObject2);
    });

    ui->replaySlider->setValue(1);  // To force the change
    ui->replaySlider->setValue(0);
  }
  catch (...) {
    isReplayable = false;
  }
}

/**
  * @brief Displays the image and the tracking data in the ui->displayReplay. Triggered when the ui->replaySlider value is changed.
*/
void Replay::loadFrame(int frameIndex) {
  if (isReplayable) {
    object1Replay->clear();

    //Mat frame = imread(replayFrames[frameIndex], IMREAD_COLOR | IMREAD_ANYDEPTH);
    Mat frame;
    video->getImage(frameIndex, frame);
    cvtColor(frame, frame, COLOR_GRAY2BGR);
    int scale = ui->replaySize->value();

    // Takes the tracking data corresponding to the replayed frame and parse data to display
    QList<int> idList = trackingData->getId(frameIndex);
    std::sort(idList.begin(), idList.end());
    for (auto const& a : idList) {
      QMap<QString, double> coordinate = trackingData->getData(frameIndex, a);
      int id = a;

      object1Replay->addItem(QString::number(id));

      if (ui->ellipseBox->currentIndex() != 4) {
        switch (ui->ellipseBox->currentIndex()) {
          case 0:  // Head + Tail
            cv::ellipse(frame, Point(static_cast<int>(coordinate.value("xHead")), static_cast<int>(coordinate.value("yHead"))), Size(static_cast<int>(coordinate.value("headMajorAxisLength")), static_cast<int>(coordinate.value("headMinorAxisLength"))), 180 - (coordinate.value("tHead") * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, 8);
            cv::ellipse(frame, Point(static_cast<int>(coordinate.value("xTail")), static_cast<int>(coordinate.value("yTail"))), Size(static_cast<int>(coordinate.value("tailMajorAxisLength")), static_cast<int>(coordinate.value("tailMinorAxisLength"))), 180 - (coordinate.value("tTail") * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA);
            break;

          case 1:  // Head
            cv::ellipse(frame, Point(static_cast<int>(coordinate.value("xHead")), static_cast<int>(coordinate.value("yHead"))), Size(static_cast<int>(coordinate.value("headMajorAxisLength")), static_cast<int>(coordinate.value("headMinorAxisLength"))), 180 - (coordinate.value("tHead") * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, 8);
            break;

          case 2:  // Tail
            cv::ellipse(frame, Point(static_cast<int>(coordinate.value("xTail")), static_cast<int>(coordinate.value("yTail"))), Size(static_cast<int>(coordinate.value("tailMajorAxisLength")), static_cast<int>(coordinate.value("tailMinorAxisLength"))), 180 - (coordinate.value("tTail") * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA);
            break;

          case 3:  // Body
            cv::ellipse(frame, Point(static_cast<int>(coordinate.value("xBody")), static_cast<int>(coordinate.value("yBody"))), Size(static_cast<int>(coordinate.value("bodyMajorAxisLength")), static_cast<int>(coordinate.value("bodyMinorAxisLength"))), 180 - (coordinate.value("tBody") * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, 8);
            break;
        }
      }

      if (ui->arrowBox->currentIndex() != 4) {
        switch (ui->arrowBox->currentIndex()) {
          case 0:
            cv::arrowedLine(frame, Point(static_cast<int>(coordinate.value("xHead")), static_cast<int>(coordinate.value("yHead"))), Point(static_cast<int>(coordinate.value("xHead") + coordinate.value("headMajorAxisLength") * cos(coordinate.value("tHead"))), static_cast<int>(coordinate.value("yHead") - coordinate.value("headMajorAxisLength") * sin(coordinate.value("tHead")))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA, 0, double(scale) / 10);
            cv::arrowedLine(frame, Point(static_cast<int>(coordinate.value("xTail")), static_cast<int>(coordinate.value("yTail"))), Point(static_cast<int>(coordinate.value("xTail") + coordinate.value("tailMajorAxisLength") * cos(coordinate.value("tTail"))), static_cast<int>(coordinate.value("yTail") - coordinate.value("tailMajorAxisLength") * sin(coordinate.value("tTail")))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA, 0, double(scale) / 10);
            break;

          case 1:
            cv::arrowedLine(frame, Point(static_cast<int>(coordinate.value("xHead")), static_cast<int>(coordinate.value("yHead"))), Point(static_cast<int>(coordinate.value("xHead") + coordinate.value("headMajorAxisLength") * cos(coordinate.value("tHead"))), static_cast<int>(coordinate.value("yHead") - coordinate.value("headMajorAxisLength") * sin(coordinate.value("tHead")))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA, 0, double(scale) / 10);
            break;

          case 2:
            cv::arrowedLine(frame, Point(static_cast<int>(coordinate.value("xTail")), static_cast<int>(coordinate.value("yTail"))), Point(static_cast<int>(coordinate.value("xTail") + coordinate.value("tailMajorAxisLength") * cos(coordinate.value("tTail"))), static_cast<int>(coordinate.value("yTail") - coordinate.value("tailMajorAxisLength") * sin(coordinate.value("tTail")))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA, 0, double(scale) / 10);
            break;

          case 3:
            cv::arrowedLine(frame, Point(static_cast<int>(coordinate.value("xBody")), static_cast<int>(coordinate.value("yBody"))), Point(static_cast<int>(coordinate.value("xBody") + coordinate.value("bodyMajorAxisLength") * cos(coordinate.value("tBody"))), static_cast<int>(coordinate.value("yBody") - coordinate.value("bodyMajorAxisLength") * sin(coordinate.value("tBody")))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA, 0, double(scale) / 10);
            break;
        }
      }

      if (ui->replayNumbers->isChecked()) {
        cv::putText(frame, to_string(id), Point(static_cast<int>(coordinate.value("xHead") + coordinate.value("headMajorAxisLength") * cos(coordinate.value("tHead"))), static_cast<int>(coordinate.value("yHead") - coordinate.value("headMajorAxisLength") * sin(coordinate.value("tHead")))), cv::FONT_HERSHEY_SIMPLEX, double(scale) * 0.5, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA);
      }

      if (ui->replayTrace->isChecked()) {
        vector<Point> memory;
        for (int j = frameIndex - ui->replayTraceLength->value(); j < frameIndex; j++) {
          if (j > 0) {
            QMap<QString, double> coordinate = trackingData->getData(j, a);
            if (coordinate.contains("xBody")) {
              memory.push_back(Point(static_cast<int>(coordinate.value("xBody")), static_cast<int>(coordinate.value("yBody"))));
            }
          }
        }
        cv::polylines(frame, memory, false, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA);
      }
    }

    int w = ui->replayDisplay->width();
    int h = ui->replayDisplay->height();
    QPixmap resizedPix = (QPixmap::fromImage(QImage(frame.data, frame.cols, frame.rows, static_cast<int>(frame.step), QImage::Format_RGB888)).scaled(w, h, Qt::KeepAspectRatio));
    ui->replayDisplay->setPixmap(resizedPix);
    resizedFrame.setWidth(resizedPix.width());
    resizedFrame.setHeight(resizedPix.height());
  }
}

/**
  * @brief Zooms in the display.
*/
void Replay::zoomIn() {
  currentZoom = abs(currentZoom);
  if (currentZoom < 2.75) {
    currentZoom += 0.25;
    ui->replayDisplay->setFixedSize(ui->replayDisplay->size() * 1.25);
    loadFrame(ui->replaySlider->value());
  }
}

/**
  * @brief Zooms out the display.
*/
void Replay::zoomOut() {
  currentZoom = abs(currentZoom);
  if (currentZoom > 0.75) {
    currentZoom -= 0.25;
    ui->replayDisplay->setFixedSize(ui->replayDisplay->size() / 1.25);
    loadFrame(ui->replaySlider->value());
  }
  currentZoom *= -1;
}

/**
  * @brief Starts the autoplay of the replay. Triggered when ui->playreplay is clicked.
*/
void Replay::toggleReplayPlay() {
  if (isReplayable && ui->playReplay->isChecked()) {
    QIcon img(":/assets/buttons/pause.png");
    ui->playReplay->setIcon(img);
    framerate->start(1000 / ui->replayFps->value());
    autoPlayerIndex = ui->replaySlider->value();
  }
  else if (isReplayable && !ui->playReplay->isChecked()) {
    QIcon img(":/assets/buttons/resume.png");
    ui->playReplay->setIcon(img);
    framerate->stop();
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
      QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

      // Left click to select an object
      if (mouseEvent->buttons() == Qt::LeftButton && isReplayable) {
        // Finds user click coordinate
        double xTop = ((double(mouseEvent->pos().x()) - 0.5 * (ui->replayDisplay->width() - resizedFrame.width())) * double(originalImageSize.width())) / double(resizedFrame.width());
        double yTop = ((double(mouseEvent->pos().y()) - 0.5 * (ui->replayDisplay->height() - resizedFrame.height())) * double(originalImageSize.height())) / double(resizedFrame.height());

        // Finds the id of the closest object
        int frameIndex = ui->replaySlider->value();
        QList<int> idList = trackingData->getId(frameIndex);

        if (!idList.isEmpty()) {
          QVector<double> distance;
          for (auto const& a : idList) {
            QMap<QString, double> coordinate = trackingData->getData(frameIndex, a);
            distance.append(pow(coordinate.value("xBody") - xTop, 2) + pow(coordinate.value("yBody") - yTop, 2));
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
        object1Replay->setStyleSheet("QComboBox { background-color: white; }");
        object2Replay->setStyleSheet("QComboBox { background-color: white; }");
      }
    }

    // Hover screen
    if (event->type() == QEvent::HoverMove) {
      QHoverEvent* hoverEvent = static_cast<QHoverEvent*>(event);
      int xPix = static_cast<int>(((double(hoverEvent->pos().x()) - 0.5 * (ui->replayDisplay->width() - resizedFrame.width())) * double(originalImageSize.width())) / double(resizedFrame.width()));
      int yPix = static_cast<int>(((double(hoverEvent->pos().y()) - 0.5 * (ui->replayDisplay->height() - resizedFrame.height())) * double(originalImageSize.height())) / double(resizedFrame.height()));
      if (xPix > 0 && yPix > 0 && xPix < originalImageSize.width() && yPix < originalImageSize.height()) {
        this->statusBar()->showMessage("x: " + QString::number(xPix) + "\ty: " + QString::number(yPix));
      }
    }
  }

  // Scroll Area event filter
  if (target == ui->scrollArea->viewport()) {
    // Moves in the image by middle click
    if (event->type() == QEvent::MouseMove) {
      QMouseEvent* moveEvent = static_cast<QMouseEvent*>(event);
      if (moveEvent->buttons() == Qt::MiddleButton) {
        ui->scrollArea->horizontalScrollBar()->setValue(static_cast<int>(ui->scrollArea->horizontalScrollBar()->value() + (panReferenceClick.x() - moveEvent->localPos().x())));
        ui->scrollArea->verticalScrollBar()->setValue(static_cast<int>(ui->scrollArea->verticalScrollBar()->value() + (panReferenceClick.y() - moveEvent->localPos().y())));
        panReferenceClick = moveEvent->localPos();
      }
    }
    if (event->type() == QEvent::Wheel) {
      QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
      zoomReferencePosition = wheelEvent->pos();
    }

    // Zoom/unzoom the display by wheel
    if (event->type() == QEvent::Wheel) {
      QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
      if (wheelEvent->angleDelta().y() > 0) {
        zoomIn();
      }
      else {
        zoomOut();
      }
      return true;
    }
    if (event->type() == QEvent::MouseButtonPress) {
      QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
      if (mouseEvent->buttons() == Qt::MiddleButton) {
        qApp->setOverrideCursor(Qt::ClosedHandCursor);
        panReferenceClick = mouseEvent->localPos();
      }
    }
    if (event->type() == QEvent::MouseButtonRelease) {
      qApp->restoreOverrideCursor();
    }
  }
  return false;
}

/**
  * @brief Update the information of an object inside a table widget.
  * @param[in] objectId The id of the object to display the data.
  * @param[in] imageIndex The index of the image where to extracts the data.
  * @param[in] table Pointer to a QTableWidget where to display the data.
*/
void Replay::updateInformation(int objectId, int imageIndex, QTableWidget* table) {
  QMap<QString, double> infoData = trackingData->getData(imageIndex, objectId);
  table->item(0, 0)->setText(QString::number(objectId));
  table->item(1, 0)->setText(QString::number(trackingData->getObjectInformation(objectId)));
  table->item(2, 0)->setText(QString::number(infoData.value("areaBody")));
  table->item(3, 0)->setText(QString::number(infoData.value("perimeterBody")));
  table->item(4, 0)->setText(QString::number(infoData.value("bodyExcentricity")));
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
    loadFrame(ui->replaySlider->value());
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
    QString savePath = QFileDialog::getSaveFileName(this, tr("Save File"), "/home/save.avi", tr("Videos (*.avi)"));
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
        QMap<QString, double> coordinate = trackingData->getData(frameIndex, a);
        int id = a;

        object1Replay->addItem(QString::number(id));
        object2Replay->addItem(QString::number(id));

        if (ui->ellipseBox->currentIndex() != 4) {
          switch (ui->ellipseBox->currentIndex()) {
            case 0:  // Head + Tail
              cv::ellipse(frame, Point(static_cast<int>(coordinate.value("xHead")), static_cast<int>(coordinate.value("yHead"))), Size(static_cast<int>(coordinate.value("headMajorAxisLength")), static_cast<int>(coordinate.value("headMinorAxisLength"))), 180 - (coordinate.value("tHead") * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, 8);
              cv::ellipse(frame, Point(static_cast<int>(coordinate.value("xTail")), static_cast<int>(coordinate.value("yTail"))), Size(static_cast<int>(coordinate.value("tailMajorAxisLength")), static_cast<int>(coordinate.value("tailMinorAxisLength"))), 180 - (coordinate.value("tTail") * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA);
              break;

            case 1:  // Head
              cv::ellipse(frame, Point(static_cast<int>(coordinate.value("xHead")), static_cast<int>(coordinate.value("yHead"))), Size(static_cast<int>(coordinate.value("headMajorAxisLength")), static_cast<int>(coordinate.value("headMinorAxisLength"))), 180 - (coordinate.value("tHead") * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, 8);
              break;

            case 2:  // Tail
              cv::ellipse(frame, Point(static_cast<int>(coordinate.value("xTail")), static_cast<int>(coordinate.value("yTail"))), Size(static_cast<int>(coordinate.value("tailMajorAxisLength")), static_cast<int>(coordinate.value("tailMinorAxisLength"))), 180 - (coordinate.value("tTail") * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA);
              break;

            case 3:  // Body
              cv::ellipse(frame, Point(static_cast<int>(coordinate.value("xBody")), static_cast<int>(coordinate.value("yBody"))), Size(static_cast<int>(coordinate.value("bodyMajorAxisLength")), static_cast<int>(coordinate.value("bodyMinorAxisLength"))), 180 - (coordinate.value("tBody") * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, 8);
              break;
          }
        }

        if (ui->arrowBox->currentIndex() != 4) {
          switch (ui->arrowBox->currentIndex()) {
            case 0:
              cv::arrowedLine(frame, Point(static_cast<int>(coordinate.value("xHead")), static_cast<int>(coordinate.value("yHead"))), Point(static_cast<int>(coordinate.value("xHead") + coordinate.value("headMajorAxisLength") * cos(coordinate.value("tHead"))), static_cast<int>(coordinate.value("yHead") - coordinate.value("headMajorAxisLength") * sin(coordinate.value("tHead")))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA, 0, double(scale) / 10);
              cv::arrowedLine(frame, Point(static_cast<int>(coordinate.value("xTail")), static_cast<int>(coordinate.value("yTail"))), Point(static_cast<int>(coordinate.value("xTail") + coordinate.value("tailMajorAxisLength") * cos(coordinate.value("tTail"))), static_cast<int>(coordinate.value("yTail") - coordinate.value("tailMajorAxisLength") * sin(coordinate.value("tTail")))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA, 0, double(scale) / 10);
              break;

            case 1:
              cv::arrowedLine(frame, Point(static_cast<int>(coordinate.value("xHead")), static_cast<int>(coordinate.value("yHead"))), Point(static_cast<int>(coordinate.value("xHead") + coordinate.value("headMajorAxisLength") * cos(coordinate.value("tHead"))), static_cast<int>(coordinate.value("yHead") - coordinate.value("headMajorAxisLength") * sin(coordinate.value("tHead")))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA, 0, double(scale) / 10);
              break;

            case 2:
              cv::arrowedLine(frame, Point(static_cast<int>(coordinate.value("xTail")), static_cast<int>(coordinate.value("yTail"))), Point(static_cast<int>(coordinate.value("xTail") + coordinate.value("tailMajorAxisLength") * cos(coordinate.value("tTail"))), static_cast<int>(coordinate.value("yTail") - coordinate.value("tailMajorAxisLength") * sin(coordinate.value("tTail")))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA, 0, double(scale) / 10);
              break;

            case 3:
              cv::arrowedLine(frame, Point(static_cast<int>(coordinate.value("xBody")), static_cast<int>(coordinate.value("yBody"))), Point(static_cast<int>(coordinate.value("xBody") + coordinate.value("bodyMajorAxisLength") * cos(coordinate.value("tBody"))), static_cast<int>(coordinate.value("yBody") - coordinate.value("bodyMajorAxisLength") * sin(coordinate.value("tBody")))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA, 0, double(scale) / 10);
              break;
          }
        }

        if (ui->replayNumbers->isChecked()) {
          cv::putText(frame, to_string(id), Point(static_cast<int>(coordinate.value("xHead") + coordinate.value("headMajorAxisLength") * cos(coordinate.value("tHead"))), static_cast<int>(coordinate.value("yHead") - coordinate.value("headMajorAxisLength") * sin(coordinate.value("tHead")))), cv::FONT_HERSHEY_SIMPLEX, double(scale) * 0.5, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA);
        }

        if (ui->replayTrace->isChecked()) {
          vector<Point> memory;
          for (int j = frameIndex - ui->replayTraceLength->value(); j < frameIndex; j++) {
            if (j > 0) {
              QMap<QString, double> coordinate = trackingData->getData(j, a);
              if (coordinate.contains("xBody")) {
                memory.push_back(Point(static_cast<int>(coordinate.value("xBody")), static_cast<int>(coordinate.value("yBody"))));
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
