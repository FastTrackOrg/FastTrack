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

#include "statanalysis.h"
#include "ui_statanalysis.h"

StatAnalysis::StatAnalysis(QWidget* parent, bool isStandalone) : QMainWindow(parent),
                                                                 ui(new Ui::StatAnalysis),
                                                                 isStandalone(isStandalone),
                                                                 trackingData(new Data()),
                                                                 settingsFile(new QSettings(QStringLiteral("FastTrack"), QStringLiteral("FastTrackOrg"), this)) {
  ui->setupUi(this);

  // Loads settings
  settingsFile->beginGroup(QStringLiteral("statanalysis"));
  restoreState(settingsFile->value(QStringLiteral("windowState")).toByteArray());

  QIcon img;
  img = QIcon(":/assets/buttons/openImage.png");
  QAction* openAction = new QAction(img, tr("&Open"), this);
  openAction->setShortcuts(QKeySequence::Open);
  openAction->setStatusTip(tr("Open tracking data"));
  connect(openAction, &QAction::triggered, this, &StatAnalysis::openTrackingData);
  ui->toolBar->addAction(openAction);

  connect(ui->objectAll, &QPushButton::clicked, this, [this](bool state) {
    for (int i = 0; i < ui->objectList->rowCount(); i++) {
      QCheckBox* object = qobject_cast<QCheckBox*>(ui->objectList->cellWidget(i, 0));
      {
        const QSignalBlocker blocker(object);
        object->setChecked(state);
      }
      if (state) {
        ui->objectAll->setText(QStringLiteral("Unselect All"));
      }
      else {
        ui->objectAll->setText(QStringLiteral("Select All"));
      }
    }
    refresh();
  });

  // Chart and ChartView are added their. Plots are defined in initPlots.
  // Then refresh and clear are automatically endled.
  plots.append(new QChart());
  ui->tabPlot->addTab(new QChartView(plots[0]), QStringLiteral("Trajectory"));
}

/**
 * @brief Opens a dialogue to select tracking data.
 */
void StatAnalysis::openTrackingData() {
  clear();
  QString file = QFileDialog::getOpenFileName(this, tr("Open Tracking Data"), memoryDir, tr("Tracking Database (*.db)"));
  if (!file.isEmpty()) {
    QString dir = QFileInfo(file).dir().path();  // Data takes the tracking folder.
    QApplication::setOverrideCursor(Qt::WaitCursor);
    trackingData->setPath(dir);
    loadObjectList();
    QList<int> objects = getSelectedObjects();
    initPlots(objects);
    QApplication::restoreOverrideCursor();
  }
  memoryDir = file;
}

void StatAnalysis::loadObjectList() {
  QList<int> objectList = trackingData->getId(0, trackingData->maxFrameIndex);
  int currentRow = 0;
  for (const auto& a : objectList) {
    ui->objectList->insertRow(currentRow);
    QCheckBox* checkBox = new QCheckBox(QString::number(a), this);
    checkBox->setChecked(true);
    connect(checkBox, &QCheckBox::stateChanged, this, &StatAnalysis::refresh);
    ui->objectList->setCellWidget(currentRow, 0, checkBox);
    currentRow++;
  }
}

QList<int> StatAnalysis::getSelectedObjects() {
  QList<int> objects;
  for (int i = 0; i < ui->objectList->rowCount(); i++) {
    QCheckBox* object = qobject_cast<QCheckBox*>(ui->objectList->cellWidget(i, 0));
    if (object->isChecked()) {
      objects.append(object->text().toInt());
    }
  }
  return objects;
}

void StatAnalysis::refresh() {
  QList<int> objects = getSelectedObjects();
  refreshPlots(objects);
}

void StatAnalysis::clear() {
  trackingData->clear();
  ui->objectList->setRowCount(0);  // Remove all rows
  clearPlots();
}

void StatAnalysis::initPlots(const QList<int>& objects) {
  // Trajectories plot
  QChart* traj = plots[0];
  for (auto const& object : objects) {
    QLineSeries* series = new QLineSeries(traj);
    QHash<QString, QList<double>> data = trackingData->getDataId(object);
    for (int i = 0; i < data.value(QStringLiteral("xBody")).size(); i++) {
      series->append(data.value(QStringLiteral("xBody")).at(i), data.value(QStringLiteral("yBody")).at(i));
    }
    series->setName(QString::number(object));
    traj->addSeries(series);
  }
  traj->createDefaultAxes();
  traj->setTitle(QStringLiteral("Trajectories"));
}

void StatAnalysis::refreshPlots(const QList<int>& objects) {
  for (auto const plot : plots) {
    QList<QAbstractSeries*> series = plot->series();
    for (auto const& serie : series) {
      if (!objects.contains(serie->name().toInt())) {
        serie->setVisible(false);
      }
      else {
        serie->setVisible(true);
      }
    }
  }
}

void StatAnalysis::clearPlots() {
  for (auto const plot : plots) {
    plot->removeAllSeries();
  }
}

StatAnalysis::~StatAnalysis() {
  clear();
  delete trackingData;
  delete ui;
}
