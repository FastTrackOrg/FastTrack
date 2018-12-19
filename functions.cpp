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
    along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
*/


#include "functions.h"
#include "Hungarian.h"

using namespace cv;
using namespace std;



/**
 * @class Tracking
 *
 * @brief This class is intented to execute a tracking analysis on an images sequence. It is initialized with the path where images are stored. This class can be used inside an application by creating a new thread and calling the method startProcess. The tracking workflow can be reimplemented by reimplementing the method startProcess and imageProcessing. This class can also be used as a library by constructing Tracking tracking("") to access public class members and build a new workflow before reimplementing it in imageProcessing public slot.
 *
 * @author Benjamin Gallois
 *
 * @version $Revision: 4.0 $
 *
 * Contact: gallois.benjamin08@gmail.com
 *
 */




/**
  * @brief Computes the center of the curvature defined as the intersection of the minor axis of the head ellipse with the minor axis of the tail ellipse of the object.
  * @param[in] head Parameters of the head ellipse: coordinate and direction of the major axis.
  * @param[in] head Parameters of the tail ellipse: coordinate and direction of the major axis.
  * @return Coordinate of the curvature center.
*/
Point2d Tracking::curvatureCenter(const Point3d &tail, const Point3d &head){

	Point2d center;
  
  // Computes the equation of the slope of the two minors axis of each ellipses
  // from the coordinate and direction of each ellipse.
	Point p1 = Point(tail.x + 10*cos(tail.z + 0.5*M_PI), tail.y + 10*sin(tail.z + 0.5*M_PI));
	Point p2 = Point(head.x + 10*cos(head.z + 0.5*M_PI), head.y + 10*sin(head.z + 0.5*M_PI));

	double a = (tail.y - p1.y)/(tail.x - p1.x);
	double c = (head.y - p2.y)/(head.x - p2.x);
	double b = tail.y - a*tail.x;
	double d = head.y - c*head.x;

  // Solves the equation system by computing the determinant. If the determinant
  // is different of zeros, the two slopes intersect.
	if(a*b == 0){ // Determinant == 0, no unique solution, no intersection
		center = Point(0, 0);
	}
	else{ // Unique solution
		center = Point((b + d)/(c - a), a*((b +d)/(c - a)) + b);
	}

   return center;
}




/**
  * @brief Computes the radius of curvature of the object defined as the inverse of the mean distance between each pixel of the object, and the center of the curvature. The center of curvature is defined as the intersection of the two minor axis of the head and tail ellipse.
  * @param[in] center Center of the curvature.
	* @param[in] image Binary image CV_8U.
  * @return Radius of curvature.
*/
double Tracking::curvature(Point2d center , const Mat &image){

	double d = 0;
	double count = 0;
  
  image.forEach<uchar>
(
  [&d, &count, center](uchar &pixel, const int position[]) -> void
  {
			if(pixel == 255){ // if inside object
        d += pow(pow(center.x - float(position[0]), 2) + pow(center.y - float(position[1]), 2), 0.5);
          count += 1;
    }
  }
); 
	return count/d;
}



/**
  * @brief Computes the usual mathematical modulo 2*PI of an angle.
  * @param[in] angle Input angle.
  * @return Output angle.
*/
double Tracking::modul(double angle) {
    return angle - 2*M_PI * floor(angle/(2*M_PI));
}


/**
  * @brief Computes least difference between two angles, alpha - beta. The difference is oriented in trigonometric convention.
  * @param[in] alpha Input angle.
  * @param[in] beta Input angle.
  * @return Least difference.
*/
double Tracking::angleDifference(double alpha, double beta) {
  
  alpha = modul(alpha);
  beta = modul(beta);
  return -(modul(alpha - beta + M_PI) - M_PI);
}


/**
  * @brief Computes the equivalente ellipse of an object by computing moments of the image.
  * @param[in] image Binary image CV_8U.
  * @return The equivalent ellipse parameters: the object center of mass coordinate and its orientation.
  * @note: This function computes the object orientation, not its direction.
*/
vector<double> Tracking::objectInformation(const UMat &image) {

    Moments moment = moments(image);

    double x = moment.m10/moment.m00;
    double y = moment.m01/moment.m00;

    double i = moment.mu20;
    double j = moment.mu11;
    double k = moment.mu02;


    double orientation = 0.5 * atan((2*j)/(i-k)) + (i<k)*(M_PI*0.5);
  	orientation += 2*M_PI*(orientation<0);
	  orientation = 2*M_PI - orientation;

    double majAxis = 2*pow( (((i + k) + pow( (i - k)*(i - k) + 4*j*j,0.5))*0.5) / moment.m00 ,0.5);
    double minAxis = 2*pow( (((i + k) - pow( (i - k)*(i - k) + 4*j*j,0.5))*0.5) / moment.m00 ,0.5);
    	
    vector<double> param {x, y, orientation, majAxis, minAxis};

  	return param;
}




/**
  * @brief Computes the direction of the object from the object parameter (coordinate of the center of mass and orientation).
  * @details To used this function, the object major axis as to be the the horizontal axis of the image. Therefore it is necessary to rotate the image before calling objectDirection.
  * @param[in] image Binary image CV_8U.
  * @information[in, out] Parameters of the object (x coordinate, y coordinate, orientation).
  * @center[in] Center of mass of the object.
  * @return True if the direction angle is the orientation angle. false if the direction angle is the orientation angle plus pi.
*/
bool Tracking::objectDirection(const UMat &image, Point center, vector<double> &information) {
    
    // Computes the distribution of the image on the horizontal axis.
    
    vector<double> projection;
    reduce(image, projection, 0, REDUCE_SUM);

    vector<double> distribution; //tmp
    double ccMax = *max_element(projection.begin(), projection.end()) / 100;
    for (size_t it = 0; it < projection.size(); ++it){
      int cc = projection.at(it);
      for (int jt = 0; jt < cc/ccMax; ++jt){
        distribution.push_back((double)(it+1));
      }
    }
    
    double mean = accumulate(distribution.begin(), distribution.end(), 0) / double(distribution.size());

    double sd = 0 , skew = 0;

    for(size_t it = 0; it < distribution.size(); ++it){
      sd += pow(distribution.at(it) - mean, 2);
      skew += pow(distribution.at(it) - mean, 3);
    }

    sd = pow(sd/((double)distribution.size()-1), 0.5);
    skew *= (1/(((double)distribution.size() - 1)*pow(sd, 3)));

    if(skew > 0){
			information.at(2) = modul(information.at(2) - M_PI);
      return true;
		}
    return false;
	}




/**
  * @brief Computes the background of an images sequence by averaging n images.
  * @param[in] files List of path for each image in the images sequence.
  * @param[in] n Number of frames to average to computes the background.
  * @return The background image.
*/
UMat Tracking::backgroundExtraction(const vector<String> &files, double n){

    UMat background;
    UMat img0;
    imread(files[0], IMREAD_GRAYSCALE).copyTo(background);
    background.setTo(0);
    imread(files[0], IMREAD_GRAYSCALE).copyTo(img0);
    background.convertTo(background, CV_32FC1);
    img0.convertTo(img0, CV_32FC1);
    int step = files.size()/n;
    UMat cameraFrameReg;
    Mat H;
    int count = 0;

	for(size_t i = 0; i < files.size(); i += step){
        imread(files[i], IMREAD_GRAYSCALE).copyTo(cameraFrameReg);
        cameraFrameReg.convertTo(cameraFrameReg, CV_32FC1);
        Point2d shift = phaseCorrelate(cameraFrameReg, img0);
        H = (Mat_<float>(2, 3) << 1.0, 0.0, shift.x, 0.0, 1.0, shift.y);
				warpAffine(cameraFrameReg, cameraFrameReg, H, cameraFrameReg.size());
				accumulate(cameraFrameReg, background);
        count ++;
	}
  background.convertTo(background, CV_8U, 1./count);

	return background;
}




/**
  * @brief Register two images by phase correlation.
  * @param[in] imageReference Reference image for the registration.
	* @param[in, out] frame Image to register.
*/
void Tracking::registration(UMat imageReference, UMat &frame){

	frame.convertTo(frame, CV_32FC1);
	imageReference.convertTo(imageReference, CV_32FC1);
	Point2d shift = phaseCorrelate(frame, imageReference);
  Mat H = (Mat_<float>(2, 3) << 1.0, 0.0, shift.x, 0.0, 1.0, shift.y);
	warpAffine(frame, frame, H, frame.size());
  frame.convertTo(frame, CV_8U);
}




/**
  * @brief Binarises the image by thresholding.
  * @param[in, out] frame Image to binarised.
	* @param[in] backgroundColor If equal at 'w' the thresholded image will be inverted, if equal at 'b' it will not be inverted.
  * @param[in] value Value at which to threshold the image.
*/
void Tracking::binarisation(UMat& frame, char backgroundColor, int value){

  frame.convertTo(frame, CV_8U);

	if(backgroundColor == 'b'){
        threshold(frame, frame, value, 255, THRESH_BINARY);
	}

	if(backgroundColor == 'w'){
        threshold(frame, frame, value, 255, THRESH_BINARY_INV);
	}
}




/**
  * @brief Computes positions of multiples objects and extracts some features.
  * @details 
  * @param[in] frame: binary image CV_8U.
	* @param[in] minSize Minimal size of an object.
	* @param[in] maxSize: Maximal size of an object.
  * @return All parameters of all objects formated as follow. One vector, inside of this vector, four vectors for parameters of the head, tail, body and features with number of object size. {  { Point(xHead, yHead, thetaHead), ...}, Point({xTail, yTail, thetaHead), ...}, {Point(xBody, yBody, thetaBody), ...}, {Point(curvature, 0, 0), ...}}
*/
vector<vector<Point3d>> Tracking::objectPosition(const UMat &frame, int minSize, int maxSize){

	vector<vector<Point> > contours;
	vector<Point3d> positionHead;
	vector<Point3d> positionTail;
	vector<Point3d> positionFull;
	vector<Point3d> ellipseHead;
	vector<Point3d> ellipseTail;
	vector<Point3d> globalParam;
	UMat dst;
	Rect roiFull, bbox;
	UMat RoiFull, RoiHead, RoiTail, rotate;
	Mat rotMatrix, p, pp;
	vector<double> parameter;
	vector<double> parameterHead;
	vector<double> parameterTail;
	Point2d radiusCurv;

	findContours(frame, contours, RETR_LIST, CHAIN_APPROX_NONE);

	for (size_t i = 0; i < contours.size(); i++){
			
      if(contourArea(contours[i]) > minSize && contourArea(contours[i]) < maxSize){ // Only select objects minArea << objectArea <<maxArea

            // Draw the object in a temporary black image avoiding to select a 
            // part of another object if two objects are very close.
						dst = UMat::zeros(frame.size(), CV_8U);
						drawContours(dst, contours, i, Scalar(255, 255, 255), FILLED,8); 
					
          	
            // Computes the x, y and orientation of the object, in the
            // frame of reference of ROIFull image.
            roiFull = boundingRect(contours[i]);
						RoiFull = dst(roiFull);
						parameter = objectInformation(RoiFull);
            

            // Rotates the image without croping and computes the direction of the object.
            Point center = Point(0.5*RoiFull.cols, 0.5*RoiFull.rows);
            rotMatrix = getRotationMatrix2D(center, -(parameter.at(2)*180)/M_PI, 1);
            bbox = RotatedRect(center, RoiFull.size(), -(parameter.at(2)*180)/M_PI).boundingRect();
            rotMatrix.at<double>(0,2) += bbox.width*0.5 - center.x;
            rotMatrix.at<double>(1,2) += bbox.height*0.5 - center.y;
            warpAffine(RoiFull, rotate, rotMatrix, bbox.size());


           // Computes the coordinate of the center of mass of the fish in the rotated
           // image frame of reference.
            p = (Mat_<double>(3,1) << parameter.at(0), parameter.at(1), 1);
            pp = rotMatrix * p;

					
            // Computes the direction of the object. If objectDirection return true, the
            // head is at the left and the tail at the right.
            Rect roiHead, roiTail;   	
            if ( objectDirection(rotate, Point(pp.at<double>(0,0), pp.at<double>(1,0)), parameter) ) {

              // Head ellipse. Parameters in the frame of reference of the RoiHead image.
              roiHead = Rect(0, 0, pp.at<double>(0,0), rotate.rows);
              RoiHead = rotate(roiHead);
              parameterHead = objectInformation(RoiHead);

              // Tail ellipse. Parameters in the frame of reference of ROITail image.
              roiTail = Rect(pp.at<double>(0,0), 0, rotate.cols-pp.at<double>(0,0), rotate.rows);
              RoiTail = rotate(roiTail);
              parameterTail = objectInformation(RoiTail);

            }
            else {
              // Head ellipse. Parameters in the frame of reference of the RoiHead image.
              roiHead = Rect(pp.at<double>(0,0), 0, rotate.cols-pp.at<double>(0,0), rotate.rows);
              RoiHead = rotate(roiHead);
              parameterHead = objectInformation(RoiHead);

              // Tail ellipse. Parameters in the frame of reference of RoiTail image.
              roiTail = Rect(0, 0, pp.at<double>(0,0), rotate.rows);
              RoiTail = rotate(roiTail);
              parameterTail = objectInformation(RoiTail);
            }


            // Gets all the parameter in the frame of reference of RoiFull image.
						invertAffineTransform(rotMatrix, rotMatrix);
						p = (Mat_<double>(3,1) << parameterHead.at(0) + roiHead.tl().x,parameterHead.at(1) + roiHead.tl().y, 1);
						pp = rotMatrix * p;

						double xHead = pp.at<double>(0,0) + roiFull.tl().x;
						double yHead = pp.at<double>(1,0) + roiFull.tl().y;
						double angleHead = parameterHead.at(2) - M_PI*(parameterHead.at(2) > M_PI);
						angleHead = modul(angleHead + parameter.at(2) + M_PI*(abs(angleHead) > 0.5*M_PI)); // Computes the direction

						p = (Mat_<double>(3,1) << parameterTail.at(0) + roiTail.tl().x, parameterTail.at(1) + roiTail.tl().y, 1);
						pp = rotMatrix * p;
						double xTail = pp.at<double>(0,0) + roiFull.tl().x;
						double yTail = pp.at<double>(1,0) + roiFull.tl().y;
						double angleTail = parameterTail.at(2) - M_PI*(parameterTail.at(2) > M_PI);
						angleTail = modul(angleTail + parameter.at(2) + M_PI*(abs(angleTail) > 0.5*M_PI)); // Computes the direction

						// Computes the curvature of the object as the invert of all distances from each
            // pixels of the fish and the intersection of the minor axis off tail and head ellipse.
						double curv = 1./1e-16;
						radiusCurv = curvatureCenter(Point3d(xTail, yTail, angleTail), Point3d(xHead, yHead, angleHead));
						if(radiusCurv.x != NAN){ //
						            curv = curvature(radiusCurv, RoiFull.getMat(ACCESS_READ));
						}


						positionHead.push_back(Point3d(xHead, yHead, angleHead));
						positionTail.push_back(Point3d(xTail, yTail, angleTail));
						positionFull.push_back(Point3d(parameter.at(0) + roiFull.tl().x, parameter.at(1) + roiFull.tl().y, parameter.at(2)));
            ellipseHead.push_back(Point3d(parameterHead.at(3), parameterHead.at(4), 0));
            ellipseTail.push_back(Point3d(parameterTail.at(3), parameterTail.at(4), 0));

						globalParam.push_back(Point3d(curv, 0, 0));
						}
      
      else if(contourArea(contours[i]) >= maxSize && contourArea(contours[i]) < 3*maxSize) {
            
      }
   }

	vector<vector<Point3d>> out = {positionHead, positionTail, positionFull, globalParam, ellipseHead, ellipseTail};
	return out;
}





/**
  * @brief Computes a cost function and use a global optimization association to associate targets between frame. 
  * @details Method adapted from: "An effective and robust method for Tracking multiple fish in video image based on fish head detection" YQ Chen et al. Use the Hungarian method implemented by Cong Ma, 2016 "https://github.com/mcximing/hungarian-algorithm-cpp" adapted from the matlab implementation by Markus Buehren "https://fr.mathworks.com/matlabcentral/fileexchange/6543-functions-for-the-rectangular-assignment-problem".
  * @param[in] prevPos Vector of objects at t minus one.
	* @param[in] pos Vector of objects parameters at t that we want to reorder in order to conserve the objects identity.
	* @param[in] length Maximal displacement of an object between two frames in pixels.
	* @param[in] angle Maximal difference in orientation an object between two frames in radians.
	* @param[in] weight Weight between distance and direction in the computation of the cost function. Close to 0 the cost function used the distance, close to one it will used the direction. 
	* @param[in] lo Maximal occlusion distance of objects between two frames in pixels.
	* @return The assignment vector containing the new index position to sort the pos vector. 
*/
vector<int> Tracking::costFunc(const vector<Point3d> &prevPos, const vector<Point3d> &pos, double LENGTH, double ANGLE, double WEIGHT, double LO){


	int n = prevPos.size();
	int m = pos.size();
	double c = -1;
	vector<vector<double>> costMatrix(n, vector<double>(m));

	for(int i = 0; i < n; ++i){

		Point3d prevCoord = prevPos.at(i);
		for(int j = 0; j < m; ++j){
			Point3d coord = pos.at(j);
      double d = pow(pow(prevCoord.x - coord.x, 2) + pow(prevCoord.y - coord.y, 2), 0.5);
      if(d < LO){
        c = WEIGHT*(d/LENGTH) + (1 - WEIGHT)*abs(angleDifference(prevCoord.z, coord.z)/ANGLE); //cost function
				costMatrix[i][j] = c;
			}
      else {
				costMatrix[i][j] = 2e307;
			}

		}
	}

	// Hungarian algorithm to solve the assignment problem O(n**3)
	HungarianAlgorithm HungAlgo;
	vector<int> assignment;
	HungAlgo.Solve(costMatrix, assignment);

	return assignment;
}




/**
  * @brief Sorts a vector accordingly to a new index. The sorted vector at index i is the input at index assignment at index i.
  * @param[in] past Vector at t minus one.
  * @param[in] input Vector at t of size m <= n to be sorted.
  * @param[in] assignment Vector with the new index that will be used to sort the input vector.
  * @return The sorted vector.
*/
vector<Point3d> Tracking::reassignment(const vector<Point3d> &past, const vector<Point3d> &input, const vector<int> &assignment){

	vector<Point3d> tmp = past;
	unsigned int n = past.size();
	unsigned int m = input.size();


	if(m == n){ // Same number of targets in two consecutive frames
		for(unsigned int i = 0; i < n; i++){
			tmp.at(i) = input.at(assignment.at(i));
		}
	}

	else if(m > n){// More target in current frame than in previous one
		for(unsigned int i = 0; i < n; i++){
			tmp.at(i) = input.at(assignment.at(i));
		}
	}

	else if(m < n){// Fewer target in current frame than in the previous one
		for(unsigned int i = 0; i < n; i++){
			if(assignment.at(i) != -1){
				tmp.at(i) = input.at(assignment.at(i));
			}
		}
	}

	else{
		qInfo() << "association error" << endl;
	}


	return tmp;
}




/**
  * @Reassignment Resamples a vector accordingly to a new index.
  * @param vector<Point3d> output: output vector of size n
  * @param vector<Point3d> input: input vector of size m <= n
  * @param vector<int> assignment: vector with the new index that will be used to resample the input vector
  * @return vector<Point3d>: output vector of size n.
*/
vector<Point3d> Tracking::prevision(vector<Point3d> past, vector<Point3d> present){

	double l = 0;
	for(unsigned int i = 0; i < past.size(); i++){
		if(past.at(i) != present.at(i)){
			l = pow(pow(past.at(i).x - present.at(i).x, 2) + pow(past.at(i).y - present.at(i).y, 2), 0.5);
			break;
		}
	}



	for(unsigned int i = 0; i < past.size(); i++){
		if(past.at(i) == present.at(i)){
			present.at(i).x += l*cos(present.at(i).z);
			present.at(i).y -= l*sin(present.at(i).z);
		}
	}
	return present;
}





/**
  * @brief Computes a random set of colors.
  * @param[in] number Number of color to generate.
  * @return The vector contaning n colors in RGB.
*/
vector<Point3d> Tracking::color(int number){

	double a, b, c;
	vector<Point3d> colorMap;
	srand (time(NULL));
	for (int j = 0; j<number; ++j){
		a = rand() % 255;
		b = rand() % 255;
		c = rand() % 255;

		colorMap.push_back(Point3d(a, b, c));
	}

	return colorMap;
}





/**
  * @brief Processes an image from an images sequence and tracks and matchs object according to previous image in the sequence.
  * @details Takes a new image from the sequence, substract the background, binarises the image and crop according to defined region of interest. Detects all the objects in the image, extracts the object features. Then match detected object with objects from the previous frame. This function emit a signal to display the image in the user interface.
*/
void Tracking::imageProcessing(){

    imread(m_files.at(m_im), IMREAD_GRAYSCALE).copyTo(m_visuFrame);
    if(statusRegistration){
      registration(m_background, m_visuFrame);
    }

    (statusBinarisation) ? (subtract(m_background, m_visuFrame, m_binaryFrame)) : (subtract(m_visuFrame, m_background, m_binaryFrame));

    binarisation(m_binaryFrame, 'b', param_thresh);

    if (param_dilatation != 0) {
      Mat element = getStructuringElement( MORPH_ELLIPSE, Size( 2*param_dilatation + 1, 2*param_dilatation + 1 ), Point( param_dilatation, param_dilatation ) );
      dilate(m_binaryFrame, m_binaryFrame, element);
    }

    if(m_ROI.width != 0 ) {
      m_binaryFrame = m_binaryFrame(m_ROI);
      m_visuFrame = m_visuFrame(m_ROI);
    }

    m_out = objectPosition(m_binaryFrame, param_minArea, param_maxArea);

    vector<int> identity = costFunc(m_outPrev.at(param_spot), m_out.at(param_spot), param_len, param_angle, param_weight, param_lo);
    for (size_t i = 0; i < m_out.size(); i++) {
      m_out.at(i) = reassignment(m_outPrev.at(i), m_out.at(i), identity);
    }
    cvtColor(m_visuFrame, m_visuFrame, COLOR_GRAY2RGB);
    // Visualisation

    for(size_t l = 0; l < m_out.at(0).size(); l++){
      Point3d coord = m_out.at(param_spot).at(l);
      arrowedLine(m_visuFrame, Point(coord.x, coord.y), Point(coord.x + 5*param_arrowSize*cos(coord.z), coord.y - 5*param_arrowSize*sin(coord.z)), Scalar(m_colorMap.at(l).x, m_colorMap.at(l).y, m_colorMap.at(l).z), param_arrowSize, 10*param_arrowSize, 0);

      if((m_im > 5)){ // Faudra refaire un buffer correct
        polylines(m_visuFrame, m_memory.at(l), false, Scalar(m_colorMap.at(l).x, m_colorMap.at(l).y, m_colorMap.at(l).z), param_arrowSize, 8, 0);
        m_memory.at(l).push_back(Point((int)coord.x, (int)coord.y));
      if(m_im > 50){
          m_memory.at(l).erase(m_memory.at(l).begin());
        }
      }


      m_savefile << m_out.at(0).at(l).x + m_ROI.tl().x << '\t' << m_out.at(0).at(l).y + m_ROI.tl().y << '\t' << m_out.at(0).at(l).z << '\t'  << m_out.at(1).at(l).x + m_ROI.tl().x << '\t' << m_out.at(1).at(l).y + m_ROI.tl().y << '\t' << m_out.at(1).at(l).z  <<  '\t' << m_out.at(2).at(l).x + m_ROI.tl().x << '\t' << m_out.at(2).at(l).y  + m_ROI.tl().y << '\t' << m_out.at(2).at(l).z <<  '\t' << m_out.at(3).at(l).x << '\t' << m_out.at(4).at(l).x << '\t' << m_out.at(4).at(l).y << '\t' << m_out.at(5).at(l).x << '\t' << m_out.at(5).at(l).y << '\t' << m_im << '\n';

    }
  
    m_im ++;
    m_outPrev = m_out;
    
    // Sending rate of images
    if ( (timer->elapsed() - m_displayTime) > 40) {
      emit(newImageToDisplay(m_visuFrame, m_binaryFrame));
     m_displayTime = timer->elapsed();
    }
    if(m_im + 1 > int(m_files.size())){
      m_savefile.flush();
      m_outputFile.close();
      emit(finished());
    qInfo() << timer->elapsed() << endl;
    }
    else {
    QTimer::singleShot(0, this, SLOT(imageProcessing()));
    }
}




/**
  * @brief Constructs the tracking object from a path to an image sequence.
  * @param[in] path path to a folder where images are stocked.
*/
Tracking::Tracking(string path) {
  m_path = path;
}




/**
  * @brief Initializes a tracking analysis and trigerred its execution.
  * @details From the path to a folder where the images sequence is stored, detects the image format and process the fist frame to detect objects. First it computes the background by averaging images from the sequence, then it substracts the background from the first frame and then binarises the resulting image. It detect the object by contour analysis and extracts features by computing the object moments. Finally it triggers the analysis of the second image of the sequence.
*/
void Tracking::startProcess() {
  
  // Find image format
  QList<QString> extensions = { "pgm", "png", "jpeg", "jpg", "tiff", "tif", "bmp", "dib", "jpe", "jp2", "webp", "pbm", "ppm", "sr", "ras", "tif" };
  QDirIterator it(QString::fromStdString(m_path), QStringList(), QDir::NoFilter);
  QString extension;
  while (it.hasNext()) {
    extension = it.next().section('.', -1);
    if( extensions.contains(extension) ) break;
  }
  
  try{
    m_path += + "*." + extension.toStdString();
    glob(m_path, m_files, false); // Get all path to frames
    statusPath = true;
    m_im = 0;
  }
  catch(...){
    statusPath = false;
    emit(finished());
  }
  sort(m_files.begin(), m_files.end());

  m_background = backgroundExtraction(m_files, param_nBackground);
  m_colorMap = color(param_n);
  vector<vector<Point> > tmp(param_n, vector<Point>());
  m_memory = tmp;


  // First frame
  imread(m_files.at(0), IMREAD_GRAYSCALE).copyTo(m_visuFrame);
  
  (statusBinarisation) ? (subtract(m_background, m_visuFrame, m_binaryFrame)) : (subtract(m_visuFrame, m_background, m_binaryFrame));

  binarisation(m_binaryFrame, 'b', param_thresh);

  if (param_dilatation != 0) {
    Mat element = getStructuringElement( MORPH_ELLIPSE, Size( 2*param_dilatation + 1, 2*param_dilatation + 1 ), Point( param_dilatation, param_dilatation ) );
    dilate(m_binaryFrame, m_binaryFrame, element);
  }

  if (m_ROI.width != 0){
    m_binaryFrame = m_binaryFrame(m_ROI);
    m_visuFrame = m_visuFrame(m_ROI);
  }

  m_out= objectPosition(m_binaryFrame, param_minArea, param_maxArea);
  
  // if less objects detected than indicated
  while( (int(m_out.at(0).size()) - param_n) < 0 ){
      for(unsigned int i = 0; i < m_out.size(); i++){
        m_out.at(i).push_back(Point3d(0,0,0));
      }
  }

  // if more objects detected than indicated
  while( (m_out.at(0).size() - param_n) > 0 ){
      for(unsigned int i = 0; i < m_out.size(); i++){
        m_out.at(i).pop_back();
      }
  }
    
  cvtColor(m_visuFrame, m_visuFrame, COLOR_GRAY2RGB);
  
  // Initialize output file and stream
  m_outputFile.setFileName(QString::fromStdString(m_path).section("*",0,0) + "Tracking_Result" + QDir::separator() + "tracking.txt" );
  if(!m_outputFile.open(QFile::WriteOnly | QFile::Text)){
    qInfo() << "Error opening folder";
  }
  m_savefile.setDevice(&m_outputFile);

  // Saving
  m_savefile << "xHead" << '\t' << "yHead" << '\t' << "tHead" << '\t'  << "xTail" << '\t' << "yTail" << '\t' << "tTail"   << '\t'  << "xBody" << '\t' << "yBody" << '\t' << "tBody"   << '\t'  << "curvature" << '\t'<< "headMajorAxisLength" << '\t' << "headMinorAxisLength" << '\t' << "tailMajorAxisLength" << '\t' << "tailMinorAxisLength" << '\t'  << "imageNumber" << "\n";
  for(size_t l = 0; l < m_out.at(0).size(); l++){
    Point3d coord = m_out.at(param_spot).at(l);      
    arrowedLine(m_visuFrame, Point(coord.x, coord.y), Point(coord.x + 5*param_arrowSize*cos(coord.z), coord.y - 5*param_arrowSize*sin(coord.z)), Scalar(m_colorMap.at(l).x, m_colorMap.at(l).y, m_colorMap.at(l).z), param_arrowSize, 10*param_arrowSize, 0);

    if((m_im > 5)){ // Faudra refaire un buffer correct
      polylines(m_visuFrame, m_memory.at(l), false, Scalar(m_colorMap.at(l).x, m_colorMap.at(l).y, m_colorMap.at(l).z), param_arrowSize, 8, 0);
      m_memory.at(l).push_back(Point((int)coord.x, (int)coord.y));
    if(m_im > 50){
        m_memory.at(l).erase(m_memory.at(l).begin());
      }
    }

    // Saving
    coord.x += m_ROI.tl().x;
    coord.y += m_ROI.tl().y;

    m_savefile << m_out.at(0).at(l).x + m_ROI.tl().x << '\t' << m_out.at(0).at(l).y + m_ROI.tl().y << '\t' << m_out.at(0).at(l).z << '\t'  << m_out.at(1).at(l).x + m_ROI.tl().x << '\t' << m_out.at(1).at(l).y + m_ROI.tl().y << '\t' << m_out.at(1).at(l).z  <<  '\t' << m_out.at(2).at(l).x + m_ROI.tl().x << '\t' << m_out.at(2).at(l).y  + m_ROI.tl().y << '\t' << m_out.at(2).at(l).z <<  '\t' << m_out.at(3).at(l).x << '\t' << m_out.at(4).at(l).x << '\t' << m_out.at(4).at(l).y << '\t' << m_out.at(5).at(l).x << '\t' << m_out.at(5).at(l).y << '\t' << m_im << '\n';

  }
  m_outPrev = m_out;
  m_im ++;
  connect(this, SIGNAL(finishedProcessFrame()), this, SLOT(imageProcessing()));
  timer = new QElapsedTimer();


  timer->start();
  m_displayTime = 0;
  emit(newImageToDisplay(m_visuFrame, m_binaryFrame));
  emit(finishedProcessFrame());
}




/**
  * @brief Updates private member from external parameters. This function links the tracking logic with the graphical user interface.
  * @param[in] parameterList List of all parameters used in the tracking.
*/
void Tracking::updatingParameters(const QMap<QString, QString> &parameterList) {

  param_n = parameterList.value("Object number").toInt();
  param_maxArea = parameterList.value("Maximal size").toInt();
  param_minArea = parameterList.value("Minimal size").toInt();
  param_spot = parameterList.value("Spot to track").toInt();
  param_len = parameterList.value("Maximal length").toDouble();
  param_angle = parameterList.value("Maximal angle").toDouble();
  param_weight = parameterList.value("Weight").toDouble();
  param_lo = parameterList.value("Maximal occlusion").toDouble();
  param_arrowSize = parameterList.value("Arrow size").toInt();

  param_thresh = parameterList.value("Binary threshold").toInt();
  param_nBackground = parameterList.value("Number of images background").toDouble();
  param_x1 = parameterList.value("ROI top x").toInt();
  param_y1 = parameterList.value("ROI top y").toInt();
  param_x2 = parameterList.value("ROI bottom x").toInt();
  param_y2 = parameterList.value("ROI bottom y").toInt();
  m_ROI = Rect(param_x1, param_y1, param_x2 - param_x1, param_y2 - param_y1);
  statusRegistration = (parameterList.value("Registration") == "yes") ? true : false;
  statusBinarisation = (parameterList.value("Light background") == "yes") ? true : false;
  param_dilatation = parameterList.value("Dilatation").toInt();

}


Tracking::~Tracking() {
}
