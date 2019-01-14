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
    along with FishyTracking.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QImage>
#include <QFileDialog>
#include <QMessageBox>
#include <string>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <fstream>
#include <QCheckBox>
#include <QSlider>
#include <QLCDNumber>
#include <QKeySequence>
#include <QComboBox>
#include <QThread>
#include <QSpinBox>
#include <stdlib.h>
#include <QLayout>
#include <QWidget>
#include <QMap>
#include <QDir>
#include <QString>
#include <QShortcut>
#include <QSettings>
#include <QDir>
#include <QThread>
#include <QDebug>
#include <QTableWidgetItem>
#include "tracking.h"
#include "setupwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QFile>
#include <QFileInfo>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <string>
#include <opencv2/core/ocl.hpp>
#include <ctime>


using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private:
    Ui::MainWindow *ui;    /*!< ui file from Qt designer. */
    QMap<QString, QString> parameterList;   /*!< All the parameters necessary for the tracking analysis. */
    
    QThread* thread;   /*!< Thread where lives the Tracking object. */
    Tracking* tracking;   /*!< Objects that track images sequence. */

    QShortcut *wShortcut;   /*!< Keyboard shortcut to next occlusion event. */
    QShortcut *qShortcut;   /*!< Keyboard shortcut to previous frame. */
    QShortcut *aShortcut;   /*!< Keyboard shortcut to previous frame. */
    QShortcut *dShortcut;   /*!< Keyboard shortcut to next frame. */

    QVector<QString> pathList;   /*!< Paths to the images sequences to analyze. */
    QVector<QString> backgroundList;   /*!< Paths the background images. */

    SetupWindow *setupWindow;   /*!< QMainWindow to display interactive setting of parameters. */


    void loadSettings();
    void saveSettings();
    QSettings *settingsFile;   /*!< Saves parameters in a settings.ini file.*/


    // Replay panel
    vector<cv::String> replayFrames;   /*!< Paths to each image of the images sequences to replay. */
    vector<Point3f> colorMap;   /*!< RGB color map to display each object in one color. */
    QVector<QString> replayTracking;   /*!< Tracking data of the replayed images sequence. */
    QVector<int> occlusionEvents;   /*!< Index of each occlusion event in the replayed images sequence. */
    int replayNumberObject;   /*!< Number of objects tracked in the replayed images sequence. */
    bool isReplayable;   /*!< True if user input is an images sequences that can be played. */
    QTimer *framerate;   /*!< Sets the time at which a new image is displayed in autoplay mode in the replay. */
    int replayFps;   /*!< Frame rate value at which a new image is displayed in autoplay mode in the replay. */
    int autoPlayerIndex;   /*!< Index of the image displayed in autoplay mode in the replay. */
    bool object;   /*!< Alternatively true or false to associate either object A or object B at each click of the user in the ui->replayDisplay. */
    QSize resizedFrame;   /*!< Width and height of displayed QPixmap to accomodate window size changment. */
    QSize originalImageSize;   /*!< Width and height of the original image in the images sequence. */

public slots:
    void updateParameterList(QTableWidgetItem* item);
    void startTracking();
    void openPathFolder();
    void openPathBackground();
    void addPath();
    void removePath();
    void display(UMat&, UMat&);
    void openReplayFolder();
    void loadReplayFolder(QString dir);
    void loadFrame(int frameIndex);
    void toggleReplayPlay();
    void mousePressEvent(QMouseEvent* event);
    void swapTrackingData(int firstObject, int secondObject, int from);
    void correctTracking();
    void nextOcclusionEvent();
    void previousOcclusionEvent();
    void saveTrackedMovie();
    void errors(int code);
signals:
  /**
  * @brief Emitted when a parameter is changed.
  * @param parameterList All parameters necessary to the tracking analysis.
  */
    void newParameterList(const QMap<QString, QString> &parameterList);
  
  /**
  * @brief Emitted when a tracking analysis is finished.
  */
    void next();
};

#endif // MAINWINDOW_H
