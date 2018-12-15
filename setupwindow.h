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
    QString imagePath;
    QPixmap image;
    cv::Mat binary;
    QPainter paint;
    int xTop, yTop, xBottom, yBottom;
    Ui::SetupWindow *ui;

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

