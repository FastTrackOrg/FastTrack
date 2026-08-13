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
                                            videoStatus(false) {
  ui->setupUi(this);
  controls = new RuntimeControls;
  controls->progressBar = ui->trackingProgressBar;
  parameterValues = {{QStringLiteral("maxArea"), 0}, {QStringLiteral("minArea"), 0}, {QStringLiteral("spot"), 0}, {QStringLiteral("normDist"), 100.0}, {QStringLiteral("normAngle"), 180.0}, {QStringLiteral("maxDist"), 0}, {QStringLiteral("maxTime"), 10}, {QStringLiteral("normArea"), 0.0}, {QStringLiteral("normPerim"), 0.0}, {QStringLiteral("thresh"), 127}, {QStringLiteral("nBack"), 1}, {QStringLiteral("methBack"), 0}, {QStringLiteral("regBack"), 0}, {QStringLiteral("reg"), 0}, {QStringLiteral("morph"), 8}, {QStringLiteral("morphSize"), 0}, {QStringLiteral("morphType"), 2}, {QStringLiteral("lightBack"), 0}, {QStringLiteral("xTop"), 0}, {QStringLiteral("yTop"), 0}, {QStringLiteral("xBottom"), 0}, {QStringLiteral("yBottom"), 0}, {QStringLiteral("startImage"), 0}, {QStringLiteral("stopImage"), 0}, {QStringLiteral("frame"), 0}, {QStringLiteral("displayMode"), 0}};
  ui->interactiveTab->setFocusPolicy(Qt::NoFocus);

  // MetaType
  qRegisterMetaType<QHash<QString, double>>("QHash<QString, double>");

  connect(ui->interactiveTab, &QTabWidget::currentChanged, this, [this](int index) {
    emit activeViewChanged(index == 1 && replayVisible);
  });

  connect(this, &Interactive::message, this, [](const QString &msg) {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText(tr("Error"));
    msgBox.setInformativeText(msg);
    msgBox.exec();
  });

  replay = new Replay(this, video);
  connect(ui->interactiveTab, &QTabWidget::tabCloseRequested, this, [this](int index) {
    if (index != 0) {
      setReplayVisible(false);
    }
  });

  // Events filter to select ROI
  ui->display->installEventFilter(this);

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

void Interactive::setReplayVisible(bool visible) {
  if (replayVisible == visible) {
    return;
  }
  replayVisible = visible;
  if (visible) {
    ui->interactiveTab->addTab(replay, tr("Replay"));
    ui->interactiveTab->setCurrentIndex(1);
  }
  else {
    while (ui->interactiveTab->count() > 1) {
      ui->interactiveTab->removeTab(1);
    }
  }
  emit replayVisibleChanged(visible);
}

QHash<QString, QVariant> Interactive::parameterState() const {
  return parameterValues;
}

void Interactive::setParameterState(const QHash<QString, QVariant> &state) {
  if (state.isEmpty()) {
    return;
  }

  QScopedValueRollback<bool> applying(applyingParameterState, true);
  for (auto it = state.cbegin(); it != state.cend(); ++it) {
    parameterValues.insert(it.key(), it.value());
  }
  display(parameterInt(QStringLiteral("frame")));
}

Replay *Interactive::replayWidget() const {
  return replay;
}

void Interactive::preview() {
  previewTracking();
}

void Interactive::startTracking() {
  if (QMessageBox::question(this,
                            tr("Confirmation"),
                            tr("You are going to start a full tracking analysis. That can take some time, are you sure?"),
                            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
    track();
  }
}

void Interactive::computeWorkspaceBackground() {
  computeBackground();
}

void Interactive::selectWorkspaceBackground() {
  selectBackground();
}

void Interactive::cropWorkspace() {
  crop();
}

void Interactive::resetWorkspaceCrop() {
  reset();
}

void Interactive::levelWorkspaceParameters() {
  level();
}

bool Interactive::canPreview() const {
  return trackingAvailable;
}

bool Interactive::canTrack() const {
  return trackingAvailable;
}

bool Interactive::isReplayVisible() const {
  return replayVisible;
}

bool Interactive::isReplayActive() const {
  return replayVisible && ui->interactiveTab->currentIndex() == 1;
}

bool Interactive::isTimelineEnabled() const {
  return timelineEnabled;
}

int Interactive::frameCount() const {
  return video->isOpened() ? static_cast<int>(video->getImageCount()) : 0;
}

QList<QString> Interactive::informationValues() const {
  return informationValuesState;
}

QString Interactive::trackingStatusText() const {
  return trackingStatusState;
}

void Interactive::setTrackingAvailable(bool available) {
  trackingAvailable = available;
  emit trackingAvailabilityChanged();
}

int Interactive::parameterInt(const QString &name) const {
  return parameterValues.value(name).toInt();
}

double Interactive::parameterDouble(const QString &name) const {
  return parameterValues.value(name).toDouble();
}

void Interactive::setParameter(const QString &name, const QVariant &value) {
  if (parameterValues.value(name) == value) {
    return;
  }
  parameterValues.insert(name, value);
  notifyParameterStateChanged();
}

void Interactive::notifyParameterStateChanged() {
  if (!applyingParameterState) {
    emit parameterStateChanged(parameterState());
  }
}

void Interactive::updateProgressEstimate(int value) {
  controls->progressBar->setValue(value);
  const int completed = value - controls->progressBar->minimum() + 1;
  const int total = controls->progressBar->maximum() - controls->progressBar->minimum() + 1;
  if (completed <= 0 || total <= 0 || !trackingElapsed.isValid()) {
    return;
  }
  const qint64 remainingMs = trackingElapsed.elapsed() * (total - completed) / completed;
  const qint64 remainingSeconds = qMax<qint64>(0, remainingMs / 1000);
  const QString remaining = QStringLiteral("%1:%2")
                                .arg(remainingSeconds / 60, 2, 10, QLatin1Char('0'))
                                .arg(remainingSeconds % 60, 2, 10, QLatin1Char('0'));
  controls->progressBar->setFormat(tr("%p% - %1 remaining").arg(remaining));
}

/**
 * @brief Asks the path to a folder where an image sequence is stored. Setups the ui and resets the class attributs for a new analysis. Triggered when the open button from the menu bar is clicked.
 */
void Interactive::openFolder(QString path) {
  // Resets the class members
  setReplayVisible(false);
  videoStatus = false;
  isBackground = false;
  memoryDir.clear();
  backgroundPath.clear();
  background.release();

  // Resets the ui
  ui->display->clear();
  video->release();
  replay->clear();
  parameterValues.insert(QStringLiteral("displayMode"), 0);

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
      setTrackingAvailable(false);
      trackingStatusState = tr("Compute the background to continue");
      parameterValues.insert(QStringLiteral("nBack"), static_cast<int>(video->getImageCount()));
      parameterValues.insert(QStringLiteral("startImage"), 0);
      parameterValues.insert(QStringLiteral("stopImage"), static_cast<int>(video->getImageCount()));

      Mat frame;
      video->getImage(0, frame);
      cvtColor(frame, frame, COLOR_GRAY2RGB);
      originalImageSize.setWidth(frame.cols);
      originalImageSize.setHeight(frame.rows);
      cropedImageSize.setWidth(originalImageSize.width());
      cropedImageSize.setHeight(originalImageSize.height());

      informationValuesState[0] = dir;
      informationValuesState[1] = QString::number(video->getImageCount());
      informationValuesState[2] = QString::number(frame.cols);
      informationValuesState[3] = QString::number(frame.rows);

      isBackground = false;
      reset();

      // Load replay
      replay->loadReplay(dir);
      if (!replay->trackingData->isEmpty) {
        setReplayVisible(true);
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

        const QString savedBackground = QFileInfo(cfgFile).absoluteDir().filePath(QStringLiteral("background.pgm"));
        imread(savedBackground.toStdString(), IMREAD_GRAYSCALE | IMREAD_ANYDEPTH).copyTo(background);
        if (!background.empty() && background.cols == originalImageSize.width() && background.rows == originalImageSize.height()) {
          backgroundPath = savedBackground;
          isBackground = true;
          setTrackingAvailable(true);
          trackingStatusState.clear();
        }
      }
      if (video->isOpened()) {
        videoStatus = true;
        emit inputOpened(QFileInfo(dir).absoluteFilePath());
      }
      display(0);
      ui->display->fitToView();
      crop();
    }
    // If an error occurs during the opening, resets the information table and warns the user
    catch (exception &e) {
      informationValuesState = {QString(), QStringLiteral("0"), QStringLiteral("0"), QStringLiteral("0"), QStringLiteral("0")};
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
  if (!videoStatus || displayInProgress) {
    return;
  }
  QScopedValueRollback<bool> displayGuard(displayInProgress, true);

  UMat frame;
  if (!video->getImage(index, frame)) {
    return;
  }

  try {
    // Computes the image with the background subtracted
    if (parameterInt(QStringLiteral("displayMode")) == 1 && isBackground) {
      (parameterInt(QStringLiteral("lightBack")) == 0) ? (subtract(background, frame, frame)) : (subtract(frame, background, frame));
      cvtColor(frame, frame, COLOR_GRAY2RGB);
    }
    // Computes the binary image an applies morphological operations
    else if (parameterInt(QStringLiteral("displayMode")) == 2 && isBackground) {
      (parameterInt(QStringLiteral("lightBack")) == 0) ? (subtract(background, frame, frame)) : (subtract(frame, background, frame));
      Tracking::binarisation(frame, 'b', parameterInt(QStringLiteral("thresh")));
      if (parameterInt(QStringLiteral("morph")) != 8) {
        const int morphSize = parameterInt(QStringLiteral("morphSize"));
        Mat element = getStructuringElement(parameterInt(QStringLiteral("morphType")), Size(2 * morphSize + 1, 2 * morphSize + 1), Point(morphSize, morphSize));
        morphologyEx(frame, frame, parameterInt(QStringLiteral("morph")), element);
      }

      vector<vector<Point>> contours;
      findContours(frame, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

      double min = parameterDouble(QStringLiteral("minArea"));
      double max = parameterDouble(QStringLiteral("maxArea"));

      // If too many contours are detected to be displayed without slowdowns, ask the user what to do
      if (contours.size() > 10000) {
        if (largeContourDisplayDecision == 0 || contourWarningOpen) {
          return;
        }
        if (largeContourDisplayDecision < 0) {
          contourWarningOpen = true;
          auto *warning = new QMessageBox(
              QMessageBox::Question,
              tr("Confirmation"),
              tr("Too many objects detected to be displayed.\nDo you want to display them anyway (the program can be slow)?"),
              QMessageBox::No | QMessageBox::Yes,
              this);
          warning->setAttribute(Qt::WA_DeleteOnClose);
          connect(warning, &QMessageBox::finished, this, [this, index, scale](int result) {
            contourWarningOpen = false;
            largeContourDisplayDecision = (result == QMessageBox::Yes) ? 1 : 0;
            if (largeContourDisplayDecision == 1) {
              display(index, scale);
            }
          });
          warning->open();
          return;
        }
      }
      else {
        // Ask again after the contour count has dropped below the warning
        // threshold and subsequently crosses it again.
        largeContourDisplayDecision = -1;
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
 * @brief Computes and displays the background image in the display.
 */
void Interactive::computeBackground() {
  if (videoStatus) {
    // Before compute background process
    int nBack = parameterInt(QStringLiteral("nBack"));
    int method = parameterInt(QStringLiteral("methBack"));
    int registrationMethod = parameterInt(QStringLiteral("regBack"));
    const string videoPath = memoryDir.toStdString();
    QApplication::setOverrideCursor(Qt::WaitCursor);
    this->setEnabled(false);

    // After compute background process
    QFutureWatcher<UMat> *watcher = new QFutureWatcher<UMat>();
    connect(watcher, &QFutureWatcher<UMat>::finished, this, [this, watcher]() {
      background = watcher->result().clone();  // Clone needed for Windows otherwise no initial display and ui buggued afterward, why if UMat is a smartpointer?
      if (!background.empty()) {
        isBackground = true;
        // Automatic background type selection based on the image mean
        int meanValue = int(mean(background)[0]);
        parameterValues.insert(QStringLiteral("lightBack"), meanValue > 128 ? 0 : 1);

        ui->interactiveTab->setCurrentIndex(0);
        setTrackingAvailable(true);
        trackingStatusState.clear();
        display(background, QImage::Format_Grayscale8);
      }
      else {
        isBackground = false;
        parameterValues.insert(QStringLiteral("displayMode"), 0);
        setTrackingAvailable(false);
      }
      this->setEnabled(true);
      QApplication::restoreOverrideCursor();
      watcher->deleteLater();
    });

    // Compute background process
    QFuture<UMat> future = QtConcurrent::run([=]() {
      UMat background;
      try {
        VideoReader backgroundVideo(videoPath);
        if (!backgroundVideo.open()) {
          throw std::runtime_error("Background computation error: the video can not be opened.");
        }
        background = Tracking::backgroundExtraction(backgroundVideo, nBack, method, registrationMethod);
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
 * @brief Opens a dialogue to select a background image.
 */
void Interactive::selectBackground() {
  QString dir = QFileDialog::getOpenFileName(this, tr("Open Background Image"), memoryDir);

  if (dir.length()) {
    backgroundPath = dir;
    imread(backgroundPath.toStdString(), IMREAD_GRAYSCALE | IMREAD_ANYDEPTH).copyTo(background);
    if (background.cols == originalImageSize.width() && background.rows == originalImageSize.height()) {
      isBackground = true;

      setTrackingAvailable(true);
      trackingStatusState.clear();

      // Automatic background type selection based on image mean
      int meanValue = int(mean(background)[0]);
      parameterValues.insert(QStringLiteral("lightBack"), meanValue > 128 ? 0 : 1);

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
  for (auto it = parameterValues.cbegin(); it != parameterValues.cend(); ++it) {
    parameters.insert(it.key(), it.value().toString());
  }
  parameters.insert(QStringLiteral("xTop"), QString::number(roi.tl().x));
  parameters.insert(QStringLiteral("yTop"), QString::number(roi.tl().y));
  parameters.insert(QStringLiteral("xBottom"), QString::number(roi.br().x));
  parameters.insert(QStringLiteral("yBottom"), QString::number(roi.br().y));
}

/**
 * @brief Does a tracking analysis on a sub-part of the image sequence.
 */
void Interactive::previewTracking() {
  if (videoStatus) {
    const int startImage = parameterInt(QStringLiteral("startImage"));
    const int stopImage = parameterInt(QStringLiteral("stopImage"));
    controls->progressBar->setRange(startImage, startImage + stopImage - 1);
    controls->progressBar->setValue(0);
    controls->progressBar->setFormat(tr("%p% - estimating..."));
    trackingElapsed.start();
    setTrackingAvailable(false);
    setReplayVisible(false);
    replay->clear();  // Avoid mixing 2 subsequent analysy

    QThread *thread = new QThread;
    Tracking *tracking = new Tracking(memoryDir.toStdString(), background, startImage, startImage + stopImage);
    tracking->moveToThread(thread);

    connect(thread, &QThread::started, tracking, &Tracking::startProcess);
    connect(tracking, &Tracking::progress, this, &Interactive::updateProgressEstimate);
    connect(tracking, &Tracking::statistic, this, [this, stopImage](int time) {
      informationValuesState[4] = QString::number(double(stopImage * 1000) / double(time));
    });
    connect(tracking, &Tracking::finished, this, [this]() {
      controls->progressBar->setValue(controls->progressBar->maximum());
      controls->progressBar->setFormat(tr("Done"));
      timelineEnabled = true;
      emit timelineEnabledChanged(timelineEnabled);
      setTrackingAvailable(true);
      replay->loadReplay(dir);
      setReplayVisible(true);
    });
    connect(tracking, &Tracking::forceFinished, this, [this](const QString &errorMessage) {
      controls->progressBar->setFormat(tr("Stopped"));
      timelineEnabled = true;
      emit timelineEnabledChanged(timelineEnabled);
      setTrackingAvailable(true);
      replay->loadReplay(dir);
      setReplayVisible(true);
      emit message(errorMessage);
    });
    connect(tracking, &Tracking::forceFinished, thread, &QThread::quit);
    connect(tracking, &Tracking::forceFinished, tracking, &Tracking::deleteLater);
    connect(tracking, &Tracking::finished, tracking, &Tracking::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    getParameters();
    tracking->updatingParameters(parameters);
    thread->start();

    timelineEnabled = false;
    emit timelineEnabledChanged(timelineEnabled);
  }
}

/**
 * @brief Does a full tracking analysis.
 */
void Interactive::track() {
  if (videoStatus) {
    controls->progressBar->setRange(0, static_cast<int>(video->getImageCount()) - 1);
    controls->progressBar->setValue(0);
    controls->progressBar->setFormat(tr("%p% - estimating..."));
    trackingElapsed.start();
    setTrackingAvailable(false);
    setReplayVisible(false);
    replay->clear();  // Avoid mixing 2 subsequent analysy

    QThread *thread = new QThread;
    Tracking *tracking = new Tracking(memoryDir.toStdString(), background);
    QSharedPointer<QHash<QString, QString>> logMap(new QHash<QString, QString>);
    logMap->insert(QStringLiteral("date"), QDateTime::currentDateTime().toString());
    logMap->insert(QStringLiteral("path"), dir);
    tracking->moveToThread(thread);

    connect(thread, &QThread::started, tracking, &Tracking::startProcess);
    connect(tracking, &Tracking::progress, this, &Interactive::updateProgressEstimate);
    connect(tracking, &Tracking::statistic, this, [this, logMap](int time) {
      informationValuesState[4] = QString::number(double(video->getImageCount() * 1000) / double(time));
      logMap->insert(QStringLiteral("time"), QString::number(time));
    });
    connect(tracking, &Tracking::forceFinished, this, [this, logMap](const QString &errorMessage) {
      controls->progressBar->setFormat(tr("Stopped"));
      timelineEnabled = true;
      emit timelineEnabledChanged(timelineEnabled);
      setTrackingAvailable(true);
      replay->loadReplay(dir);
      setReplayVisible(true);
      logMap->insert(QStringLiteral("status"), errorMessage);
      emit log(*logMap);
      emit status(tr("Tracking failed"));
      emit message(errorMessage);
    });
    connect(tracking, &Tracking::finished, thread, &QThread::quit);
    connect(tracking, &Tracking::finished, this, [this, logMap]() {
      controls->progressBar->setValue(controls->progressBar->maximum());
      controls->progressBar->setFormat(tr("Done"));
      timelineEnabled = true;
      emit timelineEnabledChanged(timelineEnabled);
      setTrackingAvailable(true);
      replay->loadReplay(dir);
      setReplayVisible(true);
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

    timelineEnabled = false;
    emit timelineEnabledChanged(timelineEnabled);
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
        parameterValues.insert(QStringLiteral("xTop"), xTop + roi.tl().x);
        parameterValues.insert(QStringLiteral("yTop"), yTop + roi.tl().y);
        parameterValues.insert(QStringLiteral("xBottom"), xBottom + roi.tl().x);
        parameterValues.insert(QStringLiteral("yBottom"), yBottom + roi.tl().y);
        notifyParameterStateChanged();
      }
    }
  }
  return QWidget::eventFilter(target, event);
}

/**
 * @brief Crops the image from a rectangle drawed by the user with the mouse on the display. Triggered when the QPushButton ui->crop is clicked.
 */
void Interactive::crop() {
  int xTop = parameterInt(QStringLiteral("xTop"));
  int yTop = parameterInt(QStringLiteral("yTop"));
  int xBottom = parameterInt(QStringLiteral("xBottom"));
  int yBottom = parameterInt(QStringLiteral("yBottom"));

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
  informationValuesState[2] = QString::number(roi.width);
  informationValuesState[3] = QString::number(roi.height);
  display(parameterInt(QStringLiteral("frame")));

  parameterValues.insert(QStringLiteral("xTop"), roi.tl().x);
  parameterValues.insert(QStringLiteral("yTop"), roi.tl().y);
  parameterValues.insert(QStringLiteral("xBottom"), roi.br().x);
  parameterValues.insert(QStringLiteral("yBottom"), roi.br().y);
  notifyParameterStateChanged();

  ui->display->setRectangle(QRect());
}

/**
 * @brief Resets the region of interest. Triggered by the reset button.
 */
void Interactive::reset() {
  cropedImageSize.setWidth(originalImageSize.width());
  cropedImageSize.setHeight(originalImageSize.height());
  parameterValues.insert(QStringLiteral("xTop"), 0);
  parameterValues.insert(QStringLiteral("yTop"), 0);
  parameterValues.insert(QStringLiteral("xBottom"), originalImageSize.width());
  parameterValues.insert(QStringLiteral("yBottom"), originalImageSize.height());
  informationValuesState[2] = QString::number(originalImageSize.width());
  informationValuesState[3] = QString::number(originalImageSize.height());
  roi = Rect(0, 0, 0, 0);
  ui->display->setRectangle(QRect());
  display(parameterInt(QStringLiteral("frame")));
  notifyParameterStateChanged();
}

/**
 * @brief Destructors.
 */
Interactive::~Interactive() {
  ui->interactiveTab->setCurrentIndex(0);
  delete video;
  delete controls;
  delete ui;
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
        parameterValues.insert(QStringLiteral("normAngle"), levelParameters.value(QStringLiteral("normAngle")));
        parameterValues.insert(QStringLiteral("normDist"), levelParameters.value(QStringLiteral("normDist")));
        parameterValues.insert(QStringLiteral("normArea"), levelParameters.value(QStringLiteral("normArea")));
        parameterValues.insert(QStringLiteral("normPerim"), levelParameters.value(QStringLiteral("normPerim")));
        notifyParameterStateChanged();
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
    for (auto it = parameterList.cbegin(); it != parameterList.cend(); ++it) {
      parameterValues.insert(it.key(), it.value());
    }
    if (parameterInt(QStringLiteral("xBottom")) == 0) {
      parameterValues.insert(QStringLiteral("xBottom"), originalImageSize.width());
    }
    if (parameterInt(QStringLiteral("yBottom")) == 0) {
      parameterValues.insert(QStringLiteral("yBottom"), originalImageSize.height());
    }
    notifyParameterStateChanged();
  }
  parameterFile.close();
}
