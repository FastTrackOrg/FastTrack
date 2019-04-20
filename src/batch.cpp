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

Batch::Batch(QWidget* parent) : QWidget(parent),
                                ui(new Ui::Batch) {
  ui->setupUi(this);
  // Setup the ui
  QDir::setCurrent(QCoreApplication::applicationDirPath());
  setWindowState(Qt::WindowMaximized);
  setWindowTitle("Fast Track");
  ui->tableParameters->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  ui->tablePath->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

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
  cv::ocl::setUseOpenCL(false);

  ////////////////////////////Parameters panel/////////////////////////////////////////////

  // Defines all the parameters to fill the parameter panel.
  // For initialization the parameters are stored in a QMap with <QString>key: name of the parameter
  // and <QString>value: value | description of the parameter for the initialization
  // After initialization the QMap parameterList is {<QString> key, <QString> value}
  ui->tableParameters->horizontalHeader()->setStretchLastSection(true);
  // Default Name, value|description
  parameterList.insert("Dilatation", "0|Dilates the image. Sets to 0 for no dilatation.");
  parameterList.insert("Maximal time", "50|.");
  parameterList.insert("Registration", "no|Sets to yes to activate registration.");
  parameterList.insert("Background method", "3|");
  parameterList.insert("Light background", "yes|Sets to yes if objects are dark on light background. Sets to no if objects are light on dark background.");
  parameterList.insert("ROI bottom y", "0|Bottom corner y coordinate of the region of interest. Sets to 0 for keeping the full image.");
  parameterList.insert("ROI bottom x", "0|Bottom corner x coordinate of the region of interest. Sets to 0 for keeping the full image.");
  parameterList.insert("ROI top y", "0|Top corner y coordinate of the region of interest. Sets to 0 for keeping the full image.");
  parameterList.insert("ROI top x", "0|Top corner x coordinate of the region of interest. Sets to 0 for keeping the full image.");
  parameterList.insert("Number of images background", "50|The number of images averaged to compute the background.");
  parameterList.insert("Arrow size", "2|Size of the arrow in the tracking display.");
  parameterList.insert("Maximal occlusion", "100|The maximum assignment distance in pixels. Only objects that have moved less than this value between two images are considered for the matching phase.");
  parameterList.insert("Weight", "0.5|Closer to one the cost function used in the matching phase will be more sensitive to change in distance, closer to zero it will be more sensitive to change in direction.");
  parameterList.insert("Maximal angle", "90|The maximal change in orientation in degrees of an object between two consecutive images.");
  parameterList.insert("Maximal length", "100|The maximal displacement in pixels of an object between two consecutive images.");
  parameterList.insert("Spot to track", "0|The part of the object used for the matching. 0: head, 1: tail, 2: full body.");
  parameterList.insert("Binary threshold", "70|The threshold value (0 to 255) to separate objects from the background.");
  parameterList.insert("Minimal size", "50|The minimal size of an object in pixels.");
  parameterList.insert("Maximal size", "5000|The maximal size of an object to in pixels.");

  loadSettings();

  // Fills the parameters table with the default names values and descriptions
  // and deletes the descriptions that are not used anymore
  for (int i = 0; i < parameterList.keys().length(); i++) {
    ui->tableParameters->setSortingEnabled(false);
    ui->tableParameters->insertRow(i);
    QString parameterName = parameterList.keys()[i];
    ui->tableParameters->setItem(i, 0, new QTableWidgetItem(parameterName));
    ui->tableParameters->item(i, 0)->setFlags(ui->tableParameters->item(i, 0)->flags() & ~Qt::ItemIsEditable);
    QStringList parameterAttributs = parameterList.value(parameterName).split('|');
    ui->tableParameters->setItem(i, 1, new QTableWidgetItem(parameterAttributs[0]));
    ui->tableParameters->setItem(i, 2, new QTableWidgetItem(parameterAttributs[1]));
    ui->tableParameters->item(i, 2)->setFlags(ui->tableParameters->item(i, 2)->flags() & ~Qt::ItemIsEditable);
    parameterList.insert(parameterName, parameterAttributs[0]);
    ui->tableParameters->setRowHeight(i, 60);
  }

  connect(ui->tableParameters, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(updateParameterList(QTableWidgetItem*)));
  connect(ui->loadSettings, &QPushButton::pressed, this, &Batch::openParameterFile);

  ////////////////////////////Path panel/////////////////////////////////////////////
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

/*******************************************************************************************\
                                    Path panel
\*******************************************************************************************/

/**
  * @brief Opens a dialogue window to select a folder and updates ui->textPathAdd. Triggered when ui->openPath is clicked.
*/
void Batch::openPathFolder() {
  ui->textBackgroundAdd->clear();
  ui->textPathAdd->clear();
  ui->textLoadSettings->clear();
  QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), memoryDir, QFileDialog::ShowDirsOnly);

  // If Tracking_Result folder selectionned, get the path to the top directory
  if (dir.right(15) == "Tracking_Result") {
    dir.truncate(dir.size() - 15);
  }

  if (dir.length()) {
    ui->textPathAdd->setText(dir);
    // Autoloads  background and tracking if auto is checked
    if (ui->isAuto->isChecked()) {
      ui->textBackgroundAdd->setText(dir + "Tracking_Result/background.pgm");
      if (loadParameterFile(dir + "Tracking_Result/parameter.param")) {
        ui->textLoadSettings->setText(dir + "Tracking_Result/parameter.param");
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
  if (!processList.isEmpty()) {
    ui->startButton->setDisabled(true);

    if (QDir(processList[0].path).exists()) {
      string path = (processList[0].path + QDir::separator()).toStdString();
      string backgroundPath = processList[0].backgroundPath.toStdString();

      thread = new QThread;
      tracking = new Tracking(path, backgroundPath);
      tracking->moveToThread(thread);

      connect(thread, &QThread::started, tracking, &Tracking::startProcess);
      connect(this, &Batch::newParameterList, tracking, &Tracking::updatingParameters);
      // When the analysis is finished, clears the path in the ui and removes it from the pathList
      connect(tracking, &Tracking::finished, [this]() {
        processList.removeFirst();
        ui->tablePath->removeRow(0);
      });

      connect(tracking, &Tracking::finished, this, &Batch::next);
      connect(tracking, &Tracking::error, this, &Batch::errors);
      connect(tracking, &Tracking::finished, thread, &QThread::quit);
      connect(tracking, &Tracking::finished, tracking, &Tracking::deleteLater);
      connect(thread, &QThread::finished, thread, &QThread::deleteLater);

      tracking->updatingParameters(processList[0].trackingParameters);
      thread->start();
    }
    else {
      processList.removeFirst();
      ui->tablePath->removeRow(0);
      ui->startButton->setDisabled(false);
    }
  }
  else {
    ui->startButton->setDisabled(false);
  }
}

/*******************************************************************************************\
                                    Settings
\*******************************************************************************************/

/**
  * @brief Updates the parameterList vector with the new parameter when users changes a parameter in the QTableWidget of parameters. Triggered when ui->tableParameters is modified. Emits the updated parameters QMap.
  * @param[in] item QTableWidgetItem from a QTableWidget.
*/
void Batch::updateParameterList(QTableWidgetItem* item) {
  int row = item->row();
  QString parameterName = ui->tableParameters->item(row, 0)->text();
  QString parameterValue = ui->tableParameters->item(row, 1)->text();
  parameterList.insert(parameterName, parameterValue);
  saveSettings();
  emit(newParameterList(parameterList));
}

/**
  * @brief Loads the settings file at the startup of the program and updates the ui->parameterTable with the new parameters.
*/
void Batch::loadSettings() {
  settingsFile = new QSettings("FastTrack", "Benjamin Gallois", this);
  settingsFile->setFallbacksEnabled(false);  // Shadows global variables added in MacOs system
  QStringList keyList = settingsFile->allKeys();

  for (auto a : keyList) {
    QStringList parameterDescription = parameterList.value(a).split('|');
    QString valueDescription = QString(settingsFile->value(a).toString() + "|" + parameterDescription[1]);
    parameterList.insert(a, valueDescription);
  }
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
  for (int i = 0; i < ui->tableParameters->rowCount(); i++) {
    QString label = ui->tableParameters->item(i, 0)->text();
    ui->tableParameters->item(i, 1)->setText(parameterList.value(label));
  }
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
