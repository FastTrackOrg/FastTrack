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

#include "batch.h"
#include "ui_batch.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace cv;
using namespace std;

/**
 * @class Batch
 *
 * @brief The Batch widget provides an convenient way to add multiple files to analyze.
 *
 * @author Benjamin Gallois
 *
 * @version $Revision: 460 $
 *
 * Contact: gallois.benjamin08@gmail.com
 *
 */

/**
  * @brief Constructs the Batch widget.
*/
Batch::Batch(QWidget *parent) : QWidget(parent),
                                ui(new Ui::Batch) {
  ui->setupUi(this);

  // Setup the ui
  QDir::setCurrent(QCoreApplication::applicationDirPath());
  setWindowState(Qt::WindowMaximized);
  setWindowTitle("Fast Track");

  QIcon img = QIcon(":/assets/buttons/open.png");
  ui->openPath->setIcon(img);
  ui->openPath->setIconSize(QSize(ui->openPath->width(), ui->openPath->height()));

  img = QIcon(":/assets/buttons/openImage.png");
  ui->openBackground->setIcon(img);
  ui->openBackground->setIconSize(QSize(ui->openBackground->width(), ui->openBackground->height()));

  img = QIcon(":/assets/buttons/openFile.png");
  ui->loadSettings->setIcon(img);
  ui->loadSettings->setIconSize(QSize(ui->openBackground->width(), ui->openBackground->height()));

  // Registers QMetaType
  qRegisterMetaType<UMat>("UMat&");
  qRegisterMetaType<QMap<QString, QString>>("QMap<QString, QString>");

  // Unactivates OpenCl that can slow-down the program
  cv::ocl::setUseOpenCL(false);

  ui->tableParameters->horizontalHeader()->setStretchLastSection(true);
  ui->tableParameters->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  ui->tablePath->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  currentPathCount = 0;
  
  // Populates the parameter table
  ui->tableParameters->insertRow(0);
  ui->tableParameters->setItem(0, 0, new QTableWidgetItem("Background method"));
  ui->tableParameters->item(0, 0)->setFlags(ui->tableParameters->item(0, 0)->flags() & ~Qt::ItemIsEditable);
  QComboBox *backMethod = new QComboBox(ui->tableParameters);
  backMethod->addItems({"Minimum", "Maximum", "Average"});
  ui->tableParameters->setCellWidget(0, 1, backMethod);
  connect(backMethod, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Batch::updateParameters);
  
  ui->tableParameters->insertRow(1);
  ui->tableParameters->setItem(1, 0, new QTableWidgetItem("Number of images background"));
  ui->tableParameters->item(1, 0)->setFlags(ui->tableParameters->item(1, 0)->flags() & ~Qt::ItemIsEditable);
  QSpinBox *backNumber = new QSpinBox(ui->tableParameters);
  backNumber->setRange(0, 9999999);
  ui->tableParameters->setCellWidget(1, 1, backNumber);
  connect(backNumber, QOverload<int>::of(&QSpinBox::valueChanged), this, &Batch::updateParameters);

  ui->tableParameters->insertRow(2);
  ui->tableParameters->setItem(2, 0, new QTableWidgetItem("Binary threshold"));
  ui->tableParameters->item(2, 0)->setFlags(ui->tableParameters->item(2, 0)->flags() & ~Qt::ItemIsEditable);
  QSpinBox *binThresh = new QSpinBox(ui->tableParameters);
  binThresh->setRange(0, 255);
  ui->tableParameters->setCellWidget(2, 1, binThresh);
  connect(binThresh, QOverload<int>::of(&QSpinBox::valueChanged), this, &Batch::updateParameters);

  ui->tableParameters->insertRow(3);
  ui->tableParameters->setItem(3, 0, new QTableWidgetItem("Light background"));
  ui->tableParameters->item(3, 0)->setFlags(ui->tableParameters->item(3, 0)->flags() & ~Qt::ItemIsEditable);
  QComboBox *backType = new QComboBox(ui->tableParameters);
  backType->addItems({"Yes", "No"});
  ui->tableParameters->setCellWidget(3, 1, backType);
  connect(backType, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Batch::updateParameters);

  ui->tableParameters->insertRow(4);
  ui->tableParameters->setItem(4, 0, new QTableWidgetItem("RIO top x"));
  ui->tableParameters->item(4, 0)->setFlags(ui->tableParameters->item(4, 0)->flags() & ~Qt::ItemIsEditable);
  QSpinBox *xTop = new QSpinBox(ui->tableParameters);
  xTop->setRange(0, 9999999);
  ui->tableParameters->setCellWidget(4, 1, xTop);
  connect(xTop, QOverload<int>::of(&QSpinBox::valueChanged), this, &Batch::updateParameters);

  ui->tableParameters->insertRow(5);
  ui->tableParameters->setItem(5, 0, new QTableWidgetItem("ROI top y"));
  ui->tableParameters->item(5, 0)->setFlags(ui->tableParameters->item(5, 0)->flags() & ~Qt::ItemIsEditable);
  QSpinBox *yTop = new QSpinBox(ui->tableParameters);
  yTop->setRange(0, 9999999);
  ui->tableParameters->setCellWidget(5, 1, yTop);
  connect(yTop, QOverload<int>::of(&QSpinBox::valueChanged), this, &Batch::updateParameters);

  ui->tableParameters->insertRow(6);
  ui->tableParameters->setItem(6, 0, new QTableWidgetItem("ROI bottom x"));
  ui->tableParameters->item(6, 0)->setFlags(ui->tableParameters->item(6, 0)->flags() & ~Qt::ItemIsEditable);
  QSpinBox *xBottom = new QSpinBox(ui->tableParameters);
  xBottom->setRange(0, 9999999);
  ui->tableParameters->setCellWidget(6, 1, xBottom);
  connect(xBottom, QOverload<int>::of(&QSpinBox::valueChanged), this, &Batch::updateParameters);

  ui->tableParameters->insertRow(7);
  ui->tableParameters->setItem(7, 0, new QTableWidgetItem("ROI bottom y"));
  ui->tableParameters->item(7, 0)->setFlags(ui->tableParameters->item(6, 0)->flags() & ~Qt::ItemIsEditable);
  QSpinBox *yBottom = new QSpinBox(ui->tableParameters);
  yBottom->setRange(0, 9999999);
  ui->tableParameters->setCellWidget(7, 1, yBottom);
  connect(yBottom, QOverload<int>::of(&QSpinBox::valueChanged), this, &Batch::updateParameters);

  ui->tableParameters->insertRow(8);
  ui->tableParameters->setItem(8, 0, new QTableWidgetItem("Registration"));
  ui->tableParameters->item(8, 0)->setFlags(ui->tableParameters->item(8, 0)->flags() & ~Qt::ItemIsEditable);
  QComboBox *registration = new QComboBox(ui->tableParameters);
  registration->addItems({"No", "Yes"});
  ui->tableParameters->setCellWidget(8, 1, registration);
  connect(registration, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Batch::updateParameters);

  ui->tableParameters->insertRow(9);
  ui->tableParameters->setItem(9, 0, new QTableWidgetItem("Morphological operation"));
  ui->tableParameters->item(9, 0)->setFlags(ui->tableParameters->item(9, 0)->flags() & ~Qt::ItemIsEditable);
  QComboBox *morphType = new QComboBox(ui->tableParameters);
  morphType->addItems({"Erosion", "Dilatation", "Opening", "Closing", "Gradient", "Top hat", "Black hat", "Hit miss"});
  ui->tableParameters->setCellWidget(9, 1, morphType);
  connect(morphType, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Batch::updateParameters);

  ui->tableParameters->insertRow(10);
  ui->tableParameters->setItem(10, 0, new QTableWidgetItem("Kernel type"));
  ui->tableParameters->item(10, 0)->setFlags(ui->tableParameters->item(10, 0)->flags() & ~Qt::ItemIsEditable);
  QComboBox *kernelType = new QComboBox(ui->tableParameters);
  kernelType->addItems({"Rectangle", "Cross", "Ellipse"});
  ui->tableParameters->setCellWidget(10, 1, kernelType);
  connect(kernelType, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Batch::updateParameters);

  ui->tableParameters->insertRow(11);
  ui->tableParameters->setItem(11, 0, new QTableWidgetItem("Kernel size"));
  ui->tableParameters->item(11, 0)->setFlags(ui->tableParameters->item(11, 0)->flags() & ~Qt::ItemIsEditable);
  QSpinBox *kernelSize = new QSpinBox(ui->tableParameters);
  kernelSize->setRange(0, 9999999);
  ui->tableParameters->setCellWidget(11, 1, kernelSize);
  connect(kernelSize, QOverload<int>::of(&QSpinBox::valueChanged), this, &Batch::updateParameters);

  ui->tableParameters->insertRow(12);
  ui->tableParameters->setItem(12, 0, new QTableWidgetItem("Minimal size"));
  ui->tableParameters->item(12, 0)->setFlags(ui->tableParameters->item(12, 0)->flags() & ~Qt::ItemIsEditable);
  QSpinBox *minSize = new QSpinBox(ui->tableParameters);
  minSize->setRange(0, 9999999);
  ui->tableParameters->setCellWidget(12, 1, minSize);
  connect(minSize, QOverload<int>::of(&QSpinBox::valueChanged), this, &Batch::updateParameters);

  ui->tableParameters->insertRow(13);
  ui->tableParameters->setItem(13, 0, new QTableWidgetItem("Maximal size"));
  ui->tableParameters->item(13, 0)->setFlags(ui->tableParameters->item(13, 0)->flags() & ~Qt::ItemIsEditable);
  QSpinBox *maxSize = new QSpinBox(ui->tableParameters);
  maxSize->setRange(0, 9999999);
  ui->tableParameters->setCellWidget(13, 1, maxSize);
  connect(maxSize, QOverload<int>::of(&QSpinBox::valueChanged), this, &Batch::updateParameters);

  ui->tableParameters->insertRow(14);
  ui->tableParameters->setItem(14, 0, new QTableWidgetItem("Maximal length"));
  ui->tableParameters->item(14, 0)->setFlags(ui->tableParameters->item(14, 0)->flags() & ~Qt::ItemIsEditable);
  QSpinBox *maxLength = new QSpinBox(ui->tableParameters);
  maxLength->setRange(0, 9999999);
  ui->tableParameters->setCellWidget(14, 1, maxLength);
  connect(maxLength, QOverload<int>::of(&QSpinBox::valueChanged), this, &Batch::updateParameters);

  ui->tableParameters->insertRow(15);
  ui->tableParameters->setItem(15, 0, new QTableWidgetItem("Maximal angle"));
  ui->tableParameters->item(15, 0)->setFlags(ui->tableParameters->item(15, 0)->flags() & ~Qt::ItemIsEditable);
  QSpinBox *maxAngle = new QSpinBox(ui->tableParameters);
  maxAngle->setRange(0, 360);
  ui->tableParameters->setCellWidget(15, 1, maxAngle);
  connect(maxAngle, QOverload<int>::of(&QSpinBox::valueChanged), this, &Batch::updateParameters);

  ui->tableParameters->insertRow(16);
  ui->tableParameters->setItem(16, 0, new QTableWidgetItem("Maximal time"));
  ui->tableParameters->item(16, 0)->setFlags(ui->tableParameters->item(16, 0)->flags() & ~Qt::ItemIsEditable);
  QSpinBox *maxTime = new QSpinBox(ui->tableParameters);
  maxTime->setRange(0, 9999999);
  ui->tableParameters->setCellWidget(16, 1, maxTime);
  connect(maxTime, QOverload<int>::of(&QSpinBox::valueChanged), this, &Batch::updateParameters);

  ui->tableParameters->insertRow(17);
  ui->tableParameters->setItem(17, 0, new QTableWidgetItem("Maximal occlusion"));
  ui->tableParameters->item(17, 0)->setFlags(ui->tableParameters->item(17, 0)->flags() & ~Qt::ItemIsEditable);
  QSpinBox *maxOccl = new QSpinBox(ui->tableParameters);
  maxOccl->setRange(0, 9999999);
  ui->tableParameters->setCellWidget(17, 1, maxOccl);
  connect(maxOccl, QOverload<int>::of(&QSpinBox::valueChanged), this, &Batch::updateParameters);

  ui->tableParameters->insertRow(18);
  ui->tableParameters->setItem(18, 0, new QTableWidgetItem("Spot to track"));
  ui->tableParameters->item(18, 0)->setFlags(ui->tableParameters->item(18, 0)->flags() & ~Qt::ItemIsEditable);
  QComboBox *spotType = new QComboBox(ui->tableParameters);
  spotType->addItems({"Head", "Tail", "Body"});
  spotType->setCurrentIndex(2);
  ui->tableParameters->setCellWidget(18, 1, spotType);
  connect(spotType, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Batch::updateParameters);

  ui->tableParameters->insertRow(19);
  ui->tableParameters->setItem(19, 0, new QTableWidgetItem("Weight"));
  ui->tableParameters->item(19, 0)->setFlags(ui->tableParameters->item(19, 0)->flags() & ~Qt::ItemIsEditable);
  QDoubleSpinBox *weight = new QDoubleSpinBox(ui->tableParameters);
  weight->setRange(0, 1);
  weight->setValue(0.5);
  weight->setSingleStep(0.1);
  ui->tableParameters->setCellWidget(19, 1, weight);
  connect(weight, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Batch::updateParameters);


  connect(ui->loadSettings, &QPushButton::pressed, this, &Batch::openParameterFile);
  loadSettings();

  // Setups the path panel
  ui->tablePath->horizontalHeader()->setStretchLastSection(true);
  ui->tablePath->setSortingEnabled(false);
  connect(ui->openPath, &QPushButton::clicked, this, &Batch::openPathFolder);
  connect(ui->openBackground, &QPushButton::clicked, this, &Batch::openPathBackground);
  connect(ui->addPath, &QPushButton::clicked, this, &Batch::addPath);
  connect(ui->removePath, &QPushButton::clicked, this, &Batch::removePath);
  connect(ui->startButton, &QPushButton::clicked, this, &Batch::startTracking);
  connect(ui->textPathAdd, &QLineEdit::textEdited, [this]() {
    ui->textBackgroundAdd->clear();
  });
  connect(ui->textLoadSettings, &QLineEdit::textEdited, [this]() {
    ui->textLoadSettings->clear();
  });
  connect(this, &Batch::next, this, &Batch::startTracking);
}

/**
  * @brief Opens a dialogue window to select a folder and updates the textPathAdd. Triggered when the openPath button is clicked.
*/
void Batch::openPathFolder() {
  ui->textBackgroundAdd->clear();
  ui->textPathAdd->clear();
  ui->textLoadSettings->clear();
  QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), memoryDir, QFileDialog::ShowDirsOnly);

  // If Tracking_Result folder selectionned, get the path to the top directory
  if (dir.right(15) == "Tracking_Result") {
    dir.truncate(dir.size() - 16);
  }

  if (dir.length()) {
    ui->textPathAdd->setText(dir);
    // Autoloads  background and tracking if auto is checked
    if (ui->isAuto->isChecked()) {
      if (QFileInfo(dir + "/Tracking_Result/background.pgm").exists()) {
        ui->textBackgroundAdd->setText(dir + "/Tracking_Result/background.pgm");
      }
      if (loadParameterFile(dir + "/Tracking_Result/parameter.param")) {
        ui->textLoadSettings->setText(dir + "/Tracking_Result/parameter.param");
        updateParameterTable();
      }
    }
  }
}

/**
  * @brief Opens a dialogue window to select a background image and updates ui->textBackgroundAdd. Triggered when ui->openBackground is clicked.
*/
void Batch::openPathBackground() {
  QString image = QFileDialog::getOpenFileName(this, tr("Open Image"), memoryDir);
  if (image.length()) {
    ui->textBackgroundAdd->setText(image);
  }
}

/**
  * @brief Takes the value of ui->textPathAdd and add it in the ui->tablePath and in the pathList vector.
*/
void Batch::addPath() {
  int row = ui->tablePath->rowCount();
  QString path = ui->textPathAdd->text();
  QString background = ui->textBackgroundAdd->text();
  ui->tablePath->insertRow(row);
  ui->tablePath->setItem(row, 0, new QTableWidgetItem(path));
  ui->tablePath->setItem(row, 1, new QTableWidgetItem(background));
  QProgressBar *progressBarPath = new QProgressBar(ui->tablePath);
  ui->tablePath->setCellWidget(row, 2, progressBarPath);
  process tmpProcess = {
      .path = path,
      .backgroundPath = background,
      .trackingParameters = parameterList};
  processList.append(tmpProcess);

  ui->textPathAdd->clear();
  ui->textBackgroundAdd->clear();
  ui->textLoadSettings->clear();
}

/**
  * @brief Deletes the selected line in the ui->tablePath and the corresponding path in the pathList. Triggered when the ui->removePath button is clicked.
*/
void Batch::removePath() {
  int row = ui->tablePath->currentRow();
  if (row != -1) {
    processList.remove(row);
    ui->tablePath->removeRow(row);
    currentPathCount --;
  }
}

/**
  * @brief Starts a new tracking analysis. First, it gets the path to the folder containing the image sequence. It creates a folder named Tracking_Result in this folder and a file parameters.txt containing the parameterList. It creates a new Tracking object that has to be run in a separate thread. When the analysis is finished, the Tracking object is destroyed and a new analysis is started. Triggerred when the start analysis button is clicked or when the signal finishedAnalysis() is emitted.
*/
void Batch::startTracking() {
  // If the list that contains all the path to process is not empty
  // And if the path exists.
  // Creates a folder "Tracking_Result" inside the path and save the parameters file.
  // Start the tracking analysis in a new thread and manage creation and destruction
  // of the tracking object.
  if (currentPathCount < processList.size()) {
    ui->startButton->setDisabled(true);
    ui->removePath->setDisabled(true);

    if (QDir(processList[currentPathCount].path).exists()) {
      int count = QDir(processList[currentPathCount].path).count(); 
      string path = (processList[currentPathCount].path + QDir::separator()).toStdString();
      string backgroundPath = processList[currentPathCount].backgroundPath.toStdString();
      QProgressBar *progressBar = qobject_cast<QProgressBar *>(ui->tablePath->cellWidget(currentPathCount, 2));
      progressBar->setRange(0, count);
      progressBar->setValue(0);

      thread = new QThread;
      tracking = new Tracking(path, backgroundPath);
      tracking->moveToThread(thread);

      connect(thread, &QThread::started, tracking, &Tracking::startProcess);
      connect(tracking, &Tracking::progress, progressBar, &QProgressBar::setValue);
      connect(tracking, &Tracking::finished, progressBar, [this, progressBar, count]() {
        ui->tablePath->setItem(currentPathCount, 3, new QTableWidgetItem("Done"));
        progressBar->setValue(count);
        currentPathCount ++;
        emit(next());
      });
      connect(tracking, &Tracking::finished, thread, &QThread::quit);
      connect(tracking, &Tracking::finished, tracking, &Tracking::deleteLater);
      connect(tracking, &Tracking::forceFinished, progressBar, [this, progressBar, count]() {
        ui->tablePath->setItem(currentPathCount, 3, new QTableWidgetItem("Error"));
        currentPathCount ++;
        emit(next());
      });
      connect(tracking, &Tracking::forceFinished, thread, &QThread::quit);
      connect(tracking, &Tracking::forceFinished, tracking, &Tracking::deleteLater);
      connect(thread, &QThread::finished, thread, &QThread::deleteLater);

      tracking->updatingParameters(processList[currentPathCount].trackingParameters);
      thread->start();
    }
    else {
      processList.removeFirst();
      ui->startButton->setDisabled(false);
      ui->removePath->setDisabled(false);
    }
  }
  else {
    ui->startButton->setDisabled(false);
    ui->removePath->setDisabled(false);
  }
}

/*******************************************************************************************\
                                    Settings
\*******************************************************************************************/

/**
  * @brief Updates the parameterList vector with the new parameter when users changes a parameter in the QTableWidget of parameters. Triggered when ui->tableParameters is modified. Emits the updated parameters QMap.
  * @param[in] item QTableWidgetItem from a QTableWidget.
*/
void Batch::updateParameters() {
  if (isEditable) {
    // Updates SpinBox parameters
    QVector<int> spinBoxIndexes = {1, 2, 4, 5, 6, 7, 11, 12, 13, 14, 15, 16, 17};
    QVector<int> doubleSpinBoxIndexes = {19};
    QVector<int> comboBoxIndexes = {3, 8, 9, 10, 18};

    for (auto &a : spinBoxIndexes) {
      parameterList.insert(ui->tableParameters->item(a, 0)->text(), QString::number(qobject_cast<QSpinBox *>(ui->tableParameters->cellWidget(a, 1))->value()));
    }
    for (auto &a : doubleSpinBoxIndexes) {
      parameterList.insert(ui->tableParameters->item(a, 0)->text(), QString::number(qobject_cast<QDoubleSpinBox *>(ui->tableParameters->cellWidget(a, 1))->value()));
    }
    for (auto &a : comboBoxIndexes) {
      parameterList.insert(ui->tableParameters->item(a, 0)->text(), QString::number(qobject_cast<QComboBox *>(ui->tableParameters->cellWidget(a, 1))->currentIndex()));
    }

    saveSettings();
    emit(newParameterList(parameterList));
  }
}

/**
  * @brief Loads the settings file at the startup of the program and updates the ui->parameterTable with the new parameters.
*/
void Batch::loadSettings() {
  settingsFile = new QSettings("FastTrack", "Benjamin Gallois", this);
  settingsFile->setFallbacksEnabled(false);  // Shadows global variables added in MacOs system
  QStringList keyList = settingsFile->allKeys();
  for (auto a : keyList) {
    parameterList.insert(a, settingsFile->value(a).toString());
  }
  updateParameterTable();
}

/**
  * @brief Saves all the parameters in the settings file.
*/
void Batch::saveSettings() {
  QList<QString> keyList = parameterList.keys();
  for (auto a : keyList) {
    settingsFile->setValue(QString(a), parameterList.value(a));
  }
}

/**
  * @brief Opens a dialogue to select a parameter file.
*/
void Batch::openParameterFile() {
  QString file = QFileDialog::getOpenFileName(this, tr("Open parameter file"), memoryDir, tr("Parameter file (*.param)"));
  if (!file.isEmpty()) {
    loadParameterFile(file);
    updateParameterTable();
    ui->textLoadSettings->setText(file);
  }
}

/**
  * @brief Reads a parameter file, updates parameters.
*/
bool Batch::loadParameterFile(QString path) {
  QFile parameterFile(path);
  if (parameterFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&parameterFile);
    QString line;
    QStringList parameters;
    while (in.readLineInto(&line)) {
      parameters = line.split(" = ", QString::SkipEmptyParts);
      parameterList.insert(parameters[0], parameters[1]);
    }
  }
  else
    return false;
  parameterFile.close();
  return true;
}

/**
  * @brief Takes the QMap parameterList and updates the parameters panel table..
*/
void Batch::updateParameterTable() {
  isEditable = false;
  for (int i = 0; i < ui->tableParameters->rowCount(); i++) {
    QString label = ui->tableParameters->item(i, 0)->text();
    if (qobject_cast<QSpinBox *>(ui->tableParameters->cellWidget(i, 1))) {
      qobject_cast<QSpinBox *>(ui->tableParameters->cellWidget(i, 1))->setValue(parameterList.value(label).toInt());
    }
    else if (qobject_cast<QDoubleSpinBox *>(ui->tableParameters->cellWidget(i, 1))) {
      qobject_cast<QDoubleSpinBox *>(ui->tableParameters->cellWidget(i, 1))->setValue(parameterList.value(label).toDouble());
    }
    else if (qobject_cast<QComboBox *>(ui->tableParameters->cellWidget(i, 1))) {
      qobject_cast<QComboBox *>(ui->tableParameters->cellWidget(i, 1))->setCurrentIndex(parameterList.value(label).toInt());
    }
  }
  isEditable = true;
}

/**
  * @brief Displays an error message.  
*/
void Batch::errors(int code) {
  QMessageBox errorBox(this);
  errorBox.setText("An error occurs during the tracking\n");
  switch (code) {
    case 0:
      errorBox.setText("Error message: provided background image is not correct. New background image is calculated instead.");
  }
  errorBox.exec();
}

Batch::~Batch() {
  saveSettings();
  delete ui;
}
