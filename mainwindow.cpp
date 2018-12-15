/*
This file is part of Fishy Tracking.

    FishyTracking is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FishyTracking is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
*/




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

/*!
    \class MainWindow
    \brief The MainWindow class is a widget displaying the main window of the program. It contains all the method to open an images sequences, sets parameters and begin a tracking analysis.
            Fishy Tracking.

*/




/*!
  \fn void &MainWindow::MainWindow(QWidget)

  Constructs the MainWindow and setup the UI.
*/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    // Setup the ui
    QDir::setCurrent(QCoreApplication::applicationDirPath());
    ui->setupUi(this);
    setWindowState(Qt::WindowMaximized);
    setWindowTitle("Fishy Tracking");
    ui->tableParameters->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tablePath->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    setupWindow = new SetupWindow(this);
    connect(ui->setupWindow, &QPushButton::clicked, [this]() {
      setupWindow->show();
    });



    qRegisterMetaType<UMat>("UMat&");
    qRegisterMetaType<QMap<QString, QString>>("QMap<QString, QString>");
    cv::ocl::setUseOpenCL(false);
    // Setup style
    QFile stylesheet(":/theme.qss");

    if(stylesheet.open(QIODevice::ReadOnly | QIODevice::Text)) { // Read the theme file
        qApp->setStyleSheet(stylesheet.readAll());
        stylesheet.close();
    }

////////////////////////////Parameters panel/////////////////////////////////////////////
    
    // Defines all parameters to fill the parameter panel.
    // For initialization parameters are stored in a QMap with <QString>key: name of the parameter
    // and <QString>value: value | description of the parameter for the initialization
    // After initialization the QMap parameterList is {<QString> key, <QString> value}
    ui->tableParameters->horizontalHeader()->setStretchLastSection(true);

    // Default Name, value|description
    parameterList.insert("Dilatation", "0|Dilate the image. Range 0 to inf.");
    parameterList.insert("Registration", "yes|Yes to activate registration.");
    parameterList.insert("Light background", "yes|Yes if dark objects on light background. No if light objects on dark background. ");
    parameterList.insert("ROI bottom y", "0|Defines region of interest bottom corner.");
    parameterList.insert("ROI bottom x", "0|Defines region of interest bottom corner.");
    parameterList.insert("ROI top y", "0|Defines region of interest top corner.");
    parameterList.insert("ROI top x", "0|Defines region of interest top corner.");
    parameterList.insert("Number of images background", "0|Number of images to average for computing the background.");
    parameterList.insert("Arrow size", "2|Display arrow on fish in the tracking display.");
    parameterList.insert("Maximal occlusion", "100|Maximum distance in pixel that a fish can travel when occluded.");
    parameterList.insert("Weight", "0.5|Relative importance between angle and distance. 0: only angle, 1: only distance. ");
    parameterList.insert("Maximal angle", "90|Maximal angular difference between two frames.");
    parameterList.insert("Maximal length", "100|Maximal distance travelled between two frames");
    parameterList.insert("Spot to track", "0|What spot to track. 0: head, 1: tail, 2: center");
    parameterList.insert("Binary threshold", "70|Threshold to separate background and object. Range from 0 to 255.");
    parameterList.insert("Minimal size", "50|Minimal size of the object to track in pixels.");
    parameterList.insert("Maximal size", "5000|Maximal size of the object to track in pixels.");
    parameterList.insert("Object number", "1|Number of moving objects to track.");

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




////////////////////////////Replay panel/////////////////////////////////////////////
    isReplayable = false;
    connect(ui->replayOpen, &QPushButton::clicked, this, &MainWindow::loadReplayFolder);
    connect(ui->replaySlider, &QSlider::valueChanged, this, &MainWindow::loadFrame);
    connect(ui->swapReplay, &QPushButton::clicked, this, &MainWindow::correctTracking);

}



/*******************************************************************************************\
                                    Path panel
\*******************************************************************************************/

/*!
  \fn void &MainWindow::updateParameterList(QTableWidgetItem* item)

  Updates a QMap when an \a item of a QTableWidget is modyfied.

  Triggered when a change is made in a QTableWidget.
  Emit the updated QMap.
*/
void MainWindow::updateParameterList(QTableWidgetItem* item) {
    int row = item->row();
    QString parameterName = ui->tableParameters->item(row, 0)->text();
    QString parameterValue = ui->tableParameters->item(row, 1)->text();
    parameterList.insert(parameterName, parameterValue);
    emit(newParameterList(parameterList));
}


/*!
  \fn void &MainWindow::addPath()

  Takes a QString from ui->textPathAdd, inserts it in a QVector pathList and updates the ui.

  Triggered when the button ui->addPath.
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

  Removes the content of the focused row of the ui->pathTable and
  updates the ui and the QVector pathList.

  Triggered when the button removePath of the UI is pressed.
*/
void MainWindow::removePath() {
    int row = ui->tablePath->currentRow();
    if ( row != -1) { 
      pathList.remove(row);
      ui->tablePath->removeRow(row);
    }
}


/*!
  \fn void &MainWindow::startTracking()
  Starts a new tracking analysis. First it get the path to the folder containing the
  images sequence. It creates a folder named Tracking_Result in this folder and a file
  parameters.txt containing the parameterList. It creates a new Tracking object that 
  have to be run in a separate thread. When the analysis is finished, the Tracking 
  object is destroyed and a new analysis is started.

  Trigerred when the start analysis button is clicked or when the
  signal finishedAnalysis() is emitted.
*/
void MainWindow::startTracking() {
    
    // If the list that contains all the path path to process is not empty
    // And if the path exist.
    // Creates a folder "Tracking_Result" inside the path and save the parameters file.
    // Start the tracking analysis in a new thread and manage creation and destruction
    // of the tracking object.
    if(!pathList.isEmpty()) {

      if (QDir(pathList.at(0)).exists()) {      
        string path = (pathList.at(0) + QDir::separator()).toStdString();
        QDir().mkdir( QString::fromStdString(path) + QDir::separator() + "Tracking_Result" );
        

        // Saves parameters in a file named "parameter.txt"
        QFile parameterFile(QString::fromStdString(path) + QDir::separator() +  "Tracking_Result" + QDir::separator() + "parameter.txt" );
        if(!parameterFile.open(QFile::WriteOnly | QFile::Text)){
          QMessageBox errorBox(this);
          errorBox.setText("You don't have the right to write in the selected folder!");
          errorBox.exec();
        }
        QTextStream out(&parameterFile);
        QList<QString> keyList = parameterList.keys();
        for(auto a: keyList) {
          out << a << " = " << parameterList.value(a) << endl;
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
    else {
        pathList.removeFirst();
        ui->tablePath->removeRow(0);
        QMessageBox errorBox;
        errorBox.setText("Wrong path");
        errorBox.exec();
    }
  }
}

/*!
  \fn void &MainWindow::display(Umat &visu, UMat &cameraFrame)
  
  Displays \a visu and \a cameraFrame image in the UI.
  
  Triggered when the signal newImageToDisplay is trigerred.
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

/*!
  \fn void &MainWindow::loadSettings()
  
  Loads a settings file settings.ini at the start up of the program.
  Updates the ui parameter QWidgetTable with the new parameters.
*/
void MainWindow::loadSettings() {
    settingsFile = new QSettings("settings.ini", QSettings::NativeFormat, this);
    QStringList keyList = settingsFile->allKeys(); 

    for(auto a: keyList) {
      QStringList parameterDescription = parameterList.value(a).split('|');
      QString valueDescription = QString( settingsFile->value(a).toString()+ "|" + parameterDescription.at(1)); 
      parameterList.insert(a, valueDescription);
    }    
}


/*!
  \fn void &MainWindow::saveSettings()
  
  Saves all parameters from ui QTableWidget in a settings file named settings.in.  
*/
void MainWindow::saveSettings() {
      QList<QString> keyList = parameterList.keys();
      for(auto a: keyList) {
        settingsFile->setValue(QString(a), parameterList.value(a));
      }
}


/*******************************************************************************************\
                                  Replay panel
\*******************************************************************************************/

/*!
  \fn void &MainWindow::loadReplayFolder()
  
  Loads a folder containing an image sequences and detect the image format.
  Loads tracking data if it exist.

  Trigerred when ui->pathButton is pressed.
*/
void MainWindow::loadReplayFolder() {

    // Delete existing data
    replayTracking.clear();
    replayFrames.clear();
    ui->object1Replay->clear();
    ui->object2Replay->clear();

    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home", QFileDialog::ShowDirsOnly);
    if (dir.length()) {
        ui->replayPath->setText(dir + QDir::separator());

        // Find image format
        QList<QString> extensions = { "pgm", "png", "jpeg", "jpg", "tiff", "tif", "bmp", "dib", "jpe", "jp2", "webp", "pbm", "ppm", "sr", "ras", "tif" };
        QDirIterator it(dir, QStringList(), QDir::NoFilter);
        QString extension;
        while (it.hasNext()) {
          extension = it.next().section('.', -1);
          if( extensions.contains(extension) ) break;
        }
  
        try{

          // Get path to all the frames in the folder a,d put it in a vector.
          // Setup the ui by setting maximum and minimum of the slider bar.
          string path = (dir + QDir::separator() + "*." + extension).toStdString();
          glob(path, replayFrames, false); // Get all path to frames
          ui->replaySlider->setMinimum(0);
          ui->replaySlider->setMaximum(replayFrames.size() - 1);
          isReplayable = true;

          // If tracking data are available, load data in a vector and find number of object in the frame.
          if( QDir().exists(dir + QDir::separator() + "Tracking_Result") ) {
            QFile trackingFile( dir + QDir::separator() + "Tracking_Result" + QDir::separator() + "tracking.txt");
            if (trackingFile.open(QIODevice::ReadOnly)) {
              QTextStream input(&trackingFile);
              QString line;
              while (input.readLineInto(&line)) {
                replayTracking.append(line);
              }
            }
            replayTracking.removeFirst(); // Delete header line

            QFile parameterFile( dir + QDir::separator() + "Tracking_Result" + QDir::separator() + "parameter.txt");
            if (parameterFile.open(QIODevice::ReadOnly)) {
              QTextStream input(&parameterFile);
              QString line;
              while (input.readLineInto(&line)) {
                if ( line.split(" = ", QString::SkipEmptyParts).at(0) == "Object number" ) {
                  replayNumberObject = line.split(" = ", QString::SkipEmptyParts).at(1).toInt();
                  break;
                }
              }
            }
          }

          QStringList range;
          for(int i = 0; i < replayNumberObject; i++) { 
            range.append(QString::number(i));
          }
          ui->object1Replay->addItems(range);
          ui->object2Replay->addItems(range);
      
          // Generates a color map.
          double a,b,c;
          srand (time(NULL));
          for (int j = 0; j < replayNumberObject ; ++j)  {
            a = rand() % 255;
            b = rand() % 255;
            c = rand() % 255;
            colorMap.push_back(Point3f(a, b, c));
          }

        }
        catch(...){
          isReplayable = false;
        }
    }
}



/*!
  \fn void &MainWindow::loadFrame(int frameIndex)
  
  Displays the image and the tracking in the ui->displayReplay. The image is selected
  when the slider ui->replaySlider is moved.

  Triggered when ui->replaySlider value changed.
*/
void MainWindow::loadFrame(int frameIndex) {

    if ( isReplayable ) {

      Mat frame = imread(replayFrames.at(frameIndex), IMREAD_COLOR);
     
      // Takes the tracking data corresponding to the replayed frame and parse data to display
      // arrow on tracked objects.
      for (int i = frameIndex*replayNumberObject; i < frameIndex*replayNumberObject + replayNumberObject; i++) {
        QStringList coordinate = replayTracking.at(i).split('\t', QString::SkipEmptyParts);
        cv::arrowedLine(frame, Point(coordinate.at(0).toDouble(), coordinate.at(1).toDouble()), Point(coordinate.at(0).toDouble() + 10*cos(coordinate.at(2).toDouble()), coordinate.at(1).toDouble() - 10*sin(coordinate.at(2).toDouble())), Scalar(colorMap.at(i - frameIndex*replayNumberObject).x, colorMap.at(i - frameIndex*replayNumberObject).y, colorMap.at(i - frameIndex*replayNumberObject).z), 2, 20, 0);
        cv::putText(frame, to_string(i - frameIndex*replayNumberObject), Point(coordinate.at(0).toDouble(), coordinate.at(1).toDouble()), cv::FONT_HERSHEY_SIMPLEX, 1.8, Scalar(colorMap.at(i - frameIndex*replayNumberObject).x, colorMap.at(i - frameIndex*replayNumberObject).y, colorMap.at(i - frameIndex*replayNumberObject).z), 5);
      }
      
      int w = ui->replayDisplay->width();
      int h = ui->replayDisplay->height();
      ui->replayDisplay->setPixmap(QPixmap::fromImage(QImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888)).scaled(w, h, Qt::KeepAspectRatio));
    }
}


/*!
  \fn void &MainWindow::swapTrackingData(int firstObject, int secondObject, int from)
    
  Swaps \a firstObject and \a secondObject tracking data from \a from to the end.  
*/
void MainWindow::swapTrackingData(int firstObject, int secondObject, int from) {
    for ( unsigned int i = from; i < replayFrames.size(); i += replayNumberObject ) {
       QString tmp = replayTracking[i + secondObject];
       replayTracking.replace(i + secondObject, replayTracking.at(i + firstObject));
       replayTracking.replace(i + firstObject, tmp);
    }
}

/*!
  \fn void &MainWindow::correctTracking()

  Gets the index from ui->object{1,2}Replay QComboBox and the index of the
  ui->replaySlider and swaps the data and saves in tracking.txt the result.
  
  Triggered when ui->swapButton is pressed.
*/
void MainWindow::correctTracking() {

    // Swaps the data
    int firstObject = ui->object1Replay->currentIndex();
    int secondObject = ui->object2Replay->currentIndex();
    int start = ui->replaySlider->value();
    swapTrackingData(firstObject, secondObject, start);

    // Saves new tracking data
    QFile file(ui->replayPath->text()+ QDir::separator() + "Tracking_Result" + QDir::separator() + "tracking.txt");
    if (file.open(QFile::WriteOnly | QFile::Text)) {
      QTextStream out(&file);
      out << "xHead" << '\t' << "yHead" << '\t' << "tHead" << '\t'  << "xTail" << '\t' << "yTail      " << '\t' << "tTail"   << '\t'  << "xBody" << '\t' << "yBody" << '\t' << "tBody"   << '\t'  << "cur      vature" << '\t'  << "imageNumber" << endl; 
      for(auto& a: replayTracking) {
        out << a << endl;
      }
    }
    file.close();

}


/*!
  \fn void &MainWindow::~MainWindow()

  Destructs the MainWindow object.  
*/
MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

