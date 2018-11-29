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

    qRegisterMetaType<UMat>("UMat&");
    //qRegisterMetaType<QMap<QString, QString>("QMap<QString, QString>");
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

    // If config file exist load saved parameter values
    // Note: will be changed with QT build in module for saving conf file
    QFile file("conf.txt");
    if(QFileInfo("conf.txt").exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while(!in.atEnd()) {
            QStringList parameterNameValue = in.readLine().split('|');
            QStringList parameterDescription = parameterList.value(parameterNameValue.at(0)).split('|');
            QString valueDescription = QString(parameterNameValue.at(1) + "|" + parameterDescription.at(1)); 
            parameterList.insert(parameterNameValue.at(0), valueDescription);
        }
        file.close();
    }


    // Fill the parameters table with the default names values and descriptions
    // and delete the description that are not used anymore
    for(int i = 0; i < parameterList.keys().length(); i++) {
      ui->tableParameters->setSortingEnabled(false);
      ui->tableParameters->insertRow(i);
      QString parameterName = parameterList.keys().at(i);
      ui->tableParameters->setItem(i, 0, new QTableWidgetItem(parameterName));
      QStringList parameterAttributs = parameterList.value(parameterName).split('|');
      ui->tableParameters->setItem(i, 1, new QTableWidgetItem(parameterAttributs.at(0)));
      ui->tableParameters->setItem(i, 2, new QTableWidgetItem(parameterAttributs.at(1)));
      parameterList.insert(parameterName, parameterAttributs.at(0));
     }


    connect(ui->tableParameters, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(updateParameterList(QTableWidgetItem*)));

////////////////////////////Path panel/////////////////////////////////////////////
    ui->tablePath->horizontalHeader()->setStretchLastSection(true);
    ui->tablePath->setSortingEnabled(false);
    pathCounter = 0;
    connect(ui->addPath, SIGNAL(clicked()), this, SLOT(addPath()));
    connect(ui->startButton, SIGNAL(clicked()), this, SLOT(startTracking()));
    connect(this, SIGNAL(next()), this, SLOT(startTracking()));

}


void MainWindow::updateParameterList(QTableWidgetItem* item) {
    int row = item->row();
    QString parameterName = ui->tableParameters->item(row, 0)->text();
    QString parameterValue = ui->tableParameters->item(row, 1)->text();
    parameterList.insert(parameterName, parameterValue);
    emit(newParameterList(parameterList));
}

void MainWindow::addPath() {
    int row = ui->tablePath->rowCount();
    QString path = ui->textPathAdd->toPlainText();
    ui->tablePath->insertRow(row);
    ui->tablePath->setItem(row, 0, new QTableWidgetItem(path));
    pathList.append(path);
    ui->textPathAdd->clear();
}

void MainWindow::newAnalysis(string path) {

    cout << "main QThreadID is " << QThread::currentThread()  << endl;
    thread = new QThread;
    tracking = new Tracking(path);
    tracking->moveToThread(thread);
    connect(thread, SIGNAL(started()), tracking, SLOT(startProcess()));
    connect(this, SIGNAL(newParameterList(QMap<QString, QString>)), tracking, SLOT(updatingParameters(QMap<QString, QString>)));
    //connect(tracking, SIGNAL(finished()), this, SIGNAL(next()));
    connect(tracking, SIGNAL(newImageToDisplay(UMat&, UMat&)), this, SLOT(display(UMat&, UMat&)));
    connect(tracking, SIGNAL(finished()), thread, SLOT(quit()));
    connect(tracking, SIGNAL(finished()), tracking, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    pathCounter ++;
    thread->start();
    tracking->updatingParameters(parameterList);
}

void MainWindow::startTracking() {
    string path = pathList.at(pathCounter).toStdString();
    newAnalysis(path);
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
}


MainWindow::~MainWindow()
{
    // Save parameters in a file that will be reload at the next strat up of the application
    QFile file("conf.txt");

    if(file.open(QIODevice::ReadWrite | QIODevice::Text)) {
      QList<QString> keyList = parameterList.keys();
      QTextStream stream( &file );
      for(auto a: keyList) {
        stream << a << "|" << parameterList.value(a) << endl;
    }
        file.close();
    }
    delete ui;
}

