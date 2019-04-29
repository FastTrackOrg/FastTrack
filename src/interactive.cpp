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

#include "interactive.h"
#include "ui_interactive.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * @class Interactive
 *
 * @brief The Interactive widget provides an environment to use the tracking widget in an interactive environment.
 *
 * @author Benjamin Gallois
 *
 * @version $Revision: 460 $
 *
 * Contact: gallois.benjamin08@gmail.com
 *
 */

/**
  * @brief Constructs the interactive object derived from a QMainWindow object.
*/
Interactive::Interactive(QWidget *parent) : QMainWindow(parent),
                                            ui(new Ui::Interactive) {
  ui->setupUi(this);

  // Menu bar
  connect(ui->actionOpen, &QAction::triggered, this, &Interactive::openFolder);
  ui->menuView->addAction(ui->imageOptions->toggleViewAction());
  ui->menuView->addAction(ui->trackingOptions->toggleViewAction());
  ui->menuView->addAction(ui->controlOptions->toggleViewAction());

  connect(ui->slider, &QSlider::valueChanged, this, static_cast<void (Interactive::*)(int)>(&Interactive::display));
  connect(ui->slider, &QSlider::valueChanged, [this](const int &newValue) {
    ui->replayNumber->setText(QString::number(newValue));
  });
#ifdef ENABLE_DEVTOOL
  connect(ui->actionbenchmark, &QAction::triggered, this, &Interactive::benchmark);
#endif
  connect(this, &Interactive::message, this, [this](QString msg) {
    QMessageBox msgBox;
    msgBox.setText(msg);
    msgBox.exec();
  });

  // Threshold slider and combobox
  connect(ui->threshSlider, &QSlider::valueChanged, ui->threshBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::setValue));
  connect(ui->threshBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), ui->threshSlider, &QSlider::setValue);
  connect(ui->threshBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this]() {
    ui->isBin->setChecked(true);
    display(ui->slider->value());
  });

  // Zoom
  connect(ui->scrollArea->verticalScrollBar(), &QScrollBar::rangeChanged, [this]() {
    QScrollBar *vertical = ui->scrollArea->verticalScrollBar();
    vertical->setValue(int(zoomReferencePosition.y() * (currentZoom - 1) + currentZoom * vertical->value()));
  });
  connect(ui->scrollArea->horizontalScrollBar(), &QScrollBar::rangeChanged, [this]() {
    QScrollBar *horizontal = ui->scrollArea->horizontalScrollBar();
    horizontal->setValue(int(zoomReferencePosition.x() * (currentZoom - 1) + horizontal->value() * currentZoom));
  });

  // Sets the roi limits
  connect(ui->x2, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int value) {
    ui->x1->setMaximum(value - 1);
    ui->x1->setMinimum(0);
  });
  connect(ui->y2, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int value) {
    ui->y1->setMaximum(value - 1);
    ui->y1->setMinimum(0);
  });

  // Updates the display after each operation
  connect(ui->morphOperation, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), [this]() {
    display(ui->slider->value());
  });
  connect(ui->kernelSize, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this]() {
    ui->isBin->setChecked(true);
    display(ui->slider->value());
  });
  connect(ui->kernelType, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), [this]() {
    ui->isBin->setChecked(true);
    display(ui->slider->value());
  });
  connect(ui->minSize, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this]() {
    ui->isBin->setChecked(true);
    display(ui->slider->value());
  });
  connect(ui->maxSize, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this]() {
    ui->isBin->setChecked(true);
    display(ui->slider->value());
  });
  connect(ui->backColor, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), [this]() {
    ui->isBin->setChecked(true);
    display(ui->slider->value());
  });
  connect(ui->isBin, &QRadioButton::clicked, [this]() {
    display(ui->slider->value());
  });
  connect(ui->isOriginal, &QRadioButton::clicked, [this]() {
    display(ui->slider->value());
  });
  connect(ui->isSub, &QRadioButton::clicked, [this]() {
    display(ui->slider->value());
  });
  connect(ui->isNone, &QRadioButton::clicked, [this]() {
    display(ui->slider->value());
  });
  connect(ui->isTracked, &QRadioButton::clicked, [this]() {
    display(ui->slider->value());
  });

  // Set the image preview limits
  connect(ui->startImage, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int startImage) {
    ui->stopImage->setRange(0, framePath.size() - startImage);
  });

  // Events filter to zoom in/out in the display
  ui->display->installEventFilter(this);
  ui->scrollArea->viewport()->installEventFilter(this);

  // Buttons connects
  connect(ui->backgroundSelectButton, &QPushButton::clicked, this, &Interactive::selectBackground);
  connect(ui->backgroundComputeButton, &QPushButton::clicked, this, &Interactive::computeBackground);
  connect(ui->previewButton, &QPushButton::clicked, this, &Interactive::previewTracking);
  connect(ui->trackButton, &QPushButton::clicked, [this]() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmation", "You are going to start a full tracking analysis. That can take some time, are you sure?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
      track();
    }
  });
  connect(ui->cropButton, &QPushButton::clicked, this, &Interactive::crop);
  connect(ui->resetButton, &QPushButton::clicked, this, &Interactive::reset);

  // Sets information table
  ui->informationTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  // Instanciates a default tracking object that will be used to access its public functions in order to perform the interactive tracking
  isBackground = false;
  tracking = new Tracking("", "");
  connect(tracking, &Tracking::backgroundProgress, ui->backgroundProgressBar, &QProgressBar::setValue);

  // Qt need a delay to update widget geometry
  QTimer::singleShot(500, [this]() {
    display(QImage(":/assets/displayHelp.png"));
    originalImageSize = QImage(":/assets/displayHelp.png").size();
  });

  // Sets a color map
  colorMap.reserve(1000000);
  double a, b, c;
  srand(time(NULL));
  for (int j = 0; j < 1000000; ++j) {
    a = rand() % 255;
    b = rand() % 255;
    c = rand() % 255;
    colorMap.push_back(Point3f(a, b, c));
  }

  // Sets the object counter on top of the display
  counterLabel = new QLabel(ui->scrollArea->viewport());
  counterLabel->move(20, 20);
}

/**
  * @brief Asks the path to a folder where an image sequence is stored. Setups the ui and resets the class attributs for a new analysis. Triggered when the open button from the menu bar is clicked.
*/
void Interactive::openFolder() {
  // Resets the class members
  isBackground = false;
  trackingData = new Data("");
  memoryDir.clear();
  framePath.clear();
  backgroundPath.clear();
  background.release();
  currentZoom = 1;

  // Resets the ui
  ui->display->clear();
  ui->display->setFixedSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
  ui->isNone->setChecked(true);
  ui->isBin->setCheckable(false);
  ui->isSub->setCheckable(false);
  ui->isOriginal->setChecked(true);
  ui->isTracked->setCheckable(false);

  dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), memoryDir, QFileDialog::ShowDirsOnly);

  // Ignores the folder Tracking_Result if selected
  if (dir.right(15) == "Tracking_Result") {
    dir.truncate(dir.size() - 15);
  }

  if (dir.length()) {
    // Finds the image format
    QList<QString> extensions = {"pgm", "png", "jpeg", "jpg", "tiff", "tif", "bmp", "dib", "jpe", "jp2", "webp", "pbm", "ppm", "sr", "ras", "tif"};
    QDirIterator it(dir, QStringList(), QDir::NoFilter);
    QString extension;
    while (it.hasNext()) {
      extension = it.next().section('.', -1);
      if (extensions.contains(extension)) break;
    }

    // Setups the class member
    try {
      string path = (dir + QDir::separator() + "*." + extension).toStdString();
      memoryDir = dir;
      glob(path, framePath, false);

      ui->slider->setMinimum(0);
      ui->slider->setMaximum(framePath.size() - 1);
      ui->nBack->setMaximum(framePath.size() - 1);
      ui->nBack->setValue(framePath.size() - 1);
      ui->startImage->setRange(0, framePath.size() - 1);
      ui->startImage->setValue(0);

      Mat frame = imread(framePath[0], IMREAD_COLOR);
      originalImageSize.setWidth(frame.cols);
      originalImageSize.setHeight(frame.rows);
      ui->x2->setMaximum(frame.cols);
      ui->y2->setMaximum(frame.rows);

      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Path", Qt::MatchExactly)[0]), 1)->setText(dir);
      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Image number", Qt::MatchExactly)[0]), 1)->setText(QString::number(framePath.size()));
      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Image width", Qt::MatchExactly)[0]), 1)->setText(QString::number(frame.cols));
      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Image height", Qt::MatchExactly)[0]), 1)->setText(QString::number(frame.rows));

      isBackground = false;
      reset();
      display(0);

      // Automatic background computation type selection based on the image mean
      int meanValue = int(mean(frame)[0]);
      if (meanValue > 128) {
        ui->back->setCurrentIndex(0);
      }
      else {
        ui->back->setCurrentIndex(1);
      }
    }
    // If an error occurs during the opening, resets the information table and warns the user
    catch (...) {
      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Path", Qt::MatchExactly)[0]), 1)->setText("");
      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Image number", Qt::MatchExactly)[0]), 1)->setText("0");
      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Image width", Qt::MatchExactly)[0]), 1)->setText("0");
      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Image height", Qt::MatchExactly)[0]), 1)->setText("0");
      emit(message("No image found."));
    }
  }
}

/**
  * @brief Displays the image at index in the image sequence in the ui.
  * @param[in] index Index of the image to display in the image sequence.
*/
void Interactive::display(int index) {
  if (!framePath.empty()) {
    UMat frame;
    imread(framePath[index], IMREAD_GRAYSCALE).copyTo(frame);
    vector<vector<Point>> displayContours;
    vector<vector<Point>> rejectedContours;

    // Computes the image with the background subtracted
    if (ui->isSub->isChecked() && isBackground) {
      (ui->backColor->currentText() == "Light background") ? (subtract(background, frame, frame)) : (subtract(frame, background, frame));
    }

    // Computes the binary image an applies morphological operations
    if (ui->isBin->isChecked() && isBackground) {
      (ui->backColor->currentText() == "Light background") ? (subtract(background, frame, frame)) : (subtract(frame, background, frame));
      tracking->binarisation(frame, 'b', ui->threshBox->value());
      Mat element = getStructuringElement(ui->kernelType->currentIndex(), Size(2 * ui->kernelSize->value() + 1, 2 * ui->kernelSize->value() + 1), Point(ui->kernelSize->value(), ui->kernelSize->value()));
      morphologyEx(frame, frame, ui->morphOperation->currentIndex(), element); // MorphTypes enum and QComboBox indexes have to match

      // Keeps only right sized objects
      // To benchmark: pushBack contours vs loop over index and call drawContours multiple times
      vector<vector<Point>> contours;
      findContours(frame, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
      displayContours.reserve(contours.size());
      rejectedContours.reserve(contours.size());

      double min = ui->minSize->value();
      double max = ui->maxSize->value();

      // If too many contours detected to be displayed without slowdowns, ask the user what to do
      if (contours.size() > 10000) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Confirmation", "Too many objects detected to be displayed. \n Do you want to display them anyway (the program can be slow)? ", QMessageBox::No | QMessageBox::Yes);
        if (reply == QMessageBox::No) {
          return;
        }
      }

      for (auto const &a : contours) {
        double size = contourArea(a);
        if (size > min && size < max) {
          displayContours.push_back(a);
        }
        else {
          rejectedContours.push_back(a);
        }
      }
      counterLabel->setText("Objects detected: " + QString::number(displayContours.size()));
      counterLabel->adjustSize();
    }

    // Displays the image in the QLabel
    cvtColor(frame, frame, COLOR_GRAY2RGB);
    if (ui->isBin->isChecked()) {
      drawContours(frame, displayContours, -1, Scalar(0, 255, 0), FILLED, 8);
      drawContours(frame, rejectedContours, -1, Scalar(255, 0, 0), FILLED, 8);
    }
    if (ui->isTracked->isChecked()) {
      QList<int> idList = trackingData->getId(index);
      double scale = 2;

      for (auto const &a : idList) {
        QMap<QString, double> coordinate = trackingData->getData(index, a);
        int id = a;
        cv::ellipse(frame, Point(coordinate.value("xBody"), coordinate.value("yBody")), Size(coordinate.value("bodyMajorAxisLength"), coordinate.value("bodyMinorAxisLength")), 180 - (coordinate.value("tBody") * 180) / M_PI, 0, 360, Scalar(colorMap[id].x, colorMap[id].y, colorMap[id].z), scale, 8);
      }
    }
    if (roi.width != 0 || roi.height != 0) {
      frame = frame(roi);
    }
    Mat image = frame.getMat(ACCESS_READ);
    resizedPix = (QPixmap::fromImage(QImage(image.data, image.cols, image.rows, image.step, QImage::Format_RGB888)).scaled(ui->display->size(), Qt::KeepAspectRatio));
    ui->display->setPixmap(resizedPix);
    resizedFrame.setWidth(resizedPix.width());
    resizedFrame.setHeight(resizedPix.height());
  }
}

/**
  * @brief Zooms in the display.
*/
void Interactive::zoomIn() {
  (currentZoom >= 3) ? (currentZoom = 3) : (currentZoom += 0.2);
  ui->display->setFixedSize(ui->display->size() * currentZoom);
  display(ui->slider->value());
}

/**
  * @brief Zooms out the display.
*/
void Interactive::zoomOut() {
  ui->display->setFixedSize((ui->display->size()) / (currentZoom));
  display(ui->slider->value());
  (currentZoom <= 1) ? (currentZoom = 1) : (currentZoom -= 0.2);
}

/**
  * @brief This is an overloaded function to display a QImage in the display.
*/
void Interactive::display(QImage image) {
  resizedPix = (QPixmap::fromImage(image).scaled(ui->display->size(), Qt::KeepAspectRatio));
  ui->display->setPixmap(resizedPix);
  resizedFrame.setWidth(resizedPix.width());
  resizedFrame.setHeight(resizedPix.height());
}

/**
  * @brief This is an overloaded function to display a UMat in the display.
*/
void Interactive::display(UMat image) {
  cvtColor(image, image, COLOR_GRAY2RGB);
  Mat frame = image.getMat(ACCESS_READ);
  resizedPix = (QPixmap::fromImage(QImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888)).scaled(ui->display->size(), Qt::KeepAspectRatio));
  ui->display->setPixmap(resizedPix);
  resizedFrame.setWidth(resizedPix.width());
  resizedFrame.setHeight(resizedPix.height());
}

/**
  * @brief Computes and displays the background image in the display. Triggered when the backgroundComputeButton is clicked.
*/
void Interactive::computeBackground() {
  if (!framePath.empty()) {
    double nBack = double(ui->nBack->value());
    int method = ui->back->currentIndex();

    ui->backgroundProgressBar->setMaximum(int(nBack));

    // Computes the background without blocking the ui
    QFuture<void> future = QtConcurrent::run([=]() {
      background = tracking->backgroundExtraction(framePath, nBack, method);

      isBackground = true;
      ui->isBin->setCheckable(true);
      ui->isSub->setCheckable(true);

      // Automatic background type selection based on the image mean
      int meanValue = int(mean(background)[0]);
      if (meanValue > 128) {
        ui->backColor->setCurrentIndex(0);
      }
      else {
        ui->backColor->setCurrentIndex(1);
      }

      display(background);
    });
  }
}

/**
  * @brief Opens a dialogue to select a background image. Triggered when ui->backgroundSelectButton is pressed.
*/
void Interactive::selectBackground() {
  QString dir = QFileDialog::getOpenFileName(this, tr("Open Background Image"), memoryDir);

  if (dir.length()) {
    backgroundPath = dir;
    imread(backgroundPath.toStdString(), IMREAD_GRAYSCALE).copyTo(background);
    if (background.cols == originalImageSize.width() && background.rows == originalImageSize.height()) {
      isBackground = true;

      ui->isBin->setCheckable(true);
      ui->isSub->setCheckable(true);

      // Automatic background type selection based on image mean
      int meanValue = int(mean(background)[0]);
      if (meanValue > 128) {
        ui->backColor->setCurrentIndex(0);
      }
      else {
        ui->backColor->setCurrentIndex(1);
      }

      display(background);
    }
    else {
      isBackground = false;
      emit(message("No image found."));
    }
  }
}

/**
  * @brief Gets all the tracking parameters from the ui and updates the parameter map that will be passed to the tracking object.
*/
void Interactive::getParameters() {
  parameters.insert("Maximal size", QString::number(ui->maxSize->value()));
  parameters.insert("Minimal size", QString::number(ui->minSize->value()));
  parameters.insert("Spot to track", QString::number(ui->spot->currentIndex()));
  parameters.insert("Maximal length", QString::number(ui->maxL->value()));
  parameters.insert("Maximal angle", QString::number(ui->maxT->value()));
  parameters.insert("Weight", QString::number(ui->weight->value()));
  parameters.insert("Maximal occlusion", QString::number(ui->lo->value()));
  parameters.insert("Maximal time", QString::number(ui->to->value()));

  parameters.insert("Binary threshold", QString::number(ui->threshBox->value()));
  parameters.insert("Number of images background", QString::number(ui->nBack->value()));
  parameters.insert("Background method", QString::number(ui->back->currentIndex()));
  parameters.insert("ROI top x", QString::number(roi.tl().x));
  parameters.insert("ROI top y", QString::number(roi.tl().y));
  parameters.insert("ROI bottom x", QString::number(roi.br().x));
  parameters.insert("ROI bottom y", QString::number(roi.br().y));
  parameters.insert("Registration", QString::number(ui->reg->currentIndex()));
  parameters.insert("Morphological operation", QString::number(ui->morphOperation->currentIndex()));
  parameters.insert("Kernel size", QString::number(ui->kernelSize->value()));
  parameters.insert("Kernel type", QString::number(ui->kernelType->currentIndex()));
  parameters.insert("Light background", QString::number(ui->backColor->currentIndex()));
}

/**
  * @brief Does a tracing analysis on a sub-part of the image sequence defined by the user. Triggered when previewButton is clicked.
*/
void Interactive::previewTracking() {
  if (!framePath.empty()) {
    ui->progressBar->setRange(ui->startImage->value(), ui->stopImage->value() - ui->startImage->value() - 1);
    ui->progressBar->setValue(0);
    ui->previewButton->setDisabled(true);
    ui->trackButton->setDisabled(true);

    QThread *thread = new QThread;
    Tracking *tracking = new Tracking(framePath, background, ui->startImage->value(), ui->startImage->value() + ui->stopImage->value());
    tracking->moveToThread(thread);

    connect(thread, &QThread::started, tracking, &Tracking::startProcess);
    connect(tracking, &Tracking::progress, ui->progressBar, &QProgressBar::setValue);
    connect(tracking, &Tracking::statistic, [this](int time) {
      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Analysis rate", Qt::MatchExactly)[0]), 1)->setText(QString::number(double(ui->stopImage->value() * 1000) / double(time)));
    });
    connect(tracking, &Tracking::finished, thread, &QThread::quit);
    connect(tracking, &Tracking::finished, [this]() {
      ui->slider->setDisabled(false);
      ui->previewButton->setDisabled(false);
      ui->trackButton->setDisabled(false);
      trackingData = new Data(dir);
    });
    connect(tracking, &Tracking::finished, tracking, &Tracking::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    getParameters();
    tracking->updatingParameters(parameters);
    thread->start();

    ui->slider->setDisabled(true);
    ui->isTracked->setCheckable(true);
    ui->isTracked->setChecked(true);
  }
}

/**
  * @brief Does a tracking analysis. Triggered when the trackButton is clicked.
*/
void Interactive::track() {
  if (!framePath.empty()) {
    ui->progressBar->setRange(0, framePath.size() - 1);
    ui->progressBar->setValue(0);
    ui->previewButton->setDisabled(true);
    ui->trackButton->setDisabled(true);

    QThread *thread = new QThread;
    Tracking *tracking = new Tracking(framePath, background);
    tracking->moveToThread(thread);

    connect(thread, &QThread::started, tracking, &Tracking::startProcess);
    connect(tracking, &Tracking::progress, ui->progressBar, &QProgressBar::setValue);
    connect(tracking, &Tracking::statistic, [this](int time) {
      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Analysis rate", Qt::MatchExactly)[0]), 1)->setText(QString::number(double(framePath.size() * 1000) / double(time)));
    });
    connect(tracking, &Tracking::finished, thread, &QThread::quit);
    connect(tracking, &Tracking::finished, [this]() {
      ui->slider->setDisabled(false);
      ui->previewButton->setDisabled(false);
      ui->trackButton->setDisabled(false);
      trackingData = new Data(dir);
    });
    connect(tracking, &Tracking::finished, tracking, &Tracking::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    getParameters();
    tracking->updatingParameters(parameters);
    thread->start();

    ui->slider->setDisabled(true);
    ui->isTracked->setCheckable(true);
    ui->isTracked->setChecked(true);
  }
}

/**
  * @brief Manages all the mouse inputs in the display.
  * @param[in] target Widget to apply the filter.
  * @param[in] event Describes the mouse event.
*/
bool Interactive::eventFilter(QObject *target, QEvent *event) {
  // Mouse event for the display
  if (target == ui->display) {
    // Set the first point for the ROI at user click
    if (event->type() == QEvent::MouseButtonPress) {
      QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
      if (mouseEvent->buttons() == Qt::LeftButton) {
        clicks.first = mouseEvent->pos();
        // The QPixmap is V/Hcentered in the Qlabel widget
        // Gets the click coordinate in the frame of reference of the centered display
        clicks.first.setX(clicks.first.x() - 0.5 * (ui->display->width() - resizedFrame.width()));
        clicks.first.setY(clicks.first.y() - 0.5 * (ui->display->height() - resizedFrame.height()));
      }
    }

    // Sets the second point and draw the roi
    if (event->type() == QEvent::MouseMove) {
      QMouseEvent *moveEvent = static_cast<QMouseEvent *>(event);
      if (moveEvent->buttons() == Qt::LeftButton) {
        clicks.second = moveEvent->pos();
        // The QPixmap is V/Hcentered in the Qlabel widget
        // Gets the click coordinate in the frame of reference of the centered display
        clicks.second.setX(clicks.second.x() - 0.5 * (ui->display->width() - resizedFrame.width()));
        clicks.second.setY(clicks.second.y() - 0.5 * (ui->display->height() - resizedFrame.height()));

        // Draws the ROI with
        QPixmap tmpImage = resizedPix;
        QPainter paint(&tmpImage);
        paint.setPen(QColor(0, 230, 0, 255));
        QRect roiRect = QRect(clicks.first.x(), clicks.first.y(), clicks.second.x() - clicks.first.x(), clicks.second.y() - clicks.first.y());
        paint.drawRect(roiRect);
        ui->display->setPixmap(tmpImage);

        // Updates ui value
        ui->x1->setValue(clicks.first.x());
        ui->y1->setValue(clicks.first.y());
        ui->x2->setValue(clicks.second.x());
        ui->y2->setValue(clicks.second.y());
      }
    }
  }

  // Scroll Area event filter
  if (target == ui->scrollArea->viewport()) {
    // Moves in the image by middle click
    if (event->type() == QEvent::MouseMove) {
      QMouseEvent *moveEvent = static_cast<QMouseEvent *>(event);
      if (moveEvent->buttons() == Qt::MiddleButton) {
        ui->scrollArea->horizontalScrollBar()->setValue(ui->scrollArea->horizontalScrollBar()->value() + (panReferenceClick.x() - moveEvent->localPos().x()));
        ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->value() + (panReferenceClick.y() - moveEvent->localPos().y()));
        panReferenceClick = moveEvent->localPos();
      }
    }
    if (event->type() == QEvent::Wheel) {
      QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
      zoomReferencePosition = wheelEvent->pos();
    }

    // Zoom/unzoom the display by wheel
    if (event->type() == QEvent::Wheel) {
      QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
      if (wheelEvent->angleDelta().y() > 0) {
        zoomIn();
      }
      else {
        zoomOut();
      }
      return true;
    }
    if (event->type() == QEvent::MouseButtonPress) {
      QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
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
   * @brief Crops the image from a rectangle drawed by the user with the mouse on the display. Triggered when the QPushButton ui->crop is clicked.
 */
void Interactive::crop() {
  // Converts clicks from display widget frame of reference to original image frame of reference
  int xTop = int(double(clicks.first.x()) * double(cropedImageSize.height()) / double(resizedFrame.height()) + roi.tl().x);
  int yTop = int(double(clicks.first.y()) * double(cropedImageSize.height()) / double(resizedFrame.height()) + roi.tl().y);
  int xBottom = int(double(clicks.second.x()) * double(cropedImageSize.height()) / double(resizedFrame.height()) + roi.tl().x);
  int yBottom = int(double(clicks.second.y()) * double(cropedImageSize.height()) / double(resizedFrame.height()) + roi.tl().y);

  // Find the true left corner of the rectangle
  int width = xBottom - xTop;
  int height = yBottom - yTop;
  if (width < 0) {
    xTop += width;
    width = -width;
  }
  if (height < 0) {
    yTop += height;
    height = -height;
  }

  //Checks for wrong values
  if (xTop < 0) xTop = roi.tl().x;
  if (yTop < 0) yTop = roi.tl().y;
  if (width > cropedImageSize.width()) width = cropedImageSize.width();
  if (height > cropedImageSize.height()) height = cropedImageSize.height();

  roi = Rect(xTop, yTop, width, height);
  cropedImageSize.setWidth(roi.width);
  cropedImageSize.setHeight(roi.height);
  ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Image width", Qt::MatchExactly)[0]), 1)->setText(QString::number(roi.width));
  ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Image height", Qt::MatchExactly)[0]), 1)->setText(QString::number(roi.height));
  display(ui->slider->value());
}

/**
   * @brief Resets the region of interest. Triggered by the reset button.
 */
void Interactive::reset() {
  cropedImageSize.setWidth(originalImageSize.width());
  cropedImageSize.setHeight(originalImageSize.height());
  ui->x1->setValue(0);
  ui->y1->setValue(0);
  ui->x2->setValue(originalImageSize.width());
  ui->y2->setValue(originalImageSize.height());
  ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Image width", Qt::MatchExactly)[0]), 1)->setText(QString::number(originalImageSize.width()));
  ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Image height", Qt::MatchExactly)[0]), 1)->setText(QString::number(originalImageSize.height()));
  roi = Rect(0, 0, 0, 0);
  display(ui->slider->value());
}

#ifdef ENABLE_DEVTOOL
/**
 * @brief This is a function for the developer. Triggered a benchmark (downloads a dataset and perform several tracking analysis) to check program performance.
 */
void Interactive::benchmark() {
  // Checks if the dataset exist and creates the destination folder if not
  QNetworkAccessManager *manager = new QNetworkAccessManager(this);
  connect(manager, &QNetworkAccessManager::finished, [this](QNetworkReply *reply) {
    if (QDir(QDir::homePath() + "/FastTrackBenchmarkTmp").exists()) {
      return;
    }

    QDir().mkdir(QDir::homePath() + "/FastTrackBenchmarkTmp");
    QFile file(QDir::homePath() + "/FastTrackBenchmarkTmp/benchmark.zip");
    if (!file.open(QIODevice::WriteOnly)) {
      qInfo() << "error";
      return;
    }

    file.write(reply->readAll());
    reply->deleteLater();
    QStringList tmp = JlCompress::extractDir(QDir::homePath() + "/FastTrackBenchmarkTmp/benchmark.zip", QDir::homePath() + "/FastTrackBenchmarkTmp/");

    //Loads parameters
    QMap<QString, QString> params;
    QFile parameterFile(QDir::homePath() + "/FastTrackBenchmarkTmp/Juvenil_Zebrafish_2/Tracking_Result/parameter.param");
    if (parameterFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
      QTextStream in(&parameterFile);
      QString line;
      QStringList parameters;
      while (in.readLineInto(&line)) {
        parameters = line.split(" = ", QString::SkipEmptyParts);
        params.insert(parameters[0], parameters[1]);
      }
    }
    parameterFile.close();
    benchmarkCount = 0;
    benchmarkAnalysis(params);
  });

  manager->get(QNetworkRequest(QUrl("http://www.fasttrack.sh/download/Benchmark.zip")));
}

/**
 * @brief This is a function for the developer. Triggered a benchmark (downloads a dataset and perform several tracking analysis) to check program performance.
 */
void Interactive::benchmarkAnalysis(QMap<QString, QString> params) {
  QThread *thread = new QThread;
  Tracking *tracking = new Tracking((QDir::homePath() + "/FastTrackBenchmarkTmp/Juvenil_Zebrafish_2/").toStdString(), "");
  tracking->moveToThread(thread);

  connect(thread, &QThread::started, tracking, &Tracking::startProcess);
  connect(tracking, &Tracking::finished, thread, &QThread::quit);
  connect(tracking, &Tracking::finished, tracking, &Tracking::deleteLater);
  connect(tracking, &Tracking::statistic, [this, params](int time) {
    if (benchmarkCount < 20) {
      benchmarkAnalysis(params);
      benchmarkTime.append(time);
      this->benchmarkCount += 1;
    }
    else {
      int mean = 0;
      for (auto &a : benchmarkTime) {
        mean += a;
      }
      mean /= benchmarkTime.size();
      qInfo() << benchmarkTime << benchmarkTime.size();
      emit(message("Benchmark result: " + QString::number(mean)));
      QDir(QDir::homePath() + "/FastTrackBenchmarkTmp").QDir::removeRecursively();
    }
  });
  connect(thread, &QThread::finished, thread, &QThread::deleteLater);
  tracking->updatingParameters(params);
  thread->start();
}
#endif

/**
  * @brief Destructors.
*/
Interactive::~Interactive() {
  delete tracking;
  delete ui;
}
