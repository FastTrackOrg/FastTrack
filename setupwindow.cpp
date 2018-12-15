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
#include "setupwindow.h"
#include "ui_setupwindow.h"


/*!
    \class SetupWindow
    \brief The SetupWindow class is a widget displaying a window that automatically help the user to select some parameters to begin a tracking analysis.
*/


SetupWindow::SetupWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::SetupWindow) {
    ui->setupUi(this);
    
    xTop = -1;
    connect(ui->open, &QPushButton::clicked, this, &SetupWindow::loadImage);
    connect(ui->crop, &QPushButton::clicked, this, &SetupWindow::cropImage);
    connect(ui->reset, &QPushButton::clicked, this, &SetupWindow::resetImage);
    connect(ui->compute, &QPushButton::clicked, this, &SetupWindow::compute);
    connect(ui->thresholdValue, &QSlider::valueChanged, this, &SetupWindow::thresholdImage);
};


/**
  * @brief Loads an image in the QLabel ui->display from a pth to an image file. Trigerred when QPushButton ui->open is clicked.
*/
void SetupWindow::loadImage() {

    QString path = QFileDialog::getOpenFileName(this, tr("Open Image"), "/home");
    if (path.length()) {
      imagePath = path;
      image = QPixmap(path);
      ui->display->setPixmap(image);
    }
}


/**
  * @brief Gets the mouse coordinate in the frame of reference of the widget where user have clicked.
  * @param[in] event Describes the mouse event.
*/
void SetupWindow::mousePressEvent(QMouseEvent* event) {

    if (event->buttons() == Qt::LeftButton) {
      // Set the top left coordinate of the rectangle at the click
      xTop = ui->display->mapFrom(this, event->pos()).x(); 
      yTop = ui->display->mapFrom(this, event->pos()).y();
    }
}


/**
  * @brief Draws a rectangle when the mouse is pressed in a QPixmap. The top left corner is at the coordinate of the first click and the bottom right at the coordinate of the released.
  * @param[in] event Describes the mouse event.
*/
void SetupWindow::mouseMoveEvent(QMouseEvent* event) {
   
      xBottom = ui->display->mapFrom(this, event->pos()).x(); 
      yBottom = ui->display->mapFrom(this, event->pos()).y();
      QPixmap tmpImage = image;
      QPainter paint(&tmpImage);
      paint.setPen(QColor(0, 230, 0, 255));
      paint.drawRect(QRect(xTop, yTop , xBottom - xTop, yBottom - yTop)); 
      ui->display->setPixmap(tmpImage);
}


/**
  * @brief Reset the original image, undo all the modifications. Triggered when the QPushButton ui->reset is clicked.
*/
void SetupWindow::resetImage() {
    image = QPixmap(imagePath);
    ui->display->setPixmap(image);
}

/**
  * @brief Crops the image from a rectangle drawed with the mouse on the ui->display. Triggered when the QPushButton ui->crop is clicked.
*/
void SetupWindow::cropImage() {

    QRect rect(xTop, yTop, xBottom - xTop, yBottom - yTop);
    image = image.copy(rect);
    ui->display->setPixmap(image);
}


/**
  * @brief Opens an image from a path ui->open, gets the threshold value from ui->thresholdSlider and the ROI from user selection and binarizes the image.
  * @param[in] int Threshold value.
*/
void SetupWindow::thresholdImage(int value) {

  cv::Mat imageMat = cv::imread(imagePath.toStdString(), cv::IMREAD_GRAYSCALE);
  
  // If user has selected a region of interest.
  if (xTop != -1) {
    cv::Rect roi(xTop, yTop, xBottom - xTop, yBottom - yTop);
    imageMat = imageMat(roi);
  }

  cv::threshold(imageMat, binary, value, 255, cv::THRESH_BINARY);
  image = QPixmap::fromImage(QImage(binary.data, binary.cols, binary.rows, binary.step, QImage::Format_Grayscale8));
  ui->display->setPixmap(image);
}


/**
  * @brief Computes the size of the biggest object in the image and gives optimal size parameters to the user.
*/
void SetupWindow::compute() {

    vector<vector<cv::Point> > contours;
    cv::findContours(binary, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

    // Finds the biggest contour
    double maxArea = 0;
    int indexMaxArea = 0;
    for (unsigned int i = 0; i < contours.size(); i++) {
      if (cv::contourArea(contours[i]) > maxArea) {
        indexMaxArea = i;
      }
    }
    cv::cvtColor(binary, binary, cv::COLOR_GRAY2BGR);
    cv::drawContours(binary, contours, indexMaxArea, cv::Scalar(0, 230, 0), cv::FILLED, 8 );
    image = QPixmap::fromImage(QImage(binary.data, binary.cols, binary.rows, binary.step, QImage::Format_RGB888));
    ui->display->setPixmap(image);

    // Indicates to the user the best minimal and maximal object size to set.
    QMessageBox msgBox(this);
    QString message = "The recommanded parameters are: \nMaximal area: " + QString::number(cv::contourArea(contours[indexMaxArea]) + 0.2*cv::contourArea(contours[indexMaxArea])) + '\n' + "Minimal area: " + QString::number(cv::contourArea(contours[indexMaxArea]) - 0.2*cv::contourArea(contours[indexMaxArea])) + '\n';
    msgBox.setText(message);
    msgBox.exec();
}

SetupWindow::~SetupWindow() {
    delete ui;  
}
