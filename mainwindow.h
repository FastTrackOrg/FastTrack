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
#include <QThread>
#include <QSpinBox>
#include <QLayout>
#include <QWidget>
#include <QMap>
#include <QString>
#include <QDir>
#include <QThread>
#include <QDebug>
#include <QTableWidgetItem>
#include "functions.h"


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
    Ui::MainWindow *ui;
    QMap<QString, QString>* parameterList;

    QThread* thread;
    Tracking* tracking;

    QVector<QString> pathList;
    int pathCounter;


public slots:
    void updateParameterList(QTableWidgetItem* item);
    void newAnalysis(string path);
    void startTracking();
    void addPath();
    void display(Mat, UMat);
signals:
    void newParameterList(QMap<QString, QString>*);
    void next();
};

#endif // MAINWINDOW_H
