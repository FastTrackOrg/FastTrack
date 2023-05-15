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
                                            ui(new Ui::Interactive),
                                            video(new VideoReader()),
                                            videoStatus(false),
                                            settingsFile(new QSettings(QStringLiteral("FastTrack"), QStringLiteral("FastTrackOrg"), this)) {
  ui->setupUi(this);
  ui->menuBar->setNativeMenuBar(false);

  // Loads settings
  settingsFile->beginGroup(QStringLiteral("interactive"));
  restoreState(settingsFile->value(QStringLiteral("windowState")).toByteArray());

  // MetaType
  qRegisterMetaType<QHash<QString, double>>("QHash<QString, double>");

  // DockWidget
  connect(ui->imageOptions, &QDockWidget::dockLocationChanged, this, [this](Qt::DockWidgetArea area) {
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
  connect(ui->trackingOptions, &QDockWidget::dockLocationChanged, this, [this](Qt::DockWidgetArea area) {
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
  connect(ui->actionOpen, &QAction::triggered, this, [this]() { this->openFolder(); });
  connect(ui->actionQuit, &QAction::triggered, qApp, &QApplication::quit);
  ui->menuView->addAction(ui->imageOptions->toggleViewAction());
  ui->menuView->addAction(ui->trackingOptions->toggleViewAction());
  ui->menuView->addAction(ui->controlOptions->toggleViewAction());

  replayAction = new QAction(tr("Tracking replay"), this);
  replayAction->setCheckable(true);
  connect(replayAction, &QAction::toggled, this, [this](bool isChecked) {
    if (isChecked) {
      ui->interactiveTab->addTab(replay, tr("Replay"));
      ui->interactiveTab->setCurrentIndex(1);
    }
    else {
      ui->interactiveTab->removeTab(1);  // Remove Replay tab
      ui->interactiveTab->removeTab(1);  // Remove Analysis tab
    }
  });
  ui->menuView->addAction(replayAction);

  connect(ui->slider, &Timeline::valueChanged, this, [this](int newValue) {
    int index = ui->interactiveTab->currentIndex();
    if (index == 0) {
      display(newValue);
    }
    else if (index == 1) {
      replay->sliderConnection(newValue);
    }
  });

  connect(ui->interactiveTab, &QTabWidget::currentChanged, this, [this](int index) {
    ui->slider->setValue(ui->slider->value());  // Stay on the same frame when changing tab
    if (index == 0) {                           // Interactive tracking
      ui->imageOptions->setVisible(true);
      ui->trackingOptions->setVisible(true);
      ui->controlOptions->setVisible(true);
    }
    else if (index == 1) {  // Replay tab
      ui->imageOptions->setVisible(false);
      ui->trackingOptions->setVisible(false);
      ui->controlOptions->setVisible(true);
    }
    else if (index == 2) {  // Analysis tab
      ui->imageOptions->setVisible(false);
      ui->trackingOptions->setVisible(false);
      ui->controlOptions->setVisible(false);
    }
  });

  // Loads prefered style
  QStringList styles = QStyleFactory::keys();
  style = settingsFile->value(QStringLiteral("window/style"), "Fusion").toString();
  QMenu *menuStyle = new QMenu(tr("Appearance"), this);
  ui->menuSettings->addMenu(menuStyle);

  for (auto const &a : styles) {
    QAction *styleAction = new QAction(a, this);
    styleAction->setCheckable(true);
    connect(styleAction, &QAction::triggered, this, [this, styleAction, menuStyle]() {
      QApplication::setStyle(QStyleFactory::create(styleAction->text()));
      style = styleAction->text();
      foreach (QAction *action, menuStyle->actions()) {
        action->setChecked(false);
      }
      styleAction->setChecked(true);
    });
    menuStyle->addAction(styleAction);
  }

  if (styles.contains(style)) {
    QApplication::setStyle(QStyleFactory::create(style));
    foreach (QAction *action, menuStyle->actions()) {
      if (action->text() == style) {
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
  QAction *ftColor = new QAction(tr("FastTrack"), this);
  ftColor->setCheckable(true);
  menuPalette->addAction(ftColor);
  connect(defaultColor, &QAction::triggered, this, [this, defaultColor, menuPalette]() {
    foreach (QAction *action, menuPalette->actions()) {
      action->setChecked(false);
    }
    defaultColor->setChecked(false);
    qApp->setStyleSheet(QLatin1String(""));
    color = QStringLiteral("default");
  });
  connect(darkColor, &QAction::triggered, this, [this, darkColor, menuPalette]() {
    foreach (QAction *action, menuPalette->actions()) {
      action->setChecked(false);
    }
    darkColor->setChecked(true);
    QFile file(QStringLiteral(":/dark.qss"));
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    qApp->setStyleSheet(stream.readAll());
    color = QStringLiteral("dark");
  });
  connect(lightColor, &QAction::triggered, this, [this, lightColor, menuPalette]() {
    foreach (QAction *action, menuPalette->actions()) {
      action->setChecked(false);
    }
    lightColor->setChecked(true);
    QFile file(QStringLiteral(":/light.qss"));
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    qApp->setStyleSheet(stream.readAll());
    color = QStringLiteral("light");
  });
  connect(ftColor, &QAction::triggered, this, [this, ftColor, menuPalette]() {
    foreach (QAction *action, menuPalette->actions()) {
      action->setChecked(false);
    }
    ftColor->setChecked(true);
    QFile file(QStringLiteral(":/theme.qss"));
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    qApp->setStyleSheet(stream.readAll());
    color = QStringLiteral("ft");
  });

  color = settingsFile->value(QStringLiteral("color"), "ft").toString();
  if (color == QLatin1String("light")) {
    lightColor->trigger();
  }
  else if (color == QLatin1String("dark")) {
    darkColor->trigger();
  }
  else if (color == QLatin1String("ft")) {
    ftColor->trigger();
  }
  else {
    ftColor->trigger();
  }

  // Layout option serves as a template of QDock placement.
  // At closing, the customized layout is saved and restored.
  // This, no action is check by default of the layoutGroup.
  QActionGroup *layoutGroup = new QActionGroup(this);
  ui->menuBar->removeAction(ui->menuLayout->menuAction());
  ui->menuSettings->addMenu(ui->menuLayout);
  layoutGroup->addAction(ui->actionLayout1);
  connect(ui->actionLayout1, &QAction::triggered, this, [this]() {
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
  });
  layoutGroup->addAction(ui->actionLayout2);
  connect(ui->actionLayout2, &QAction::triggered, this, [this]() {
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
  });
  layoutGroup->addAction(ui->actionLayout3);
  connect(ui->actionLayout3, &QAction::triggered, this, [this]() {
    ui->controlOptions->setVisible(true);
    ui->imageOptions->setVisible(true);
    ui->trackingOptions->setVisible(true);

    ui->controlOptions->setFloating(false);
    ui->imageOptions->setFloating(false);
    ui->trackingOptions->setFloating(false);

    addDockWidget(Qt::LeftDockWidgetArea, ui->imageOptions);
    addDockWidget(Qt::RightDockWidgetArea, ui->trackingOptions);
    addDockWidget(Qt::BottomDockWidgetArea, ui->controlOptions);
  });
  layoutGroup->addAction(ui->actionLayout4);
  connect(ui->actionLayout4, &QAction::triggered, this, [this]() {
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
  });

  QAction *actionMode = new QAction(tr("Expert Mode"), this);
  actionMode->setCheckable(true);
  isExpert = settingsFile->value(QStringLiteral("mode"), false).toBool();
  actionMode->setChecked(isExpert);
  connect(actionMode, &QAction::toggled, this, [this](bool isChecked) {
    emit modeChanged(isChecked);
    isExpert = isChecked;
  });
  QTimer::singleShot(500, actionMode, [this]() {
    emit modeChanged(isExpert);
  });  // Need to wait for the connection initialization
  ui->menuSettings->addAction(actionMode);

  // Help menu
  connect(ui->actionDoc, &QAction::triggered, []() {
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://www.fasttrack.sh/docs/intro"), QUrl::TolerantMode));
  });
  connect(ui->actionTuto, &QAction::triggered, []() {
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://www.youtube.com/watch?v=RzzmcZs04E4&list=PLGjsUpRojSmO4RHrd-TbpbNpJrfjNYlIm"), QUrl::TolerantMode));
  });
  connect(ui->actionAsk, &QAction::triggered, []() {
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://github.com/FastTrackOrg/FastTrack/discussions"), QUrl::TolerantMode));
  });
  connect(ui->actionIssue, &QAction::triggered, []() {
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://github.com/FastTrackOrg/FastTrack/issues"), QUrl::TolerantMode));
  });
  connect(ui->actionContact, &QAction::triggered, []() {
    QDesktopServices::openUrl(QUrl(QStringLiteral("mailto:benjamin.gallois@fasttrack.sh?subject=[fasttrack]"), QUrl::TolerantMode));
  });
  connect(ui->actionGenerateLog, &QAction::triggered, this, [this]() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Log File"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), tr("Logs (*.log)"));
    QFile::remove(fileName);
    QFile::copy(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/fasttrack.log", fileName);
  });
  connect(ui->actionSupport, &QAction::triggered, []() {
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://ko-fi.com/bgallois"), QUrl::TolerantMode));
  });
  connect(ui->actionAbout, &QAction::triggered, this, []() {
    QMessageBox aboutBox;
    aboutBox.setText(tr("FastTrack version %1 is a desktop tracking software, easy to install, easy to use, and performant.<br>Created and maintained by Benjamin Gallois.<br>Distributed under the terms of the <a href='https://www.gnu.org/licenses/gpl-3.0'>GPL3.0 license</a>.<br>").arg(QApplication::applicationVersion()));
    aboutBox.exec();
  });
  connect(ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);

  connect(this, &Interactive::message, this, [](const QString &msg) {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText(tr("Error"));
    msgBox.setInformativeText(msg);
    msgBox.exec();
  });

  // Threshold slider and combobox
  connect(ui->threshSlider, &QSlider::valueChanged, ui->threshBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::setValue));
  connect(ui->threshBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), ui->threshSlider, &QSlider::setValue);
  connect(ui->threshBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [this]() {
    ui->isBin->setChecked(true);
    ui->interactiveTab->setCurrentIndex(0);
    display(ui->slider->value());
  });

  // Draws scale
  connect(ui->lo, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [this](int scale) {
    display(ui->slider->value(), scale);
  });
  connect(ui->maxL, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](int scale) {
    display(ui->slider->value(), static_cast<int>(scale));
  });

  // Replay tab
  replay = new Replay(this, false, ui->slider, video);
  connect(ui->interactiveTab, &QTabWidget::tabCloseRequested, this, [this](int index) {
    if (index != 0) {
      replayAction->setChecked(false);  // The replayAction takes care of the tabs removing
    }
  });

  // Updates the display after each operation
  connect(ui->morphOperation, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, [this]() {
    display(ui->slider->value());
  });
  connect(ui->kernelSize, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [this]() {
    ui->isBin->setChecked(true);
    display(ui->slider->value());
  });
  connect(ui->kernelType, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, [this]() {
    ui->isBin->setChecked(true);
    display(ui->slider->value());
  });
  connect(ui->minSize, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [this]() {
    ui->isBin->setChecked(true);
    display(ui->slider->value());
  });
  connect(ui->maxSize, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [this]() {
    ui->isBin->setChecked(true);
    display(ui->slider->value());
  });
  connect(ui->backColor, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, [this]() {
    ui->isBin->setChecked(true);
    display(ui->slider->value());
  });
  connect(ui->isBin, &QRadioButton::clicked, this, [this]() {
    display(ui->slider->value());
  });
  connect(ui->isOriginal, &QRadioButton::clicked, this, [this]() {
    display(ui->slider->value());
  });
  connect(ui->isSub, &QRadioButton::clicked, this, [this]() {
    display(ui->slider->value());
  });

  // Set the image preview limits
  connect(ui->startImage, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [this](int startImage) {
    ui->stopImage->setRange(0, static_cast<int>(video->getImageCount()) - startImage);
  });

  // Events filter to select ROI
  ui->display->installEventFilter(this);

  // Buttons connects
  ui->previewButton->setDisabled(true);
  ui->trackButton->setDisabled(true);
  connect(ui->backgroundSelectButton, &QPushButton::clicked, this, &Interactive::selectBackground);
  connect(ui->backgroundComputeButton, &QPushButton::clicked, this, &Interactive::computeBackground);
  connect(ui->previewButton, &QPushButton::clicked, this, &Interactive::previewTracking);
  connect(ui->trackButton, &QPushButton::clicked, this, [this]() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Confirmation"), tr("You are going to start a full tracking analysis. That can take some time, are you sure?"), QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
      track();
    }
  });
  connect(ui->cropButton, &QPushButton::clicked, this, &Interactive::crop);
  connect(ui->resetButton, &QPushButton::clicked, this, &Interactive::reset);
  connect(ui->levelButton, &QPushButton::clicked, this, &Interactive::level);

  // Sets information table
  ui->informationTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  isBackground = false;

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
  counterLabel = new QLabel(ui->display);  // TODO put as QGraphicsItem
  counterLabel->move(20, 20);
}

/**
 * @brief Asks the path to a folder where an image sequence is stored. Setups the ui and resets the class attributs for a new analysis. Triggered when the open button from the menu bar is clicked.
 */
void Interactive::openFolder(QString path) {
  // Resets the class members
  replayAction->setChecked(false);
  isBackground = false;
  ui->interactiveTab->removeTab(1);
  memoryDir.clear();
  backgroundPath.clear();
  background.release();

  // Resets the ui
  ui->display->clear();
  video->release();
  replay->clear();
  ui->backgroundProgressBar->setValue(0);
  ui->isBin->setCheckable(false);
  ui->isSub->setCheckable(false);
  ui->isOriginal->setChecked(true);
  ui->backgroundStatus->setText(tr("No background selected"));
  ui->backgroundStatus->setStyleSheet(QStringLiteral("background: red; color: white;"));

  if (path.isEmpty()) {
    dir = QFileDialog::getOpenFileName(this, tr("Open File"), memoryDir);
  }
  else {
    dir = path;
  }

  QApplication::setOverrideCursor(Qt::WaitCursor);
  if (dir.length()) {
    // Finds the image format
    // Setups the class member
    try {
      memoryDir = dir;
      video->open(dir.toStdString());
      ui->slider->setMinimum(0);
      ui->slider->setMaximum(static_cast<int>(video->getImageCount()) - 1);
      ui->previewButton->setDisabled(true);
      ui->trackButton->setDisabled(true);
      ui->trackingStatus->setText(tr("Compute the background to continue"));
      ui->nBack->setMaximum(static_cast<int>(video->getImageCount()));
      ui->nBack->setValue(static_cast<int>(video->getImageCount()));
      ui->startImage->setRange(0, static_cast<int>(video->getImageCount()) - 1);
      ui->startImage->setValue(0);

      Mat frame;
      video->getImage(0, frame);
      cvtColor(frame, frame, COLOR_GRAY2RGB);
      originalImageSize.setWidth(frame.cols);
      originalImageSize.setHeight(frame.rows);
      cropedImageSize.setWidth(originalImageSize.width());
      cropedImageSize.setHeight(originalImageSize.height());
      ui->x1->setMaximum(originalImageSize.width());
      ui->y1->setMaximum(originalImageSize.height());
      ui->x2->setMaximum(originalImageSize.width());
      ui->y2->setMaximum(originalImageSize.height());

      // Need to be changed for traduction
      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems(QStringLiteral("Path"), Qt::MatchExactly).at(0)), 1)->setText(dir);
      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems(QStringLiteral("Image number"), Qt::MatchExactly).at(0)), 1)->setText(QString::number(video->getImageCount()));
      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems(QStringLiteral("Image width"), Qt::MatchExactly).at(0)), 1)->setText(QString::number(frame.cols));
      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems(QStringLiteral("Image height"), Qt::MatchExactly).at(0)), 1)->setText(QString::number(frame.rows));

      isBackground = false;
      reset();

      // Load replay
      replay->loadReplay(dir);
      if (!replay->trackingData->isEmpty) {
        replayAction->setChecked(true);
      }

      // Load parameters
      QFileInfo savingInfo(dir);
      QString savingFilename = savingInfo.baseName();
      QString cfgFile = savingInfo.absolutePath();
      if (video->isSequence()) {
        cfgFile.append(QStringLiteral("/Tracking_Result") + QDir::separator() + "cfg.toml");
      }
      else {
        cfgFile.append(QStringLiteral("/Tracking_Result_") + savingFilename + QDir::separator() + "cfg.toml");
      }
      if (QFileInfo::exists(cfgFile)) {
        loadParameters(cfgFile);
      }
      if (video->isOpened()) {
        videoStatus = true;
      }
      display(0);
      ui->display->fitToView();
      crop();
    }
    // If an error occurs during the opening, resets the information table and warns the user
    catch (exception &e) {
      // Need to be changed for traduction
      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems(QStringLiteral("Path"), Qt::MatchExactly).at(0)), 1)->setText(QLatin1String(""));
      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems(QStringLiteral("Image number"), Qt::MatchExactly).at(0)), 1)->setText(QStringLiteral("0"));
      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems(QStringLiteral("Image width"), Qt::MatchExactly).at(0)), 1)->setText(QStringLiteral("0"));
      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems(QStringLiteral("Image height"), Qt::MatchExactly).at(0)), 1)->setText(QStringLiteral("0"));
      qWarning() << QString::fromStdString(e.what()) << "occurs during opening of " << dir;
      emit message(tr("No image found."));
    }
  }
  QApplication::restoreOverrideCursor();
}

/**
 * @brief Displays the image at index in the image sequence in the ui.
 * @param[in] index Index of the image to display in the image sequence.
 * @param[in] scale Optional scale to display.
 */
void Interactive::display(int index, int scale) {
  if (!videoStatus) {
    return;
  }

  UMat frame;
  if (!video->getImage(index, frame)) {
    return;
  }

  try {
    // Computes the image with the background subtracted
    if (ui->isSub->isChecked() && isBackground) {
      (ui->backColor->currentText() == QLatin1String("Light background")) ? (subtract(background, frame, frame)) : (subtract(frame, background, frame));
      cvtColor(frame, frame, COLOR_GRAY2RGB);
    }
    // Computes the binary image an applies morphological operations
    else if (ui->isBin->isChecked() && isBackground) {
      (ui->backColor->currentText() == QLatin1String("Light background")) ? (subtract(background, frame, frame)) : (subtract(frame, background, frame));
      Tracking::binarisation(frame, 'b', ui->threshBox->value());
      if (ui->morphOperation->currentIndex() != 8) {
        Mat element = getStructuringElement(ui->kernelType->currentIndex(), Size(2 * ui->kernelSize->value() + 1, 2 * ui->kernelSize->value() + 1), Point(ui->kernelSize->value(), ui->kernelSize->value()));
        morphologyEx(frame, frame, ui->morphOperation->currentIndex(), element);  // MorphTypes enum and QComboBox indexes have to match
      }

      vector<vector<Point>> contours;
      findContours(frame, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

      double min = ui->minSize->value();
      double max = ui->maxSize->value();

      // If too many contours are detected to be displayed without slowdowns, ask the user what to do
      if (contours.size() > 10000) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Confirmation"), tr("Too many objects detected to be displayed. \n Do you want to display them anyway (the program can be slow)? "), QMessageBox::No | QMessageBox::Yes);
        if (reply == QMessageBox::No) {
          return;
        }
      }

      vector<vector<Point>> displayContours;
      vector<vector<Point>> rejectedContours;
      displayContours.reserve(contours.size());
      rejectedContours.reserve(contours.size());
      for (auto const &a : contours) {
        double size = contourArea(a);
        if (size > min && size < max) {
          displayContours.push_back(a);
        }
        else {
          rejectedContours.push_back(a);
        }
      }
      cvtColor(frame, frame, COLOR_GRAY2RGB);
      drawContours(frame, displayContours, -1, Scalar(0, 255, 0), FILLED, 8);
      drawContours(frame, rejectedContours, -1, Scalar(255, 0, 0), FILLED, 8);
      counterLabel->setText("Objects detected: " + QString::number(displayContours.size()));
      counterLabel->adjustSize();
    }
    else {
      cvtColor(frame, frame, COLOR_GRAY2RGB);
    }

    // Draws the scale
    if (scale != 0) {
      line(frame, Point(20, 20), Point(20 + scale, 20), Scalar(255, 0, 0), 2);
    }

    // Crops the image
    if ((roi.width != 0 || roi.height != 0) && (roi.width != originalImageSize.width() || roi.height != originalImageSize.height())) {
      frame = frame(roi);
    }
    display(frame);
  }
  catch (const std::exception &e) {
    qWarning() << QString::fromStdString(e.what()) << " occurs at image " << index << " display";
    emit message(QString::fromStdString(e.what()) + tr(" occurs on image %1.").arg(index));
  }
  catch (...) {
    qWarning() << "Unknown error occurs at image " << index << " display";
    emit message(tr("An error occurs on image %1.").arg(index));
  }
}

/**
 * @brief This is an overloaded function to display a QImage in the display.
 */
void Interactive::display(const QImage &image) {
  ui->display->setImage(image);
}

/**
 * @brief This is an overloaded function to display a UMat in the display.
 * @param[in] image UMat to display.
 * @param[in] format enum QImage::Format.
 */
void Interactive::display(const UMat &image, QImage::Format format) {
  // getMat is needed instead of accessing data directly through u->data because the offset generated by the crop is not applied otherwise.
  QImage qimage(image.getMat(ACCESS_FAST).data, image.cols, image.rows, static_cast<int>(image.step), format);
  ui->display->setImage(qimage);
}

/**
 * @brief Computes and displays the background image in the display. Triggered when the backgroundComputeButton is clicked.
 */
void Interactive::computeBackground() {
  if (videoStatus) {
    // Before compute background process
    int nBack = ui->nBack->value();
    int method = ui->back->currentIndex();
    int registrationMethod = ui->registrationBack->currentIndex();
    ui->backgroundProgressBar->setValue(0);
    ui->backgroundProgressBar->setMaximum(0);
    ui->backgroundComputeButton->setEnabled(false);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    this->setEnabled(false);

    // After compute background process
    QFutureWatcher<UMat> *watcher = new QFutureWatcher<UMat>();
    connect(watcher, &QFutureWatcher<UMat>::finished, this, [this, watcher]() {
      background = watcher->result().clone();  // Clone needed for Windows otherwise no initial display and ui buggued afterward, why if UMat is a smartpointer?
      if (!background.empty()) {
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

        ui->interactiveTab->setCurrentIndex(0);
        ui->previewButton->setDisabled(false);
        ui->trackButton->setDisabled(false);
        ui->backgroundStatus->setText(tr("Using COMPUTED background"));
        ui->backgroundStatus->setStyleSheet(QStringLiteral("background: green; color: white;"));
        ui->trackingStatus->setText(QStringLiteral());
        display(background, QImage::Format_Grayscale8);
      }
      else {
        isBackground = false;
        ui->isBin->setCheckable(false);
        ui->isSub->setCheckable(false);
        ui->previewButton->setDisabled(true);
        ui->trackButton->setDisabled(true);
      }
      ui->backgroundProgressBar->setMaximum(1);
      ui->backgroundProgressBar->setValue(1);
      ui->backgroundComputeButton->setDisabled(false);
      this->setEnabled(true);
      QApplication::restoreOverrideCursor();
      watcher->deleteLater();
    });

    // Compute background process
    QFuture<UMat> future = QtConcurrent::run([=]() {
      UMat background;
      try {
        background = Tracking::backgroundExtraction(*video, nBack, method, registrationMethod);
      }
      catch (const std::runtime_error &e) {
        qWarning() << QString::fromStdString(e.what()) << "occurs during background computation";
        emit message(e.what());
      }
      catch (...) {
        qWarning() << "Unknown error occurs during background computation";
        emit message(tr("An error occurs. Please change the registration method"));
      }
      return background;
    });
    watcher->setFuture(future);
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
      ui->backgroundStatus->setText(tr("Using %1 as background").arg(dir));
      ui->backgroundStatus->setStyleSheet(QStringLiteral("background: green; color: white;"));
      ui->trackingStatus->setText(QStringLiteral());

      // Automatic background type selection based on image mean
      int meanValue = int(mean(background)[0]);
      if (meanValue > 128) {
        ui->backColor->setCurrentIndex(0);
      }
      else {
        ui->backColor->setCurrentIndex(1);
      }

      display(background, QImage::Format_Grayscale8);
    }
    else {
      isBackground = false;
      emit message(tr("The width or height of the background image does not match the video width or height."));
    }
  }
}

/**
 * @brief Gets all the tracking parameters from the ui and updates the parameter map that will be passed to the tracking object.
 */
void Interactive::getParameters() {
  parameters.insert(QStringLiteral("maxArea"), QString::number(ui->maxSize->value()));
  parameters.insert(QStringLiteral("minArea"), QString::number(ui->minSize->value()));
  parameters.insert(QStringLiteral("spot"), QString::number(ui->spot->currentIndex()));
  parameters.insert(QStringLiteral("normDist"), QString::number(ui->maxL->value()));
  parameters.insert(QStringLiteral("normAngle"), QString::number(ui->maxT->value()));
  parameters.insert(QStringLiteral("maxDist"), QString::number(ui->lo->value()));
  parameters.insert(QStringLiteral("maxTime"), QString::number(ui->to->value()));
  parameters.insert(QStringLiteral("normArea"), QString::number(ui->normArea->value()));
  parameters.insert(QStringLiteral("normPerim"), QString::number(ui->normPerim->value()));

  parameters.insert(QStringLiteral("thresh"), QString::number(ui->threshBox->value()));
  parameters.insert(QStringLiteral("nBack"), QString::number(ui->nBack->value()));
  parameters.insert(QStringLiteral("methBack"), QString::number(ui->back->currentIndex()));
  parameters.insert(QStringLiteral("regBack"), QString::number(ui->registrationBack->currentIndex()));
  parameters.insert(QStringLiteral("xTop"), QString::number(roi.tl().x));
  parameters.insert(QStringLiteral("yTop"), QString::number(roi.tl().y));
  parameters.insert(QStringLiteral("xBottom"), QString::number(roi.br().x));
  parameters.insert(QStringLiteral("yBottom"), QString::number(roi.br().y));
  parameters.insert(QStringLiteral("reg"), QString::number(ui->reg->currentIndex()));
  parameters.insert(QStringLiteral("morph"), QString::number(ui->morphOperation->currentIndex()));
  parameters.insert(QStringLiteral("morphSize"), QString::number(ui->kernelSize->value()));
  parameters.insert(QStringLiteral("morphType"), QString::number(ui->kernelType->currentIndex()));
  parameters.insert(QStringLiteral("lightBack"), QString::number(ui->backColor->currentIndex()));
}

/**
 * @brief Does a tracking analysis on a sub-part of the image sequence defined by the user. Triggered when previewButton is clicked.
 */
void Interactive::previewTracking() {
  if (videoStatus) {
    ui->progressBar->setRange(ui->startImage->value(), ui->stopImage->value() + ui->startImage->value() - 1);
    ui->progressBar->setValue(0);
    ui->previewButton->setDisabled(true);
    ui->trackButton->setDisabled(true);
    replayAction->setChecked(false);
    replay->clear();  // Avoid mixing 2 subsequent analysy

    QThread *thread = new QThread;
    Tracking *tracking = new Tracking(memoryDir.toStdString(), background, ui->startImage->value(), ui->startImage->value() + ui->stopImage->value());
    tracking->moveToThread(thread);

    connect(thread, &QThread::started, tracking, &Tracking::startProcess);
    connect(tracking, &Tracking::progress, ui->progressBar, &QProgressBar::setValue);
    connect(tracking, &Tracking::statistic, this, [this](int time) {
      // Need to be changed for traduction
      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems(QStringLiteral("Analysis rate"), Qt::MatchExactly).at(0)), 1)->setText(QString::number(double(ui->stopImage->value() * 1000) / double(time)));
    });
    connect(tracking, &Tracking::finished, this, [this]() {
      ui->slider->setDisabled(false);
      ui->previewButton->setDisabled(false);
      ui->trackButton->setDisabled(false);
      replay->loadReplay(dir);
      replayAction->setChecked(true);
    });
    connect(tracking, &Tracking::forceFinished, this, [this](const QString &errorMessage) {
      ui->slider->setDisabled(false);
      ui->previewButton->setDisabled(false);
      ui->trackButton->setDisabled(false);
      replay->loadReplay(dir);
      replayAction->setChecked(true);
      emit message(errorMessage);
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
 * @brief Does a tracking analysis. Triggered when the trackButton is clicked.
 */
void Interactive::track() {
  if (videoStatus) {
    ui->progressBar->setRange(0, static_cast<int>(video->getImageCount()) - 1);
    ui->progressBar->setValue(0);
    ui->previewButton->setDisabled(true);
    ui->trackButton->setDisabled(true);
    replayAction->setChecked(false);
    replay->clear();  // Avoid mixing 2 subsequent analysy

    QThread *thread = new QThread;
    Tracking *tracking = new Tracking(memoryDir.toStdString(), background);
    QSharedPointer<QHash<QString, QString>> logMap(new QHash<QString, QString>);
    logMap->insert(QStringLiteral("date"), QDateTime::currentDateTime().toString());
    logMap->insert(QStringLiteral("path"), dir);
    tracking->moveToThread(thread);

    connect(thread, &QThread::started, tracking, &Tracking::startProcess);
    connect(tracking, &Tracking::progress, ui->progressBar, &QProgressBar::setValue);
    connect(tracking, &Tracking::statistic, this, [this, logMap](int time) {
      // Need to be changed for traduction
      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems(QStringLiteral("Analysis rate"), Qt::MatchExactly).at(0)), 1)->setText(QString::number(double(video->getImageCount() * 1000) / double(time)));
      logMap->insert(QStringLiteral("time"), QString::number(time));
    });
    connect(tracking, &Tracking::forceFinished, this, [this, logMap](const QString &errorMessage) {
      ui->slider->setDisabled(false);
      ui->previewButton->setDisabled(false);
      ui->trackButton->setDisabled(false);
      replay->loadReplay(dir);
      replayAction->setChecked(true);
      logMap->insert(QStringLiteral("status"), errorMessage);
      emit log(*logMap);
      emit status(tr("Tracking failed"));
      emit message(errorMessage);
    });
    connect(tracking, &Tracking::finished, thread, &QThread::quit);
    connect(tracking, &Tracking::finished, this, [this, logMap]() {
      ui->slider->setDisabled(false);
      ui->previewButton->setDisabled(false);
      ui->trackButton->setDisabled(false);
      replay->loadReplay(dir);
      replayAction->setChecked(true);
      logMap->insert(QStringLiteral("status"), QStringLiteral("Done"));
      emit log(*logMap);
      emit status(tr("Tracking succeeded"));
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
      QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent *>(event);
      if (mouseEvent->buttons() == Qt::LeftButton) {
        clicks.first = ui->display->mapToScene(mouseEvent->pos());
      }
    }

    // Sets the second point and draw the roi
    if (event->type() == QEvent::MouseMove) {
      QMouseEvent *moveEvent = dynamic_cast<QMouseEvent *>(event);
      if (moveEvent->buttons() == Qt::LeftButton) {
        clicks.second = ui->display->mapToScene(moveEvent->pos());

        // Draws the ROI with
        QRect roiRect = QRect(clicks.first.x(), clicks.first.y(), clicks.second.x() - clicks.first.x(), clicks.second.y() - clicks.first.y());
        ui->display->setRectangle(roiRect);

        // Updates ui value
        // Find the rectangle top corner
        int xTop, yTop, xBottom, yBottom;
        if (clicks.first.y() < clicks.second.y()) {
          xTop = clicks.first.x();
          yTop = clicks.first.y();
          xBottom = clicks.second.x();
          yBottom = clicks.second.y();
        }
        else {
          xBottom = clicks.first.x();
          yBottom = clicks.first.y();
          xTop = clicks.second.x();
          yTop = clicks.second.y();
        }
        // Find the left corner of the rectangle
        int width = xBottom - xTop;
        if (width < 0) {
          xTop += width;
          xBottom -= width;
        }
        // Converts clicks from display widget frame of reference to original image frame of reference
        ui->x1->setValue(xTop + roi.tl().x);
        ui->y1->setValue(yTop + roi.tl().y);
        ui->x2->setValue(xBottom + roi.tl().x);
        ui->y2->setValue(yBottom + roi.tl().y);
      }
    }
  }
  return QWidget::eventFilter(target, event);
}

/**
 * @brief Crops the image from a rectangle drawed by the user with the mouse on the display. Triggered when the QPushButton ui->crop is clicked.
 */
void Interactive::crop() {
  int xTop = ui->x1->value();
  int yTop = ui->y1->value();
  int xBottom = ui->x2->value();
  int yBottom = ui->y2->value();

  // Checks for wrong values
  int width = xBottom - xTop;
  int height = yBottom - yTop;
  if (xTop < roi.tl().x) xTop = roi.tl().x;
  if (yTop < roi.tl().y) yTop = roi.tl().y;
  if (width > cropedImageSize.width() - xTop + roi.tl().x) width = cropedImageSize.width() - xTop + roi.tl().x;
  if (height > cropedImageSize.height() - yTop + roi.tl().y) height = cropedImageSize.height() - yTop + roi.tl().y;

  roi = Rect(xTop, yTop, width, height);
  cropedImageSize.setWidth(roi.width);
  cropedImageSize.setHeight(roi.height);
  // Need to be changed for traduction
  ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems(QStringLiteral("Image width"), Qt::MatchExactly).at(0)), 1)->setText(QString::number(roi.width));
  ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems(QStringLiteral("Image height"), Qt::MatchExactly).at(0)), 1)->setText(QString::number(roi.height));
  display(ui->slider->value());

  // Sets the roi limits
  ui->x1->setMaximum(roi.tl().x + roi.width);
  ui->y1->setMaximum(roi.tl().y + roi.height);
  ui->x2->setMaximum(roi.tl().x + roi.width);
  ui->y2->setMaximum(roi.tl().y + roi.height);
  ui->x1->setMinimum(roi.tl().x);
  ui->y1->setMinimum(roi.tl().y);
  ui->x2->setMinimum(roi.tl().x);
  ui->y2->setMinimum(roi.tl().y);

  ui->display->setRectangle(QRect());
}

/**
 * @brief Resets the region of interest. Triggered by the reset button.
 */
void Interactive::reset() {
  cropedImageSize.setWidth(originalImageSize.width());
  cropedImageSize.setHeight(originalImageSize.height());
  ui->x1->setMaximum(originalImageSize.width());
  ui->y1->setMaximum(originalImageSize.height());
  ui->x2->setMaximum(originalImageSize.width());
  ui->y2->setMaximum(originalImageSize.height());
  ui->x1->setMinimum(0);
  ui->y1->setMinimum(0);
  ui->x2->setMinimum(0);
  ui->y2->setMinimum(0);
  ui->x1->setValue(0);
  ui->y1->setValue(0);
  ui->x2->setValue(originalImageSize.width());
  ui->y2->setValue(originalImageSize.height());
  // Need to be changed for traduction
  ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems(QStringLiteral("Image width"), Qt::MatchExactly).at(0)), 1)->setText(QString::number(originalImageSize.width()));
  ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems(QStringLiteral("Image height"), Qt::MatchExactly).at(0)), 1)->setText(QString::number(originalImageSize.height()));
  roi = Rect(0, 0, 0, 0);
  ui->display->setRectangle(QRect());
  display(ui->slider->value());
}

/**
 * @brief Destructors.
 */
Interactive::~Interactive() {
  ui->interactiveTab->setCurrentIndex(0);
  saveSettings();
  delete video;
  delete ui;
}

/**
 * @brief Saves the settings.
 */
void Interactive::saveSettings() {
  settingsFile->setValue(QStringLiteral("style"), style);
  settingsFile->setValue(QStringLiteral("color"), color);
  settingsFile->setValue(QStringLiteral("mode"), isExpert);
  settingsFile->setValue(QStringLiteral("windowState"), saveState());
}

/**
 * @brief Level the parameters.
 */
void Interactive::level() {
  if (videoStatus) {
    try {
      QThread *thread = new QThread;
      getParameters();
      AutoLevel *autolevel = new AutoLevel(memoryDir.toStdString(), background, parameters);
      autolevel->moveToThread(thread);

      connect(thread, &QThread::started, autolevel, &AutoLevel::level);
      connect(autolevel, &AutoLevel::forceFinished, this, [this](const QString &errorMessage) {
        this->setEnabled(true);
        emit message(errorMessage);
      });
      connect(autolevel, &AutoLevel::finished, this, [this]() {
        this->setEnabled(true);
      });
      connect(autolevel, &AutoLevel::levelParametersChanged, this, [this](const QHash<QString, double> &levelParameters) {
        ui->maxT->setValue(levelParameters.value(QStringLiteral("normAngle")));
        ui->maxL->setValue(levelParameters.value(QStringLiteral("normDist")));
        ui->normArea->setValue(levelParameters.value(QStringLiteral("normArea")));
        ui->normPerim->setValue(levelParameters.value(QStringLiteral("normPerim")));
        QApplication::restoreOverrideCursor();
      });
      connect(autolevel, &AutoLevel::finished, thread, &QThread::quit);
      connect(autolevel, &AutoLevel::finished, autolevel, &AutoLevel::deleteLater);
      connect(autolevel, &AutoLevel::forceFinished, thread, &QThread::quit);
      connect(autolevel, &AutoLevel::forceFinished, autolevel, &AutoLevel::deleteLater);
      connect(thread, &QThread::finished, thread, &QThread::deleteLater);

      QApplication::setOverrideCursor(Qt::WaitCursor);
      thread->start();
      this->setEnabled(false);
    }
    catch (const std::exception &e) {
      qWarning() << QString::fromStdString(e.what());
      emit message(QString::fromStdString(e.what()));
    }
  }
}

/**
 * @brief Reads a parameter file, updates parameters.
 */
void Interactive::loadParameters(const QString &path) {
  QFile parameterFile(path);
  if (parameterFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&parameterFile);
    QString line;
    QStringList parameters;
    QHash<QString, QString> parameterList;
    while (in.readLineInto(&line)) {
      if (line.contains(QLatin1String("="))) {
        parameters = line.split(QStringLiteral("="), Qt::SkipEmptyParts);
        parameterList.insert(parameters[0].trimmed(), parameters[1].trimmed());
      }
    }
    ui->maxSize->setValue(parameterList.value(QStringLiteral("maxArea")).toInt());
    ui->minSize->setValue(parameterList.value(QStringLiteral("minArea")).toInt());
    ui->spot->setCurrentIndex(parameterList.value(QStringLiteral("spot")).toInt());
    ui->maxL->setValue(parameterList.value(QStringLiteral("normDist")).toDouble());
    ui->maxT->setValue(parameterList.value(QStringLiteral("normAngle")).toDouble());
    ui->lo->setValue(parameterList.value(QStringLiteral("maxDist")).toInt());
    ui->to->setValue(parameterList.value(QStringLiteral("maxTime")).toInt());
    ui->normArea->setValue(parameterList.value(QStringLiteral("normArea")).toDouble());
    ui->normPerim->setValue(parameterList.value(QStringLiteral("normPerim")).toDouble());

    ui->threshBox->setValue(parameterList.value(QStringLiteral("thresh")).toInt());
    ui->nBack->setValue(parameterList.value(QStringLiteral("nBack")).toInt());
    ui->back->setCurrentIndex(parameterList.value(QStringLiteral("methBack")).toInt());
    ui->registrationBack->setCurrentIndex(parameterList.value(QStringLiteral("regBack")).toInt());
    ui->x1->setValue(parameterList.value(QStringLiteral("xTop")).toInt());
    ui->y1->setValue(parameterList.value(QStringLiteral("yTop")).toInt());
    (parameterList.value(QStringLiteral("xBottom")).toInt() == 0) ? ui->x2->setValue(originalImageSize.width()) : ui->x2->setValue(parameterList.value(QStringLiteral("xBottom")).toInt());
    (parameterList.value(QStringLiteral("yBottom")).toInt() == 0) ? ui->y2->setValue(originalImageSize.height()) : ui->y2->setValue(parameterList.value(QStringLiteral("yBottom")).toInt());
    ui->reg->setCurrentIndex(parameterList.value(QStringLiteral("reg")).toInt());
    ui->backColor->setCurrentIndex(parameterList.value(QStringLiteral("lightBack")).toInt());
    ui->morphOperation->setCurrentIndex(parameterList.value(QStringLiteral("morph")).toInt());
    ui->kernelSize->setValue(parameterList.value(QStringLiteral("morphSize")).toInt());
    ui->kernelType->setCurrentIndex(parameterList.value(QStringLiteral("morphType")).toInt());
  }
  parameterFile.close();
}

/**
 * @brief Gets drop events that have url and try to open the file.
 */
void Interactive::dropEvent(QDropEvent *dropEvent) {
  const QMimeData *mimeData = dropEvent->mimeData();
  if (mimeData->hasUrls()) {
    QUrl file = mimeData->urls().at(0);
    this->openFolder(file.toLocalFile());
  }
}

/**
 * @brief Accepts all the drag enter event.
 */
void Interactive::dragEnterEvent(QDragEnterEvent *event) {
  event->acceptProposedAction();
}
