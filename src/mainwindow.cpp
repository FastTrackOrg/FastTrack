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

#include "mainwindow.h"

#include <QActionGroup>
#include <QButtonGroup>
#include <QDesktopServices>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QHeaderView>
#include <QMenuBar>
#include <QMimeData>
#include <QPushButton>
#include <QSignalBlocker>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QTabWidget>
#include <QTextStream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

/**
 * @class MainWindow
 *
 * @brief The MainWindow class is derived from a QMainWindow widget. It displays the main window of the program.
 *
 * @author Benjamin Gallois
 *
 * @version $Revision: 4.0 $
 *
 * Contact: gallois.benjamin08@gmail.com
 *
 */

/**
 * @brief Constructs the MainWindow QObject and initializes the UI.
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow),
                                          updater(new Updater(this)),
                                          settingsFile(new QSettings(QStringLiteral("FastTrack"), QStringLiteral("FastTrackOrg"), this)) {
  QDir::setCurrent(QCoreApplication::applicationDirPath());
  ui->setupUi(this);
  setAcceptDrops(true);
  setWindowTitle(qApp->applicationName() + " " + APP_VERSION);

  settingsFile->beginGroup(QStringLiteral("main"));
  restoreGeometry(settingsFile->value(QStringLiteral("geometry")).toByteArray());
  savedDockState = settingsFile->value(QStringLiteral("dockState")).toByteArray();
  const QString style = settingsFile->value(QStringLiteral("style"), QStringLiteral("Fusion")).toString();
  if (QStyleFactory::keys().contains(style)) {
    QApplication::setStyle(QStyleFactory::create(style));
  }
  applyTheme(settingsFile->value(QStringLiteral("theme"), QStringLiteral("ft")).toString());

  // Tray icon
  trayIcon = new QSystemTrayIcon(QIcon(":/assets/icon.svg"), this);
  QMenu *trayMenu = new QMenu(this);
  QAction *restore = trayMenu->addAction(tr("Restore"));
  connect(restore, &QAction::triggered, this, &MainWindow::showNormal);
  QAction *close = trayMenu->addAction(tr("Close"));
  connect(close, &QAction::triggered, this, &MainWindow::showNormal);  // Needed for close to work when window is hidden
  connect(close, &QAction::triggered, this, &MainWindow::close);
  connect(trayIcon, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
      case QSystemTrayIcon::Trigger: {
        this->setVisible(this->isHidden());
        break;
      }
      case QSystemTrayIcon::DoubleClick: {
        break;
      }
      default:;
    }
  });
  trayIcon->setContextMenu(trayMenu);
  trayIcon->show();

  updater->checkForUpdate();

  fileMenu = menuBar()->addMenu(tr("&File"));
  viewMenu = menuBar()->addMenu(tr("&View"));
  settingsMenu = menuBar()->addMenu(tr("&Settings"));
  helpMenu = menuBar()->addMenu(tr("&Help"));
  imageOptionsDock = ui->imageOptionsDock;
  trackingOptionsDock = ui->trackingOptionsDock;
  controlOptionsDock = ui->controlOptionsDock;
  replayDisplayDock = ui->replayDisplayDock;
  annotationDock = ui->annotationDock;
  informationDock = ui->informationDock;
  QButtonGroup *displayModeGroup = new QButtonGroup(this);
  displayModeGroup->addButton(ui->isOriginal, 0);
  displayModeGroup->addButton(ui->isSub, 1);
  displayModeGroup->addButton(ui->isBin, 2);
  if (savedDockState.isEmpty()) {
    addDockWidget(Qt::LeftDockWidgetArea, imageOptionsDock);
    addDockWidget(Qt::LeftDockWidgetArea, trackingOptionsDock);
    addDockWidget(Qt::LeftDockWidgetArea, replayDisplayDock);
    addDockWidget(Qt::LeftDockWidgetArea, annotationDock);
    addDockWidget(Qt::LeftDockWidgetArea, informationDock);
    tabifyDockWidget(imageOptionsDock, trackingOptionsDock);
    tabifyDockWidget(trackingOptionsDock, replayDisplayDock);
    tabifyDockWidget(replayDisplayDock, annotationDock);
    tabifyDockWidget(annotationDock, informationDock);
  }
  imageOptionsDock->hide();
  trackingOptionsDock->hide();
  controlOptionsDock->hide();
  replayDisplayDock->hide();
  annotationDock->hide();
  informationDock->hide();

  parameterControls = {{QStringLiteral("methBack"), ui->back},
                       {QStringLiteral("nBack"), ui->nBack},
                       {QStringLiteral("regBack"), ui->registrationBack},
                       {QStringLiteral("lightBack"), ui->backColor},
                       {QStringLiteral("thresh"), ui->threshBox},
                       {QStringLiteral("maxArea"), ui->maxSize},
                       {QStringLiteral("minArea"), ui->minSize},
                       {QStringLiteral("xTop"), ui->x1},
                       {QStringLiteral("yTop"), ui->y1},
                       {QStringLiteral("xBottom"), ui->x2},
                       {QStringLiteral("yBottom"), ui->y2},
                       {QStringLiteral("maxTime"), ui->to},
                       {QStringLiteral("maxDist"), ui->lo},
                       {QStringLiteral("spot"), ui->spot},
                       {QStringLiteral("normDist"), ui->maxL},
                       {QStringLiteral("normAngle"), ui->maxT},
                       {QStringLiteral("normArea"), ui->normArea},
                       {QStringLiteral("normPerim"), ui->normPerim},
                       {QStringLiteral("reg"), ui->reg},
                       {QStringLiteral("morphSize"), ui->kernelSize},
                       {QStringLiteral("morph"), ui->morphOperation},
                       {QStringLiteral("morphType"), ui->kernelType},
                       {QStringLiteral("startImage"), ui->startImage},
                       {QStringLiteral("stopImage"), ui->stopImage}};
  const auto applyParameters = [this]() { applyParameterControls(); };
  for (QWidget *control : parameterControls) {
    if (QSpinBox *box = qobject_cast<QSpinBox *>(control)) {
      connect(box, QOverload<int>::of(&QSpinBox::valueChanged), this, applyParameters);
    }
    else if (QDoubleSpinBox *box = qobject_cast<QDoubleSpinBox *>(control)) {
      connect(box, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, applyParameters);
    }
    else if (QComboBox *box = qobject_cast<QComboBox *>(control)) {
      connect(box, QOverload<int>::of(&QComboBox::currentIndexChanged), this, applyParameters);
    }
  }
  connect(ui->backgroundComputeButton, &QPushButton::clicked, this, [this]() { if (Interactive *interactive = activeInteractive()) interactive->computeWorkspaceBackground(); });
  connect(ui->backgroundSelectButton, &QPushButton::clicked, this, [this]() { if (Interactive *interactive = activeInteractive()) interactive->selectWorkspaceBackground(); });
  connect(ui->cropButton, &QPushButton::clicked, this, [this]() { if (Interactive *interactive = activeInteractive()) interactive->cropWorkspace(); });
  connect(ui->resetButton, &QPushButton::clicked, this, [this]() { if (Interactive *interactive = activeInteractive()) interactive->resetWorkspaceCrop(); });
  connect(ui->levelButton, &QPushButton::clicked, this, [this]() { if (Interactive *interactive = activeInteractive()) interactive->levelWorkspaceParameters(); });
  connect(ui->threshSlider, &QSlider::valueChanged, ui->threshBox, QOverload<int>::of(&QSpinBox::setValue));
  connect(ui->threshBox, QOverload<int>::of(&QSpinBox::valueChanged), ui->threshSlider, &QSlider::setValue);
  const auto selectBinaryDisplay = [this]() {
    if (Interactive *interactive = activeInteractive()) {
      QHash<QString, QVariant> state = workspaceStates.value(interactive, interactive->parameterState());
      state.insert(QStringLiteral("displayMode"), 2);
      workspaceStates.insert(interactive, state);
      ui->isBin->setChecked(true);
      interactive->setParameterState(state);
    }
  };
  for (QWidget *control : {static_cast<QWidget *>(ui->threshBox),
                           static_cast<QWidget *>(ui->maxSize),
                           static_cast<QWidget *>(ui->minSize),
                           static_cast<QWidget *>(ui->backColor),
                           static_cast<QWidget *>(ui->kernelSize),
                           static_cast<QWidget *>(ui->morphOperation),
                           static_cast<QWidget *>(ui->kernelType)}) {
    if (QSpinBox *box = qobject_cast<QSpinBox *>(control)) {
      connect(box, QOverload<int>::of(&QSpinBox::valueChanged), this, selectBinaryDisplay);
    }
    else if (QComboBox *box = qobject_cast<QComboBox *>(control)) {
      connect(box, QOverload<int>::of(&QComboBox::currentIndexChanged), this, selectBinaryDisplay);
    }
  }
  connect(ui->slider, &Timeline::valueChanged, this, [this](int frame) {
    if (Interactive *interactive = activeInteractive()) {
      QHash<QString, QVariant> state = workspaceStates.value(interactive, interactive->parameterState());
      state.insert(QStringLiteral("frame"), frame);
      workspaceStates.insert(interactive, state);
      if (interactive->isReplayActive()) {
        interactive->replayWidget()->sliderConnection(frame);
      }
      else {
        interactive->setParameterState(state);
      }
    }
  });
  const auto applyDisplayMode = [this](int mode) {
    if (Interactive *interactive = activeInteractive()) {
      QHash<QString, QVariant> state = workspaceStates.value(interactive, interactive->parameterState());
      state.insert(QStringLiteral("displayMode"), mode);
      workspaceStates.insert(interactive, state);
      interactive->setParameterState(state);
    }
  };
  connect(ui->isOriginal, &QRadioButton::clicked, this, [applyDisplayMode]() { applyDisplayMode(0); });
  connect(ui->isSub, &QRadioButton::clicked, this, [applyDisplayMode]() { applyDisplayMode(1); });
  connect(ui->isBin, &QRadioButton::clicked, this, [applyDisplayMode]() { applyDisplayMode(2); });

  replayEllipseBox = ui->replayEllipseBox;
  replayArrowBox = ui->replayArrowBox;
  replayTraceBox = ui->replayTraceBox;
  replayTraceLengthBox = ui->replayTraceLengthBox;
  replayNumbersBox = ui->replayNumbersBox;
  replaySizeBox = ui->replaySizeBox;
  replayFpsBox = ui->replayFpsBox;
  annotationFindLine = ui->annotationFindLine;
  annotationEdit = ui->annotationEdit;
  informationTable1 = ui->informationTable1;
  informationTable2 = ui->informationTable2;
  const QStringList replayShapes = {tr("Head + Tail"), tr("Head"), tr("Tail"), tr("Body"), tr("None")};
  replayEllipseBox->addItems(replayShapes);
  replayArrowBox->addItems(replayShapes);
  replayTraceLengthBox->setRange(1, 50000);
  replaySizeBox->setRange(1, 150);
  replayFpsBox->setRange(1, 1000);
  for (QTableWidget *table : {informationTable1, informationTable2}) {
    table->setRowCount(5);
    table->setColumnCount(1);
    table->setHorizontalHeaderLabels({tr("Value")});
    table->setVerticalHeaderLabels({tr("Object id"), tr("First appearance"), tr("Area (px)"), tr("Perimeter (px)"), tr("Eccentricity")});
    for (int row = 0; row < table->rowCount(); ++row) {
      table->setItem(row, 0, new QTableWidgetItem(QStringLiteral("0")));
    }
    table->horizontalHeader()->setStretchLastSection(true);
  }
  const auto applyReplay = [this]() { applyReplayDisplayControls(); };
  connect(replayEllipseBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, applyReplay);
  connect(replayArrowBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, applyReplay);
  connect(replayTraceBox, &QCheckBox::toggled, this, applyReplay);
  connect(replayTraceLengthBox, QOverload<int>::of(&QSpinBox::valueChanged), this, applyReplay);
  connect(replayNumbersBox, &QCheckBox::toggled, this, applyReplay);
  connect(replaySizeBox, QOverload<int>::of(&QSpinBox::valueChanged), this, applyReplay);
  connect(replayFpsBox, QOverload<int>::of(&QSpinBox::valueChanged), this, applyReplay);
  connect(annotationEdit, &QTextEdit::textChanged, this, [this]() { if (Interactive *interactive = activeInteractive()) interactive->replayWidget()->setAnnotationText(annotationEdit->toPlainText()); });
  connect(annotationFindLine, &QLineEdit::textEdited, this, [this](const QString &text) { if (Interactive *interactive = activeInteractive()) interactive->replayWidget()->findAnnotation(text); });
  connect(ui->annotationNextButton, &QPushButton::clicked, this, [this]() { if (Interactive *interactive = activeInteractive()) interactive->replayWidget()->nextAnnotation(); });
  connect(ui->annotationPreviousButton, &QPushButton::clicked, this, [this]() { if (Interactive *interactive = activeInteractive()) interactive->replayWidget()->previousAnnotation(); });
  QToolBar *workspaceToolBar = addToolBar(tr("Tracking"));
  workspaceToolBar->setObjectName(QStringLiteral("trackingToolBar"));
  previewAction = workspaceToolBar->addAction(QIcon(":/assets/buttons/preview.png"), tr("Preview"));
  previewAction->setToolTip(tr("Preview tracking for the active workspace"));
  trackAction = workspaceToolBar->addAction(QIcon(":/assets/buttons/track.png"), tr("Track"));
  trackAction->setToolTip(tr("Start tracking for the active workspace"));
  connect(previewAction, &QAction::triggered, this, [this]() {
    if (Interactive *interactive = activeInteractive()) {
      interactive->preview();
      updateWorkspaceActions();
    }
  });
  connect(trackAction, &QAction::triggered, this, [this]() {
    if (Interactive *interactive = activeInteractive()) {
      interactive->startTracking();
      updateWorkspaceActions();
    }
  });

  mdiModeMenu = new QMenu(tr("Window mode"), this);
  QActionGroup *viewModeGroup = new QActionGroup(this);
  QAction *stackedAction = mdiModeMenu->addAction(tr("Stacked"));
  stackedAction->setCheckable(true);
  QAction *floatingAction = mdiModeMenu->addAction(tr("Floating"));
  floatingAction->setCheckable(true);
  viewModeGroup->addAction(stackedAction);
  viewModeGroup->addAction(floatingAction);
  connect(stackedAction, &QAction::triggered, this, [this]() {
    ui->mdiArea->setViewMode(QMdiArea::TabbedView);
    settingsFile->setValue(QStringLiteral("mdiMode"), QMdiArea::TabbedView);
  });
  connect(floatingAction, &QAction::triggered, this, [this]() {
    ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
    settingsFile->setValue(QStringLiteral("mdiMode"), QMdiArea::SubWindowView);
  });

  mdiArrangeMenu = new QMenu(tr("Arrange floating windows"), this);
  QAction *cascadeAction = mdiArrangeMenu->addAction(tr("Cascaded"));
  connect(cascadeAction, &QAction::triggered, ui->mdiArea, &QMdiArea::cascadeSubWindows);
  QAction *tileAction = mdiArrangeMenu->addAction(tr("Tiled"));
  connect(tileAction, &QAction::triggered, ui->mdiArea, &QMdiArea::tileSubWindows);
  connect(floatingAction, &QAction::toggled, mdiArrangeMenu, &QMenu::setEnabled);

  const QMdiArea::ViewMode mode = static_cast<QMdiArea::ViewMode>(
      settingsFile->value(QStringLiteral("mdiMode"), QMdiArea::TabbedView).toInt());
  ui->mdiArea->setViewMode(mode);
  if (mode == QMdiArea::SubWindowView) {
    floatingAction->setChecked(true);
  }
  else {
    stackedAction->setChecked(true);
    mdiArrangeMenu->setEnabled(false);
  }

  ui->mdiArea->setTabsClosable(true);
  ui->mdiArea->setTabsMovable(true);
  ui->mdiArea->setAcceptDrops(false);
  ui->mdiArea->viewport()->setAcceptDrops(false);

  connect(ui->mdiArea, &QMdiArea::subWindowActivated, this, [this](QMdiSubWindow *subWindow) {
    Interactive *interactive = qobject_cast<Interactive *>(subWindow ? subWindow->widget() : nullptr);
    showInteractiveDocks(interactive);
    showReplayDocks(interactive);
    refreshContextMenus(subWindow);
    updateWorkspaceActions();
  });
  showInteractiveDocks(nullptr);
  showReplayDocks(nullptr);
  refreshContextMenus(nullptr);
  updateWorkspaceActions();

}  // Constructor

/**
 * @brief Close event reimplemented to ask confirmation before closing.
 */
void MainWindow::closeEvent(QCloseEvent *event) {
  QMessageBox msgBox(this);
  msgBox.setTextFormat(Qt::RichText);
  msgBox.setWindowTitle(tr("Confirmation"));
  msgBox.setText(tr("<b>Are you sure you want to quit?</b>"));
  msgBox.setIcon(QMessageBox::Question);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  QPushButton *minimizeButton = msgBox.addButton(tr("Minimize"), QMessageBox::AcceptRole);
  msgBox.setDefaultButton(minimizeButton);
  int reply = msgBox.exec();
  if (reply == QMessageBox::Yes) {
    settingsFile->setValue(QStringLiteral("geometry"), saveGeometry());
    settingsFile->setValue(QStringLiteral("dockState"), saveState());
    event->accept();
  }
  else if (reply == QMessageBox::AcceptRole) {
    trayIcon->show();
    this->hide();
    trayIcon->showMessage(tr("Hey!"), tr("I'm there"), QIcon(":/assets/icon.svg"), 1500);
    event->ignore();
  }
  else {
    event->ignore();
  }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
  const QList<QUrl> urls = event->mimeData()->urls();
  if (!urls.isEmpty() && urls.first().isLocalFile()) {
    event->acceptProposedAction();
  }
  else {
    event->ignore();
  }
}

void MainWindow::dropEvent(QDropEvent *event) {
  const QList<QUrl> urls = event->mimeData()->urls();
  if (urls.isEmpty() || !urls.first().isLocalFile()) {
    event->ignore();
    return;
  }

  QMdiSubWindow *subWindow = newInteractiveWindow();
  ui->mdiArea->setActiveSubWindow(subWindow);
  Interactive *interactive = qobject_cast<Interactive *>(subWindow->widget());
  interactive->openFolder(urls.first().toLocalFile());
  event->acceptProposedAction();
}

void MainWindow::openInteractive() {
  const QString path = QFileDialog::getOpenFileName(this,
                                                    tr("Open video or image"),
                                                    settingsFile->value(QStringLiteral("lastOpenDirectory")).toString());
  if (path.isEmpty()) {
    return;
  }

  settingsFile->setValue(QStringLiteral("lastOpenDirectory"), QFileInfo(path).absolutePath());
  QMdiSubWindow *subWindow = newInteractiveWindow();
  ui->mdiArea->setActiveSubWindow(subWindow);
  qobject_cast<Interactive *>(subWindow->widget())->openFolder(path);
}

QMdiSubWindow *MainWindow::newInteractiveWindow() {
  Interactive *interactive = new Interactive();
  if (!savedDockState.isEmpty()) {
    restoreState(savedDockState);
    savedDockState.clear();
  }
  QMdiSubWindow *subWindow = ui->mdiArea->addSubWindow(interactive);
  subWindow->setAttribute(Qt::WA_DeleteOnClose);
  subWindow->setWindowTitle(tr("Interactive tracking"));
  subWindow->show();
  connect(interactive, &Interactive::inputOpened, this, [this, subWindow, interactive](const QString &path) {
    subWindow->setWindowTitle(path);
    saveWorkspaceState(interactive);
    if (activeInteractive() == interactive) {
      showInteractiveDocks(interactive);
      updateWorkspaceActions();
    }
  });
  connect(interactive, &Interactive::status, this, [this](const QString &message) {
    trayIcon->showMessage(QStringLiteral("FastTrack"), message, QSystemTrayIcon::Information, 3000);
  });
  connect(interactive, &Interactive::trackingAvailabilityChanged, this, [this, interactive]() {
    if (activeInteractive() == interactive) {
      updateWorkspaceActions();
    }
  });
  connect(interactive, &Interactive::timelineEnabledChanged, this, [this, interactive](bool enabled) {
    if (activeInteractive() == interactive) {
      ui->slider->setEnabled(enabled);
    }
  });
  connect(interactive, &Interactive::replayVisibleChanged, this, [this, interactive](bool visible) {
    if (activeInteractive() == interactive) {
      showInteractiveDocks(interactive);
      showReplayDocks(interactive);
    }
  });
  connect(interactive, &Interactive::activeViewChanged, this, [this, interactive](bool) {
    if (activeInteractive() == interactive) {
      showInteractiveDocks(interactive);
      showReplayDocks(interactive);
    }
  });
  connect(interactive, &Interactive::parameterStateChanged, this, [this, interactive](const QHash<QString, QVariant> &state) {
    QHash<QString, QVariant> &workspaceState = workspaceStates[interactive];
    for (auto it = state.cbegin(); it != state.cend(); ++it) {
      workspaceState.insert(it.key(), it.value());
    }
  });
  workspaceStates.insert(interactive, interactive->parameterState());
  connect(subWindow, &QObject::destroyed, this, [this, interactive]() {
    if (shuttingDown) {
      return;
    }
    workspaceStates.remove(interactive);
  });
  Replay *replay = interactive->replayWidget();
  connect(replay, &Replay::frameRequested, this, [this, interactive](int frame) {
    if (activeInteractive() == interactive) {
      ui->slider->setValue(frame);
    }
  });
  connect(replay, &Replay::annotationTextChanged, this, [this, interactive](const QString &text) {
    if (activeInteractive() == interactive) {
      QSignalBlocker blocker(annotationEdit);
      annotationEdit->setPlainText(text);
    }
  });
  connect(replay, &Replay::informationChanged, this, [this, interactive](int table, const QList<QString> &values) {
    if (activeInteractive() == interactive) {
      updateInformationTable(table == 1 ? informationTable1 : informationTable2, values);
    }
  });
  return subWindow;
}

void MainWindow::showInteractiveDocks(Interactive *interactive) {
  const bool trackingVisible = interactive && !interactive->isReplayActive();
  imageOptionsDock->setVisible(trackingVisible);
  trackingOptionsDock->setVisible(trackingVisible);
  controlOptionsDock->setVisible(interactive != nullptr);
  if (interactive) {
    QHash<QString, QVariant> &state = workspaceStates[interactive];
    if (state.isEmpty()) {
      state = interactive->parameterState();
    }
    loadParameterControls(state);
    const QSignalBlocker sliderBlocker(ui->slider);
    const QSignalBlocker originalBlocker(ui->isOriginal);
    const QSignalBlocker subBlocker(ui->isSub);
    const QSignalBlocker binBlocker(ui->isBin);
    ui->slider->setMinimum(0);
    ui->slider->setMaximum(qMax(0, interactive->frameCount() - 1));
    ui->slider->setValue(state.value(QStringLiteral("frame")).toInt());
    switch (state.value(QStringLiteral("displayMode")).toInt()) {
      case 1:
        ui->isSub->setChecked(true);
        break;
      case 2:
        ui->isBin->setChecked(true);
        break;
      default:
        ui->isOriginal->setChecked(true);
        break;
    }
    const QList<QString> values = interactive->informationValues();
    for (int row = 0; row < values.count() && row < ui->informationTable->rowCount(); ++row) {
      ui->informationTable->item(row, 1)->setText(values.at(row));
    }
    ui->trackingStatus->setText(interactive->trackingStatusText());
  }
}

void MainWindow::saveWorkspaceState(Interactive *interactive) {
  if (!interactive) {
    return;
  }
  workspaceStates[interactive] = interactive->parameterState();
}

void MainWindow::loadParameterControls(const QHash<QString, QVariant> &state) {
  for (auto it = parameterControls.cbegin(); it != parameterControls.cend(); ++it) {
    QSignalBlocker blocker(it.value());
    const QVariant value = state.value(it.key());
    if (QSpinBox *box = qobject_cast<QSpinBox *>(it.value())) {
      box->setValue(value.toInt());
    }
    else if (QDoubleSpinBox *box = qobject_cast<QDoubleSpinBox *>(it.value())) {
      box->setValue(value.toDouble());
    }
    else if (QComboBox *box = qobject_cast<QComboBox *>(it.value())) {
      box->setCurrentIndex(value.toInt());
    }
  }
  const QSignalBlocker thresholdSliderBlocker(ui->threshSlider);
  ui->threshSlider->setValue(state.value(QStringLiteral("thresh")).toInt());
}

void MainWindow::applyParameterControls() {
  Interactive *interactive = activeInteractive();
  if (!interactive) {
    return;
  }
  QHash<QString, QVariant> state = workspaceStates.value(interactive, interactive->parameterState());
  for (auto it = parameterControls.cbegin(); it != parameterControls.cend(); ++it) {
    if (QSpinBox *box = qobject_cast<QSpinBox *>(it.value())) {
      state.insert(it.key(), box->value());
    }
    else if (QDoubleSpinBox *box = qobject_cast<QDoubleSpinBox *>(it.value())) {
      state.insert(it.key(), box->value());
    }
    else if (QComboBox *box = qobject_cast<QComboBox *>(it.value())) {
      state.insert(it.key(), box->currentIndex());
    }
  }
  workspaceStates.insert(interactive, state);
  interactive->setParameterState(state);
}

void MainWindow::loadReplayDisplayControls(Replay *replay) {
  const QHash<QString, QVariant> state = replay->displayParameters();
  const QSignalBlocker ellipseBlocker(replayEllipseBox);
  const QSignalBlocker arrowBlocker(replayArrowBox);
  const QSignalBlocker traceBlocker(replayTraceBox);
  const QSignalBlocker traceLengthBlocker(replayTraceLengthBox);
  const QSignalBlocker numbersBlocker(replayNumbersBox);
  const QSignalBlocker sizeBlocker(replaySizeBox);
  const QSignalBlocker fpsBlocker(replayFpsBox);
  replayEllipseBox->setCurrentIndex(state.value(QStringLiteral("ellipse")).toInt());
  replayArrowBox->setCurrentIndex(state.value(QStringLiteral("arrow")).toInt());
  replayTraceBox->setChecked(state.value(QStringLiteral("trace")).toBool());
  replayTraceLengthBox->setValue(state.value(QStringLiteral("traceLength")).toInt());
  replayNumbersBox->setChecked(state.value(QStringLiteral("numbers")).toBool());
  replaySizeBox->setValue(state.value(QStringLiteral("size")).toInt());
  replayFpsBox->setValue(state.value(QStringLiteral("fps")).toInt());
}

void MainWindow::applyReplayDisplayControls() {
  Interactive *interactive = activeInteractive();
  if (!interactive) {
    return;
  }
  interactive->replayWidget()->setDisplayParameters({{QStringLiteral("ellipse"), replayEllipseBox->currentIndex()},
                                                     {QStringLiteral("arrow"), replayArrowBox->currentIndex()},
                                                     {QStringLiteral("trace"), replayTraceBox->isChecked()},
                                                     {QStringLiteral("traceLength"), replayTraceLengthBox->value()},
                                                     {QStringLiteral("numbers"), replayNumbersBox->isChecked()},
                                                     {QStringLiteral("size"), replaySizeBox->value()},
                                                     {QStringLiteral("fps"), replayFpsBox->value()}});
}

void MainWindow::updateInformationTable(QTableWidget *table, const QList<QString> &values) {
  for (int row = 0; row < values.count() && row < table->rowCount(); ++row) {
    table->item(row, 0)->setText(values.at(row));
  }
}

void MainWindow::showReplayDocks(Interactive *interactive) {
  const bool visible = interactive && interactive->isReplayActive();
  replayDisplayDock->setVisible(visible);
  annotationDock->setVisible(visible);
  informationDock->setVisible(visible);
  if (visible) {
    Replay *replay = interactive->replayWidget();
    loadReplayDisplayControls(replay);
    replay->sliderConnection(ui->slider->value());
  }
}

Interactive *MainWindow::activeInteractive() const {
  QMdiSubWindow *subWindow = ui->mdiArea->activeSubWindow();
  return qobject_cast<Interactive *>(subWindow ? subWindow->widget() : nullptr);
}

void MainWindow::updateWorkspaceActions() {
  Interactive *interactive = activeInteractive();
  previewAction->setEnabled(interactive && interactive->canPreview());
  trackAction->setEnabled(interactive && interactive->canTrack());
}

QMdiSubWindow *MainWindow::newBatchWindow() {
  Batch *batch = new Batch();
  QMdiSubWindow *subWindow = ui->mdiArea->addSubWindow(batch);
  subWindow->setAttribute(Qt::WA_DeleteOnClose);
  subWindow->setWindowTitle(tr("Batch tracking"));
  subWindow->show();
  connect(batch, &Batch::status, this, [this](const QString &message) {
    trayIcon->showMessage(QStringLiteral("FastTrack"), message, QSystemTrayIcon::Information, 3000);
  });
  return subWindow;
}

void MainWindow::applyTheme(const QString &theme) {
  QString resource;
  if (theme == QLatin1String("dark")) {
    resource = QStringLiteral(":/dark.qss");
  }
  else if (theme == QLatin1String("light")) {
    resource = QStringLiteral(":/light.qss");
  }
  else if (theme == QLatin1String("ft")) {
    resource = QStringLiteral(":/theme.qss");
  }

  if (resource.isEmpty()) {
    qApp->setStyleSheet(QString());
  }
  else {
    QFile file(resource);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
      QTextStream stream(&file);
      qApp->setStyleSheet(stream.readAll());
    }
  }
  settingsFile->setValue(QStringLiteral("theme"), theme);
}

void MainWindow::refreshContextMenus(QMdiSubWindow *subWindow) {
  fileMenu->clear();
  viewMenu->clear();
  settingsMenu->clear();
  helpMenu->clear();

  QAction *openAction = fileMenu->addAction(tr("&Open video or image"));
  openAction->setShortcut(QKeySequence::Open);
  connect(openAction, &QAction::triggered, this, &MainWindow::openInteractive);
  QAction *newBatchAction = fileMenu->addAction(tr("New batch tracking"));
  connect(newBatchAction, &QAction::triggered, this, [this]() { newBatchWindow(); });
  fileMenu->addSeparator();

  viewMenu->addMenu(mdiModeMenu);
  viewMenu->addMenu(mdiArrangeMenu);

  if (Interactive *interactive = qobject_cast<Interactive *>(subWindow ? subWindow->widget() : nullptr)) {
    viewMenu->addSeparator();
    viewMenu->addAction(imageOptionsDock->toggleViewAction());
    viewMenu->addAction(trackingOptionsDock->toggleViewAction());
    viewMenu->addAction(controlOptionsDock->toggleViewAction());
    QAction *replayDisplayAction = replayDisplayDock->toggleViewAction();
    QAction *annotationAction = annotationDock->toggleViewAction();
    QAction *informationAction = informationDock->toggleViewAction();
    replayDisplayAction->setEnabled(interactive->isReplayVisible());
    annotationAction->setEnabled(interactive->isReplayVisible());
    informationAction->setEnabled(interactive->isReplayVisible());
    viewMenu->addAction(replayDisplayAction);
    viewMenu->addAction(annotationAction);
    viewMenu->addAction(informationAction);
    connect(interactive, &Interactive::replayVisibleChanged, replayDisplayAction, &QAction::setEnabled);
    connect(interactive, &Interactive::replayVisibleChanged, annotationAction, &QAction::setEnabled);
    connect(interactive, &Interactive::replayVisibleChanged, informationAction, &QAction::setEnabled);
  }

  QMenu *styleMenu = settingsMenu->addMenu(tr("Appearance"));
  QActionGroup *styleGroup = new QActionGroup(styleMenu);
  const QString currentStyle = settingsFile->value(QStringLiteral("style"), QStringLiteral("Fusion")).toString();
  for (const QString &style : QStyleFactory::keys()) {
    QAction *styleAction = styleMenu->addAction(style);
    styleAction->setCheckable(true);
    styleAction->setChecked(style == currentStyle);
    styleGroup->addAction(styleAction);
    connect(styleAction, &QAction::triggered, this, [this, style]() {
      QApplication::setStyle(QStyleFactory::create(style));
      settingsFile->setValue(QStringLiteral("style"), style);
    });
  }

  QMenu *themeMenu = settingsMenu->addMenu(tr("Theme"));
  QActionGroup *themeGroup = new QActionGroup(themeMenu);
  const QString currentTheme = settingsFile->value(QStringLiteral("theme"), QStringLiteral("ft")).toString();
  const QList<QPair<QString, QString>> themes = {{tr("Default"), QStringLiteral("default")},
                                                 {tr("Breeze Dark"), QStringLiteral("dark")},
                                                 {tr("Breeze Light"), QStringLiteral("light")},
                                                 {tr("FastTrack"), QStringLiteral("ft")}};
  for (const auto &theme : themes) {
    QAction *themeAction = themeMenu->addAction(theme.first);
    themeAction->setCheckable(true);
    themeAction->setChecked(theme.second == currentTheme);
    themeGroup->addAction(themeAction);
    connect(themeAction, &QAction::triggered, this, [this, theme]() { applyTheme(theme.second); });
  }

  QAction *documentationAction = helpMenu->addAction(tr("FastTrack documentation"));
  documentationAction->setShortcut(QKeySequence::HelpContents);
  connect(documentationAction, &QAction::triggered, []() {
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://www.fasttrack.sh/docs/intro")));
  });
  QAction *tutorialAction = helpMenu->addAction(tr("FastTrack tutorial"));
  connect(tutorialAction, &QAction::triggered, []() {
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://www.youtube.com/watch?v=RzzmcZs04E4&list=PLGjsUpRojSmO4RHrd-TbpbNpJrfjNYlIm")));
  });
  helpMenu->addSeparator();
  QAction *questionAction = helpMenu->addAction(tr("Ask a question"));
  connect(questionAction, &QAction::triggered, []() {
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://github.com/FastTrackOrg/FastTrack/discussions")));
  });
  QAction *logAction = helpMenu->addAction(tr("Generate a log"));
  connect(logAction, &QAction::triggered, this, [this]() {
    const QString fileName = QFileDialog::getSaveFileName(this, tr("Save Log File"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), tr("Logs (*.log)"));
    if (!fileName.isEmpty()) {
      QFile::remove(fileName);
      QFile::copy(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/fasttrack.log", fileName);
    }
  });
  QAction *issueAction = helpMenu->addAction(tr("Report an issue"));
  connect(issueAction, &QAction::triggered, []() {
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://github.com/FastTrackOrg/FastTrack/issues")));
  });
  QAction *contactAction = helpMenu->addAction(tr("Contact"));
  connect(contactAction, &QAction::triggered, []() {
    QDesktopServices::openUrl(QUrl(QStringLiteral("mailto:benjamin.gallois@fasttrack.sh?subject=[fasttrack]")));
  });
  helpMenu->addSeparator();
  QAction *aboutAction = helpMenu->addAction(tr("About FastTrack..."));
  connect(aboutAction, &QAction::triggered, this, []() {
    QMessageBox::about(nullptr, tr("About FastTrack"), tr("FastTrack version %1 is a desktop tracking software, easy to install, easy to use, and performant.<br>Created and maintained by Benjamin Gallois.<br>Distributed under the terms of the <a href='https://www.gnu.org/licenses/gpl-3.0'>GPL3.0 license</a>.<br>").arg(QApplication::applicationVersion()));
  });
  helpMenu->addAction(tr("About Qt"), qApp, &QApplication::aboutQt);

  fileMenu->addSeparator();
  QAction *quitAction = fileMenu->addAction(tr("&Quit"));
  quitAction->setShortcut(QKeySequence::Quit);
  connect(quitAction, &QAction::triggered, this, &MainWindow::close);
}

/**
 * @brief Destructs the MainWindow object and saves the previous set of parameters.
 */
MainWindow::~MainWindow() {
  shuttingDown = true;
  delete ui;
}
