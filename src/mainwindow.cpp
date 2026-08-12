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
#include <QDesktopServices>
#include <QFileDialog>
#include <QMenuBar>
#include <QMimeData>
#include <QStandardPaths>
#include <QStyleFactory>
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

  connect(ui->mdiArea, &QMdiArea::subWindowActivated, this, &MainWindow::refreshContextMenus);
  refreshContextMenus(nullptr);

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
  QMdiSubWindow *subWindow = ui->mdiArea->addSubWindow(interactive);
  subWindow->setAttribute(Qt::WA_DeleteOnClose);
  subWindow->setWindowTitle(tr("Interactive tracking"));
  subWindow->show();
  connect(interactive, &Interactive::status, this, [this](const QString &message) {
    trayIcon->showMessage(QStringLiteral("FastTrack"), message, QSystemTrayIcon::Information, 3000);
  });
  return subWindow;
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
    QAction *imageOptionsAction = viewMenu->addAction(tr("Image Options"));
    imageOptionsAction->setCheckable(true);
    imageOptionsAction->setChecked(interactive->imageOptionsVisible());
    connect(imageOptionsAction, &QAction::toggled, interactive, &Interactive::setImageOptionsVisible);
    QAction *trackingOptionsAction = viewMenu->addAction(tr("Tracking Options"));
    trackingOptionsAction->setCheckable(true);
    trackingOptionsAction->setChecked(interactive->trackingOptionsVisible());
    connect(trackingOptionsAction, &QAction::toggled, interactive, &Interactive::setTrackingOptionsVisible);
    QAction *controlOptionsAction = viewMenu->addAction(tr("Video Controls"));
    controlOptionsAction->setCheckable(true);
    controlOptionsAction->setChecked(interactive->controlOptionsVisible());
    connect(controlOptionsAction, &QAction::toggled, interactive, &Interactive::setControlOptionsVisible);
    QAction *replayAction = viewMenu->addAction(tr("Tracking replay"));
    replayAction->setCheckable(true);
    replayAction->setChecked(interactive->isReplayVisible());
    connect(replayAction, &QAction::toggled, interactive, &Interactive::setReplayVisible);
    connect(interactive, &Interactive::replayVisibleChanged, replayAction, &QAction::setChecked);
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
  delete ui;
}
