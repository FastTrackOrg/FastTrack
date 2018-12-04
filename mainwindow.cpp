#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QFile>
#include <QFileInfo>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <string>
#include <functions.h>
#include <opencv2/core/ocl.hpp>
#include <ctime>



using namespace cv;
using namespace std;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    // Setup the ui
    QDir::setCurrent(QCoreApplication::applicationDirPath());
    ui->setupUi(this);
    setWindowState(Qt::WindowMaximized);
    setWindowTitle("Fishy Tracking");
    statusBar()->showMessage(tr("Ready"));
    ui->tableParameters->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tablePath->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);



    qRegisterMetaType<UMat>("UMat&");
    qRegisterMetaType<QMap<QString, QString>>("QMap<QString, QString>");
    cv::ocl::setUseOpenCL(false);
    // Setup style
    QFile stylesheet(":/darkTheme.qss");

    if(stylesheet.open(QIODevice::ReadOnly | QIODevice::Text)) { // Read the theme file
        qApp->setStyleSheet(stylesheet.readAll());
        stylesheet.close();
    }

    else{ // If the theme is not found print an error (for dev purpose)
        cout << "Theme not found" << '\n';
    }




////////////////////////////Parameters panel/////////////////////////////////////////////
    
    // Defines all parameters to fill the parameter panel.
    // For initialization parameters are stored in a QMap with <QString>key: name of the parameter
    // and <QString>value: value | description of the parameter for the initialization
    // After initialization the QMap parameterList is {<QString> key, <QString> value}
    ui->tableParameters->horizontalHeader()->setStretchLastSection(true);

    // Default Name, value|description
    parameterList.insert("Registration", "yes|Yes if darks object on light background. No if light objects on dark background. ");
    parameterList.insert("Light background", "yes|Yes if darks object on light background. No if light objects on dark background. ");
    parameterList.insert("ROI bottom y", "0|");
    parameterList.insert("ROI bottom x", "0|");
    parameterList.insert("ROI top y", "0|");
    parameterList.insert("ROI top x", "0|");
    parameterList.insert("Number image background", "0|");
    parameterList.insert("Arrow size", "0|");
    parameterList.insert("Maximal occlusion", "0|");
    parameterList.insert("Weight", "0|");
    parameterList.insert("Maximal angle", "0|");
    parameterList.insert("Maximal length", "0|");
    parameterList.insert("Spot to track", "0|What spot to track. 0: head, 1: tail, 2: center");
    parameterList.insert("Binary threshold", "124|Threshold to separate background and object. Range from 0 to 255.");
    parameterList.insert("Minimal size", "1000|Minimal size of the object to track in pixels.");
    parameterList.insert("Maximal size", "1000|Maximal size of the object to track in pixels.");
    parameterList.insert("Object number", "1|Number of moving object to track.");

    loadSettings();

    // Fill the parameters table with the default names values and descriptions
    // and delete the description that are not used anymore
    for(int i = 0; i < parameterList.keys().length(); i++) {
      ui->tableParameters->setSortingEnabled(false);
      ui->tableParameters->insertRow(i);
      QString parameterName = parameterList.keys().at(i);
      ui->tableParameters->setItem(i, 0, new QTableWidgetItem(parameterName));
      ui->tableParameters->item(i, 0)->setFlags(ui->tableParameters->item(i, 0)->flags() & ~Qt::ItemIsEditable);
      QStringList parameterAttributs = parameterList.value(parameterName).split('|');
      ui->tableParameters->setItem(i, 1, new QTableWidgetItem(parameterAttributs.at(0)));
      ui->tableParameters->setItem(i, 2, new QTableWidgetItem(parameterAttributs.at(1)));
      ui->tableParameters->item(i, 2)->setFlags(ui->tableParameters->item(i, 2)->flags() & ~Qt::ItemIsEditable);
      parameterList.insert(parameterName, parameterAttributs.at(0));
     }


    connect(ui->tableParameters, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(updateParameterList(QTableWidgetItem*)));

////////////////////////////Path panel/////////////////////////////////////////////
    ui->tablePath->horizontalHeader()->setStretchLastSection(true);
    ui->tablePath->setSortingEnabled(false);
    pathCounter = 0;
    connect(ui->addPath, &QPushButton::clicked, this, &MainWindow::addPath);
    connect(ui->removePath, &QPushButton::clicked, this, &MainWindow::removePath);
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::startTracking);
    connect(this, &MainWindow::next, this, &MainWindow::startTracking);

}

/*!
  \fn void &MainWindow::updateParameterList(QTableWidget* item)

  Updates parameters from the \a item, if an element it is modified.

  If a parameter is modifyed, this method emit a signal with a new QMap
  containing new parameters.
*/
void MainWindow::updateParameterList(QTableWidgetItem* item) {
    int row = item->row();
    QString parameterName = ui->tableParameters->item(row, 0)->text();
    QString parameterValue = ui->tableParameters->item(row, 1)->text();
    parameterList.insert(parameterName, parameterValue);
    qInfo() << "Send parameters" << endl;
    emit(newParameterList(parameterList));
}

/*!
  \fn void &MainWindow::addPath()

  Inserts the content of the textPathAdd inside the tablePath and clears the textPathAdd in the UI.
  Inserts the content of the textPathAdd of the Ui inside the QList pathList that contains all path
  inserted by the user.

  AddPath is triggered when the button addPath of the UI is pressed.
*/
void MainWindow::addPath() {
    int row = ui->tablePath->rowCount();
    QString path = ui->textPathAdd->toPlainText();
    ui->tablePath->insertRow(row);
    ui->tablePath->setItem(row, 0, new QTableWidgetItem(path));
    pathList.append(path);
    ui->textPathAdd->clear();
}

/*!
  \fn void &MainWindow::removePath()

  Removes the content of the focused row of the pathTable of the UI.
  Removes the content of the focused row of the pathTable of the UI in
  the QList pathList that contains all path inserted by the user.

  RemovePath is triggered when the button removePath of the UI is pressed.
*/
void MainWindow::removePath() {
    int row = ui->tablePath->currentRow();
    if ( row != -1) { 
      pathList.remove(row);
      ui->tablePath->removeRow(row);
    }
}

void MainWindow::startTracking() {
    
    // If the list that contains all the path path to process is not empty
    // Creates a folder "Tracking_Result" inside the path and save the parameters file.
    // Start the tracking analysis in a new thread and manage creation and destruction
    // of the tracking object.
    if(!pathList.isEmpty()) {
      
      string path = pathList.at(0).toStdString();
      QDir().mkdir( QString::fromStdString(path) + QDir::separator() + "Tracking_Result" );
      

      // Saves parameters in a file named "parameter.txt"
      QFile parameterFile(QString::fromStdString(path) + QDir::separator() +  "Tracking_Result" + QDir::separator() + "parameter.txt" );
      if(!parameterFile.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox errorBox;
        errorBox.setText("You don't have the right to write in the selected folder!");
        errorBox.exec();
      }
      QTextStream out(&parameterFile);
      QList<QString> keyList = parameterList.keys();
      for(auto a: keyList) {
        out << a << " = " << parameterList.value(a) << endl; ;
      }
   
      // Member variables to display statistic in the ui 
      frameAnalyzed = 0;
    
    
      thread = new QThread;
      tracking = new Tracking(path);
      tracking->moveToThread(thread);
    
      connect(thread, &QThread::started, tracking, &Tracking::startProcess);
      connect(this, &MainWindow::newParameterList, tracking, &Tracking::updatingParameters);
      // When tha analysis is finished, clears the path in the Ui and removes it from the pathList
      connect(tracking, &Tracking::finished, [this]() {
        pathList.removeFirst();
        ui->tablePath->removeRow(0);
      });

      connect(tracking, &Tracking::finished, this, &MainWindow::next);
      connect(tracking, &Tracking::newImageToDisplay, this, &MainWindow::display);
      connect(tracking, &Tracking::finished, thread, &QThread::quit);
      connect(tracking, &Tracking::finished, tracking, &Tracking::deleteLater);
      connect(thread, &QThread::finished, thread, &QThread::deleteLater);
      
      thread->start();
      tracking->updatingParameters(parameterList);
  }
}

/**
    * @Display: display the originla image and/or the binary image in two Qlabels.
    * @param Mat visu: original image.
    * @param UMat cameraFrame: binary image.
*/
void MainWindow::display(UMat &visu, UMat &cameraFrame){

    int w = ui->display->width();
    int w2 = ui->display->width();
    int h = ui->display->height();
    int h2 = ui->display->height();

    ui->display->setPixmap(QPixmap::fromImage(QImage(cameraFrame.getMat(cv::ACCESS_READ).data, cameraFrame.cols, cameraFrame.rows, cameraFrame.step, QImage::Format_Grayscale8)).scaled(w, h, Qt::KeepAspectRatio));
    ui->display2->setPixmap(QPixmap::fromImage(QImage(visu.getMat(cv::ACCESS_READ).data, visu.cols, visu.rows, visu.step, QImage::Format_RGB888)).scaled(w2, h2, Qt::KeepAspectRatio));
  frameAnalyzed ++;
  statusBar()->showMessage(QString::number(frameAnalyzed));
}




/*******************************************************************************************\
                                    Settings
\*******************************************************************************************/

void MainWindow::loadSettings() {
    settingsFile = new QSettings("settings.ini", QSettings::NativeFormat, this);
    QStringList keyList = settingsFile->allKeys(); 

    for(auto a: keyList) {
      QStringList parameterDescription = parameterList.value(a).split('|');
      QString valueDescription = QString( settingsFile->value(a).toString()+ "|" + parameterDescription.at(1)); 
      parameterList.insert(a, valueDescription);
    }    
}

void MainWindow::saveSettings() {
      QList<QString> keyList = parameterList.keys();
      for(auto a: keyList) {
        settingsFile->setValue(QString(a), parameterList.value(a));
      }
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

