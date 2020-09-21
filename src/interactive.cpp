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
 * @version $Revision: 480 $
 *
 * Contact: benjamin.gallois@fasttrack.sh
 *
 */

/**
  * @brief Constructs the interactive object derived from a QMainWindow object.
*/
Interactive::Interactive(QWidget *parent) : QMainWindow(parent),
                                            ui(new Ui::Interactive) {
  ui->setupUi(this);

  // Loads settings
  settingsFile = new QSettings("FastTrack", "Benjamin Gallois", this);
  loadSettings();
  videoStatus = false;

  //DockWidget
  connect(ui->imageOptions, &QDockWidget::dockLocationChanged, [this](Qt::DockWidgetArea area) {
    switch (area) {
      case Qt::LeftDockWidgetArea:
        ui->optionsTab->setTabPosition(QTabWidget::West);
        break;
      case Qt::RightDockWidgetArea:
        ui->optionsTab->setTabPosition(QTabWidget::East);
        break;
      case Qt::TopDockWidgetArea:
        ui->optionsTab->setTabPosition(QTabWidget::South);
        break;
      case Qt::BottomDockWidgetArea:
        ui->optionsTab->setTabPosition(QTabWidget::North);
        break;
      default:
        ui->optionsTab->setTabPosition(QTabWidget::West);
    }
  });
  connect(ui->trackingOptions, &QDockWidget::dockLocationChanged, [this](Qt::DockWidgetArea area) {
    switch (area) {
      case Qt::LeftDockWidgetArea:
        ui->trackingTab->setTabPosition(QTabWidget::West);
        break;
      case Qt::RightDockWidgetArea:
        ui->trackingTab->setTabPosition(QTabWidget::East);
        break;
      case Qt::TopDockWidgetArea:
        ui->trackingTab->setTabPosition(QTabWidget::South);
        break;
      case Qt::BottomDockWidgetArea:
        ui->trackingTab->setTabPosition(QTabWidget::North);
        break;
      default:
        ui->optionsTab->setTabPosition(QTabWidget::West);
    }
  });

  // Menu bar
  connect(ui->actionOpen, &QAction::triggered, this, &Interactive::openFolder);
  ui->menuView->addAction(ui->imageOptions->toggleViewAction());
  ui->menuView->addAction(ui->trackingOptions->toggleViewAction());
  ui->menuView->addAction(ui->controlOptions->toggleViewAction());

  replayAction = new QAction(tr("Tracking replay"), this);
  replayAction->setCheckable(true);
  connect(replayAction, &QAction::toggled, [this](bool isChecked) {
    if (isChecked) {
      ui->interactiveTab->addTab(replay, tr("Replay"));
      replay->loadReplayFolder(dir);
      ui->interactiveTab->setCurrentIndex(1);
    }
    else {
      ui->interactiveTab->removeTab(1);
    }
  });
  ui->menuView->addAction(replayAction);

  connect(ui->slider, &Timeline::valueChanged, [this](const int &newValue) {
    int index = ui->interactiveTab->currentIndex();
    if (index == 0) {
      display(newValue);
    }
    else if (index == 1) {
      replay->loadFrame(newValue);
    }
  });

  // Stay on the same frame when changing tab
  connect(ui->interactiveTab, &QTabWidget::currentChanged, [this]() {
    ui->slider->setValue(ui->slider->value());
  });

  // Loads prefered style
  QStringList styles = QStyleFactory::keys();
  QString defaultStyle = settings.value("style");
  QMenu *menuStyle = new QMenu(tr("Appearance"), this);
  ui->menuSettings->addMenu(menuStyle);

  for (auto const &a : styles) {
    QAction *styleAction = new QAction(a, this);
    styleAction->setCheckable(true);
    connect(styleAction, &QAction::triggered, [this, styleAction, menuStyle]() {
      qApp->setStyle(QStyleFactory::create(styleAction->text()));
      settings.insert("style", styleAction->text());
      foreach (QAction *action, menuStyle->actions()) {
        action->setChecked(false);
      }
      styleAction->setChecked(true);
    });
    menuStyle->addAction(styleAction);
  }

  if (styles.contains(defaultStyle)) {
    qApp->setStyle(QStyleFactory::create(defaultStyle));
    foreach (QAction *action, menuStyle->actions()) {
      if (action->text() == defaultStyle) {
        action->setChecked(true);
      }
    }
  }

  // Palette
  QMenu *menuPalette = new QMenu(tr("Theme"), this);
  ui->menuSettings->addMenu(menuPalette);
  QAction *defaultColor = new QAction(tr("Default"), this);
  defaultColor->setCheckable(true);
  menuPalette->addAction(defaultColor);
  QAction *darkColor = new QAction(tr("Breeze Dark"), this);
  darkColor->setCheckable(true);
  menuPalette->addAction(darkColor);
  QAction *lightColor = new QAction(tr("Breeze Light"), this);
  lightColor->setCheckable(true);
  menuPalette->addAction(lightColor);
  connect(defaultColor, &QAction::triggered, [this, defaultColor, menuPalette]() {
    foreach (QAction *action, menuPalette->actions()) {
      action->setChecked(false);
    }
    defaultColor->setChecked(true);
    qApp->setStyleSheet("");
    settings.insert("color", "default");
  });
  connect(darkColor, &QAction::triggered, [this, darkColor, menuPalette]() {
    foreach (QAction *action, menuPalette->actions()) {
      action->setChecked(false);
    }
    darkColor->setChecked(true);
    QFile file(":/dark.qss");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    qApp->setStyleSheet(stream.readAll());
    settings.insert("color", "dark");
  });
  connect(lightColor, &QAction::triggered, [this, lightColor, menuPalette]() {
    foreach (QAction *action, menuPalette->actions()) {
      action->setChecked(false);
    }
    lightColor->setChecked(true);
    QFile file(":/light.qss");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    qApp->setStyleSheet(stream.readAll());
    settings.insert("color", "light");
  });

  QString defaultColorTheme = settings.value("color");
  if (defaultColorTheme == "light") {
    lightColor->trigger();
  }
  else if (defaultColorTheme == "dark") {
    darkColor->trigger();
  }
  else {
    defaultColor->trigger();
  }

  // Layout options
  ui->menuBar->removeAction(ui->menuLayout->menuAction());
  ui->menuSettings->addMenu(ui->menuLayout);
  connect(ui->actionLayout1, &QAction::triggered, [this]() {
    ui->controlOptions->setVisible(true);
    ui->imageOptions->setVisible(true);
    ui->trackingOptions->setVisible(true);

    ui->controlOptions->setFloating(false);
    ui->imageOptions->setFloating(false);
    ui->trackingOptions->setFloating(false);

    addDockWidget(Qt::LeftDockWidgetArea, ui->imageOptions);
    tabifyDockWidget(ui->imageOptions, ui->trackingOptions);
    ui->imageOptions->raise();
    addDockWidget(Qt::BottomDockWidgetArea, ui->controlOptions);

    ui->actionLayout1->setChecked(true);
    ui->actionLayout2->setChecked(false);
    ui->actionLayout3->setChecked(false);
    ui->actionLayout4->setChecked(false);

    settings.insert("layout", "1");
  });
  connect(ui->actionLayout2, &QAction::triggered, [this]() {
    ui->controlOptions->setVisible(true);
    ui->imageOptions->setVisible(true);
    ui->trackingOptions->setVisible(true);

    ui->controlOptions->setFloating(false);
    ui->imageOptions->setFloating(false);
    ui->trackingOptions->setFloating(false);

    addDockWidget(Qt::RightDockWidgetArea, ui->imageOptions);
    tabifyDockWidget(ui->imageOptions, ui->trackingOptions);
    ui->imageOptions->raise();
    addDockWidget(Qt::BottomDockWidgetArea, ui->controlOptions);

    ui->actionLayout1->setChecked(false);
    ui->actionLayout2->setChecked(true);
    ui->actionLayout3->setChecked(false);
    ui->actionLayout4->setChecked(false);

    settings.insert("layout", "2");
  });
  connect(ui->actionLayout3, &QAction::triggered, [this]() {
    ui->controlOptions->setVisible(true);
    ui->imageOptions->setVisible(true);
    ui->trackingOptions->setVisible(true);

    ui->controlOptions->setFloating(false);
    ui->imageOptions->setFloating(false);
    ui->trackingOptions->setFloating(false);

    addDockWidget(Qt::LeftDockWidgetArea, ui->imageOptions);
    addDockWidget(Qt::RightDockWidgetArea, ui->trackingOptions);
    addDockWidget(Qt::BottomDockWidgetArea, ui->controlOptions);

    ui->actionLayout1->setChecked(false);
    ui->actionLayout2->setChecked(false);
    ui->actionLayout3->setChecked(true);
    ui->actionLayout4->setChecked(false);

    settings.insert("layout", "3");
  });
  connect(ui->actionLayout4, &QAction::triggered, [this]() {
    ui->controlOptions->setVisible(true);
    ui->imageOptions->setVisible(true);
    ui->trackingOptions->setVisible(true);

    ui->controlOptions->setFloating(false);
    ui->imageOptions->setFloating(false);
    ui->trackingOptions->setFloating(false);

    addDockWidget(Qt::LeftDockWidgetArea, ui->imageOptions);
    tabifyDockWidget(ui->imageOptions, ui->trackingOptions);
    ui->imageOptions->raise();
    addDockWidget(Qt::LeftDockWidgetArea, ui->controlOptions);

    ui->actionLayout1->setChecked(false);
    ui->actionLayout2->setChecked(false);
    ui->actionLayout3->setChecked(false);
    ui->actionLayout4->setChecked(true);

    settings.insert("layout", "4");
  });

  // Loads previous layout
  int preferedLayout = settings.value("layout").toInt();

  switch (preferedLayout) {
    case 1:
      ui->actionLayout1->activate(QAction::Trigger);
      break;
    case 2:
      ui->actionLayout2->activate(QAction::Trigger);
      break;
    case 3:
      ui->actionLayout3->activate(QAction::Trigger);
      break;
    case 4:
      ui->actionLayout4->activate(QAction::Trigger);
      break;
    default:
      ui->actionLayout1->activate(QAction::Trigger);
  }

  // Help menu
  connect(ui->actionDoc, &QAction::triggered, []() {
    QDesktopServices::openUrl(QUrl("http://www.fasttrack.sh/UserManual/docs/intro.html", QUrl::TolerantMode));
  });
  connect(ui->actionTuto, &QAction::triggered, []() {
    QDesktopServices::openUrl(QUrl("https://www.youtube.com/watch?v=RzzmcZs04E4&list=PLGjsUpRojSmO4RHrd-TbpbNpJrfjNYlIm", QUrl::TolerantMode));
  });
  connect(ui->actionIssue, &QAction::triggered, []() {
    QDesktopServices::openUrl(QUrl("https://github.com/FastTrackOrg/FastTrack/issues", QUrl::TolerantMode));
  });
  connect(ui->actionContact, &QAction::triggered, []() {
    QDesktopServices::openUrl(QUrl("mailto:benjamin.gallois@fasttrack.sh?subject=[fasttrack]", QUrl::TolerantMode));
  });
  connect(ui->actionAbout, &QAction::triggered, []() {
    QMessageBox aboutBox;
    aboutBox.setText("FastTrack is a desktop tracking software, easy to install, easy to use, and performant.<br>Created and maintained by Benjamin Gallois.<br>Distributed under the terms of the <a href='https://www.gnu.org/licenses/gpl-3.0'>GPL3.0 license</a>.<br>");
    aboutBox.exec();
  });

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

  // Draws scale
  connect(ui->lo, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int scale) {
    display(ui->slider->value(), scale);
  });
  connect(ui->maxL, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int scale) {
    display(ui->slider->value(), scale);
  });

  // Zoom
  connect(ui->scrollArea->verticalScrollBar(), &QScrollBar::rangeChanged, [this]() {
    QScrollBar *vertical = ui->scrollArea->verticalScrollBar();
    if (currentZoom > 0) {
      vertical->setValue(int(zoomReferencePosition.y() * 0.25 + vertical->value() * 1.25));
    }
    else {
      vertical->setValue(int(-zoomReferencePosition.y() * 0.25 + vertical->value() / 1.25));
    }
  });
  connect(ui->scrollArea->horizontalScrollBar(), &QScrollBar::rangeChanged, [this]() {
    QScrollBar *horizontal = ui->scrollArea->horizontalScrollBar();
    if (currentZoom > 0) {
      horizontal->setValue(int(zoomReferencePosition.x() * 0.25 + horizontal->value() * 1.25));
    }
    else {
      horizontal->setValue(int(-zoomReferencePosition.x() * 0.25 + horizontal->value() / 1.25));
    }
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

  // Replay tab
  replay = new Replay(this, false);
  connect(ui->interactiveTab, &QTabWidget::tabCloseRequested, [this](int index) {
    if (index != 0) {
      ui->interactiveTab->removeTab(index);
      replayAction->setChecked(false);
    }
  });
  connect(ui->interactiveTab, &QTabWidget::currentChanged, [this](int index) {
    if (index == 1) {
      int frame = ui->slider->value();
      replay->loadReplayFolder(dir);
      ui->slider->setValue(frame);
    }
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

  // Set the image preview limits
  connect(ui->startImage, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int startImage) {
    ui->stopImage->setRange(0, video->getImageCount() - startImage);
  });

  // Events filter to zoom in/out in the display
  ui->display->installEventFilter(this);
  ui->scrollArea->viewport()->installEventFilter(this);

  // Buttons connects
  ui->previewButton->setDisabled(true);
  ui->trackButton->setDisabled(true);
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
  connect(tracking, &Tracking::forceFinished, [this]() {
    message("An error occured.");
  });

  // Sets a color map
  colorMap.reserve(1000000);
  int a, b, c;
  srand((unsigned int)time(NULL));
  for (int j = 0; j < 1000000; ++j) {
    a = rand() % 255;
    b = rand() % 255;
    c = rand() % 255;
    colorMap.push_back(Point3i(a, b, c));
  }

  // Sets the object counter on top of the display
  counterLabel = new QLabel(ui->scrollArea->viewport());
  counterLabel->move(20, 20);

  video = nullptr;
}

/**
  * @brief Asks the path to a folder where an image sequence is stored. Setups the ui and resets the class attributs for a new analysis. Triggered when the open button from the menu bar is clicked.
*/
void Interactive::openFolder() {
  // Resets the class members
  replayAction->setChecked(false);
  isBackground = false;
  ui->interactiveTab->removeTab(1);
  memoryDir.clear();
  backgroundPath.clear();
  background.release();
  currentZoom = 1;

  // Resets the ui
  ui->display->clear();
  ui->isBin->setCheckable(false);
  ui->isSub->setCheckable(false);
  ui->isOriginal->setChecked(true);

  dir = QFileDialog::getOpenFileName(this, tr("Open File"), memoryDir);

  if (dir.length()) {
    // Finds the image format
    // Setups the class member
    try {
      memoryDir = dir;
      delete video;
      video = new VideoReader(dir.toStdString());
      ui->slider->setMinimum(0);
      ui->slider->setMaximum(video->getImageCount() - 1);
      ui->previewButton->setDisabled(true);
      ui->trackButton->setDisabled(true);
      ui->nBack->setMaximum(video->getImageCount() - 1);
      ui->nBack->setValue(video->getImageCount() - 1);
      ui->startImage->setRange(0, video->getImageCount() - 1);
      ui->startImage->setValue(0);
      replay->loadReplayFolder(dir);

      Mat frame;
      video->getImage(0, frame);
      cvtColor(frame, frame, COLOR_GRAY2RGB);
      originalImageSize.setWidth(frame.cols);
      originalImageSize.setHeight(frame.rows);
      cropedImageSize.setWidth(originalImageSize.width());
      cropedImageSize.setHeight(originalImageSize.height());
      ui->x2->setMaximum(frame.cols);
      ui->y2->setMaximum(frame.rows);

      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Path", Qt::MatchExactly)[0]), 1)->setText(dir);
      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Image number", Qt::MatchExactly)[0]), 1)->setText(QString::number(video->getImageCount()));
      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Image width", Qt::MatchExactly)[0]), 1)->setText(QString::number(frame.cols));
      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Image height", Qt::MatchExactly)[0]), 1)->setText(QString::number(frame.rows));

      isBackground = false;
      reset();

      // Automatic background computation type selection based on the image mean
      int meanValue = int(mean(frame)[0]);
      if (meanValue > 128) {
        ui->back->setCurrentIndex(0);
      }
      else {
        ui->back->setCurrentIndex(1);
      }
      if (video->isOpened()) {
        videoStatus = true;
      }
      display(0);
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
  * @param[in] scale Optional scale to display.
*/
void Interactive::display(int index, int scale) {
  if (videoStatus) {
    UMat frame;
    if (!video->getImage(index, frame)) {
      return;
    }
    vector<vector<Point>> displayContours;
    vector<vector<Point>> rejectedContours;

    // Computes the image with the background subtracted
    if (ui->isSub->isChecked() && isBackground) {
      (ui->backColor->currentText() == "Light background") ? (subtract(background, frame, frame)) : (subtract(frame, background, frame));
      // Crops the image
      if (roi.width != 0 || roi.height != 0) {
        frame = frame(roi);
      }
    }

    // Computes the binary image an applies morphological operations
    else if (ui->isBin->isChecked() && isBackground) {
      (ui->backColor->currentText() == "Light background") ? (subtract(background, frame, frame)) : (subtract(frame, background, frame));
      tracking->binarisation(frame, 'b', ui->threshBox->value());
      if (ui->morphOperation->currentIndex() != 8) {
        Mat element = getStructuringElement(ui->kernelType->currentIndex(), Size(2 * ui->kernelSize->value() + 1, 2 * ui->kernelSize->value() + 1), Point(ui->kernelSize->value(), ui->kernelSize->value()));
        morphologyEx(frame, frame, ui->morphOperation->currentIndex(), element);  // MorphTypes enum and QComboBox indexes have to match
      }

      // Keeps only right sized objects
      // To benchmark: pushBack contours vs loop over index and call drawContours multiple times

      // Crops the image
      if (roi.width != 0 || roi.height != 0) {
        frame = frame(roi);
      }

      vector<vector<Point>> contours;
      findContours(frame, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
      displayContours.reserve(contours.size());
      rejectedContours.reserve(contours.size());

      double min = ui->minSize->value();
      double max = ui->maxSize->value();

      // If too many contours are detected to be displayed without slowdowns, ask the user what to do
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

    else {
      // Crops the image
      if (roi.width != 0 || roi.height != 0) {
        frame = frame(roi);
      }
    }

    // Displays the image in the QLabel
    cvtColor(frame, frame, COLOR_GRAY2RGB);

    // Draws the scale
    if (scale != 0) {
      line(frame, Point(20, 20), Point(20 + scale, 20), Scalar(255, 0, 0), 2);
    }

    if (ui->isBin->isChecked()) {
      drawContours(frame, displayContours, -1, Scalar(0, 255, 0), FILLED, 8);
      drawContours(frame, rejectedContours, -1, Scalar(255, 0, 0), FILLED, 8);
    }
    Mat image = frame.getMat(ACCESS_READ);
    resizedPix = (QPixmap::fromImage(QImage(image.data, image.cols, image.rows, static_cast<int>(image.step), QImage::Format_RGB888)).scaled(ui->display->size(), Qt::KeepAspectRatio));
    ui->display->setPixmap(resizedPix);
    resizedFrame.setWidth(resizedPix.width());
    resizedFrame.setHeight(resizedPix.height());
  }
}

/**
  * @brief Zooms in the display.
*/
void Interactive::zoomIn() {
  currentZoom = abs(currentZoom);
  if (currentZoom < 2.75) {
    currentZoom += 0.25;
    ui->display->setFixedSize(ui->display->size() * 1.25);
    display(ui->slider->value());
  }
}

/**
  * @brief Zooms out the display.
*/
void Interactive::zoomOut() {
  currentZoom = abs(currentZoom);
  if (currentZoom > 0.75) {
    currentZoom -= 0.25;
    ui->display->setFixedSize(ui->display->size() / 1.25);
    display(ui->slider->value());
  }
  currentZoom *= -1;
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
  resizedPix = (QPixmap::fromImage(QImage(frame.data, frame.cols, frame.rows, static_cast<int>(frame.step), QImage::Format_RGB888)).scaled(ui->display->size(), Qt::KeepAspectRatio));
  ui->display->setPixmap(resizedPix);
  resizedFrame.setWidth(resizedPix.width());
  resizedFrame.setHeight(resizedPix.height());
}

/**
  * @brief Computes and displays the background image in the display. Triggered when the backgroundComputeButton is clicked.
*/
void Interactive::computeBackground() {
  if (videoStatus) {
    double nBack = double(ui->nBack->value());
    int method = ui->back->currentIndex();
    int registrationMethod = ui->registrationBack->currentIndex();

    ui->backgroundProgressBar->setMaximum(int(nBack));

    // Computes the background without blocking the ui
    QFuture<void> future = QtConcurrent::run([=]() {
      try {
        ui->slider->setEnabled(false);
        background = tracking->backgroundExtraction(*video, static_cast<int>(nBack), method, registrationMethod);
      }
      catch (const std::exception &ex) {
        message("An error occurs. Please change the registration method");
      }
      ui->slider->setEnabled(true);
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
      ui->previewButton->setDisabled(false);
      ui->trackButton->setDisabled(false);
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
    imread(backgroundPath.toStdString(), IMREAD_GRAYSCALE | IMREAD_ANYDEPTH).copyTo(background);
    if (background.cols == originalImageSize.width() && background.rows == originalImageSize.height()) {
      isBackground = true;

      ui->isBin->setCheckable(true);
      ui->isSub->setCheckable(true);
      ui->previewButton->setDisabled(false);
      ui->trackButton->setDisabled(false);

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
  parameters.insert("Maximal occlusion", QString::number(ui->lo->value()));
  parameters.insert("Maximal time", QString::number(ui->to->value()));
  parameters.insert("Normalization area", QString::number(ui->normArea->value()));
  parameters.insert("Normalization perimeter", QString::number(ui->normPerim->value()));

  parameters.insert("Binary threshold", QString::number(ui->threshBox->value()));
  parameters.insert("Number of images background", QString::number(ui->nBack->value()));
  parameters.insert("Background method", QString::number(ui->back->currentIndex()));
  parameters.insert("Background registration method", QString::number(ui->registrationBack->currentIndex()));
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
  if (videoStatus) {
    ui->progressBar->setRange(ui->startImage->value(), ui->stopImage->value() + ui->startImage->value() - 1);
    ui->progressBar->setValue(0);
    ui->previewButton->setDisabled(true);
    ui->trackButton->setDisabled(true);
    replayAction->setChecked(false);

    QThread *thread = new QThread;
    Tracking *tracking = new Tracking(memoryDir.toStdString(), background, ui->startImage->value(), ui->startImage->value() + ui->stopImage->value());
    tracking->moveToThread(thread);

    connect(thread, &QThread::started, tracking, &Tracking::startProcess);
    connect(tracking, &Tracking::progress, ui->progressBar, &QProgressBar::setValue);
    connect(tracking, &Tracking::forceFinished, [this]() {
      ui->slider->setDisabled(false);
      ui->previewButton->setDisabled(false);
      ui->trackButton->setDisabled(false);
      message("An error occurs during the tracking.");
    });
    connect(tracking, &Tracking::statistic, [this](int time) {
      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Analysis rate", Qt::MatchExactly)[0]), 1)->setText(QString::number(double(ui->stopImage->value() * 1000) / double(time)));
    });
    connect(tracking, &Tracking::finished, this, [this]() {
      ui->slider->setDisabled(false);
      ui->previewButton->setDisabled(false);
      ui->trackButton->setDisabled(false);
      replayAction->setChecked(true);
    });
    connect(tracking, &Tracking::forceFinished, this, [this]() {
      ui->slider->setDisabled(false);
      ui->previewButton->setDisabled(false);
      ui->trackButton->setDisabled(false);
    });
    connect(tracking, &Tracking::finished, thread, &QThread::quit);
    connect(tracking, &Tracking::finished, tracking, &Tracking::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    getParameters();
    tracking->updatingParameters(parameters);
    thread->start();

    ui->slider->setDisabled(true);
  }
}

/**
  * @brief Does a tracking analysis. Triggered when the trackButton is clicked.
*/
void Interactive::track() {
  if (videoStatus) {
    ui->progressBar->setRange(0, video->getImageCount() - 1);
    ui->progressBar->setValue(0);
    ui->previewButton->setDisabled(true);
    ui->trackButton->setDisabled(true);
    replayAction->setChecked(false);

    QThread *thread = new QThread;
    Tracking *tracking = new Tracking(memoryDir.toStdString(), background);
    QMap<QString, QString> *logMap = new QMap<QString, QString>;
    logMap->insert("date", QDateTime::currentDateTime().toString());
    logMap->insert("path", dir);
    tracking->moveToThread(thread);

    connect(thread, &QThread::started, tracking, &Tracking::startProcess);
    connect(tracking, &Tracking::forceFinished, [this, logMap]() {
      ui->slider->setDisabled(false);
      ui->previewButton->setDisabled(false);
      ui->trackButton->setDisabled(false);
      logMap->insert("status", "Error");
      emit(log(*logMap));
      message("An error occurs during the tracking.");
    });
    connect(tracking, &Tracking::progress, ui->progressBar, &QProgressBar::setValue);
    connect(tracking, &Tracking::statistic, [this, logMap](int time) {
      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Analysis rate", Qt::MatchExactly)[0]), 1)->setText(QString::number(double(video->getImageCount() * 1000) / double(time)));
      logMap->insert("time", QString::number(time));
    });
    connect(tracking, &Tracking::finished, thread, &QThread::quit);
    connect(tracking, &Tracking::finished, this, [this, logMap]() {
      ui->slider->setDisabled(false);
      ui->previewButton->setDisabled(false);
      ui->trackButton->setDisabled(false);
      replayAction->setChecked(true);
      logMap->insert("status", "Done");
      emit(log(*logMap));
    });
    connect(tracking, &Tracking::forceFinished, this, [this, logMap]() {
      ui->slider->setDisabled(false);
      ui->previewButton->setDisabled(false);
      ui->trackButton->setDisabled(false);
      replayAction->setChecked(true);
      logMap->insert("status", "Error");
      emit(log(*logMap));
    });
    connect(tracking, &Tracking::forceFinished, thread, &QThread::quit);
    connect(tracking, &Tracking::forceFinished, tracking, &Tracking::deleteLater);
    connect(tracking, &Tracking::finished, tracking, &Tracking::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    getParameters();
    tracking->updatingParameters(parameters);
    thread->start();

    ui->slider->setDisabled(true);
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
        clicks.first.setX(static_cast<unsigned int>(clicks.first.x() - 0.5 * (ui->display->width() - resizedFrame.width())));
        clicks.first.setY(static_cast<unsigned int>(clicks.first.y() - 0.5 * (ui->display->height() - resizedFrame.height())));
      }
    }

    // Sets the second point and draw the roi
    if (event->type() == QEvent::MouseMove) {
      QMouseEvent *moveEvent = static_cast<QMouseEvent *>(event);
      if (moveEvent->buttons() == Qt::LeftButton) {
        clicks.second = moveEvent->pos();
        // The QPixmap is V/Hcentered in the Qlabel widget
        // Gets the click coordinate in the frame of reference of the centered display
        clicks.second.setX(static_cast<unsigned int>(clicks.second.x() - 0.5 * (ui->display->width() - resizedFrame.width())));
        clicks.second.setY(static_cast<unsigned int>(clicks.second.y() - 0.5 * (ui->display->height() - resizedFrame.height())));

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
        ui->scrollArea->horizontalScrollBar()->setValue(static_cast<unsigned int>(ui->scrollArea->horizontalScrollBar()->value() + (panReferenceClick.x() - moveEvent->localPos().x())));
        ui->scrollArea->verticalScrollBar()->setValue(static_cast<unsigned int>(ui->scrollArea->verticalScrollBar()->value() + (panReferenceClick.y() - moveEvent->localPos().y())));
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
  if (xTop < roi.tl().x) xTop = roi.tl().x;
  if (yTop < roi.tl().y) yTop = roi.tl().y;
  if (width > cropedImageSize.width() - xTop + roi.tl().x) width = cropedImageSize.width() - xTop + roi.tl().x;
  if (height > cropedImageSize.height() - yTop + roi.tl().y) height = cropedImageSize.height() - yTop + roi.tl().y;

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

/**
  * @brief Destructors.
*/
Interactive::~Interactive() {
  saveSettings();
  delete tracking;
  delete video;
  delete ui;
}

/**
  * @brief Loads the settings.
*/
void Interactive::loadSettings() {
  QList<QString> keys = settingsFile->allKeys();
  for (auto &a : keys) {
    settings.insert(a, settingsFile->value(a).toString());
  }
}
/**
  * @brief Saves the settings.
*/
void Interactive::saveSettings() {
  QList<QString> keys = settings.keys();
  for (auto &a : keys) {
    settingsFile->setValue(a, settings.value(a));
  }
}
