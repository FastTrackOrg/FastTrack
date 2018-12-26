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

  QElapsedTimer *timer;   /*!< Timer that measured the time during the analysis execution. */

  UMat m_background;   /*!< Background image CV_8U. */
  
  bool statusRegistration;    /*!< Enable registration. */
  bool statusBinarisation;   /*!< True if wite objects on dark background, flase otherwise. */
  bool statusPath;   /*!< True if the path is correct, ie the path folder contains an images sequence. */

  int m_im;   /*!< Index of the next image to process in the m_files list. */
  Rect m_ROI;   /*!< Rectangular region of interrest. */
  QTextStream m_savefile;   /*!< Stream to output tracking data. */
  QFile m_outputFile;   /*!< Path to the file where to save tracking data. */
  vector<cv::String> m_files;   /*!< Vector containing the path for each image in the images sequence. */
  vector<Point3d> m_colorMap;   /*!< Vector containing RBG color. */
  vector<vector<Point>> m_memory;   /*!< Vector containing the last 50 tracking data. */

  string m_path;   /*!< Binary image CV_8U */
  int m_displayTime;   /*!< Binary image CV_8U */
  
  int param_n;   /*!< Number of objects. */
  int param_maxArea;   /*!< Maximal area of an object. */
  int param_minArea;   /*!< Minimal area of an object. */
  int param_spot;   /*!< Which spot parameters are used to computes the cost function. 0: head, 1: tail, 2: body. */
  double param_len;   /*!< Maximal length travelled by an object between two images. */
  double param_angle;   /*!< Maximal change in direction of an object between two images. */
  double param_weight;   /*!< Weight between distance and direction in the cost function.  */
  double param_lo;   /*!< Maximal distance allowed by an object to travel during an occlusion event. */
  int param_arrowSize;   /*!< Size of the arrow in the displayed image. */
  int param_thresh;   /*!< Value of the threshold to binarize the image. */
  double param_nBackground;   /*!< Number of images to average to compute the background. */
  int param_x1;   /*!< Top x corner of the region of interest. */
  int param_y1;   /*!< Top y corner of the region of interest. */
  int param_x2;   /*!< Bottom x corner of the region of interest. */
  int param_y2;   /*!< Bottom y corner of the region of interest. */
  int param_dilatation;   /*!< Dilatation coefficient to dilate binary image. */




  public:
  Tracking(string path);
  ~Tracking();

  Point2d curvatureCenter(const Point3d &tail, const Point3d &head);
  double curvature(Point2d center , const Mat &image);
  double modul(double angle);
  double angleDifference(double alpha, double beta);
  bool objectDirection(const UMat &image, Point center, vector<double> &information);
  vector<double> objectInformation(const UMat &image);
  vector<Point3d> reassignment(const vector<Point3d> &past, const vector<Point3d> &input, const vector<int> &assignment);
  UMat backgroundExtraction(const vector<String> &files, double n);
  void registration(UMat imageReference, UMat& frame);
  void binarisation(UMat& frame, char backgroundColor, int value);
  vector<vector<Point3d> > objectPosition(const UMat &frame, int minSize, int maxSize);
  vector<int> costFunc(const vector<Point3d> &prevPos, const vector<Point3d> &pos, double LENGHT, double ANGLE, double WEIGHT, double LO);
  vector<Point3d> prevision(vector<Point3d> past, vector<Point3d> present);
  vector<Point3d> color(int number);
  vector<int> findOcclusion(vector<int> assignment); 

  UMat m_binaryFrame;   /*!< Binary image CV_8U */
  UMat m_visuFrame;   /*!< Image 8 bit CV_8U */
  vector<vector<Point3d>> m_out;   /*!< Objects information at iteration minus one */
  vector<vector<Point3d>> m_outPrev;   /*!< Objects information at current iteration */
  
  
  public slots:
  void startProcess();
  void updatingParameters(const QMap<QString, QString> &);

  private slots:
  void imageProcessing();

  signals:
  /**
  * @brief Emitted when images are available to display. The framerate can't be more than 20 fps.
  * @param m_binaryFrame Binary image CV_8U.
  * @param m_visuFrame Image CV_8UC3 containing the tracking visualisation.
  */
  void newImageToDisplay(UMat& m_binaryFrame, UMat& m_visuFrame);
  
  
  /**
  * @brief Emitted when the first image has been processed to trigger the starting of the analysis.
  */
  void finishedProcessFrame();
  
  
  /**
  * @brief Emitted when all images have been processed.
  */
  void finished();

};  


#endif
