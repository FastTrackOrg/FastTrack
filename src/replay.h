#ifndef REPLAY_H
#define REPLAY_H

#include <vector>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QImage>
#include <QFileDialog>
#include <QMessageBox>
#include <string>
#include <vector>
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
#include <QDirIterator>
#include <QMouseEvent>
#include <QThread>
#include <QDebug>
#include <QTableWidgetItem>
#include <QDebug>
#include <QTimer>
#include <QFile>
#include <QFileInfo>
#include <string>
#include <opencv2/core/ocl.hpp>
#include <ctime>
#include "data.h"
using namespace std;
using namespace cv;

namespace Ui {
class Replay;
}

class Replay : public QWidget
{
    Q_OBJECT

public:
    explicit Replay(QWidget *parent = nullptr);
    ~Replay();

private:
    Ui::Replay *ui;
    QShortcut *wShortcut;   /*!< Keyboard shortcut to next occlusion. */
    QShortcut *qShortcut;   /*!< Keyboard shortcut to previous frame. */
    QShortcut *aShortcut;   /*!< Keyboard shortcut to previous frame. */
    QShortcut *dShortcut;   /*!< Keyboard shortcut to next frame. */

    QString memoryDir;   /*!< Saves the path of the last opened folder.*/

    Data *trackingData;

    vector<cv::String> replayFrames;   /*!< Paths to each image of the images sequences to replay. */
    vector<Point3f> colorMap;   /*!< RGB color map to display each object in one color. */
    QVector<int> occlusionEvents;   /*!< Index of each occlusion event in the replayed images sequence. */
    int replayNumberObject;   /*!< Number of objects tracked in the replayed images sequence. */
    bool isReplayable;   /*!< True if user input is an images sequences that can be played. */
    QTimer *framerate;   /*!< Sets the time at which a new image is displayed in autoplay mode in the replay. */
    int replayFps;   /*!< Frame rate value at which a new image is displayed in autoplay mode in the replay. */
    int autoPlayerIndex;   /*!< Index of the image displayed in autoplay mode in the replay. */
    bool object;   /*!< Alternatively true or false to associate either object A or object B at each click of the user in the ui->replayDisplay. */
    QSize resizedFrame;   /*!< Width and height of displayed QPixmap to accomodate window size changment. */
    QSize originalImageSize;   /*!< Width and height of the original image in the images sequence. */
    int currentZoom;

public slots:

    void openReplayFolder();
    void loadReplayFolder(QString dir);
    void loadFrame(int frameIndex);
    void zoom(double scale);
    void toggleReplayPlay();
    void mousePressEvent(QMouseEvent* event);
    void correctTracking();
    void nextOcclusionEvent();
    void previousOcclusionEvent();
    void saveTrackedMovie();
};

#endif // REPLAY_H
