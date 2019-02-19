#include "interactive.h"
#include "ui_interactive.h"

Interactive::Interactive(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Interactive)
{
    ui->setupUi(this);

    connect(ui->actionOpen, &QAction::triggered, this, &Interactive::openFolder);
    connect(ui->slider, &QSlider::valueChanged, this, static_cast<void (Interactive::*)(int)>(&Interactive::display));
    connect(ui->threshSlider, &QSlider::valueChanged, ui->threshBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::setValue));
    connect(ui->threshBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), ui->threshSlider, &QSlider::setValue);
    connect(ui->threshBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this]() {
        display(ui->slider->value());
    });

    connect(ui->computeBack, &QPushButton::clicked, this, &Interactive::computeBackground);
    connect(ui->preview, &QPushButton::clicked, this, &Interactive::previewTracking); 
    isBackground = false;
    tracking = new Tracking("", "");

}


void Interactive::openFolder() {
    
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), memoryDir, QFileDialog::ShowDirsOnly);
    
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
          glob(path, framePath, false); // Gets all the paths to frames
          ui->slider->setMinimum(0);
          ui->slider->setMaximum(framePath.size() - 1);
          ui->nBack->setMaximum(framePath.size() - 1);
          Mat frame = imread(framePath.at(0), IMREAD_COLOR);
          originalImageSize.setWidth(frame.cols);
          originalImageSize.setHeight(frame.rows);
          isBackground = false;
          path = dir.toStdString();
          display(0);
        }
        catch(...){
        }
    }
}

void Interactive::display(int index) {
  
  if ( !framePath.empty() ) {
    
    UMat image;
    imread(framePath.at(index), IMREAD_GRAYSCALE).copyTo(image);

    
    // Computes the image with the background subtracted
    if ( ui->isSub->isChecked() && isBackground ) {
      (ui->backColor->currentText() == "Light background") ? (subtract(background, image, image)) : (subtract(image, background, image));
    }
    
    // Computes the binary image
    if ( ui->isBin->isChecked() && isBackground ) {
      (ui->backColor->currentText() == "Light background") ? (subtract(background, image, image)) : (subtract(image, background, image));
      tracking->binarisation(image, 'b', ui->threshBox->value());
    }

    if ( ui->isTracked->isChecked() && !buffer.isEmpty() ) {
      //To do
    }

    // Displays the image in the QLabel
    Mat frame = image.getMat(ACCESS_READ);
    cvtColor(frame, frame, COLOR_GRAY2RGB);  
    double w = ui->display->width();
    double h = ui->display->height();
    QPixmap resizedPix = (QPixmap::fromImage(QImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888)).scaled(w, h, Qt::KeepAspectRatio));
    ui->display->setPixmap(resizedPix);
    resizedFrame.setWidth(resizedPix.width());
    resizedFrame.setHeight(resizedPix.height());
  }
  
}

void Interactive::display(UMat image) {

    cvtColor(image, image, COLOR_GRAY2RGB);  
    Mat frame = image.getMat(ACCESS_READ);
    double w = ui->display->width();
    double h = ui->display->height();
    QPixmap resizedPix = (QPixmap::fromImage(QImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888)).scaled(w, h, Qt::KeepAspectRatio));
    ui->display->setPixmap(resizedPix);
    resizedFrame.setWidth(resizedPix.width());
    resizedFrame.setHeight(resizedPix.height());
 
}


void Interactive::computeBackground() {
 
 if ( !framePath.empty() ) { 
    double nBack = double(ui->nBack->value());
    int method = ui->back->currentIndex();
    background = tracking->backgroundExtraction(framePath, nBack, method); 
    display(background);
    isBackground = true;
 }

}

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
  parameters.insert("ROI top x", QString::number(ui->x1->value()));
  parameters.insert("ROI top y", QString::number(ui->y1->value()));
  parameters.insert("ROI bottom x", QString::number(ui->x2->value()));
  parameters.insert("ROI bottom y", QString::number(ui->y2->value()));
  parameters.insert("Registration", QString(ui->reg->currentText()));
  (QString(ui->backColor->currentText()) == "Dark background") ? parameters.insert("Light background", "no"): parameters.insert("Light background", "yes");
  parameters.insert("Dilatation", QString::number(ui->dilatation->value()));
  parameters.insert("Erosion", QString::number(ui->erosion->value()));

}

void Interactive::previewTracking() {
  buffer.clear();
  QThread *thread = new QThread;
  Tracking *tracking = new Tracking(path.toStdString(), backgroundPath.toStdString(), ui->startImage->value(), ui->startImage->value() + ui->stopImage->value());
  tracking->moveToThread(thread);

  connect(tracking, &Tracking::finished, thread, &QThread::quit);
  connect(tracking, &Tracking::finished, tracking, &Tracking::deleteLater);
  connect(thread, &QThread::finished, thread, &QThread::deleteLater);
  connect(tracking, &Tracking::newImageToDisplay, [this](UMat& binary, UMat& visu) {
    buffer.append(visu);
  });

  tracking->updatingParameters(parameters);
  thread->start();

}


Interactive::~Interactive()
{
    delete tracking;
    delete ui;
}
