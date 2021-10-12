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
  setWindowTitle(qApp->applicationName() + " " + APP_VERSION);
  setWindowState(Qt::WindowMaximized);
  showMaximized();

  // Tray icon
  trayIcon = new QSystemTrayIcon(QIcon(":/assets/icon.svg"), this);
  QMenu *trayMenu = new QMenu;
  QAction *restore = new QAction(tr("Restore"));
  connect(restore, &QAction::triggered, this, &MainWindow::showNormal);
  trayMenu->addAction(restore);
  QAction *close = new QAction(tr("Close"));
  connect(close, &QAction::triggered, this, &MainWindow::close);
  trayMenu->addAction(close);
  connect(trayIcon, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
      case QSystemTrayIcon::Trigger: {
        this->setVisible(true);
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

  // Setup style
  QFile stylesheet("");

  if (stylesheet.open(QIODevice::ReadOnly | QIODevice::Text)) {  // Read the theme file
    qApp->setStyleSheet(stylesheet.readAll());
    stylesheet.close();
  }

#ifndef NO_WEB
  QNetworkAccessManager *manager = new QNetworkAccessManager(this);
  connect(manager, &QNetworkAccessManager::finished, [this](QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
      return;
    }

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

    if (lastVersion != APP_VERSION) {
      QMessageBox msgBox;
      msgBox.setWindowTitle("Information");
      msgBox.setTextFormat(Qt::RichText);
      msgBox.setText("FastTrack version " + lastVersion + " is available! <br> Please update. <br> <a href='http://www.fasttrack.sh/docs/installation/#update'>Need help to update?</a> <br>" + message + "<br>" + warning);
      QTimer::singleShot(5000, &msgBox, &QMessageBox::close);
      msgBox.exec();
      this->statusBar()->addWidget(new QLabel("FastTrack version " + lastVersion + " is available!"));
    }
    else if (!warning.isEmpty()) {
      QMessageBox msgBox;
      msgBox.setWindowTitle("Information");
      msgBox.setText(warning);
      msgBox.exec();
    }
  });

  manager->get(QNetworkRequest(QUrl("https://www.fasttrack.sh/download/FastTrack/platforms.txt")));

  QSettings settingsFile("FastTrack", "FastTrackOrg");
  int isChosed = settingsFile.value("main/consent", -1).toInt();
  switch (isChosed) {
    case -1: {
      QMessageBox *consent = new QMessageBox(this);
      consent->setWindowTitle("Consent");
      consent->setText("To help to monitor the development of the software, FastTrack would like to collect some information (IP address, operating system)");
      consent->setStandardButtons(QMessageBox::Yes);
      consent->addButton(QMessageBox::No);
      QTimer::singleShot(5000, consent, &QMessageBox::close);
      if (consent->exec() == QMessageBox::Yes) {
        QWebEngineView *view = new QWebEngineView(this);
        view->setUrl(QUrl("https://www.fasttrack.sh/soft.html"));
        connect(view->page(), &QWebEnginePage::loadFinished, [this, view]() {
          delete view;
        });
        settingsFile.setValue("main/consent", 0);
      }
      else {
        settingsFile.setValue("main/consent", 1);
      }
      break;
    }
    case 0: {
      QWebEngineView *view = new QWebEngineView(this);
      view->setUrl(QUrl("https://www.fasttrack.sh/soft.html"));
      connect(view->page(), &QWebEnginePage::loadFinished, [this, view]() {
        delete view;
      });
      view = new QWebEngineView(this);
      view->setUrl(QUrl("https://www.fasttrack.sh/soft.html"));
      connect(view->page(), &QWebEnginePage::loadFinished, [this, view]() {
        delete view;
      });
      break;
    }
  }
#endif

  interactive = new Interactive(this);
  ui->tabWidget->addTab(interactive, tr("Interactive tracking"));
  connect(interactive, &Interactive::status, [this](QString message) {
    trayIcon->showMessage("FastTrack", message, QSystemTrayIcon::Information, 3000);
  });

  batch = new Batch(this);
  ui->tabWidget->addTab(batch, tr("Batch tracking"));
  connect(batch, &Batch::status, [this](QString message) {
    trayIcon->showMessage("FastTrack", message, QSystemTrayIcon::Information, 3000);
  });

  replay = new Replay(this);
  ui->tabWidget->addTab(replay, tr("Tracking inspector"));

  trackingManager = new TrackingManager(this);
  ui->tabWidget->addTab(trackingManager, tr("Tracking Manager"));
  connect(interactive, &Interactive::log, trackingManager, &TrackingManager::addLogEntry);
  connect(batch, &Batch::log, trackingManager, &TrackingManager::addLogEntry);

#ifndef NO_WEB
  manual = new QWebEngineView(this);
  ui->tabWidget->addTab(manual, tr("User Manual"));
  connect(ui->tabWidget, &QTabWidget::currentChanged, [this](int index) {
    if (index == 4) {
      manual->setUrl(QUrl("https://www.fasttrack.sh/docs/intro"));
    }
  });
  dataset = new QWebEngineView(this);
  ui->tabWidget->addTab(dataset, tr("TD²"));
  connect(ui->tabWidget, &QTabWidget::currentChanged, [this](int index) {
    if (index == 5) {
      dataset->setUrl(QUrl("http://data.ljp.upmc.fr/datasets/TD2/"));
    }
  });
#endif

}  // Constructor

/**
  * @brief Close event reimplemented to ask confirmation before closing.
*/
void MainWindow::closeEvent(QCloseEvent *event) {
  QMessageBox msgBox(this);
  msgBox.setTextFormat(Qt::RichText);
  msgBox.setWindowTitle("Confirmation");
  msgBox.setText("<b>Are you sure you want to quit?</b>");
  msgBox.setIcon(QMessageBox::Question);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.addButton(tr("Minimize"), QMessageBox::AcceptRole);
  int reply = msgBox.exec();
  if (reply == QMessageBox::Yes) {
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
  * @brief Destructs the MainWindow object and saves the previous set of parameters.
*/
MainWindow::~MainWindow() {
  delete ui;
}
