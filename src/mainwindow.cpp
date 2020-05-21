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
                                          ui(new Ui::MainWindow) {
  QDir::setCurrent(QCoreApplication::applicationDirPath());
  ui->setupUi(this);
  setWindowState(Qt::WindowMaximized);
  setWindowTitle("FastTrack " + version);

  // Setup style
  QFile stylesheet(":/theme.qss");

  if (stylesheet.open(QIODevice::ReadOnly | QIODevice::Text)) {  // Read the theme file
    qApp->setStyleSheet(stylesheet.readAll());
    stylesheet.close();
  }

  QNetworkAccessManager *manager = new QNetworkAccessManager(this);
  connect(manager, &QNetworkAccessManager::finished, [this](QNetworkReply *reply) {
    QByteArray downloadedData = reply->readAll();
    reply->deleteLater();

#ifdef Q_OS_UNIX
    QByteArray lastVersion = downloadedData.mid(downloadedData.indexOf("lin") + 4, 5);
#elif defined(Q_OS_WIN32)
    QByteArray lastVersion = downloadedData.mid(downloadedData.indexOf("win") + 4, 5);
#elif defined(Q_OS_MAC)
    QByteArray lastVersion = downloadedData.mid(downloadedData.indexOf("mac") + 4, 5);
#else
    QByteArray lastVersion = "unknown version";
#endif

    QByteArray message = downloadedData.mid(downloadedData.indexOf("message") + 7, downloadedData.indexOf("!message") - downloadedData.indexOf("message") - 7);
    QByteArray warning = downloadedData.mid(downloadedData.indexOf("warning") + 7, downloadedData.indexOf("!warning") - downloadedData.indexOf("warning") - 7);

    if (lastVersion != version) {
      QMessageBox msgBox;
      msgBox.setTextFormat(Qt::RichText);
      msgBox.setText("FastTrack version " + lastVersion + " is available! <br> Please update. <br> <a href='http://www.fasttrack.sh/UserManual/docs/installation/#update'>Need help to update?</a> <br>" + message + "<br>" + warning);
      msgBox.exec();
    }
    else if (!warning.isEmpty()) {
      QMessageBox msgBox;
      msgBox.setText(warning);
      msgBox.exec();
    }
  });

  manager->get(QNetworkRequest(QUrl("http://www.fasttrack.sh/download/FastTrack/platforms.txt")));

  QWebEngineView *view = new QWebEngineView(this);
  view->setUrl(QUrl("http://www.fasttrack.sh/soft.html"));
  connect(view->page(), &QWebEnginePage::loadFinished, [this, view]() {
    delete view;
  });

  interactive = new Interactive(this);
  ui->tabWidget->addTab(interactive, tr("Interactive tracking"));

  batch = new Batch(this);
  ui->tabWidget->addTab(batch, tr("Batch tracking"));

  replay = new Replay(this);
  ui->tabWidget->addTab(replay, tr("Tracking inspector"));

  trackingManager = new TrackingManager(this);
  ui->tabWidget->addTab(trackingManager, tr("Tracking Manager"));
  connect(interactive, &Interactive::log, trackingManager, &TrackingManager::addLogEntry);
  connect(batch, &Batch::log, trackingManager, &TrackingManager::addLogEntry);

}  // Constructor

/**
  * @brief Close event reimplemented to ask confirmation before closing. 
*/
void MainWindow::closeEvent(QCloseEvent *event) {
  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(this, "Quit?", "Are you sure you want to quit?", QMessageBox::Yes | QMessageBox::No);
  if (reply == QMessageBox::Yes) {
    event->accept();
  }
  else {
    event->ignore();
  }
}

/**
  * @brief Destructs the MainWindow object and saves the previous set of parameters.  
*/
MainWindow::~MainWindow() {
  delete ui;
}
