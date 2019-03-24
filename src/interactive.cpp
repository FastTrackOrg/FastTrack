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


#include "interactive.h"
#include "ui_interactive.h"

/**
 * @class Interactive
 *
 * @brief The Interactive widget provides an environment to use the tracking widget in an interactive environment.
 *
 * @author Benjamin Gallois
 *
 * @version $Revision: 421 $
 *
 * Contact: gallois.benjamin08@gmail.com
 *
 */



/**
  * @brief Constructs the interactive object derived from a QMainWindow object.
*/
Interactive::Interactive(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Interactive)
{
    ui->setupUi(this);

    // Menu bar
    connect(ui->actionOpen, &QAction::triggered, this, &Interactive::openFolder);
    connect(ui->slider, &QSlider::valueChanged, this, static_cast<void (Interactive::*)(int)>(&Interactive::display));
    connect(ui->actionbenchmark, &QAction::triggered, this, &Interactive::benchmark);
    connect(this, &Interactive::message, this, [this](QString msg) {
      QMessageBox msgBox;
      msgBox.setText(msg);
      msgBox.exec();
    });
    
    // Threshold slider and combobox
    connect(ui->threshSlider, &QSlider::valueChanged, ui->threshBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::setValue));
    connect(ui->threshBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), ui->threshSlider, &QSlider::setValue);
    connect(ui->threshBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this]() {
      ui->isBin->setChecked(true);
      display(ui->slider->value());
    });

    // Sets the roi limits
    connect(ui->x2, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int value) {
      ui->x1->setMaximum(value - 1);
      ui->x1->setMinimum(0);
    });
    connect(ui->y2, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int value) {
      ui->y1->setMaximum(value - 1);
      ui->y1->setMinimum(0);
    });

    // Update after operations
    connect(ui->erosion, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this]() {
      ui->isBin->setChecked(true);
      display(ui->slider->value());
    });
    connect(ui->dilatation, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this]() {
      ui->isBin->setChecked(true);
      display(ui->slider->value());
    });

    connect(ui->minSize, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this]() {
      ui->isBin->setChecked(true);
      display(ui->slider->value());
    });
    connect(ui->maxSize, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this]() {
      ui->isBin->setChecked(true);
      display(ui->slider->value());
    });
    connect(ui->backColor, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),[this](){
      ui->isBin->setChecked(true);
      display(ui->slider->value());
    });
    connect(ui->isBin, &QRadioButton::clicked,[this](){
      display(ui->slider->value());
    });
    connect(ui->isOriginal, &QRadioButton::clicked,[this](){
      display(ui->slider->value());
    });
    connect(ui->isSub, &QRadioButton::clicked,[this](){
      display(ui->slider->value());
    });
    connect(ui->isNone, &QRadioButton::clicked,[this](){
      display(ui->slider->value());
    });
    connect(ui->isTracked, &QRadioButton::clicked,[this](){
      display(ui->slider->value());
    });

    // Set image preview limits
    connect(ui->startImage, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int startImage){
      ui->stopImage->setRange(0, framePath.size() - startImage); 
    });

    
    // Ui buttons connects
    connect(ui->backgroundSelectButton, &QPushButton::clicked, this, &Interactive::selectBackground);
    connect(ui->backgroundComputeButton, &QPushButton::clicked, this, &Interactive::computeBackground);
    connect(ui->previewButton, &QPushButton::clicked, this, &Interactive::previewTracking); 
    connect(ui->trackButton, &QPushButton::clicked, [this]() {
      QMessageBox::StandardButton reply;
      reply = QMessageBox::question(this, "Confirmation", "You are going to start a full tracking analysis. That can take some time, are you sure?", QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes) {
          track();
      }
    });
    connect(ui->cropButton, &QPushButton::clicked, this, &Interactive::crop); 
    connect(ui->resetButton, &QPushButton::clicked, this, &Interactive::reset); 
    
    
    isBackground = false;
    tracking = new Tracking("", "");

    double a,b,c;
    srand (time(NULL));
    for (int j = 0; j < 90000 ; ++j)  {
      a = rand() % 255;
      b = rand() % 255;
      c = rand() % 255;
      colorMap.push_back(Point3f(a, b, c));
    }
}


/**
  * @brief Gets the path to a folder where an image sequence is stored. Triggered when the open button from menu bar is clicked.
*/
void Interactive::openFolder() {
    
    // Reset the class members
    backgroundPath.clear();
    reset();
    trackingData = new Data("");
    background.release();
    memoryDir.clear();
    framePath.clear();
    ui->display->clear();

    // Reset the ui display
    ui->isNone->setChecked(true);
    ui->isBin->setCheckable(false);
    ui->isSub->setCheckable(false);
    ui->isOriginal->setChecked(true);
    ui->isTracked->setCheckable(false);
    

    dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), memoryDir, QFileDialog::ShowDirsOnly);

    // If Tracking_Result folder selectionned, get the path to the top directory
    if(dir.right(15) == "Tracking_Result") {
      dir.truncate(dir.size() - 15);
    }
     
    if (dir.length()) {

      // Finds the image format
      QList<QString> extensions = { "pgm", "png", "jpeg", "jpg", "tiff", "tif", "bmp", "dib", "jpe", "jp2", "webp", "pbm", "ppm", "sr", "ras", "tif" };
      QDirIterator it(dir, QStringList(), QDir::NoFilter);
      QString extension;
      while (it.hasNext()) {
        extension = it.next().section('.', -1);
        if( extensions.contains(extension) ) break;
      }

      // Setups the class member
      try{
        string path = (dir + QDir::separator() + "*." + extension).toStdString();
        memoryDir = dir;
        glob(path, framePath, false);

        ui->slider->setMinimum(0);
        ui->slider->setMaximum(framePath.size() - 1);
        ui->nBack->setMaximum(framePath.size() - 1);
        ui->nBack->setValue(framePath.size() - 1);
        ui->startImage->setRange(0, framePath.size() - 1);
        ui->startImage->setValue(0);

        Mat frame = imread(framePath.at(0), IMREAD_COLOR);
        originalImageSize.setWidth(frame.cols);
        originalImageSize.setHeight(frame.rows);
        ui->x2->setMaximum(frame.cols);
        ui->y2->setMaximum(frame.rows);

        ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Path", Qt::MatchExactly).at(0)), 1)->setText(dir);
        ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Image number", Qt::MatchExactly).at(0)), 1)->setText(QString::number(framePath.size()));
        ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Image width", Qt::MatchExactly).at(0)), 1)->setText(QString::number(frame.cols));
        ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Image height", Qt::MatchExactly).at(0)), 1)->setText(QString::number(frame.rows));

        isBackground = false;
        reset();
        display(0);
      }
      catch(...){
        ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Path", Qt::MatchExactly).at(0)), 1)->setText("");
        ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Image number", Qt::MatchExactly).at(0)), 1)->setText("0");
        ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Image width", Qt::MatchExactly).at(0)), 1)->setText("0");
        ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Image height", Qt::MatchExactly).at(0)), 1)->setText("0");
        emit(message("No image found."));
      }
    }
}


/**
  * @brief Displays the image at index in the image sequence in the ui->display.
*/
void Interactive::display(int index) {
  
  if ( !framePath.empty() ) {
    
    UMat frame;
    imread(framePath.at(index), IMREAD_GRAYSCALE).copyTo(frame);
    vector<vector<Point>> displayContours;
    vector<vector<Point>> rejectedContours;

    // Computes the image with the background subtracted
    if ( ui->isSub->isChecked() && isBackground ) {
      (ui->backColor->currentText() == "Light background") ? (subtract(background, frame, frame)) : (subtract(frame, background, frame));
    }
    
    // Computes the binary image an applies morphological operations
    if ( ui->isBin->isChecked() && isBackground ) {
      (ui->backColor->currentText() == "Light background") ? (subtract(background, frame, frame)) : (subtract(frame, background, frame));
      tracking->binarisation(frame, 'b', ui->threshBox->value());
      Mat element = getStructuringElement( MORPH_ELLIPSE, Size( 2*ui->dilatation->value() + 1, 2*ui->dilatation->value() + 1 ), Point( ui->dilatation->value(), ui->dilatation->value() ) );
      dilate(frame, frame, element);
      element = getStructuringElement( MORPH_ELLIPSE, Size( 2*ui->erosion->value() + 1, 2*ui->erosion->value() + 1 ), Point( ui->erosion->value(), ui->erosion->value() ) );
      erode(frame, frame, element);
      
      // Keeps only right sized object.
      vector<vector<Point>> contours;
      findContours(frame, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

      double min = ui->minSize->value();
      double max = ui->maxSize->value();

      for(auto const &a: contours) {
        double size = contourArea(a);
        if(size > min && size < max) {
          displayContours.push_back(a); 
        }
        else{
          rejectedContours.push_back(a);
        }
      }
    }

    // Displays the image in the QLabel
    cvtColor(frame, frame, COLOR_GRAY2RGB);  
    if( ui->isBin->isChecked() ) {
      drawContours(frame, displayContours, -1, Scalar(0, 255, 0), FILLED, 8);
      drawContours(frame, rejectedContours, -1, Scalar(255, 0, 0), FILLED, 8);
    }
    if ( ui->isTracked->isChecked() ) {
      QList<int> idList = trackingData->getId(index);
        
      double scale = 2;
      
      for (auto const &a: idList) {

        QMap<QString, double> coordinate = trackingData->getData(index, a);
        int id = a;
        cv::ellipse(frame, Point( coordinate.value("xBody"), coordinate.value("yBody") ), Size( coordinate.value("bodyMajorAxisLength"), coordinate.value("bodyMinorAxisLength") ), 180 - (coordinate.value("tBody")*180)/M_PI, 0, 360,  Scalar(colorMap.at(id).x, colorMap.at(id).y, colorMap.at(id).z), scale, 8 );
      }
    }
    if(roi.width != 0 || roi.height != 0 ) {
      frame = frame(roi);
    }
    double w = ui->display->width();
    double h = ui->display->height();
    Mat image = frame.getMat(ACCESS_READ);
    resizedPix = (QPixmap::fromImage(QImage(image.data, image.cols, image.rows, image.step, QImage::Format_RGB888)).scaled(w, h, Qt::KeepAspectRatio));
    ui->display->setPixmap(resizedPix);
    resizedFrame.setWidth(resizedPix.width());
    resizedFrame.setHeight(resizedPix.height());
  }
  
}


/**
  * @brief This is an overloaded function to display an image UMat in the ui->display.
*/
void Interactive::display(UMat image) {

    cvtColor(image, image, COLOR_GRAY2RGB);  
    Mat frame = image.getMat(ACCESS_READ);
    double w = ui->display->width();
    double h = ui->display->height();
    resizedPix = (QPixmap::fromImage(QImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888)).scaled(w, h, Qt::KeepAspectRatio));
    ui->display->setPixmap(resizedPix);
    resizedFrame.setWidth(resizedPix.width());
    resizedFrame.setHeight(resizedPix.height());
 
}


/**
  * @brief Computes and displays the background image in the ui->display. Triggered when ui->backgroundComputeButton is clicked.
*/
void Interactive::computeBackground() {

  double nBack = double(ui->nBack->value());
  int method = ui->back->currentIndex();
  background = tracking->backgroundExtraction(framePath, nBack, method); 
  display(background);
  isBackground = true;
  ui->isBin->setCheckable(true);
  ui->isSub->setCheckable(true);

}


/**
  * @brief Opens a dialogue to select a background image. Triggered when ui->backgroundSelectButton is pressed.
*/
void Interactive::selectBackground() {
    
  QString dir = QFileDialog::getOpenFileName(this, tr("Open Background Image"), memoryDir);
    
  if (dir.length()) {
    backgroundPath = dir;
    imread(backgroundPath.toStdString(), IMREAD_GRAYSCALE).copyTo(background);
    isBackground = true;
    ui->isBin->setCheckable(true);
    ui->isSub->setCheckable(true);
    display(background);
  }
}



/**
  * @brief Gets all the tracking parameters from the ui and updates the parameter map.
*/
void Interactive::getParameters() {

  parameters.insert("Maximal size", QString::number(ui->maxSize->value()));
  parameters.insert("Minimal size", QString::number(ui->minSize->value()));
  parameters.insert("Spot to track", QString::number(ui->spot->currentIndex()));
  parameters.insert("Maximal length", QString::number(ui->maxL->value()));
  parameters.insert("Maximal angle", QString::number(ui->maxT->value()));
  parameters.insert("Weight", QString::number(ui->weight->value()));
  parameters.insert("Maximal occlusion", QString::number(ui->lo->value()));
  parameters.insert("Maximal time", QString::number(ui->to->value()));

  parameters.insert("Binary threshold", QString::number(ui->threshBox->value()));
  parameters.insert("Number of images background", QString::number(ui->nBack->value()));
  parameters.insert("Background method", QString::number(ui->back->currentIndex()));
  parameters.insert("ROI top x", QString::number(roi.tl().x));
  parameters.insert("ROI top y", QString::number(roi.tl().y));
  parameters.insert("ROI bottom x", QString::number(roi.br().x));
  parameters.insert("ROI bottom y", QString::number(roi.br().y));
  parameters.insert("Registration", QString(ui->reg->currentText()));
  (QString(ui->backColor->currentText()) == "Dark background") ? parameters.insert("Light background", "no"): parameters.insert("Light background", "yes");
  parameters.insert("Dilatation", QString::number(ui->dilatation->value()));
  parameters.insert("Erosion", QString::number(ui->erosion->value()));

}


/**
  * @brief Does a preview of a tracking analysis. Triggered when previewButton is clicked.
*/
void Interactive::previewTracking() {
  if(!framePath.empty()) {
    ui->progressBar->setRange(ui->startImage->value(), ui->stopImage->value() - ui->startImage->value() - 1);
    ui->progressBar->setValue(0);
    ui->previewButton->setDisabled(true);
    ui->trackButton->setDisabled(true);

    QThread *thread = new QThread;
    Tracking *tracking = new Tracking(framePath, background, ui->startImage->value(), ui->startImage->value() + ui->stopImage->value());
    tracking->moveToThread(thread);

    connect(thread, &QThread::started, tracking, &Tracking::startProcess);
    connect(tracking, &Tracking::progress, ui->progressBar, &QProgressBar::setValue);
    connect(tracking, &Tracking::statistic, [this](int time) {
      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Analysis rate", Qt::MatchExactly).at(0)), 1)->setText(QString::number(double(ui->stopImage->value()*1000)/double(time)));
    });
    connect(tracking, &Tracking::finished, thread, &QThread::quit);
    connect(tracking, &Tracking::finished, [this]() {
      ui->slider->setDisabled(false);
      ui->previewButton->setDisabled(false);
      ui->trackButton->setDisabled(false);
      trackingData = new Data(dir);
    });
    connect(tracking, &Tracking::finished, tracking, &Tracking::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    getParameters();
    tracking->updatingParameters(parameters);
    thread->start();
    ui->slider->setDisabled(true);
    ui->isTracked->setCheckable(true);
    ui->isTracked->setChecked(true);
  }

}


/**
  * @brief Does a tracking analysis. Triggered when the trackButton is clicked.
*/
void Interactive::track() {
  if(!framePath.empty()) {
    ui->progressBar->setRange(0, framePath.size() - 1);
    ui->progressBar->setValue(0);
    ui->previewButton->setDisabled(true);
    ui->trackButton->setDisabled(true);

    QThread *thread = new QThread;
    Tracking *tracking = new Tracking(framePath, background);
    tracking->moveToThread(thread);

    connect(thread, &QThread::started, tracking, &Tracking::startProcess);
    connect(tracking, &Tracking::progress, ui->progressBar, &QProgressBar::setValue);
    connect(tracking, &Tracking::statistic, [this](int time) {
      ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Analysis rate", Qt::MatchExactly).at(0)), 1)->setText(QString::number(double(framePath.size()*1000)/double(time)));
    });
    connect(tracking, &Tracking::finished, thread, &QThread::quit);
    connect(tracking, &Tracking::finished, [this]() {
      ui->slider->setDisabled(false);
      ui->previewButton->setDisabled(false);
      ui->trackButton->setDisabled(false);
      trackingData = new Data(dir);
    });
    connect(tracking, &Tracking::finished, tracking, &Tracking::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    getParameters();
    tracking->updatingParameters(parameters);
    thread->start();
    ui->slider->setDisabled(true);
    ui->isTracked->setCheckable(true);
    ui->isTracked->setChecked(true);
  }
}


/**
  * @brief Gets the mouse coordinate in the frame of reference of the widget where user have clicked.
  * @param[in] event Describes the mouse event.
*/
void Interactive::mousePressEvent(QMouseEvent* event) {

    if (event->buttons() == Qt::LeftButton) {
      clicks.first = ui->display->mapFrom(this, event->pos()); 
      
      // The QPixmap is V/Hcentered in the Qlabel widget
      // Gets the click coordinate in the frame of reference of the centered display
      clicks.first.setX(clicks.first.x() -  0.5*(ui->display->width() - resizedFrame.width())); 
      clicks.first.setY(clicks.first.y() - 0.5*(ui->display->height() - resizedFrame.height()));
    }
}


/**
  * @brief Draws a rectangle when the mouse is pressed in a QPixmap. The top left corner is at the coordinate of the first click and the bottom right at the coordinate of the released.
  * @param[in] event Describes the mouse event.
*/
void Interactive::mouseMoveEvent(QMouseEvent* event) {
   
      clicks.second = (ui->display->mapFrom(this, event->pos())); 
      
      // The QPixmap is V/Hcentered in the Qlabel widget
      // Gets the click coordinate in the frame of reference of the centered display
      clicks.second.setX(clicks.second.x() -  0.5*(ui->display->width() - resizedFrame.width())); 
      clicks.second.setY(clicks.second.y() - 0.5*(ui->display->height() - resizedFrame.height()));
      
      QPixmap tmpImage = resizedPix;
      QPainter paint(&tmpImage);
      paint.setPen(QColor(0, 230, 0, 255));
      QRect roiRect = QRect(clicks.first.x(), clicks.first.y(), clicks.second.x() - clicks.first.x(), clicks.second.y() - clicks.first.y());
      paint.drawRect(roiRect); 
      ui->display->setPixmap(tmpImage);

      // Updates ui value
      ui->x1->setValue(clicks.first.x());
      ui->y1->setValue(clicks.first.y()); 
      ui->x2->setValue(clicks.second.x());
      ui->y2->setValue(clicks.second.y()); 
}

 /**
   * @brief Crops the image from a rectangle drawed with the mouse on the ui->display. Triggered when the QPushButton ui->crop is clicked.
 */
void Interactive::crop() {
 
  // Converts clicks from display widget frame of reference to original image frame of reference 
  int xTop = int(double(clicks.first.x())*double(cropedImageSize.height()) / double(resizedFrame.height()) + roi.tl().x);
  int yTop = int(double(clicks.first.y())*double(cropedImageSize.height()) / double(resizedFrame.height()) + roi.tl().y);
  int xBottom = int(double(clicks.second.x())*double(cropedImageSize.height()) / double(resizedFrame.height()) + roi.tl().x);
  int yBottom = int(double(clicks.second.y())*double(cropedImageSize.height()) / double(resizedFrame.height()) + roi.tl().y);
 
  //Checks for wrong values
  if( xTop < 0 ) xTop = 0;
  if( xBottom < 0 ) xBottom = 0;
  if( yTop < 0 ) yTop = 0;
  if( yBottom < 0 ) yBottom = 0;
  if( xTop > cropedImageSize.width() ) xTop = cropedImageSize.width();
  if( xBottom > cropedImageSize.width()) xBottom = cropedImageSize.width();
  if( yTop > cropedImageSize.height()) yTop = cropedImageSize.height();
  if( yBottom > cropedImageSize.height()) yBottom = cropedImageSize.height();
  
  // Find the true left corner of the rectangle  
  int width = xBottom - xTop;
  int height = yBottom - yTop;
  if (width < 0) {
    xTop += width;
    width = - width;
  }
  if(height < 0) {
    yTop += height;
    height = - height;
  }

   
  roi = Rect(xTop, yTop, width, height);
  cropedImageSize.setWidth(roi.width);
  cropedImageSize.setHeight(roi.height);
  ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Image width", Qt::MatchExactly).at(0)), 1)->setText(QString::number(roi.width));
  ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Image height", Qt::MatchExactly).at(0)), 1)->setText(QString::number(roi.height));
  display(ui->slider->value());
 }
 


 /**
   * @brief Reset the region of interest. Triggered by the ui->reset
 */
void Interactive::reset() {
  cropedImageSize.setWidth(originalImageSize.width());
  cropedImageSize.setHeight(originalImageSize.height());
  ui->x1->setValue(0);
  ui->y1->setValue(0); 
  ui->x2->setValue(originalImageSize.width());
  ui->y2->setValue(originalImageSize.height()); 
  ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Image width", Qt::MatchExactly).at(0)), 1)->setText(QString::number(originalImageSize.width()));
  ui->informationTable->item(ui->informationTable->row(ui->informationTable->findItems("Image height", Qt::MatchExactly).at(0)), 1)->setText(QString::number(originalImageSize.height()));
  roi = Rect(0, 0, 0, 0);
  display(ui->slider->value());
 }


 /**

 */
void Interactive::benchmark() {
    
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, [this] (QNetworkReply *reply) {
     
      if( QDir(QDir::homePath() + "/FastTrackBenchmarkTmp").exists() ) {
        return;
        }

      QDir().mkdir(QDir::homePath() + "/FastTrackBenchmarkTmp"); 
      QFile file(QDir::homePath() + "/FastTrackBenchmarkTmp/benchmark.zip");
      if (!file.open(QIODevice::WriteOnly)){
        qInfo() << "error"; 
        return;
      }

      file.write(reply->readAll());
      reply->deleteLater();
      QStringList tmp = JlCompress::extractDir(QDir::homePath() + "/FastTrackBenchmarkTmp/benchmark.zip", QDir::homePath() + "/FastTrackBenchmarkTmp/");
      

      //Loads parameters
      QMap<QString, QString> params;
      QFile parameterFile(QDir::homePath() + "/FastTrackBenchmarkTmp/Juvenil_Zebrafish_2/Tracking_Result/parameter.param");
      if (parameterFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&parameterFile);
        QString line;
        QStringList parameters;
        while (in.readLineInto(&line)) {
          parameters = line.split(" = ", QString::SkipEmptyParts);
          params.insert(parameters.at(0), parameters.at(1));
        }
      }
      parameterFile.close();
      benchmarkCount = 0;
      benchmarkAnalysis(params); 
    });

    manager->get(QNetworkRequest(QUrl("http://fasttrack.benjamin-gallois.fr/download/Benchmark.zip")));
 }


void Interactive::benchmarkAnalysis(QMap<QString, QString> params) {
      
      QThread *thread = new QThread;
      Tracking *tracking = new Tracking((QDir::homePath() + "/FastTrackBenchmarkTmp/Juvenil_Zebrafish_2/").toStdString(), "");
      tracking->moveToThread(thread);

      connect(thread, &QThread::started, tracking, &Tracking::startProcess);
      connect(tracking, &Tracking::finished, thread, &QThread::quit);
      connect(tracking, &Tracking::finished, tracking, &Tracking::deleteLater);
      connect(tracking, &Tracking::statistic, [this, params](int time) {
        if (benchmarkCount < 20) {
          benchmarkAnalysis(params);
          benchmarkTime.append(time); 
          this->benchmarkCount+= 1;
        }
        else {
          int mean = 0;
          for(auto &a: benchmarkTime) {
            mean += a;
          }
          mean /= benchmarkTime.size();
          qInfo() << benchmarkTime << benchmarkTime.size();
          emit(message("Benchmark result: " + QString::number(mean)));
          QDir(QDir::homePath() + "/FastTrackBenchmarkTmp").QDir::removeRecursively();
        }
      });
      connect(thread, &QThread::finished, thread, &QThread::deleteLater);
      tracking->updatingParameters(params);
      thread->start();
}

/**
  * @brief Destructors.
*/
Interactive::~Interactive() {
    delete tracking;
    delete ui;
}
