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
                                          interactive(new Interactive(this)),
                                          batch(new Batch(this)),
                                          replay(new Replay(this)),
                                          trackingManager(new TrackingManager(this)),
                                          settingsFile(new QSettings(QStringLiteral("FastTrack"), QStringLiteral("FastTrackOrg"), this)) {
  QDir::setCurrent(QCoreApplication::applicationDirPath());
  ui->setupUi(this);
  setWindowTitle(qApp->applicationName() + " " + APP_VERSION);

  settingsFile->beginGroup(QStringLiteral("main"));
  restoreGeometry(settingsFile->value(QStringLiteral("geometry")).toByteArray());

  // Tray icon
  trayIcon = new QSystemTrayIcon(QIcon(":/assets/icon.svg"), this);
  QMenu *trayMenu = new QMenu(this);
  QAction *restore = trayMenu->addAction(tr("Restore"));
  connect(restore, &QAction::triggered, this, &MainWindow::showNormal);
  QAction *close = trayMenu->addAction(tr("Close"));
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

  ui->tabWidget->addTab(interactive, tr("Interactive tracking"));
  connect(interactive, &Interactive::status, this, [this](const QString &message) {
    trayIcon->showMessage(QStringLiteral("FastTrack"), message, QSystemTrayIcon::Information, 3000);
  });
  connect(interactive, &Interactive::modeChanged, this, &MainWindow::setMode);

  ui->tabWidget->addTab(batch, tr("Batch tracking"));
  connect(batch, &Batch::status, this, [this](const QString &message) {
    trayIcon->showMessage(QStringLiteral("FastTrack"), message, QSystemTrayIcon::Information, 3000);
  });

  ui->tabWidget->addTab(replay, tr("Tracking inspector"));

  ui->tabWidget->addTab(trackingManager, tr("Tracking Manager"));
  connect(interactive, &Interactive::log, trackingManager, &TrackingManager::addLogEntry);
  connect(batch, &Batch::log, trackingManager, &TrackingManager::addLogEntry);

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
    QMessageBox msgSupport(this);
    msgSupport.setTextFormat(Qt::RichText);
    msgSupport.setWindowTitle(tr("Support"));
    msgSupport.setText(tr("Don't forget that you can become a FastTrack supporter <a href='https://ko-fi.com/bgallois'>there</a>!"));
    msgSupport.setIcon(QMessageBox::Information);
    msgSupport.exec();
    // msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
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

/**
 * @brief Changes the software mode.
 * @param[in] isExpert True if the software is in expert mode with advanced functions.
 */
void MainWindow::setMode(bool isExpert) {
  ui->tabWidget->tabBar()->setVisible(isExpert);
}

/**
 * @brief Destructs the MainWindow object and saves the previous set of parameters.
 */
MainWindow::~MainWindow() {
  delete ui;
}
