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




#include "mainwindow.h"

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

using namespace cv;
using namespace std;

/**
 * @class MainWindow
 * 
 * @brief The MainWindow class is derived from a QMainWindow widget. It displays the main window of the program.
 *
 * @author Benjamin Gallois
 *
 * @version $Revision: 4.0 $
 *
 * Contact: gallois.benjamin08@gmail.com
 *
*/




/**
 * @brief Constructs the MainWindow QObject and initializes the UI. 
*/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    // Setup the ui
    QDir::setCurrent(QCoreApplication::applicationDirPath());
    ui->setupUi(this);
    setWindowState(Qt::WindowMaximized);
    setWindowTitle("Fast Track");
    ui->tableParameters->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tablePath->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    QIcon img(":/buttons/play.png");
    ui->playReplay->setIcon(img);
    ui->playReplay->setIconSize(QSize(ui->playReplay->width(), ui->playReplay->height()));
    
    img = QIcon(":/buttons/open.png");
    ui->replayOpen->setIcon(img);
    ui->replayOpen->setIconSize(QSize(ui->replayOpen->width(), ui->replayOpen->height()));
    ui->openPath->setIcon(img);
    ui->openPath->setIconSize(QSize(ui->openPath->width(), ui->openPath->height()));
    
    img = QIcon(":/buttons/next.png");
    ui->replayNext->setIcon(img);
    ui->replayNext->setIconSize(QSize(ui->replayNext->width(), ui->replayNext->height()));

    img = QIcon(":/buttons/previous.png");
    ui->replayPrevious->setIcon(img);
    ui->replayPrevious->setIconSize(QSize(ui->replayPrevious->width(), ui->replayPrevious->height()));
    
    img = QIcon(":/buttons/help.png");
    ui->replayHelp->setIcon(img);
    ui->replayHelp->setIconSize(QSize(ui->replayHelp->width(), ui->replayHelp->height()));
    
    img = QIcon(":/buttons/save.png");
    ui->replaySave->setIcon(img);
    ui->replaySave->setIconSize(QSize(ui->replaySave->width(), ui->replaySave->height()));
    
    img = QIcon(":/buttons/refresh.png");
    ui->replayRefresh->setIcon(img);
    ui->replayRefresh->setIconSize(QSize(ui->replayRefresh->width(), ui->replayRefresh->height()));
    
    img = QIcon(":/buttons/openImage.png");
    ui->openBackground->setIcon(img);
    ui->openBackground->setIconSize(QSize(ui->openBackground->width(), ui->openBackground->height()));

    setupWindow = new SetupWindow(this);
    connect(ui->setupWindow, &QPushButton::clicked, [this]() {
      setupWindow->show();
    });

    // Keyboard shorcut
    // AZERTY keyboard shorcuts are set in the ui
    wShortcut = new QShortcut(QKeySequence("w"), this);
    connect(wShortcut, &QShortcut::activated, [this](){ ui->replayNext->animateClick(); });

    qShortcut = new QShortcut(QKeySequence("q"), this);
    connect(qShortcut, &QShortcut::activated, [this](){ ui->replaySlider->setValue(ui->replaySlider->value() - 1); });

    aShortcut = new QShortcut(QKeySequence("a"), this);
    connect(aShortcut, &QShortcut::activated, [this](){ ui->replaySlider->setValue(ui->replaySlider->value() - 1); });

    dShortcut = new QShortcut(QKeySequence("d"), this);
    connect(dShortcut, &QShortcut::activated, [this](){ ui->replaySlider->setValue(ui->replaySlider->value() + 1); });

    // Registers QMetaType
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
    
    // Defines all the parameters to fill the parameter panel.
    // For initialization the parameters are stored in a QMap with <QString>key: name of the parameter
    // and <QString>value: value | description of the parameter for the initialization
    // After initialization the QMap parameterList is {<QString> key, <QString> value}
    ui->tableParameters->horizontalHeader()->setStretchLastSection(true);
    // Default Name, value|description
    parameterList.insert("Dilatation", "0|Dilates the image. Sets to 0 for no dilatation.");
    parameterList.insert("Registration", "no|Sets to yes to activate registration.");
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
    parameterList.insert("Object number", "1|The number of objects to track.");

    loadSettings();

    // Fills the parameters table with the default names values and descriptions
    // and deletes the descriptions that are not used anymore
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
      ui->tableParameters->setRowHeight(i, 60);
     }

    connect(ui->tableParameters, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(updateParameterList(QTableWidgetItem*)));

////////////////////////////Path panel/////////////////////////////////////////////
    ui->tablePath->horizontalHeader()->setStretchLastSection(true);
    ui->tablePath->setSortingEnabled(false);
    connect(ui->openPath, &QPushButton::clicked, this, &MainWindow::openPathFolder);
    connect(ui->openBackground, &QPushButton::clicked, this, &MainWindow::openPathBackground);
    connect(ui->addPath, &QPushButton::clicked, this, &MainWindow::addPath);
    connect(ui->removePath, &QPushButton::clicked, this, &MainWindow::removePath);
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::startTracking);
    connect(ui->textPathAdd, &QLineEdit::textEdited, [this]() {
      ui->textBackgroundAdd->clear();
    });
    connect(this, &MainWindow::next, this, &MainWindow::startTracking);




////////////////////////////Replay panel/////////////////////////////////////////////
    isReplayable = false;
    framerate = new QTimer();
    ui->ellipseBox->addItems({"Head + Tail", "Head", "Tail", "Body"});
    ui->arrowBox->addItems({"Head + Tail", "Head", "Tail", "Body"});

    connect(ui->replayOpen, &QPushButton::clicked, this, &MainWindow::openReplayFolder);
    connect(ui->replayPath, &QLineEdit::textChanged, this, &MainWindow::loadReplayFolder);
    connect(ui->replaySlider, &QSlider::valueChanged, this, &MainWindow::loadFrame);
    connect(ui->replaySlider, &QSlider::valueChanged, [this](const int &newValue) {
      ui->replayNumber->setText(QString::number(newValue));
    });
    connect(ui->replayRefresh, &QPushButton::clicked, [this]() {
      loadReplayFolder(ui->replayPath->text());
    });
    connect(ui->swapReplay, &QPushButton::clicked, this, &MainWindow::correctTracking);
    
    connect(framerate, &QTimer::timeout, [this]() {
      ui->replaySlider->setValue(autoPlayerIndex);
      autoPlayerIndex++;
      if (autoPlayerIndex % int(replayFrames.size()) != autoPlayerIndex) {
        autoPlayerIndex = 0;
      }
    });
    connect(ui->replayFps, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this]() {
      if (isReplayable && ui->playReplay->isChecked()) {
        framerate->stop();
        framerate->start(1000/ui->replayFps->value());
      }
    });
    connect(ui->playReplay, &QPushButton::clicked, this, &MainWindow::toggleReplayPlay);
    
    connect(ui->replayNext, &QPushButton::clicked, this, &MainWindow::nextOcclusionEvent);
    connect(ui->replayPrevious, &QPushButton::clicked, this, &MainWindow::previousOcclusionEvent);

    connect(ui->replaySave, &QPushButton::clicked, this, &MainWindow::saveTrackedMovie);

    connect(ui->replayHelp, &QPushButton::clicked, [this]() {
          QMessageBox helpBox(this);
          helpBox.setIconPixmap(QPixmap(":/buttons/helpImg.png"));
          helpBox.exec();
    });

} // Constructor



/*******************************************************************************************\
                                    Path panel
\*******************************************************************************************/

/**
  * @brief Updates the parameterList vector with the new parameter when users changes a parameter in the QTableWidget of parameters. Triggered when ui->tableParameters is modified. Emits the updated parameters QMap.
  * @param[in] item QTableWidgetItem from a QTableWidget.
*/
void MainWindow::updateParameterList(QTableWidgetItem* item) {
    int row = item->row();
    QString parameterName = ui->tableParameters->item(row, 0)->text();
    QString parameterValue = ui->tableParameters->item(row, 1)->text();
    parameterList.insert(parameterName, parameterValue);
    emit(newParameterList(parameterList));
}

/**
  * @brief Opens a dialogue window to select a folder and updates ui->textPathAdd. Triggered when ui->openPath is clicked.
*/
void MainWindow::openPathFolder() {
    ui->textBackgroundAdd->clear();
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), memoryDir, QFileDialog::ShowDirsOnly);
    if (dir.length()) {
        ui->textPathAdd->setText(dir);
    }
}

/**
  * @brief Opens a dialogue window to select a background image and updates ui->textBackgroundAdd. Triggered when ui->openBackground is clicked.
*/
void MainWindow::openPathBackground() {
    QString image = QFileDialog::getOpenFileName(this, tr("Open Image"), memoryDir);
    if (image.length()) {
        ui->textBackgroundAdd->setText(image);
    }
}

/**
  * @brief Takes the value of ui->textPathAdd and add it in the ui->tablePath and in the pathList vector.
*/
void MainWindow::addPath() {
    int row = ui->tablePath->rowCount();
    QString path = ui->textPathAdd->text();
    QString background = ui->textBackgroundAdd->text();
    ui->tablePath->insertRow(row);
    ui->tablePath->setItem(row, 0, new QTableWidgetItem(path));
    ui->tablePath->setItem(row, 1, new QTableWidgetItem(background));
    pathList.append(path);
    backgroundList.append(background);
    ui->textPathAdd->clear();
    ui->textBackgroundAdd->clear();
}


/**
  * @brief Deletes the selected line in the ui->tablePath and the corresponding path in the pathList. Triggered when the ui->removePath button is clicked.
*/
void MainWindow::removePath() {
    int row = ui->tablePath->currentRow();
    if ( row != -1) { 
      pathList.remove(row);
      backgroundList.remove(row);
      ui->tablePath->removeRow(row);
    }
}


/**
  * @brief Starts a new tracking analysis. First, it gets the path to the folder containing the image sequence. It creates a folder named Tracking_Result in this folder and a file parameters.txt containing the parameterList. It creates a new Tracking object that has to be run in a separate thread. When the analysis is finished, the Tracking object is destroyed and a new analysis is started. Triggerred when the start analysis button is clicked or when the signal finishedAnalysis() is emitted.
*/
void MainWindow::startTracking() {
    
    // If the list that contains all the path to process is not empty
    // And if the path exists.
    // Creates a folder "Tracking_Result" inside the path and save the parameters file.
    // Start the tracking analysis in a new thread and manage creation and destruction
    // of the tracking object.
    if(!pathList.isEmpty()) {
      
      ui->startButton->setDisabled(true);

      if (QDir(pathList.at(0)).exists()) {      
        string path = (pathList.at(0) + QDir::separator()).toStdString();
        string backgroundPath = backgroundList.at(0).toStdString();
        QDir().mkdir( QString::fromStdString(path) + QDir::separator() + "Tracking_Result" );
        

        // Saves the parameters in a file named "parameter.txt"
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
     
      
      
        thread = new QThread;
        tracking = new Tracking(path, backgroundPath);
        tracking->moveToThread(thread);
      
        connect(thread, &QThread::started, tracking, &Tracking::startProcess);
        connect(this, &MainWindow::newParameterList, tracking, &Tracking::updatingParameters);
        // When the analysis is finished, clears the path in the ui and removes it from the pathList
        connect(tracking, &Tracking::finished, [this]() {
          pathList.removeFirst();
          backgroundList.removeFirst();
          ui->tablePath->removeRow(0);
        });

        connect(tracking, &Tracking::finished, this, &MainWindow::next);
        connect(tracking, &Tracking::newImageToDisplay, this, &MainWindow::display);
        connect(tracking, &Tracking::error, this, &MainWindow::errors);
        connect(tracking, &Tracking::finished, thread, &QThread::quit);
        connect(tracking, &Tracking::finished, tracking, &Tracking::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        
        tracking->updatingParameters(parameterList);
        thread->start();
    }
    else {
        pathList.removeFirst();
        backgroundList.removeFirst();
        ui->tablePath->removeRow(0);
        ui->startButton->setDisabled(false);
        QMessageBox errorBox(this);
        errorBox.setText("Wrong path");
        errorBox.exec();
    }
  }
  else {
    ui->startButton->setDisabled(false);
  }
}


/**
  * @brief Displays the images received from the Tracking object. Triggered when the signal newImageToDisplay is emitted.
*/
void MainWindow::display(UMat &visu, UMat &cameraFrame){

    int w = ui->display->width();
    int w2 = ui->display->width();
    int h = ui->display->height();
    int h2 = ui->display->height();

    ui->display->setPixmap(QPixmap::fromImage(QImage(cameraFrame.getMat(cv::ACCESS_READ).data, cameraFrame.cols, cameraFrame.rows, cameraFrame.step, QImage::Format_Grayscale8)).scaled(w, h, Qt::KeepAspectRatio));
    ui->display2->setPixmap(QPixmap::fromImage(QImage(visu.getMat(cv::ACCESS_READ).data, visu.cols, visu.rows, visu.step, QImage::Format_RGB888)).scaled(w2, h2, Qt::KeepAspectRatio));
}




/*******************************************************************************************\
                                    Settings
\*******************************************************************************************/

/**
  * @brief Loads the settings file at the startup of the program and updates the ui->parameterTable with the new parameters.
*/
void MainWindow::loadSettings() {

    settingsFile = new QSettings("FastTrack", "Benjamin Gallois", this);
    settingsFile->setFallbacksEnabled(false); // Shadows global variables added in MacOs system
    QStringList keyList = settingsFile->allKeys();

    for(auto a: keyList) {
      QStringList parameterDescription = parameterList.value(a).split('|');
      QString valueDescription = QString( settingsFile->value(a).toString()+ "|" + parameterDescription.at(1)); 
      parameterList.insert(a, valueDescription);
    }
}


/**
  * @brief Saves all the parameters in the settings file.
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

void MainWindow::openReplayFolder() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), memoryDir, QFileDialog::ShowDirsOnly);
    ui->replayPath->setText(dir + QDir::separator());
}


/**
  * @brief Loads a folder containing an image sequence and the tracking data if it exists. Triggerred when ui->pathButton is pressed.
  * @arg[in] dir Path to the folder where the image sequence is stored.
*/
void MainWindow::loadReplayFolder(QString dir) {

    // Delete existing data
    replayTracking.clear();
    replayFrames.clear();
    occlusionEvents.clear();
    ui->object1Replay->clear();
    ui->object2Replay->clear();
    framerate->stop();
    object = true;
    if (dir.length()) {

        // Finds image format
        QList<QString> extensions = { "pgm", "png", "jpeg", "jpg", "tiff", "tif", "bmp", "dib", "jpe", "jp2", "webp", "pbm", "ppm", "sr", "ras", "tif" };
        QDirIterator it(dir, QStringList(), QDir::NoFilter);
        QString extension;
        while (it.hasNext()) {
          extension = it.next().section('.', -1);
          if( extensions.contains(extension) ) break;
        }
  
        try{

          // Gets the paths to all the frames in the folder and puts it in a vector.
          // Setups the ui by setting maximum and minimum of the slider bar.
          string path = (dir + QDir::separator() + "*." + extension).toStdString();
          glob(path, replayFrames, false); // Gets all the paths to frames
          ui->replaySlider->setMinimum(0);
          ui->replaySlider->setMaximum(replayFrames.size() - 1);
          Mat frame = imread(replayFrames.at(0), IMREAD_COLOR);
          originalImageSize.setWidth(frame.cols);
          originalImageSize.setHeight(frame.rows);
          isReplayable = true;

          // If tracking data are available, load data in a vector and find the number of objects in the frame.
          if( QDir().exists(dir + QDir::separator() + "Tracking_Result") ) {
            QFile trackingFile( dir + QDir::separator() + "Tracking_Result" + QDir::separator() + "tracking.txt");
            if (trackingFile.open(QIODevice::ReadOnly)) {
              QTextStream input(&trackingFile);
              QString line;
              while (input.readLineInto(&line)) {
                replayTracking.append(line);
              }
            }
            replayTracking.removeFirst(); // Deletes header line


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
            // Finds occlusion events, ie line where the value is equal to "NaN"
            for(int i = 0; i < replayTracking.size(); i++) {
              if(replayTracking.at(i).split('\t', QString::SkipEmptyParts).at(0) == "NaN") {
                occlusionEvents.append( i / replayNumberObject);
              }
            }
            auto last = std::unique(occlusionEvents.begin(), occlusionEvents.end());
            occlusionEvents.erase(last, occlusionEvents.end());
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
        loadFrame(0);
        }
        catch(...){
          isReplayable = false;
        }
    }
}



/**
  * @brief Displays the image and the tracking data in the ui->displayReplay. Triggered when the ui->replaySlider value is changed.
*/
void MainWindow::loadFrame(int frameIndex) {

    if ( isReplayable ) {

      Mat frame = imread(replayFrames.at(frameIndex), IMREAD_COLOR);
      int scale = ui->replaySize->value();
     
      // Takes the tracking data corresponding to the replayed frame and parse data to display
      // arrow on the tracked objects.
      for (int i = frameIndex*replayNumberObject; i < frameIndex*replayNumberObject + replayNumberObject; i++) {
        
        if (replayTracking.at(i).split('\t', QString::SkipEmptyParts).at(0) == "NaN") {
          continue;
        }

        QStringList coordinate = replayTracking.at(i).split('\t', QString::SkipEmptyParts);

        if (ui->replayEllipses->isChecked()) {

          switch(ui->ellipseBox->currentIndex()) {
            
            case 0 : // Head + Tail
              cv::ellipse(frame, Point( coordinate.at(0).toDouble(), coordinate.at(1).toDouble() ), Size( coordinate.at(12).toDouble(), coordinate.at(13).toDouble() ), 180 - (coordinate.at(2).toDouble()*180)/M_PI, 0, 360,  Scalar(colorMap.at(i - frameIndex*replayNumberObject).x, colorMap.at(i - frameIndex*replayNumberObject).y, colorMap.at(i - frameIndex*replayNumberObject).z), scale, 8 );
              cv::ellipse(frame, Point( coordinate.at(3).toDouble(), coordinate.at(4).toDouble() ), Size( coordinate.at(15).toDouble(), coordinate.at(16).toDouble() ), 180 - (coordinate.at(5).toDouble()*180)/M_PI, 0, 360,  Scalar(colorMap.at(i - frameIndex*replayNumberObject).x, colorMap.at(i - frameIndex*replayNumberObject).y, colorMap.at(i - frameIndex*replayNumberObject).z), scale, cv::LINE_AA );
              break;

            case 1 : // Head
              cv::ellipse(frame, Point( coordinate.at(0).toDouble(), coordinate.at(1).toDouble() ), Size( coordinate.at(12).toDouble(), coordinate.at(13).toDouble() ), 180 - (coordinate.at(2).toDouble()*180)/M_PI, 0, 360,  Scalar(colorMap.at(i - frameIndex*replayNumberObject).x, colorMap.at(i - frameIndex*replayNumberObject).y, colorMap.at(i - frameIndex*replayNumberObject).z), scale, 8 );
              break;

            case 2 : // Tail
              cv::ellipse(frame, Point( coordinate.at(3).toDouble(), coordinate.at(4).toDouble() ), Size( coordinate.at(15).toDouble(), coordinate.at(16).toDouble() ), 180 - (coordinate.at(5).toDouble()*180)/M_PI, 0, 360,  Scalar(colorMap.at(i - frameIndex*replayNumberObject).x, colorMap.at(i - frameIndex*replayNumberObject).y, colorMap.at(i - frameIndex*replayNumberObject).z), scale, cv::LINE_AA );
              break;

            case 3 : // Body
              cv::ellipse(frame, Point( coordinate.at(6).toDouble(), coordinate.at(7).toDouble() ), Size( coordinate.at(18).toDouble(), coordinate.at(19).toDouble() ), 180 - (coordinate.at(8).toDouble()*180)/M_PI, 0, 360,  Scalar(colorMap.at(i - frameIndex*replayNumberObject).x, colorMap.at(i - frameIndex*replayNumberObject).y, colorMap.at(i - frameIndex*replayNumberObject).z), scale, 8 );
              break;
          }
        }

        if (ui->replayArrows->isChecked()) {
          
          switch(ui->arrowBox->currentIndex()) {
          
          case 0 :
            cv::arrowedLine(frame, Point(coordinate.at(0).toDouble(), coordinate.at(1).toDouble()), Point(coordinate.at(0).toDouble() + coordinate.at(12).toDouble()*cos(coordinate.at(2).toDouble()), coordinate.at(1).toDouble() - coordinate.at(12).toDouble()*sin(coordinate.at(2).toDouble())), Scalar(colorMap.at(i - frameIndex*replayNumberObject).x, colorMap.at(i - frameIndex*replayNumberObject).y, colorMap.at(i - frameIndex*replayNumberObject).z), scale, cv::LINE_AA, 0, double(scale)/10);
            cv::arrowedLine(frame, Point(coordinate.at(3).toDouble(), coordinate.at(4).toDouble()), Point(coordinate.at(3).toDouble() + coordinate.at(15).toDouble()*cos(coordinate.at(5).toDouble()), coordinate.at(4).toDouble() - coordinate.at(15).toDouble()*sin(coordinate.at(5).toDouble())), Scalar(colorMap.at(i - frameIndex*replayNumberObject).x, colorMap.at(i - frameIndex*replayNumberObject).y, colorMap.at(i - frameIndex*replayNumberObject).z), scale, cv::LINE_AA, 0, double(scale)/10);
            break;

          case 1 :
            cv::arrowedLine(frame, Point(coordinate.at(0).toDouble(), coordinate.at(1).toDouble()), Point(coordinate.at(0).toDouble() + coordinate.at(12).toDouble()*cos(coordinate.at(2).toDouble()), coordinate.at(1).toDouble() - coordinate.at(12).toDouble()*sin(coordinate.at(2).toDouble())), Scalar(colorMap.at(i - frameIndex*replayNumberObject).x, colorMap.at(i - frameIndex*replayNumberObject).y, colorMap.at(i - frameIndex*replayNumberObject).z), scale, cv::LINE_AA, 0, double(scale)/10);
            break;

          case 2 :
            cv::arrowedLine(frame, Point(coordinate.at(3).toDouble(), coordinate.at(4).toDouble()), Point(coordinate.at(3).toDouble() + coordinate.at(15).toDouble()*cos(coordinate.at(5).toDouble()), coordinate.at(4).toDouble() - coordinate.at(15).toDouble()*sin(coordinate.at(5).toDouble())), Scalar(colorMap.at(i - frameIndex*replayNumberObject).x, colorMap.at(i - frameIndex*replayNumberObject).y, colorMap.at(i - frameIndex*replayNumberObject).z), scale, cv::LINE_AA, 0, double(scale)/10);
            break;

          case 3 :
            cv::arrowedLine(frame, Point(coordinate.at(6).toDouble(), coordinate.at(7).toDouble()), Point(coordinate.at(6).toDouble() + coordinate.at(18).toDouble()*cos(coordinate.at(8).toDouble()), coordinate.at(7).toDouble() - coordinate.at(18).toDouble()*sin(coordinate.at(8).toDouble())), Scalar(colorMap.at(i - frameIndex*replayNumberObject).x, colorMap.at(i - frameIndex*replayNumberObject).y, colorMap.at(i - frameIndex*replayNumberObject).z), scale, cv::LINE_AA, 0, double(scale)/10);
            break;

          }
        }
        
        if (ui->replayNumbers->isChecked()) {
          cv::putText(frame, to_string(i - frameIndex*replayNumberObject), Point(coordinate.at(0).toDouble() + coordinate.at(12).toDouble()*cos(coordinate.at(2).toDouble()), coordinate.at(1).toDouble() - coordinate.at(12).toDouble()*sin(coordinate.at(2).toDouble()) ), cv::FONT_HERSHEY_SIMPLEX, double(scale)*0.5, Scalar(colorMap.at(i - frameIndex*replayNumberObject).x, colorMap.at(i - frameIndex*replayNumberObject).y, colorMap.at(i - frameIndex*replayNumberObject).z), scale*1.2, cv::LINE_AA);
        }
        
        if (ui->replayTrace->isChecked()) {
          // Gets the 20 previous cordinates to display the trajectory trace
          vector<Point> memory;
          for (int j = i - 20*replayNumberObject; j < i; j += replayNumberObject) {
            if (j > 0 && replayTracking.at(j).split('\t', QString::SkipEmptyParts).at(0) != "NaN") {
              QStringList coordinateMemory = replayTracking.at(j).split('\t', QString::SkipEmptyParts);
              memory.push_back(Point(coordinateMemory.at(6).toDouble(), coordinateMemory.at(7).toDouble()));
            }
          }
          cv::polylines(frame, memory, false, Scalar(colorMap.at(i - frameIndex*replayNumberObject).x, colorMap.at(i - frameIndex*replayNumberObject).y, colorMap.at(i - frameIndex*replayNumberObject).z), scale*1.2, cv::LINE_AA);
        }
      }
      
      double w = ui->replayDisplay->width();
      double h = ui->replayDisplay->height();
      QPixmap resizedPix = (QPixmap::fromImage(QImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888)).scaled(w, h, Qt::KeepAspectRatio));
      ui->replayDisplay->setPixmap(resizedPix);
      resizedFrame.setWidth(resizedPix.width());
      resizedFrame.setHeight(resizedPix.height());
    }
}

/**
  * @brief Starts the autoplay of the replay. Triggered when ui->playreplay is clicked.
*/
void MainWindow::toggleReplayPlay() {
  
    if (isReplayable && ui->playReplay->isChecked()) {
      QIcon img(":/buttons/pause.png");
      ui->playReplay->setIcon(img);
      ui->playReplay->setIconSize(QSize(ui->playReplay->width(), ui->playReplay->height()));
      framerate->start(1000/ui->replayFps->value());
      autoPlayerIndex = ui->replaySlider->value();
    }
    else if (isReplayable && !ui->playReplay->isChecked()) {
      QIcon img(":/buttons/resume.png");
      ui->playReplay->setIcon(img);
      ui->playReplay->setIconSize(QSize(ui->playReplay->width(), ui->playReplay->height()));
      framerate->stop();
    }
}

/**
  * @brief In the tracking data, swaps two objects from a selected index to the end.
  * @arg[in] firstObject First object to swap.
  * @arg[in] secondObject Second object to swap.
  * @arg[in] from Start index from which the data will be swapped.
*/
void MainWindow::swapTrackingData(int firstObject, int secondObject, int from) {
    for ( int i = from*replayNumberObject; i < int(replayFrames.size()*replayNumberObject); i += replayNumberObject ) {
       QString tmp = replayTracking[i + secondObject];
       replayTracking.replace(i + secondObject, replayTracking.at(i + firstObject));
       replayTracking.replace(i + firstObject, tmp);
    }
}

/**
  * @brief Gets the mouse coordinate in the frame of reference of the widget where the user has clicked.
  * @param[in] event Describes the mouse event.
*/
void MainWindow::mousePressEvent(QMouseEvent* event) {

    // Left click event
    if (event->buttons() == Qt::LeftButton) {

      double xTop = ((double(ui->replayDisplay->mapFrom(this, event->pos()).x())- 0.5*( ui->replayDisplay->width() - resizedFrame.width()))*double(originalImageSize.width()))/double(resizedFrame.width()) ; 
      double yTop = ((double(ui->replayDisplay->mapFrom(this, event->pos()).y()) - 0.5*( ui->replayDisplay->height() - resizedFrame.height()))*double(originalImageSize.height()))/double(resizedFrame.height()) ; 

      // If the replay tracking is available, computes the distance between the user click and all the
      // objects to find on which object the user has clicked
      if (!replayTracking.isEmpty()) {

        int frameIndex = ui->replaySlider->value();
        QVector<double> distance;

        // Computes the distance between the user click and all the objects
        // If the coordinate is not available due to occlusion, the distance is computed 
        // with the last previous known coordinate
        for (int i = frameIndex*replayNumberObject; i < frameIndex*replayNumberObject + replayNumberObject; i++) {
          QStringList coordinate = replayTracking.at(i).split('\t', QString::SkipEmptyParts);
          if (coordinate.at(0) != "NaN") {
            distance.append( pow( coordinate.at(0).toDouble() - xTop, 2 ) + pow( coordinate.at(1).toDouble() - yTop, 2) );
          }
          else {
            QStringList previousCoordinate = replayTracking.at(i - replayNumberObject).split('\t', QString::SkipEmptyParts);
            int count = 0;
            // Finds last known position of the occluded object
            while (previousCoordinate.at(0) == "NaN") {
              count++;
              previousCoordinate = replayTracking.at(i - replayNumberObject*count).split('\t', QString::SkipEmptyParts);
            }  
            distance.append( pow( previousCoordinate.at(0).toDouble() - xTop, 2 ) + pow( previousCoordinate.at(1).toDouble() - yTop, 2) );
          }
        }

      // Finds the minimal distance and updates the UI  
      int min = std::min_element(distance.begin(), distance.end()) - distance.begin();
      if (object) {
        ui->object1Replay->setCurrentIndex(min);
        ui->object1Replay->setStyleSheet("QComboBox { background-color: rgb(" + QString::number(colorMap.at(min).x) + "," + QString::number(colorMap.at(min).y) + "," + QString::number(colorMap.at(min).z) + "); }");
        object = false;
      }
      else {
        ui->object2Replay->setCurrentIndex(min);
        ui->object2Replay->setStyleSheet("QComboBox { background-color: rgb(" + QString::number(colorMap.at(min).x) + "," + QString::number(colorMap.at(min).y) + "," + QString::number(colorMap.at(min).z) + "); }");
        object = true;
      }
    }
  }

  // Right click event
  else if (event->buttons() == Qt::RightButton) {
    ui->swapReplay->animateClick();
    ui->object1Replay->setStyleSheet("QComboBox { background-color: white; }");
    ui->object2Replay->setStyleSheet("QComboBox { background-color: white; }");
  }
}


/**
  * @brief Gets the index of the two selected objects, the start index, swaps the data from the start index to the end, and saves the new tracking data. Triggered when ui->swapButton is pressed or a right-click event is registered inside the replayDisplay.
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
      out << "xHead" << '\t' << "yHead" << '\t' << "tHead" << '\t'  << "xTail" << '\t' << "yTail" << '\t' << "tTail"   << '\t'  << "xBody" << '\t' << "yBody" << '\t' << "tBody"   << '\t'  << "curvature"  << '\t' << "areaBody" << '\t' << "perimeterBody" << '\t' << "headMajorAxisLength" << '\t' << "headMinorAxisLength" << '\t' << "headExcentricity" << '\t' << "tailMajorAxisLength" << '\t' << "tailMinorAxisLength" << '\t' << "tailExcentricity" << '\t'<< "bodyMajorAxisLength" << '\t' << "bodyMinorAxisLength" << '\t' << "bodyExcentricity" << '\t' << "imageNumber" << endl;
      for(auto& a: replayTracking) {
        out << a << endl;
      }
    }
    file.close();
    loadFrame(ui->replaySlider->value());
}


/**
  * @brief Finds and displays the next occlusion event on the ui->replayDisplay. Triggered when ui->nextReplay is pressed.
*/
void MainWindow::nextOcclusionEvent() {
    if( !occlusionEvents.isEmpty() ){
      int current = ui->replaySlider->value();
      int nextOcclusion = *std::upper_bound(occlusionEvents.begin(), occlusionEvents.end(), current);
      ui->replaySlider->setValue(nextOcclusion);
    }
}
      

/**
  * @brief Finds and displays the previous occlusion event on the ui->replayDisplay. Triggered when ui->previousReplay is pressed.
*/
void MainWindow::previousOcclusionEvent() {
    if( !occlusionEvents.isEmpty() ){
      int current = ui->replaySlider->value();
      int previousOcclusion = occlusionEvents.at(std::upper_bound(occlusionEvents.begin(), occlusionEvents.end(), current) - occlusionEvents.begin() - 2);
      ui->replaySlider->setValue(previousOcclusion);
    }
}


/**
  * @brief Saves the tracked movie in .avi. Triggered when ui->previousReplay is pressed.
*/
void MainWindow::saveTrackedMovie() {
    
    // If tracking data are available, gets the display settings and saves the movie in the
    // selected folder
    if( isReplayable ){
      QString savePath = QFileDialog::getSaveFileName(this, tr("Save File"), "/home/save.avi", tr("Videos (*.avi)"));
      cv::VideoWriter outputVideo(savePath.toStdString(), cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), ui->replayFps->value(), Size(originalImageSize.width(), originalImageSize.height()));
      int scale = ui->replaySize->value();
     
     for(size_t frameIndex = 0; frameIndex < replayFrames.size(); frameIndex++) { 
        Mat frame = imread(replayFrames.at(frameIndex), IMREAD_COLOR);
       
        // Takes the tracking data corresponding to the replayed frame and parse data to display
        // arrows on tracked objects.
        for (size_t i = frameIndex*replayNumberObject; i < frameIndex*replayNumberObject + replayNumberObject; i++) {
          
          if (replayTracking.at(i).split('\t', QString::SkipEmptyParts).at(0) == "NaN") {
            continue;
          }

          QStringList coordinate = replayTracking.at(i).split('\t', QString::SkipEmptyParts);
          
          if (ui->replayEllipses->isChecked()) {

            switch(ui->ellipseBox->currentIndex()) {
              
              case 0 : // Head + Tail
                cv::ellipse(frame, Point( coordinate.at(0).toDouble(), coordinate.at(1).toDouble() ), Size( coordinate.at(12).toDouble(), coordinate.at(13).toDouble() ), 180 - (coordinate.at(2).toDouble()*180)/M_PI, 0, 360,  Scalar(colorMap.at(i - frameIndex*replayNumberObject).x, colorMap.at(i - frameIndex*replayNumberObject).y, colorMap.at(i - frameIndex*replayNumberObject).z), scale, 8 );
                cv::ellipse(frame, Point( coordinate.at(3).toDouble(), coordinate.at(4).toDouble() ), Size( coordinate.at(15).toDouble(), coordinate.at(16).toDouble() ), 180 - (coordinate.at(5).toDouble()*180)/M_PI, 0, 360,  Scalar(colorMap.at(i - frameIndex*replayNumberObject).x, colorMap.at(i - frameIndex*replayNumberObject).y, colorMap.at(i - frameIndex*replayNumberObject).z), scale, cv::LINE_AA );
                break;

              case 1 : // Head
                cv::ellipse(frame, Point( coordinate.at(0).toDouble(), coordinate.at(1).toDouble() ), Size( coordinate.at(12).toDouble(), coordinate.at(13).toDouble() ), 180 - (coordinate.at(2).toDouble()*180)/M_PI, 0, 360,  Scalar(colorMap.at(i - frameIndex*replayNumberObject).x, colorMap.at(i - frameIndex*replayNumberObject).y, colorMap.at(i - frameIndex*replayNumberObject).z), scale, 8 );
                break;

              case 2 : // Tail
                cv::ellipse(frame, Point( coordinate.at(3).toDouble(), coordinate.at(4).toDouble() ), Size( coordinate.at(15).toDouble(), coordinate.at(16).toDouble() ), 180 - (coordinate.at(5).toDouble()*180)/M_PI, 0, 360,  Scalar(colorMap.at(i - frameIndex*replayNumberObject).x, colorMap.at(i - frameIndex*replayNumberObject).y, colorMap.at(i - frameIndex*replayNumberObject).z), scale, cv::LINE_AA );
                break;

              case 3 : // Body
                cv::ellipse(frame, Point( coordinate.at(6).toDouble(), coordinate.at(7).toDouble() ), Size( coordinate.at(18).toDouble(), coordinate.at(19).toDouble() ), 180 - (coordinate.at(8).toDouble()*180)/M_PI, 0, 360,  Scalar(colorMap.at(i - frameIndex*replayNumberObject).x, colorMap.at(i - frameIndex*replayNumberObject).y, colorMap.at(i - frameIndex*replayNumberObject).z), scale, 8 );
                break;
            }
          }

          if (ui->replayArrows->isChecked()) {
            
            switch(ui->arrowBox->currentIndex()) {
            
            case 0 :
              cv::arrowedLine(frame, Point(coordinate.at(0).toDouble(), coordinate.at(1).toDouble()), Point(coordinate.at(0).toDouble() + coordinate.at(12).toDouble()*cos(coordinate.at(2).toDouble()), coordinate.at(1).toDouble() - coordinate.at(12).toDouble()*sin(coordinate.at(2).toDouble())), Scalar(colorMap.at(i - frameIndex*replayNumberObject).x, colorMap.at(i - frameIndex*replayNumberObject).y, colorMap.at(i - frameIndex*replayNumberObject).z), scale, cv::LINE_AA, 0, double(scale)/10);
              cv::arrowedLine(frame, Point(coordinate.at(3).toDouble(), coordinate.at(4).toDouble()), Point(coordinate.at(3).toDouble() + coordinate.at(15).toDouble()*cos(coordinate.at(5).toDouble()), coordinate.at(4).toDouble() - coordinate.at(15).toDouble()*sin(coordinate.at(5).toDouble())), Scalar(colorMap.at(i - frameIndex*replayNumberObject).x, colorMap.at(i - frameIndex*replayNumberObject).y, colorMap.at(i - frameIndex*replayNumberObject).z), scale, cv::LINE_AA, 0, double(scale)/10);
              break;

            case 1 :
              cv::arrowedLine(frame, Point(coordinate.at(0).toDouble(), coordinate.at(1).toDouble()), Point(coordinate.at(0).toDouble() + coordinate.at(12).toDouble()*cos(coordinate.at(2).toDouble()), coordinate.at(1).toDouble() - coordinate.at(12).toDouble()*sin(coordinate.at(2).toDouble())), Scalar(colorMap.at(i - frameIndex*replayNumberObject).x, colorMap.at(i - frameIndex*replayNumberObject).y, colorMap.at(i - frameIndex*replayNumberObject).z), scale, cv::LINE_AA, 0, double(scale)/10);
              break;

            case 2 :
              cv::arrowedLine(frame, Point(coordinate.at(3).toDouble(), coordinate.at(4).toDouble()), Point(coordinate.at(3).toDouble() + coordinate.at(15).toDouble()*cos(coordinate.at(5).toDouble()), coordinate.at(4).toDouble() - coordinate.at(15).toDouble()*sin(coordinate.at(5).toDouble())), Scalar(colorMap.at(i - frameIndex*replayNumberObject).x, colorMap.at(i - frameIndex*replayNumberObject).y, colorMap.at(i - frameIndex*replayNumberObject).z), scale, cv::LINE_AA, 0, double(scale)/10);
              break;

            case 3 :
              cv::arrowedLine(frame, Point(coordinate.at(6).toDouble(), coordinate.at(7).toDouble()), Point(coordinate.at(6).toDouble() + coordinate.at(18).toDouble()*cos(coordinate.at(8).toDouble()), coordinate.at(7).toDouble() - coordinate.at(18).toDouble()*sin(coordinate.at(8).toDouble())), Scalar(colorMap.at(i - frameIndex*replayNumberObject).x, colorMap.at(i - frameIndex*replayNumberObject).y, colorMap.at(i - frameIndex*replayNumberObject).z), scale, cv::LINE_AA, 0, double(scale)/10);
              break;

            }
          }
          
          if (ui->replayNumbers->isChecked()) {
            cv::putText(frame, to_string(i - frameIndex*replayNumberObject), Point(coordinate.at(0).toDouble() + coordinate.at(12).toDouble()*cos(coordinate.at(2).toDouble()), coordinate.at(1).toDouble() - coordinate.at(12).toDouble()*sin(coordinate.at(2).toDouble()) ), cv::FONT_HERSHEY_SIMPLEX, double(scale)*0.5, Scalar(colorMap.at(i - frameIndex*replayNumberObject).x, colorMap.at(i - frameIndex*replayNumberObject).y, colorMap.at(i - frameIndex*replayNumberObject).z), scale*1.2, cv::LINE_AA);
          }
          
          if (ui->replayTrace->isChecked()) {
            // Gets the 20 previous cordinates to display the trajectory trace
            vector<Point> memory;
            for (unsigned int j = i - 20*replayNumberObject; j < i; j += replayNumberObject) {
              if (j > 0 && replayTracking.at(j).split('\t', QString::SkipEmptyParts).at(0) != "NaN") {
                QStringList coordinateMemory = replayTracking.at(j).split('\t', QString::SkipEmptyParts);
                memory.push_back(Point(coordinateMemory.at(6).toDouble(), coordinateMemory.at(7).toDouble()));
              }
            }
            cv::polylines(frame, memory, false, Scalar(colorMap.at(i - frameIndex*replayNumberObject).x, colorMap.at(i - frameIndex*replayNumberObject).y, colorMap.at(i - frameIndex*replayNumberObject).z), scale*1.2, cv::LINE_AA);
          }

        }
      outputVideo.write(frame);
      ui->replaySlider->setValue(frameIndex);
      }
      outputVideo.release();
    }
}

/**
  * @brief Displays an error message.  
*/
void MainWindow::errors(int code) {
    QMessageBox errorBox(this);
    errorBox.setText("An error occurs during the tracking\n");
    switch(code) {
      case 0 : errorBox.setText("Error message: provided background image is not correct. New background image is calculated instead.");
    }
    errorBox.exec();
}

/**
  * @brief Destructs the MainWindow object and saves the previous set of parameters.  
*/
MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

