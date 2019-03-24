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


#ifndef INTERACTIVE_H
#define INTERACTIVE_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QAction>
#include <QMap>
#include <QDir>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QString>
#include <QDirIterator>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPainter>
#include <QFile>
#include <QElapsedTimer>
#include <QEventLoop>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "tracking.h"
#include "data.h"
#include <quazip/JlCompress.h>


using namespace std;
using namespace cv;

namespace Ui {
class Interactive;
}

class Interactive : public QMainWindow
{
    Q_OBJECT

public:
    explicit Interactive(QWidget *parent = nullptr);
    ~Interactive();

private slots:
    void openFolder();
   
    void display(int index);
    void display(UMat image);

    void getParameters();

    void previewTracking();
    void track();


    void computeBackground();
    void selectBackground();

    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void crop();
    void reset();

    void benchmark();
    void benchmarkAnalysis(QMap<QString, QString>);

private:
    Ui::Interactive *ui;
    QString memoryDir;   /*!< Saves the path to the last opened folder in dialog. */
    vector<String> framePath;   /*!< Path to all the image of the image sequence to display. */
    QSize resizedFrame;   /*!< Size of the resized image in the display QWidget. */
    QSize originalImageSize;   /*!< Size of the original image. */
    QSize cropedImageSize;   /*!< Size of the croped image. */
    QMap<QString, QString> parameters;   /*!< Tracking parameters. */
    QString path;   /*!< */
    QString backgroundPath;   /*!< Path to the background image. */
    QString dir;   /*!< Path to the folder where the image sequence to display is stored. */
    Tracking *tracking;   /*!< Tracking object. */
    UMat background;   /*!< Background image. */
    bool isBackground;   /*!< Is the background computed. */
    QPair<QPoint, QPoint> clicks;
    Rect roi;
    QPixmap resizedPix;
    Data *trackingData;
    vector<Point3f> colorMap;
    QVector<int> benchmarkTime;
    int benchmarkCount;

signals:
    void message(QString message);

};

#endif // INTERACTIVE_H
