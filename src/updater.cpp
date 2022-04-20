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

#include "updater.h"

/**
 * @class Updater
 *
 * @brief This class checks if updates are available on an remote host. Support Linux, MacOs and WIndows.
 *
 * @author Benjamin Gallois
 *
 * @version $Revision: 6.2.6 $
 *
 * Contact: benjamin.gallois@fasttrack.sh
 *
 */
Updater::Updater(QWidget *parent) : QWidget(parent) {
  connect(this, &Updater::hasUpdate, this, &Updater::displayUpdate);
  connect(this, &Updater::hasWarning, this, &Updater::displayWarning);
}

/**
 * @brief Check if updates are available on an remote server.
 */
void Updater::checkForUpdate() {
  QNetworkAccessManager *manager = new QNetworkAccessManager(this);
  connect(manager, &QNetworkAccessManager::finished, this, [this](QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
      status = NoNetwork;
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
    status = NotSupported;
#endif

    QByteArray message = downloadedData.mid(downloadedData.indexOf("message") + 7, downloadedData.indexOf("!message") - downloadedData.indexOf("message") - 7);
    QByteArray warning = downloadedData.mid(downloadedData.indexOf("warning") + 7, downloadedData.indexOf("!warning") - downloadedData.indexOf("warning") - 7);

    if (lastVersion > APP_VERSION) {
      status = HasUpdate;
      emit hasUpdate(lastVersion, message);
    }
    else {
      status = NoUpdate;
    }
    if (!warning.isEmpty()) {
      emit hasWarning(warning);
    }

    QMainWindow *main = qobject_cast<QMainWindow *>(this->parent());
    if (main != nullptr) {
      switch (status) {
        case updateStatus::HasUpdate: {
          main->statusBar()->addWidget(new QLabel(QStringLiteral("FastTrack version %1 is available!").arg(lastVersion)));
          QPushButton *button = new QPushButton(QStringLiteral("Download FastTrack %1").arg(lastVersion), this);
          connect(button, &QPushButton::clicked, this, [=, this]() {
            displayUpdate(lastVersion, message);
          });
          main->statusBar()->addPermanentWidget(button);
          break;
        }
        case updateStatus::NoUpdate: {
          main->statusBar()->showMessage(QStringLiteral("FastTrack is up to date!"), 10000);
          break;
        }
        case updateStatus::NotSupported: {
          main->statusBar()->showMessage(QStringLiteral("Platform does not support updates!"), 10000);
          break;
        }
        case updateStatus::NoNetwork: {
          main->statusBar()->showMessage(QStringLiteral("Connect to network to check for updates!"), 10000);
          break;
        }
        default: {
          main->statusBar()->showMessage(QStringLiteral("Connect to network to check for updates!"), 10000);
        }
      }
    }
  });

  manager->get(QNetworkRequest(remoteURL));
}

/**
 * @brief Displays a message box to help the user to upgrade FastTrack.
 * @param[in] version The version number.
 * @param[in] message Changelog associated with the version.
 */
void Updater::displayUpdate(const QByteArray &version, const QByteArray &message) {
  QMessageBox msgBox;
  msgBox.setWindowTitle(QStringLiteral("FastTrack Version"));
  msgBox.setTextFormat(Qt::RichText);
  msgBox.setIcon(QMessageBox::Information);
  msgBox.setText(QStringLiteral("<strong>FastTrack version %1 is available!</strong> <br> Please update. <br> <a href='http://www.fasttrack.sh/docs/installation/#update'>Need help to update?</a> <br> %2").arg(version, message));
  msgBox.addButton(QMessageBox::Close);
  QPushButton *downloadButton = msgBox.addButton(QStringLiteral("Download new version"), QMessageBox::ActionRole);
#ifdef Q_OS_UNIX
  connect(downloadButton, &QPushButton::clicked, this, [this]() {
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://www.fasttrack.sh/download/FastTrack-x86_64.AppImage"), QUrl::TolerantMode));
  });
#elif defined(Q_OS_WIN32)
  connect(downloadButton, &QPushButton::clicked, this, [this]() {
    QProcess::startDetached(QCoreApplication::applicationDirPath() + QStringLiteral("/MaintenanceTool.exe"));
  });
#elif defined(Q_OS_MAC)
  connect(downloadButton, &QPushButton::clicked, this, [this]() {
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://www.fasttrack.sh/download/FastTrack.dmg"), QUrl::TolerantMode));
  });
#endif
  msgBox.exec();
}

/**
 * @brief Displays a warning box if warning messages are available on the remote server.
 * @param[in] warning Warning message.
 */
void Updater::displayWarning(const QByteArray &warning) {
  QMessageBox msgBox;
  msgBox.setWindowTitle(QStringLiteral("Warning"));
  msgBox.setTextFormat(Qt::RichText);
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setText(warning);
  msgBox.exec();
}

/**
 * @brief Gets update avaibility.
 * @return Updates status.
 */
Updater::updateStatus Updater::isUpdate() const {
  return status;
}

Updater::~Updater() {}
