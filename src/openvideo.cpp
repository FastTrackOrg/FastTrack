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

#include "openvideo.h"
#include "ui_openvideo.h"

using namespace cv;

OpenVideo::OpenVideo(QWidget *parent) : QDialog(parent),
                                        ui(new Ui::OpenVideo) {
  ui->setupUi(this);

  connect(ui->openButton, &QPushButton::clicked, this, &OpenVideo::selectVideo);
  connect(ui->cancelButton, &QPushButton::clicked, this, &OpenVideo::deleteLater);
}

OpenVideo::~OpenVideo() {
  delete ui;
}

/**
  * @brief Opens a dialog window to select videos and convert videos in image sequences.
*/
void OpenVideo::selectVideo() {
  // Enable multiple selection
  QFileDialog dialog;
  dialog.setFileMode(QFileDialog::ExistingFiles);
  if (dialog.exec()) {
    QStringList videoList = dialog.selectedFiles();

    // Update the ui with the selected file
    for (auto const &a : videoList) {
      int row = ui->table->rowCount();
      ui->table->insertRow(row);
      ui->table->setItem(row, 0, new QTableWidgetItem(a));
    }
    qApp->processEvents();  // Force repaint the widget

    // Process the file
    for (int i = 0; i < videoList.size(); i++) {
      QProgressBar *progressBar = new QProgressBar(ui->table);
      ui->table->setCellWidget(i, 1, progressBar);
      createImageSequence(videoList[i], progressBar);
    }
  }
  this->deleteLater();
}

/**
  * @brief Creates an image sequence from a video.
  * @param[in] pathVid Path to the video file.
  * @param[in] bar Pointer to a progress bar to update.
*/
void OpenVideo::createImageSequence(QString pathVid, QProgressBar *bar) {
  QFileInfo fi(pathVid);
  QString name = QFileInfo(fi.fileName()).completeBaseName();
  QString path = fi.canonicalPath();
  QString ext = fi.completeSuffix();

  QString outputDir = path + "/" + name;
  if (!QDir(outputDir).exists()) {
    QDir().mkdir(outputDir);
  }
  else {
    QMessageBox msgBox;
    msgBox.setText(outputDir + " already exist, overwrite?");
    QPushButton *connectButton = msgBox.addButton(tr("Overwrite"), QMessageBox::ActionRole);
    QPushButton *abortButton = msgBox.addButton(QMessageBox::Abort);
    msgBox.exec();
    if (msgBox.clickedButton() == abortButton) {
      return;
    }
  }

  VideoCapture cap(pathVid.toStdString());
  if (!cap.isOpened()) {
    QMessageBox msgBox;
    msgBox.setText(pathVid + " can be opened.");
    msgBox.exec();
    return;
  }

  int maxIndex = cap.get(CAP_PROP_FRAME_COUNT);
  bar->setRange(0, maxIndex);
  bar->setValue(0);
  int i = 0;
  for (int i = 0; i < maxIndex; i++) {
    UMat frame;
    cap >> frame;
    if (!frame.empty()) {
      imwrite((outputDir + "/frame_" + QString("%1").arg(i, 6, 10, QChar('0')) + ".png").toStdString(), frame);
    }
    bar->setValue(i);
  }
  bar->setValue(maxIndex);
}

