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
                                                                 settingsFile(new QSettings(QStringLiteral("FastTrack"), QStringLiteral("FastTrackOrg"), this)),
                                                                 ruler(1),
                                                                 timeScale(1),
                                                                 isData(false) {
  ui->setupUi(this);

  scale = new QLabel(QStringLiteral("1px = 1m and 1 timestep = 1s"), this);
  ui->statusbar->addPermanentWidget(scale);

  // Loads settings
  settingsFile->beginGroup(QStringLiteral("statanalysis"));
  restoreState(settingsFile->value(QStringLiteral("windowState")).toByteArray());

  QIcon img;
  img = QIcon(":/assets/buttons/openImage.png");
  QAction* openAction = new QAction(img, tr("&Open"), this);
  openAction->setShortcuts(QKeySequence::Open);
  openAction->setStatusTip(tr("Open tracking data"));
  connect(openAction, &QAction::triggered, this, qOverload<>(&StatAnalysis::openTrackingData));
  ui->toolBar->addAction(openAction);

  img = QIcon(":/assets/buttons/option.png");
  QAction* optionAction = new QAction(img, tr("&Ruler"), this);
  optionAction->setIcon(img);
  optionAction->setStatusTip(tr("Set scale"));
  connect(optionAction, &QAction::triggered, this, [this]() {
    bool ok;
    double d = QInputDialog::getDouble(this, QStringLiteral("Get convertion factor"),
                                       QStringLiteral("1px equal ? (in meters)"), ruler, -10000000, 1000000, 9, &ok,
                                       Qt::WindowFlags(), 1);
    if (ok) {
      ruler = d;

      d = QInputDialog::getDouble(this, QStringLiteral("Get convertion factor"),
                                  QStringLiteral("1 timestep equal ? (in seconds)"), timeScale, -10000000, 1000000, 9, &ok,
                                  Qt::WindowFlags(), 1);
      if (ok) {
        timeScale = d;
        clearPlots();
        QList<int> objects = trackingData->getId(0, trackingData->maxFrameIndex);
        initPlots(objects);
        emit ui->objectAll->clicked(true);
        scale->setText(QString("1px = %1m and 1 timestep = %2s").arg(QString::number(ruler), QString::number(timeScale)));
      }
    }
  });
  ui->toolBar->addAction(optionAction);

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

  if (!isStandalone) {
    openAction->setVisible(false);
  }

  // Chart and ChartView are added there. Plots are defined in initPlots.
  // Then refresh and clear are automatically endled.
  QList<QString> plotLabel{"Trajectory", "Displacement", "Velocity"};
  for (int i = 0; i < plotLabel.size(); i++) {
    plots.append(new QChart());
    QChartView* chartView = new QChartView(plots[i]);
    chartView->setRubberBand(QChartView::RectangleRubberBand);
    ui->tabPlot->addTab(chartView, plotLabel[i]);
  }
}

/**
 * @brief Opens a dialogue to select tracking data.
 */
void StatAnalysis::openTrackingData() {
  QString file = QFileDialog::getOpenFileName(this, tr("Open Tracking Data"), memoryDir, tr("Tracking Database (*.db)"));
  if (!file.isEmpty()) {
    openTrackingData(file);
  }
  memoryDir = file;
}

void StatAnalysis::openTrackingData(const QString& file, bool loadOnDemand) {
  clear();
  QString dir = QFileInfo(file).dir().path();  // Data takes the tracking folder.
  QApplication::setOverrideCursor(Qt::WaitCursor);
  trackingData->setPath(dir);
  if (!trackingData->isEmpty && !loadOnDemand) {  // If loadOnDemand is true, need to call reload to actually load data.
    loadObjectList();
    QList<int> objects = trackingData->getId(0, trackingData->maxFrameIndex);
    initPlots(objects);
  }
  isData = !trackingData->isEmpty;
  QApplication::restoreOverrideCursor();
}

void StatAnalysis::openTrackingData(Data* data, bool loadOnDemand) {
  clear();
  QApplication::setOverrideCursor(Qt::WaitCursor);
  trackingData = new Data(*data);
  if (!trackingData->isEmpty && !loadOnDemand) {  // If loadOnDemand is true, need to call reload to actually load data.
    loadObjectList();
    QList<int> objects = trackingData->getId(0, trackingData->maxFrameIndex);
    initPlots(objects);
  }
  isData = !trackingData->isEmpty;
  QApplication::restoreOverrideCursor();
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

void StatAnalysis::reload() {
  if (isData) {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    ui->objectList->setRowCount(0);  // Remove all rows
    clearPlots();
    ruler = 1;
    loadObjectList();
    QList<int> objects = trackingData->getId(0, trackingData->maxFrameIndex);
    initPlots(objects);
    QApplication::restoreOverrideCursor();
  }
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
      series->append(data.value(QStringLiteral("xBody")).at(i) * ruler, data.value(QStringLiteral("yBody")).at(i) * ruler);
    }
    series->setName(QString::number(object));
    traj->addSeries(series);
  }
  traj->createDefaultAxes();

  // Displacement plot
  QList<QList<double>> velocity;

  QChart* dis = plots[1];
  for (auto const& object : objects) {
    QHash<QString, QList<double>> data = trackingData->getDataId(object);
    if (data.value(QStringLiteral("imageNumber")).size() > 4) {  // Need at least 4 points in the trajectory
      auto x = data.value(QStringLiteral("xBody"));
      std::adjacent_difference(x.begin(), x.end(), x.begin());
      auto y = data.value(QStringLiteral("yBody"));
      std::adjacent_difference(y.begin(), y.end(), y.begin());
      auto t = data.value(QStringLiteral("imageNumber"));
      std::transform(x.begin(), x.end(), y.begin(), x.begin(), [](double& a, double& b) { return pow(pow(a, 2) + pow(b, 2), 0.5); });
      std::adjacent_difference(t.begin(), t.end(), t.begin());
      std::transform(x.begin(), x.end(), t.begin(), x.begin(), std::divides<double>());
      x.removeAt(0);  // Remove first element that is keep unchanged with adjacent_difference to keep size.
      velocity.append(x);
      std::sort(x.begin(), x.end());
      QBoxPlotSeries* displacements = new QBoxPlotSeries();
      QBoxSet* set = new QBoxSet(" ");  // " " Avoid number label on xaxis
      set->setValue(QBoxSet::LowerExtreme, x.first() * ruler);
      set->setValue(QBoxSet::UpperExtreme, x.last() * ruler);
      set->setValue(QBoxSet::Median, median(0, x.size(), x) * ruler);
      set->setValue(QBoxSet::LowerQuartile, median(0, x.size() / 2, x) * ruler);
      set->setValue(QBoxSet::UpperQuartile, median(x.size() / 2 + (x.size() / 2 % 2), x.size(), x) * ruler);
      displacements->append(set);
      displacements->setName(QString::number(object));
      dis->addSeries(displacements);
    }
  }
  dis->createDefaultAxes();
  dis->axes(Qt::Vertical).at(0)->setTitleText(QStringLiteral("Displacement (m)"));

  // Time plot
  QChart* time = plots[2];
  for (auto const& object : objects) {
    QLineSeries* series = new QLineSeries(traj);
    QHash<QString, QList<double>> data = trackingData->getDataId(object);
    if (data.value(QStringLiteral("imageNumber")).size() > 4) {  // Need at least 4 points in the trajectory
      int i = 0;
      for (auto const& a : velocity[object]) {
        series->append(data.value(QStringLiteral("imageNumber")).at(i) * timeScale, a / timeScale);
        i++;
      }
      series->setName(QString::number(object));
      time->addSeries(series);
    }
  }
  time->createDefaultAxes();
  time->axes(Qt::Vertical).at(0)->setTitleText(QStringLiteral("Velocity (m/s)"));
  time->axes(Qt::Horizontal).at(0)->setTitleText(QStringLiteral("Time (s)"));
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

double StatAnalysis::median(int begin, int end, const QList<double>& sortedList) {
  int count = end - begin;
  if (count % 2) {
    return sortedList.at(count / 2 + begin);
  }
  else {
    double right = sortedList.at(count / 2 + begin);
    double left = sortedList.at(count / 2 - 1 + begin);
    return (right + left) / 2.0;
  }
}

StatAnalysis::~StatAnalysis() {
  clear();
  delete trackingData;
  delete ui;
}
