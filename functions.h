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

using namespace cv;
using namespace std;


class Tracking{ 
  
  // Member

  

  UMat m_img0;
  UMat m_background;
  
  bool statusRegistration; 
  bool statusBinarisation;
  bool statusPath;

  int m_im;
  Rect m_ROI;
  ofstream m_savefile;
  vector<String> m_files; // OpenCV String class
  string m_path;
  vector<Point3f> m_colorMap;
  vector<vector<Point>> m_memory;

  Point2f curvatureCenter(Point3f tail, Point3f head);
  double curvature(Point2f center , Mat image);
  double modul(double angle);
  vector<double> orientation(UMat image, bool dir);
  vector<Point3f> reassignment(vector<Point3f> inputPrev, vector<Point3f> input, vector<int> assignment);
  UMat backgroundExtraction(vector<String> files, double n);
  void registration(UMat imageReference, UMat& frame);
  void binarisation(UMat& frame, char backgroundColor, int value);
  vector<vector<Point3f> > objectPosition(UMat frame, int minSize, int maxSize);
  vector<int> costFunc(vector<Point3f> prevPos, vector<Point3f> pos, const double LENGHT, const double ANGLE, const double WEIGHT, const double LO);
  vector<Point3f> prevision(vector<Point3f> past, vector<Point3f> present);
  vector<Point3f> color(int number);
  void imageProcessing(String a, vector<vector<Point3f>>& out, vector<vector<Point3f>>& outPrev);
  Tracking();

  public:

  UMat m_binaryFrame;
  Mat m_visuFrame;

  int param_n;
  int param_maxArea;
  int param_minArea;
  int param_spot;
  double param_len;
  double param_angle;
  double param_weight;
  double param_lo;
  int param_arrowSize;
  string param_folder;
  int param_tresh;
  double param_nBackground;
  bool param_thresh;
  int param_x1;
  int param_y1;
  int param_x2;
  int param_y2;
};  


#endif
