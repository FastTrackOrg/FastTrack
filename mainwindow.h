#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QImage>
#include <QMessageBox>
#include <string>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <fstream>
#include <QCheckBox>
#include <QSlider>
#include <QLCDNumber>
#include <QComboBox>



using namespace std;
using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    static int const EXIT_CODE_REBOOT = -123456789;



private:

    Ui::MainWindow *ui;
    QPushButton *QuitButton;
    QPushButton *GoButton;
    QPushButton *DefaultButton;
    QPushButton *ResetButton;
    QPushButton *ReplayButton;
    QCheckBox *normal;
    QCheckBox *binary;
    QString version = "v2.3.1dev";

    QLabel *path;
    QLabel *numLabel;
    QLabel *maxArea;
    QLabel *minArea;
    QLabel *length;
    QLabel *lo;
    QLabel *w;
    QLabel *angle;
    QLabel *nBack;
    QLabel *thresh;
    QLabel *save;
    QLabel *x1ROI;
    QLabel *y1ROI;
    QLabel *x2ROI;
    QLabel *y2ROI;
    QLabel *arrow;
    QLabel *fps;
    QLabel *trackingSpotLabel;

    QLineEdit *pathField;
    QLineEdit *numField;
    QLineEdit *maxAreaField;
    QLineEdit *minAreaField;
    QLineEdit *lengthField;
    QLineEdit *loField;
    QLineEdit *wField;
    QLineEdit *angleField;
    QLineEdit *nBackField;
    QLineEdit *threshField;
    QLineEdit *saveField;
    QLineEdit *x1ROIField;
    QLineEdit *y1ROIField;
    QLineEdit *x2ROIField;
    QLineEdit *y2ROIField;
    QLCDNumber *arrowField;
    QLCDNumber *fpsField;

    QLabel *display;
    QLabel *display2;
    QTimer *timer;
    QTimer *timerReplay;
    QMessageBox msgBox;
    QMessageBox errorBox;
    QMessageBox *welcomeBox;
    QProgressBar *progressBar;
    QSlider *arrowSlider;
    QSlider *fpsSlider;
    QComboBox *trackingSpot;

    vector<String> files;
    vector <String>::iterator a;

    Mat cameraFrame;
    Mat visu;
    Mat img0;;
    Mat background;;
    vector<vector<Point> > memory;
    vector<Point3f> colorMap;
    vector<vector<Point3f> > out;
    vector<vector<Point3f> > outPrev;
    vector<int> identity;
    unsigned int im;
    unsigned int imr;
    ofstream savefile;
    vector<Point3f> internalSaving;

public slots:
    void Go();
    void Write();
    void Reset();
    void Replay();
};

#endif // MAINWINDOW_H
