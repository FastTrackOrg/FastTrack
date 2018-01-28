#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <string>
#include <functions.h>

using namespace cv;
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(1600, 1000);


    QuitButton = new QPushButton("Quit", this);
    QuitButton->move(300, 320);
    QObject::connect(QuitButton, SIGNAL(clicked()), qApp, SLOT(quit()));


    GoButton = new QPushButton("Go", this);
    GoButton->move(50, 320);
    QObject::connect(GoButton, SIGNAL(clicked()), this, SLOT(Go()));


    path = new QLabel(this);
    path->setText("Path:");
    path->move(50, 50);
    path->adjustSize();

    pathField = new QLineEdit(this);
    pathField ->move(200,50);
    pathField->setText("/home/benjamin/Videos/collectiveBehavior3Weeks/*png");
    pathField->adjustSize();


    numLabel = new QLabel(this);
    numLabel->setText("Number of objects:");
    numLabel->move(50, 100);
    numLabel->adjustSize();

    numField = new QLineEdit(this);
    numField ->move(200,100);
    numField->setText("6");


    maxArea = new QLabel(this);
    maxArea->setText("Maximal area:");
    maxArea->move(50, 150);
    maxArea->adjustSize();

    maxAreaField = new QLineEdit(this);
    maxAreaField ->move(200,150);
    maxAreaField->setText("300");


    minArea = new QLabel(this);
    minArea->setText("Minimal area:");
    minArea->move(50, 200);
    minArea->adjustSize();


    minAreaField = new QLineEdit(this);
    minAreaField ->move(200,200);
    minAreaField->setText("50");


    thresh = new QLabel(this);
    thresh->setText("Binary threshold:");
    thresh->move(50, 250);
    thresh->adjustSize();


    threshField = new QLineEdit(this);
    threshField ->move(200,250);
    threshField->setText("35");
    threshField->adjustSize();


    length = new QLabel(this);
    length->setText("Max displacement:");
    length->move(500, 50);
    length->adjustSize();

    lengthField = new QLineEdit(this);
    lengthField ->move(700,50);
    lengthField->setText("20");


    angle = new QLabel(this);
    angle->setText("Max angle:");
    angle->move(500, 100);
    angle->adjustSize();


    angleField = new QLineEdit(this);
    angleField ->move(700,100);
    angleField->setText("90");



    lo = new QLabel(this);
    lo->setText("Max occlusion:");
    lo->move(500, 150);
    lo->adjustSize();


    loField = new QLineEdit(this);
    loField ->move(700,150);
    loField->setText("50");


    w = new QLabel(this);
    w->setText("Weight:");
    w->move(500, 200);
    w->adjustSize();


    wField = new QLineEdit(this);
    wField ->move(700,200);
    wField->setText("0.2");


    nBack = new QLabel(this);
    nBack->setText("Background images: \n (can't be changed after the start)");
    nBack->move(500, 250);
    nBack->adjustSize();


    nBackField = new QLineEdit(this);
    nBackField ->move(700,250);
    nBackField->setText("200");


    save = new QLabel(this);
    save->setText("Save to:");
    save->move(900, 50);
    save->adjustSize();


    saveField = new QLineEdit(this);
    saveField ->move(1200,50);
    saveField->setText("/home/benjamin/Videos/collectiveBehavior3Weeks/save.txt");


    x1ROI = new QLabel(this);
    x1ROI->setText("Top corner x position for the ROI:");
    x1ROI->move(900, 100);
    x1ROI->adjustSize();


    x1ROIField = new QLineEdit(this);
    x1ROIField ->move(1200,100);
    x1ROIField->setText("253");



    x2ROI = new QLabel(this);
    x2ROI->setText("Bottom corner x position for the ROI:");
    x2ROI->move(900, 150);
    x2ROI->adjustSize();


    x2ROIField = new QLineEdit(this);
    x2ROIField ->move(1200,150);
    x2ROIField->setText("1200");



    y1ROI = new QLabel(this);
    y1ROI->setText("Top corner y position for the ROI:");
    y1ROI->move(900, 200);
    y1ROI->adjustSize();


    y1ROIField = new QLineEdit(this);
    y1ROIField ->move(1200,200);
    y1ROIField->setText("33");



    y2ROI = new QLabel(this);
    y2ROI->setText("Bottom corner y position for the ROI:");
    y2ROI->move(900, 250);



    y2ROIField = new QLineEdit(this);
    y2ROIField ->move(1200,250);
    y2ROIField->setText("350");


    display = new QLabel(this);
    display ->move(50, 400);
    display ->setScaledContents(true);
    display->adjustSize();



    progressBar = new QProgressBar(this);
    progressBar ->move(50, 350);
    progressBar->setFixedWidth(1400);






    im = 0;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(Go()));



}

void MainWindow::Go(){

    QString path = pathField->text();
    QString num = numField->text();
    QString maxArea = maxAreaField->text();
    QString minArea = minAreaField->text();
    QString lenght = lengthField->text();
    QString angle = angleField->text();
    QString lo = loField->text();
    QString w = wField->text();
    QString nBack = nBackField->text();
    QString thresh = threshField->text();
    QString savePath = saveField->text();
    QString x1ROI = x1ROIField->text();
    QString x2ROI = x2ROIField->text();
    QString y1ROI = y1ROIField->text();
    QString y2ROI = y2ROIField->text();




    int MAXAREA = maxArea.toInt(); //9000; //minimal area of detected objects
    int MINAREA = minArea.toInt(); //3000; //maximal area of detected objects
    double LENGTH = lenght.toInt(); //maximal distance covered between two frames
    double ANGLE = (angle.toDouble()*M_PI)/180; //maximal angle between two frames
    unsigned int NUMBER = num.toInt(); //number of objects to track
    double WEIGHT = w.toDouble();// weight of the orientation/angle in the cost function, 0 = orientation, 1 = distance
    double LO = lo.toInt(); // maximum occlusion distance
    int nBackground = nBack.toInt();
    int threshValue = thresh.toInt();

    string save = savePath.toStdString();



    if(im == 0){ // Initialization

        timer->start(1);
        string folder = path.toStdString();
        glob(folder, files, false);
        sort(files.begin(), files.end());
        a = files.begin();
        string name = *a;
        progressBar ->setRange(0, files.size());
        cameraFrame = imread(name, IMREAD_GRAYSCALE);
        visu = imread(name);
        img0 = imread(name, IMREAD_GRAYSCALE);
        background = BackgroundExtraction(files, nBackground);
        vector<vector<Point>> tmp(NUMBER, vector<Point>());
        memory = tmp;
        colorMap = Color(NUMBER);

    }


    string name = *a;
    savefile.open(save);
    Rect ROI(x1ROI.toInt(), y1ROI.toInt(), x2ROI.toInt() - x1ROI.toInt(), y2ROI.toInt() - y1ROI.toInt());
    cameraFrame = imread(name, IMREAD_GRAYSCALE);
    visu = imread(name);
    cameraFrame.convertTo(cameraFrame, CV_8U);
    cameraFrame = background - cameraFrame;
    Binarisation(cameraFrame, 'b', threshValue);
    cameraFrame = cameraFrame(ROI);
    visu = visu(ROI);


    // Position computation
    out = ObjectPosition(cameraFrame, MINAREA, MAXAREA);


    if(im == 0){ // First frame initialization

        if(out.at(0).size() <= NUMBER + 1 && out.at(0).size() != 0){
            while((out.at(0).size() - NUMBER) > 0){
                out.at(0).push_back(Point3f(0, 0, 0));
                out.at(1).push_back(Point3f(0, 0, 0));
                out.at(2).push_back(Point3f(0, 0, 0));
            }
            outPrev = out;
        }

        else{ // Error message
            cout << "ERROR minArea too small!!!";
            //break;

        }
    }



    else if(im > 0){ // Matching fish identity with the previous frame
        identity = CostFunc(outPrev.at(0), out.at(0), LENGTH, ANGLE, WEIGHT, LO);
        out.at(0) = Reassignment(outPrev.at(0), out.at(0), identity);
        out.at(1) = Reassignment(outPrev.at(1), out.at(1), identity);
        out.at(2) = Reassignment(outPrev.at(2), out.at(2), identity);
        out.at(0) = Prevision(outPrev.at(0), out.at(0));
        outPrev = out;
    }



    // Visualization & saving
   for(unsigned int l = 0; l < out.at(0).size(); l++){
        Point3f coord;
        coord = out.at(0).at(l);
        //putText(visu, to_string(l), Point(coord.x, coord.y), FONT_HERSHEY_DUPLEX, .5, Scalar(colorMap.at(l).x, colorMap.at(l).y, colorMap.at(l).z), 1);
        //circle(visu, Point(coord.x, coord.y), 1, Scalar(colorMap.at(l).x, colorMap.at(l).y, colorMap.at(l).z), 2, 2, 0);
        //arrowedLine(visu, Point(coord.x, coord.y), Point(coord.x + 10*cos(coord.z), coord.y - 10*sin(coord.z)), Scalar(colorMap.at(l).x, colorMap.at(l).y, colorMap.at(l).z), 10, 10, 0);

        if((im > 5)){
            polylines(visu, memory.at(l), false, Scalar(colorMap.at(l).x, colorMap.at(l).y, colorMap.at(l).z), 1, 8, 0);
            memory.at(l).push_back(Point((int)coord.x, (int)coord.y));
            if(im>50){
                memory.at(l).erase(memory.at(l).begin());
                }
        }



        // Saving
        ofstream savefile;
        savefile.open(save, ios::out | ios::app );
        savefile << coord.x << "   " << coord.y << "   " << coord.z << "   "  << out.at(2).at(l).x  <<  "   " << im << "\n";


    }



    cvtColor(visu,visu,CV_BGR2RGB);
    Size size = visu.size();

    if(size.height > 600){
        display->setFixedHeight(600);
        display->setFixedWidth((600*size.height)/(size.width));
    }
    else if(size.width > 1500){
        display->setFixedWidth(1500);
        display->setFixedWidth((1500*size.width)/(size.height));
    }
    else{
        display->setFixedHeight(size.height);
        display->setFixedWidth(size.width);
    }

    display->setPixmap(QPixmap::fromImage(QImage(visu.data, visu.cols, visu.rows, visu.step, QImage::Format_RGB888)));

    if(im < files.size() - 1){
        a ++;
        im ++;
        progressBar->setValue(im);
     }

    else{
        timer->stop();
        QMessageBox msgBox;
        msgBox.setText("The tracking is done, go to work now!!!");
        msgBox.exec();
    }

}




MainWindow::~MainWindow()
{
    delete ui;
}

