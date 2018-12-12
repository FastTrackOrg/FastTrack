/**############################################################################################
								    main.cpp
								    Purpose: functions header for the functions.cpp

								    @author Benjamin GALLOIS
										@email benjamin.gallois@upmc.fr
										@website benjamin-gallois.fr
								    @version 2.0
										@date 2017
###############################################################################################*/

/*
     .-""L_        		     .-""L_
;`, /   ( o\ 					;`, /   ( o\
\  ;    `, /   				\  ;    `, /
;_/"`.__.-"						;_/"`.__.-"

     .-""L_        		     .-""L_
;`, /   ( o\ 					;`, /   ( o\
\  ;    `, /   				\  ;    `, /
;_/"`.__.-"						;_/"`.__.-"


*/


#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>
#include <time.h>
#include <fstream>
#include <math.h>
#include <numeric>
#include <stdlib.h>
#include <QObject>
#include <QMap>
#include <QDebug>
#include <QThread>
#include <QString>
#include <QTimer>
#include <QFile>
#include <QDir>
#include <QList>
#include <QDirIterator>
#include <QFileInfo>
#include <QTextStream>
#include <QElapsedTimer>

using namespace cv;
using namespace std;



class Tracking : public QObject {

    Q_OBJECT



  QElapsedTimer *timer;

  UMat m_background;
  
  bool statusRegistration; 
  bool statusBinarisation;
  bool statusPath;

  int m_im;
  Rect m_ROI;
  QTextStream m_savefile;
  QFile m_outputFile;
  vector<cv::String> m_files; // OpenCV String class
  vector<Point3f> m_colorMap;
  vector<vector<Point>> m_memory;

  Point2f curvatureCenter(Point3f tail, Point3f head);
  double curvature(Point2f center , Mat image);
  double modul(double angle);
  bool objectDirection(UMat image, Point center, vector<double> &information);
  vector<double> objectInformation(UMat image);
  vector<Point3f> reassignment(vector<Point3f> inputPrev, vector<Point3f> input, vector<int> assignment);
  UMat backgroundExtraction(const vector<String> &files, double n);
  void registration(UMat imageReference, UMat& frame);
  void binarisation(UMat& frame, char backgroundColor, int value);
  vector<vector<Point3f> > objectPosition(UMat frame, int minSize, int maxSize);
  vector<int> costFunc(vector<Point3f> prevPos, vector<Point3f> pos, double LENGHT, double ANGLE, double WEIGHT, double LO);
  vector<Point3f> prevision(vector<Point3f> past, vector<Point3f> present);
  vector<Point3f> color(int number);
  vector<vector<Point3f>> m_out;
  vector<vector<Point3f>> m_outPrev;
  string m_path;
  int m_displayTime;

  public:
  Tracking(string path);
  ~Tracking();

  UMat m_binaryFrame;
  UMat m_visuFrame;

  int param_n;
  int param_maxArea;
  int param_minArea;
  int param_spot;
  double param_len;
  double param_angle;
  double param_weight;
  double param_lo;
  int param_arrowSize;
  int param_thresh;
  double param_nBackground;
  int param_x1;
  int param_y1;
  int param_x2;
  int param_y2;
  int param_dilatation;

  public slots:
  void startProcess();
  void updatingParameters(const QMap<QString, QString> &);
  void imageProcessing();

  signals:
  void newImageToDisplay(UMat&, UMat&);
  void finishedProcessFrame();
  void finished();

};  


#endif
