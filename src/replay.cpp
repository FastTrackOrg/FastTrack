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

Replay::Replay(QWidget* parent, bool standalone, QSlider* control) : QMainWindow(parent),
                                  ui(new Ui::Replay) {
  ui->setupUi(this);

  QIcon img = QIcon(":/assets/buttons/play.png");
  ui->playReplay->setIcon(img);
  ui->playReplay->setIconSize(QSize(ui->playReplay->width(), ui->playReplay->height()));

  img = QIcon(":/assets/buttons/open.png");
  QAction *openAction = new QAction(img, tr("&Open"), this);
  openAction->setShortcuts(QKeySequence::Open);
  openAction->setStatusTip(tr("Open a tracked movie"));
  connect(openAction, &QAction::triggered, this, &Replay::openReplayFolder);
  ui->toolBar->addAction(openAction);

  img = QIcon(":/assets/buttons/refresh.png");
  QAction *refreshAction = new QAction(img, tr("&Refresh"), this);
  refreshAction->setStatusTip(tr("Refresh a tracked movie"));
  connect(refreshAction, &QAction::triggered, [this]() {
    loadReplayFolder(memoryDir);
  });
  ui->toolBar->addAction(refreshAction);

  img = QIcon(":/assets/buttons/save.png");
  QAction *exportAction = new QAction(img, tr("&Export"), this);
  exportAction->setStatusTip(tr("Export the tracked movie"));
  connect(exportAction, &QAction::triggered, this, &Replay::saveTrackedMovie);
  ui->toolBar->addAction(exportAction);

  commandStack = new QUndoStack(this);
  img = QIcon(":/assets/buttons/undo.png");
  QAction *undoAction = commandStack->createUndoAction(this, tr("&Undo"));
  undoAction->setIcon(img);
  undoAction->setShortcuts(QKeySequence::Undo);
  undoAction->setStatusTip(tr("Undo"));
  connect(undoAction, &QAction::triggered, [this]() {
    loadFrame(ui->replaySlider->value());
  });
  ui->toolBar->addAction(undoAction);

  img = QIcon(":/assets/buttons/redo.png");
  QAction *redoAction = commandStack->createRedoAction(this, tr("&Redo"));
  redoAction->setIcon(img);
  redoAction->setShortcuts(QKeySequence::Redo);
  redoAction->setStatusTip(tr("Redo"));
  connect(redoAction, &QAction::triggered, [this]() {
    loadFrame(ui->replaySlider->value());
  });
  ui->toolBar->addAction(redoAction);

  ui->toolBar->addSeparator();

  object1Replay = new QComboBox(this);
  object1Replay->setStatusTip(tr("First selected object"));
  ui->toolBar->addWidget(object1Replay);
  
  img = QIcon(":/assets/buttons/replace.png");
  QAction *swapAction = new QAction(img, tr("&Swap"), this);
  swapAction->setStatusTip(tr("Swap the two objects"));
  connect(swapAction, &QAction::triggered, this, &Replay::correctTracking);
  ui->toolBar->addAction(swapAction);

  object2Replay = new QComboBox(this);
  object2Replay->setStatusTip(tr("Second selected object"));
  ui->toolBar->addWidget(object2Replay);

  img = QIcon(":/assets/buttons/delete.png");
  QAction* deleteAction = new QAction(img, tr("&Delete"), this);
  deleteAction->setStatusTip(tr("Delete the object from this frame on the selected number of frames"));
  connect(deleteAction, &QAction::triggered, [this]() {
    if (isReplayable) {
      DeleteData* del = new DeleteData(object2Replay->currentText().toInt(), ui->replaySlider->value(), ui->replaySlider->value() + deletedFrameNumber->value() - 1, trackingData);
      commandStack->push(del);
      loadFrame(ui->replaySlider->value());
    }
  });
  ui->toolBar->addAction(deleteAction);

  deletedFrameNumber = new QSpinBox(this);
  deletedFrameNumber->setStatusTip(tr("Number of frames where to delete the selected object"));
  connect(ui->replaySlider, &QSlider::valueChanged, [this]() {
    deletedFrameNumber->setMaximum(replayFrames.size() - ui->replaySlider->value());
    deletedFrameNumber->setValue(replayFrames.size() - ui->replaySlider->value());
    }); 
  ui->toolBar->addWidget(deletedFrameNumber);

  img = QIcon(":/assets/buttons/previous.png");
  QAction *previousAction = new QAction(img, tr("&Previous"), this);
  previousAction->setStatusTip(tr("Previous occlusion"));
  connect(previousAction, &QAction::triggered, this, &Replay::previousOcclusionEvent);
  ui->toolBar->addAction(previousAction);

  img = QIcon(":/assets/buttons/next.png");
  QAction *nextAction = new QAction(img, tr("&Next"), this);
  nextAction->setStatusTip(tr("Next occlusion"));
  connect(nextAction, &QAction::triggered, this, &Replay::nextOcclusionEvent);
  ui->toolBar->addAction(nextAction);

  img = QIcon(":/assets/buttons/help.png");
  QAction *helpAction = new QAction(img, tr("&Help"), this);
  helpAction->setStatusTip(tr("Help"));
  connect(helpAction, &QAction::triggered, [this]() {
    QMessageBox helpBox(this);
    helpBox.setIconPixmap(QPixmap(":/assets/buttons/helpImg.png"));
    helpBox.exec();
  });
  ui->toolBar->addAction(helpAction);

  if (!standalone) {
    ui->controls->hide();
  }

  if (control) {
    connect(control, &QSlider::valueChanged, ui->replaySlider, &QSlider::setValue);
  }

  // Keyboard shorcut
  // AZERTY keyboard shorcuts are set in the ui
  wShortcut = new QShortcut(QKeySequence("w"), this);
  //connect(wShortcut, &QShortcut::activated, [this](nextAction) { nextAction->triggered(); });

  qShortcut = new QShortcut(QKeySequence("q"), this);
  connect(qShortcut, &QShortcut::activated, [this]() { ui->replaySlider->setValue(ui->replaySlider->value() - 1); });

  aShortcut = new QShortcut(QKeySequence("a"), this);
  connect(aShortcut, &QShortcut::activated, [this]() { ui->replaySlider->setValue(ui->replaySlider->value() - 1); });

  dShortcut = new QShortcut(QKeySequence("d"), this);
  connect(dShortcut, &QShortcut::activated, [this]() { ui->replaySlider->setValue(ui->replaySlider->value() + 1); });

  ui->replayDisplay->installEventFilter(this);
  ui->scrollArea->viewport()->installEventFilter(this);

  // Zoom
  connect(ui->scrollArea->verticalScrollBar(), &QScrollBar::rangeChanged, [this]() {
    QScrollBar* vertical = ui->scrollArea->verticalScrollBar();
    vertical->setValue(int(zoomReferencePosition.y() * (currentZoom - 1) + currentZoom * vertical->value()));
  });
  connect(ui->scrollArea->horizontalScrollBar(), &QScrollBar::rangeChanged, [this]() {
    QScrollBar* horizontal = ui->scrollArea->horizontalScrollBar();
    horizontal->setValue(int(zoomReferencePosition.x() * (currentZoom - 1) + currentZoom * horizontal->value()));
  });

  isReplayable = false;
  framerate = new QTimer();
  ui->ellipseBox->addItems({"Head + Tail", "Head", "Tail", "Body", "None"});
  ui->arrowBox->addItems({"Head + Tail", "Head", "Tail", "Body", "None"});

  connect(ui->replaySlider, &QSlider::valueChanged, this, &Replay::loadFrame);
  connect(ui->replaySlider, &QSlider::valueChanged, [this](const int& newValue) {
    ui->replayNumber->setText(QString::number(newValue));
  });

  connect(framerate, &QTimer::timeout, [this]() {
    ui->replaySlider->setValue(autoPlayerIndex);
    autoPlayerIndex++;
    if (autoPlayerIndex % int(replayFrames.size()) != autoPlayerIndex) {
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
}

Replay::~Replay() {
  delete ui;
}

/**
  * @brief Opens a dialogue to select a folder.
*/
void Replay::openReplayFolder() {

  QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), memoryDir, QFileDialog::ShowDirsOnly);

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
  replayFrames.clear();
  occlusionEvents.clear();
  object1Replay->clear();
  object2Replay->clear();
  ui->replayDisplay->clear();
  framerate->stop();
  object = true;
  currentZoom = 1;
  memoryDir.clear();

  if (!dir.length()) return;

  memoryDir = dir;
  QString trackingDir = dir;
  if (dir.contains("Tracking_Result")) {
    dir.truncate(dir.lastIndexOf("/", -2));
  }
  else {
    trackingDir.append("/Tracking_Result");
  }

  // Finds image format
  QList<QString> extensions = {"pgm", "png", "jpeg", "jpg", "tiff", "tif", "bmp", "dib", "jpe", "jp2", "webp", "pbm", "ppm", "sr", "ras", "tif"};
  QDirIterator it(dir, QStringList(), QDir::NoFilter);
  QString extension;
  while (it.hasNext()) {
    extension = it.next().section('.', -1);
    if (extensions.contains(extension)) break;
  }

  try {
    // Gets the paths to all the frames in the folder and puts it in a vector.
    // Setups the ui by setting maximum and minimum of the slider bar.
    string path = (dir + QDir::separator() + "*." + extension).toStdString();
    glob(path, replayFrames, false);  // Gets all the paths to frames
    if (replayFrames.empty()) {
      return;
    }
    ui->replaySlider->setMinimum(0);
    ui->replaySlider->setMaximum(replayFrames.size() - 1);
    Mat frame = imread(replayFrames[0], IMREAD_COLOR);
    originalImageSize.setWidth(frame.cols);
    originalImageSize.setHeight(frame.rows);
    deletedFrameNumber->setRange(1, replayFrames.size()); 
    deletedFrameNumber->setValue(replayFrames.size()); 
    isReplayable = true;

    trackingData = new Data(trackingDir);
    for (int i = 0; i < trackingData->maxId + 1; i++) {
      object2Replay->addItem(QString::number(i));
    }

    // Generates a color map.
    // TO REDO
    double a, b, c;
    srand(time(NULL));
    for (int j = 0; j < 9000; ++j) {
      a = rand() % 255;
      b = rand() % 255;
      c = rand() % 255;
      colorMap.push_back(Point3f(a, b, c));
    }
    loadFrame(0);
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

    Mat frame = imread(replayFrames[frameIndex], IMREAD_COLOR);
    int scale = ui->replaySize->value();

    // Takes the tracking data corresponding to the replayed frame and parse data to display
    QList<int> idList = trackingData->getId(frameIndex);
    for (auto const& a : idList) {
      QMap<QString, double> coordinate = trackingData->getData(frameIndex, a);
      int id = a;

      object1Replay->addItem(QString::number(id));

      if (ui->ellipseBox->currentIndex() != 4) {
        switch (ui->ellipseBox->currentIndex()) {
          case 0:  // Head + Tail
            cv::ellipse(frame, Point(coordinate.value("xHead"), coordinate.value("yHead")), Size(coordinate.value("headMajorAxisLength"), coordinate.value("headMinorAxisLength")), 180 - (coordinate.value("tHead") * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, 8);
            cv::ellipse(frame, Point(coordinate.value("xTail"), coordinate.value("yTail")), Size(coordinate.value("tailMajorAxisLength"), coordinate.value("tailMinorAxisLength")), 180 - (coordinate.value("tTail") * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA);
            break;

          case 1:  // Head
            cv::ellipse(frame, Point(coordinate.value("xHead"), coordinate.value("yHead")), Size(coordinate.value("headMajorAxisLength"), coordinate.value("headMinorAxisLength")), 180 - (coordinate.value("tHead") * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, 8);
            break;

          case 2:  // Tail
            cv::ellipse(frame, Point(coordinate.value("xTail"), coordinate.value("yTail")), Size(coordinate.value("tailMajorAxisLength"), coordinate.value("tailMinorAxisLength")), 180 - (coordinate.value("tTail") * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA);
            break;

          case 3:  // Body
            cv::ellipse(frame, Point(coordinate.value("xBody"), coordinate.value("yBody")), Size(coordinate.value("bodyMajorAxisLength"), coordinate.value("bodyMinorAxisLength")), 180 - (coordinate.value("tBody") * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, 8);
            break;
        }
      }

      if (ui->arrowBox->currentIndex() != 4) {
        switch (ui->arrowBox->currentIndex()) {
          case 0:
            cv::arrowedLine(frame, Point(coordinate.value("xHead"), coordinate.value("yHead")), Point(coordinate.value("xHead") + coordinate.value("headMajorAxisLength") * cos(coordinate.value("tHead")), coordinate.value("yHead") - coordinate.value("headMajorAxisLength") * sin(coordinate.value("tHead"))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA, 0, double(scale) / 10);
            cv::arrowedLine(frame, Point(coordinate.value("xTail"), coordinate.value("yTail")), Point(coordinate.value("xTail") + coordinate.value("tailMajorAxisLength") * cos(coordinate.value("tTail")), coordinate.value("yTail") - coordinate.value("tailMajorAxisLength") * sin(coordinate.value("tTail"))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA, 0, double(scale) / 10);
            break;

          case 1:
            cv::arrowedLine(frame, Point(coordinate.value("xHead"), coordinate.value("yHead")), Point(coordinate.value("xHead") + coordinate.value("headMajorAxisLength") * cos(coordinate.value("tHead")), coordinate.value("yHead") - coordinate.value("headMajorAxisLength") * sin(coordinate.value("tHead"))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA, 0, double(scale) / 10);
            break;

          case 2:
            cv::arrowedLine(frame, Point(coordinate.value("xTail"), coordinate.value("yTail")), Point(coordinate.value("xTail") + coordinate.value("tailMajorAxisLength") * cos(coordinate.value("tTail")), coordinate.value("yTail") - coordinate.value("tailMajorAxisLength") * sin(coordinate.value("tTail"))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA, 0, double(scale) / 10);
            break;

          case 3:
            cv::arrowedLine(frame, Point(coordinate.value("xBody"), coordinate.value("yBody")), Point(coordinate.value("xBody") + coordinate.value("bodyMajorAxisLength") * cos(coordinate.value("tBody")), coordinate.value("yBody") - coordinate.value("bodyMajorAxisLength") * sin(coordinate.value("tBody"))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA, 0, double(scale) / 10);
            break;
        }
      }

      if (ui->replayNumbers->isChecked()) {
        cv::putText(frame, to_string(id), Point(coordinate.value("xHead") + coordinate.value("headMajorAxisLength") * cos(coordinate.value("tHead")), coordinate.value("yHead") - coordinate.value("headMajorAxisLength") * sin(coordinate.value("tHead"))), cv::FONT_HERSHEY_SIMPLEX, double(scale) * 0.5, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale * 1.2, cv::LINE_AA);
      }

      if (ui->replayTrace->isChecked()) {
        vector<Point> memory;
        for (int j = frameIndex - ui->replayTraceLength->value(); j < frameIndex; j++) {
          if (j > 0) {
            QMap<QString, double> coordinate = trackingData->getData(j, a);
            if (coordinate.contains("xBody")) {
              memory.push_back(Point(coordinate.value("xBody"), coordinate.value("yBody")));
            }
          }
        }
        cv::polylines(frame, memory, false, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale * 1.2, cv::LINE_AA);
      }
    }

    double w = ui->replayDisplay->width();
    double h = ui->replayDisplay->height();
    QPixmap resizedPix = (QPixmap::fromImage(QImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888)).scaled(w, h, Qt::KeepAspectRatio));
    ui->replayDisplay->setPixmap(resizedPix);
    resizedFrame.setWidth(resizedPix.width());
    resizedFrame.setHeight(resizedPix.height());
  }
}

/**
  * @brief Zoom the display from a scale factor.
*/
void Replay::zoomIn() {
  (currentZoom >= 3) ? (currentZoom = 3) : (currentZoom += 0.2);
  ui->replayDisplay->setFixedSize(ui->replayDisplay->size() * currentZoom);
  loadFrame(ui->replaySlider->value());
}

/**
  * @brief Zoom the display from a scale factor.
*/
void Replay::zoomOut() {
  ui->replayDisplay->setFixedSize((ui->replayDisplay->size()) / (currentZoom));
  loadFrame(ui->replaySlider->value());
  (currentZoom <= 1) ? (currentZoom = 1) : (currentZoom -= 0.2);
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
          int min = idList.at(std::min_element(distance.begin(), distance.end()) - distance.begin());
          if (object) {
            object1Replay->setCurrentIndex(object1Replay->findText(QString::number(min)));
            object1Replay->setStyleSheet("QComboBox { background-color: rgb(" + QString::number(colorMap[min].x) + "," + QString::number(colorMap[min].y) + "," + QString::number(colorMap[min].z) + "); }");
            object = false;
          }
          else {
            object2Replay->setCurrentIndex(object2Replay->findText(QString::number(min)));
            object2Replay->setStyleSheet("QComboBox { background-color: rgb(" + QString::number(colorMap[min].x) + "," + QString::number(colorMap[min].y) + "," + QString::number(colorMap[min].z) + "); }");
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
  }

  // Scroll Area event filter
  if (target == ui->scrollArea->viewport()) {
    // Moves in the image by middle click
    if (event->type() == QEvent::MouseMove) {
      QMouseEvent* moveEvent = static_cast<QMouseEvent*>(event);
      if (moveEvent->buttons() == Qt::MiddleButton) {
        ui->scrollArea->horizontalScrollBar()->setValue(ui->scrollArea->horizontalScrollBar()->value() + (panReferenceClick.x() - moveEvent->localPos().x()));
        ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->value() + (panReferenceClick.y() - moveEvent->localPos().y()));
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
  * @brief Gets the index of the two selected objects, the start index, swaps the data from the start index to the end, and saves the new tracking data. Triggered when ui->swapButton is pressed or a right-click event is registered inside the replayDisplay.
*/
void Replay::correctTracking() {
  if (isReplayable) {
    // Swaps the data
    int firstObject = object1Replay->currentText().toInt();
    int secondObject = object2Replay->currentText().toInt();
    int start = ui->replaySlider->value();
    SwapData *swap = new SwapData(firstObject, secondObject, start, trackingData);
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
    int previousOcclusion = occlusionEvents.at(std::upper_bound(occlusionEvents.begin(), occlusionEvents.end(), current) - occlusionEvents.begin() - 2);
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

    for (size_t frameIndex = 0; frameIndex < replayFrames.size(); frameIndex++) {
      Mat frame = imread(replayFrames[frameIndex], IMREAD_COLOR);
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
              cv::ellipse(frame, Point(coordinate.value("xHead"), coordinate.value("yHead")), Size(coordinate.value("headMajorAxisLength"), coordinate.value("headMinorAxisLength")), 180 - (coordinate.value("tHead") * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, 8);
              cv::ellipse(frame, Point(coordinate.value("xTail"), coordinate.value("yTail")), Size(coordinate.value("tailMajorAxisLength"), coordinate.value("tailMinorAxisLength")), 180 - (coordinate.value("tTail") * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA);
              break;

            case 1:  // Head
              cv::ellipse(frame, Point(coordinate.value("xHead"), coordinate.value("yHead")), Size(coordinate.value("headMajorAxisLength"), coordinate.value("headMinorAxisLength")), 180 - (coordinate.value("tHead") * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, 8);
              break;

            case 2:  // Tail
              cv::ellipse(frame, Point(coordinate.value("xTail"), coordinate.value("yTail")), Size(coordinate.value("tailMajorAxisLength"), coordinate.value("tailMinorAxisLength")), 180 - (coordinate.value("tTail") * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA);
              break;

            case 3:  // Body
              cv::ellipse(frame, Point(coordinate.value("xBody"), coordinate.value("yBody")), Size(coordinate.value("bodyMajorAxisLength"), coordinate.value("bodyMinorAxisLength")), 180 - (coordinate.value("tBody") * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, 8);
              break;
          }
        }

        if (ui->arrowBox->currentIndex() != 4) {
          switch (ui->arrowBox->currentIndex()) {
            case 0:
              cv::arrowedLine(frame, Point(coordinate.value("xHead"), coordinate.value("yHead")), Point(coordinate.value("xHead") + coordinate.value("headMajorAxisLength") * cos(coordinate.value("tHead")), coordinate.value("yHead") - coordinate.value("headMajorAxisLength") * sin(coordinate.value("tHead"))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA, 0, double(scale) / 10);
              cv::arrowedLine(frame, Point(coordinate.value("xTail"), coordinate.value("yTail")), Point(coordinate.value("xTail") + coordinate.value("tailMajorAxisLength") * cos(coordinate.value("tTail")), coordinate.value("yTail") - coordinate.value("tailMajorAxisLength") * sin(coordinate.value("tTail"))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA, 0, double(scale) / 10);
              break;

            case 1:
              cv::arrowedLine(frame, Point(coordinate.value("xHead"), coordinate.value("yHead")), Point(coordinate.value("xHead") + coordinate.value("headMajorAxisLength") * cos(coordinate.value("tHead")), coordinate.value("yHead") - coordinate.value("headMajorAxisLength") * sin(coordinate.value("tHead"))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA, 0, double(scale) / 10);
              break;

            case 2:
              cv::arrowedLine(frame, Point(coordinate.value("xTail"), coordinate.value("yTail")), Point(coordinate.value("xTail") + coordinate.value("tailMajorAxisLength") * cos(coordinate.value("tTail")), coordinate.value("yTail") - coordinate.value("tailMajorAxisLength") * sin(coordinate.value("tTail"))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA, 0, double(scale) / 10);
              break;

            case 3:
              cv::arrowedLine(frame, Point(coordinate.value("xBody"), coordinate.value("yBody")), Point(coordinate.value("xBody") + coordinate.value("bodyMajorAxisLength") * cos(coordinate.value("tBody")), coordinate.value("yBody") - coordinate.value("bodyMajorAxisLength") * sin(coordinate.value("tBody"))), Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, cv::LINE_AA, 0, double(scale) / 10);
              break;
          }
        }

        if (ui->replayNumbers->isChecked()) {
          cv::putText(frame, to_string(id), Point(coordinate.value("xHead") + coordinate.value("headMajorAxisLength") * cos(coordinate.value("tHead")), coordinate.value("yHead") - coordinate.value("headMajorAxisLength") * sin(coordinate.value("tHead"))), cv::FONT_HERSHEY_SIMPLEX, double(scale) * 0.5, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale * 1.2, cv::LINE_AA);
        }

        if (ui->replayTrace->isChecked()) {
          vector<Point> memory;
          for (int j = frameIndex - ui->replayTraceLength->value(); j < frameIndex; j++) {
            if (j > 0) {
              QMap<QString, double> coordinate = trackingData->getData(j, a);
              if (coordinate.contains("xBody")) {
                memory.push_back(Point(coordinate.value("xBody"), coordinate.value("yBody")));
              }
            }
          }
          cv::polylines(frame, memory, false, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale * 1.2, cv::LINE_AA);
        }
      }
      outputVideo.write(frame);
      ui->replaySlider->setValue(frameIndex);
    }
    outputVideo.release();
  }
}
