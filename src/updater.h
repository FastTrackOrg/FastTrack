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

#ifndef UPDATER_H
#define UPDATER_H

#include <QAbstractButton>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QPushButton>
#include <QStatusBar>
#include <QUrl>
#include <QWidget>

class Updater : public QWidget {
  Q_OBJECT
 public:
  Updater(QWidget *parent = 0);
  Updater(const Updater &T) = delete;
  Updater &operator=(const Updater &T) = delete;
  Updater &operator=(Updater &&T) = delete;
  Updater(Updater &&T) = delete;
  ~Updater();
  /*! Updates availability status. */
  enum updateStatus { HasUpdate,    /*!< a new version of FastTrack is available. */
                      NoUpdate,     /*!< no new version of FastTrack is available. */
                      NotSupported, /*!< this platform is not Linux, Windows or MacOS. */
                      NoNetwork     /*!< connection to the remote server is no possible. */
  };
  void checkForUpdate();
  updateStatus isUpdate() const;
  void displayUpdate(const QByteArray &version, const QByteArray &message);
  void displayWarning(const QByteArray &warning);

 private:
  updateStatus status;
  QUrl remoteURL = QUrl(QStringLiteral("https://www.fasttrack.sh/download/FastTrack/platforms.txt"));
 signals:
  void hasUpdate(QByteArray version, QByteArray message);
  void hasWarning(QByteArray warning);
};

#endif
