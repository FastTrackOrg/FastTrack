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


#ifndef SETUPWINDOW_H
#define SETUPWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QImage>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QDebug>
#include <QMessageBox>
#include <QString>
#include <string>

using namespace std;

namespace Ui {
class SetupWindow;
}

class SetupWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SetupWindow(QWidget *parent = nullptr);
    ~SetupWindow();
private:
    QString imagePath;    /*!< Path to the image to open. */
    QPixmap image;    /*!< Image to display */
    cv::Mat binary;    /*!< Binary image after thresholding */
    QPainter paint;    /*!< QPainter to draw on top of the Pixmap. */
    int xTop, yTop, xBottom, yBottom;    /*!< Region of interest top left and bottom right corners coordinates. */
    Ui::SetupWindow *ui;   /*!< ui file from Qt designer. */

private slots:
    void loadImage();
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void cropImage();
    void resetImage();
    void compute();
    void thresholdImage(int value);

};

#endif // SETUPWINDOW_H

