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

#include "trackingmanager.h"
#include "ui_trackingmanager.h"

/**
 * @class TrackingManager
 *
 * @brief The TrackingManager widget provides an environment to manage the log of FastTrack tracking analysis.
 *
 * @author Benjamin Gallois
 *
 * @version $Revision: 490 $
 *
 * Contact: benjamin.gallois@fasttrack.sh
 *
 */

/**
 * @brief Constructs the trackingmanager object derived from a QWidget object.
 */
TrackingManager::TrackingManager(QWidget *parent) : QWidget(parent),
                                                    ui(new Ui::TrackingManager) {
  ui->setupUi(this);

  ui->logTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  QString configFolder = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).at(0) + "/FastTrack";
  if (QDir(configFolder).exists()) {
  }
  else {
    QDir().mkdir(configFolder);
  }

  connect(ui->pathButton, &QPushButton::clicked, this, [this]() {
    ui->logTable->clear();
    ui->logTable->setRowCount(0);
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    QStringList fileNames;
    if (dialog.exec()) {
      fileNames = dialog.selectedFiles();
      logPath = fileNames[0];
      ui->path->setText(logPath);
      readFromFile(logPath);
    }
  });

  logPath = configFolder + "/default_log.fst";
  ui->path->setText(logPath);
  readFromFile(logPath);
}

TrackingManager::~TrackingManager() {
  delete ui;
}

/**
 * @brief Adds a log entry inside the ui table and in a log file.
 * @param[in] log QMap that contains the log entry.
 */
void TrackingManager::addLogEntry(QMap<QString, QString> log) {
  ui->logTable->insertRow(0);
  ui->logTable->setItem(0, 0, new QTableWidgetItem(log[QStringLiteral("date")]));
  ui->logTable->setItem(0, 1, new QTableWidgetItem(log[QStringLiteral("path")]));
  ui->logTable->setItem(0, 2, new QTableWidgetItem(log[QStringLiteral("status")]));
  ui->logTable->setItem(0, 3, new QTableWidgetItem(log[QStringLiteral("time")]));
  appendToFile(logPath, log);
}

/**
 * @brief Appends the log entry in a file.
 * @param[in] path QString that contains the path to the output file.
 * @param[in] line QMap that contains the log entry.
 */
void TrackingManager::appendToFile(const QString &path, const QMap<QString, QString> &line) {
  QFile file(path);
  if (file.open(QIODevice::Append)) {
    QDataStream out(&file);
    out << line;
  }
}

/**
 * @brief Writes log entries in a file.
 * @param[in] path QString that contains the path to the output file.
 * @param[in] lines QList ofQMap that contains log entries.
 */
void TrackingManager::writeToFile(const QString &path, const QList<QMap<QString, QString>> &lines) {
  QFile file(path);
  if (file.open(QIODevice::WriteOnly)) {
    QDataStream out(&file);
    for (auto const &line : lines) {
      out << line;
    }
  }
}

/**
 * @brief Reads a log file.
 * @param[in] path QString that contains the path to the input file.
 */
void TrackingManager::readFromFile(const QString &path) {
  QFile file(path);
  if (file.open(QIODevice::ReadOnly)) {
    QDataStream in(&file);
    QMap<QString, QString> log;
    in >> log;
    while (in.status() == QDataStream::Ok) {
      ui->logTable->insertRow(0);
      ui->logTable->setItem(0, 0, new QTableWidgetItem(log[QStringLiteral("date")]));
      ui->logTable->setItem(0, 1, new QTableWidgetItem(log[QStringLiteral("path")]));
      ui->logTable->setItem(0, 2, new QTableWidgetItem(log[QStringLiteral("status")]));
      ui->logTable->setItem(0, 3, new QTableWidgetItem(log[QStringLiteral("time")]));
      in >> log;
    }
  }
}
