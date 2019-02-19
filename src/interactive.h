#ifndef INTERACTIVE_H
#define INTERACTIVE_H

#include <QMainWindow>
#include <QFileDialog>
#include <QAction>
#include <QMap>
#include <QString>
#include <QDirIterator>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "tracking.h"


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


    void computeBackground();


private:
    Ui::Interactive *ui;
    QString memoryDir;
    vector<String> framePath;
    QSize resizedFrame;
    QSize originalImageSize;
    QMap<QString, QString> parameters;
    QString path;
    QString backgroundPath;

    Tracking *tracking;
    UMat background;
    bool isBackground;
    QVector<UMat> buffer;
};

#endif // INTERACTIVE_H
