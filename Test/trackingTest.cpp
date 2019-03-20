#include "gtest/gtest.h"
#include "../src/tracking.cpp"
#include <string>

using namespace std;
using namespace cv;

namespace{
class TrackingTest : public ::testing::Test {
 protected:
	  virtual void SetUp() {
	  }
	 
	  virtual void TearDown() {
	  }
};

//Curvature method test
TEST_F(TrackingTest, CurvatureCenter) {
  Tracking tracking("test", "");
  EXPECT_EQ(tracking.curvatureCenter( Point3d(1, 1, 0.25*M_PI + 0.5*M_PI), Point3d(2, 2, 0.5*M_PI + 0.5*M_PI)), Point2d(0, 0));
  EXPECT_EQ(tracking.curvatureCenter( Point3d(1, 1, 0.25*5*M_PI + 0.5*M_PI), Point3d(2, 2, 0.5*M_PI + 0.5*M_PI)), Point2d(0, 0));
  EXPECT_EQ(tracking.curvatureCenter( Point3d(1, 1, -3*0.25*M_PI + 0.5*M_PI), Point3d(2, 2, 0.5*M_PI + 0.5*M_PI)), Point2d(0, 0));
}


//Registration test
TEST_F(TrackingTest, Registration) {
  Tracking tracking("test", "");
  
  UMat imageReference, registered, diff;
  Mat H;
  imread("../dataSet/len_full.jpg", IMREAD_GRAYSCALE).copyTo(imageReference);

  // Adds padding to avoid cropping
  UMat padded(imageReference.rows + 100, imageReference.cols + 100, imageReference.depth());
  copyMakeBorder(imageReference, padded, 50, 50, 50, 50, BORDER_CONSTANT);

  H = (Mat_<float>(2, 3) << 1.0, 0.0, -20, 0.0, 1.0, -20);
	warpAffine(padded, registered, H, padded.size());
  tracking.registration(padded, registered);
  compare(registered, padded, diff, cv::CMP_NE);
  EXPECT_EQ(countNonZero(diff), 0);
  
  H = (Mat_<float>(2, 3) << 1.0, 0.0, 20, 0.0, 1.0, 10);
	warpAffine(padded, registered, H, padded.size());
  tracking.registration(padded, registered);
  compare(registered, padded, diff, cv::CMP_NE);
  EXPECT_EQ(countNonZero(diff), 0);

  H = (Mat_<float>(2, 3) << 1.0, 0.0, 0, 0.0, 1.0, 0);
	warpAffine(padded, registered, H, padded.size());
  tracking.registration(padded, registered);
  compare(registered, padded, diff, cv::CMP_NE);
  EXPECT_EQ(countNonZero(diff), 0);
}

TEST_F(TrackingTest, Module) {
  Tracking tracking("test", "");
  EXPECT_EQ(tracking.modul(0), 0);
  EXPECT_EQ(tracking.modul(M_PI), M_PI);
  EXPECT_EQ(tracking.modul(M_PI*0.5 + 2*M_PI), M_PI*0.5);
  EXPECT_EQ(tracking.modul(M_PI*0.5 - 2*M_PI), M_PI*0.5);
  EXPECT_EQ(tracking.modul(-M_PI*0.5 + 2*M_PI), 3*M_PI*0.5);
  EXPECT_EQ(tracking.modul(-M_PI*0.5 - 2*M_PI), 3*M_PI*0.5);
  EXPECT_EQ(tracking.modul(-M_PI), M_PI);
  EXPECT_EQ(tracking.modul(4*M_PI), 0);
  EXPECT_EQ(tracking.modul(3*M_PI), M_PI);
  EXPECT_EQ(tracking.modul(-0.5*M_PI), 0.5*3*M_PI);
  EXPECT_EQ(tracking.modul(-0.5*M_PI), 0.5*3*M_PI);
}


TEST_F(TrackingTest, AngleDifferrence) {
  Tracking tracking("test", "");
  EXPECT_EQ(tracking.angleDifference(M_PI, 0.5*M_PI), -0.5*M_PI);
  EXPECT_EQ(tracking.angleDifference(0.5*M_PI, M_PI), 0.5*M_PI);
  EXPECT_EQ(tracking.angleDifference(-0.5*M_PI, -M_PI), -0.5*M_PI);
  EXPECT_EQ(tracking.angleDifference(-M_PI, -0.5*M_PI), +0.5*M_PI);
  EXPECT_EQ(tracking.angleDifference(4*M_PI + M_PI, 4*M_PI + 0.5*M_PI), -0.5*M_PI);
  EXPECT_EQ(tracking.angleDifference(4*M_PI + 0.5*M_PI, 4*M_PI + M_PI), 0.5*M_PI);
  EXPECT_EQ(tracking.angleDifference(-4*M_PI + 0.5*M_PI, -4*M_PI + M_PI), +0.5*M_PI);
  EXPECT_EQ(tracking.angleDifference(-4*M_PI + M_PI, 4*M_PI + -0.5*M_PI), +0.5*M_PI);
  EXPECT_EQ(tracking.angleDifference(4*M_PI + M_PI, 0.5*M_PI), -0.5*M_PI);
  EXPECT_EQ(tracking.angleDifference(4*M_PI + 0.5*M_PI, M_PI), 0.5*M_PI);
  EXPECT_EQ(tracking.angleDifference(-4*M_PI + 0.5*M_PI, -M_PI), 0.5*M_PI);
  EXPECT_EQ(tracking.angleDifference(-4*M_PI + M_PI, -0.5*M_PI), +0.5*M_PI);
  EXPECT_EQ(tracking.angleDifference(M_PI, 4*M_PI + 0.5*M_PI), -0.5*M_PI);
  EXPECT_EQ(tracking.angleDifference(0.5*M_PI, 4*M_PI + M_PI), 0.5*M_PI);
  EXPECT_EQ(tracking.angleDifference(-0.5*M_PI, -4*M_PI + M_PI), -0.5*M_PI);
  EXPECT_EQ(tracking.angleDifference(-M_PI, 4*M_PI + -0.5*M_PI), +0.5*M_PI);
}


TEST_F(TrackingTest, Reassignement) {
  Tracking tracking("test", "");
  
  vector<Point3d> past, input, comp;
  vector<int> order, id, lost, idComp, lostComp;
  

  past = { Point3d(1), Point3d(2), Point3d(3) };
  input = {  };
  order = { -1, -1, -1 };
  EXPECT_EQ(tracking.reassignment(past, input, order), past);
  
  
  past = { Point3d(1), Point3d(2), Point3d(3) };
  input = { Point3d(3), Point3d(2), Point3d(1) };
  order = { 2, 1, 0 };
  EXPECT_EQ(tracking.reassignment(past, input, order), past);


  past = { Point3d(1), Point3d(2), Point3d(3) };
  input = { Point3d(3), Point3d(2) };
  order = { -1, 1, 0 };
  EXPECT_EQ(tracking.reassignment(past, input, order), past);


  past = { Point3d(1), Point3d(2), Point3d(3) };
  input = { Point3d(3), Point3d(2), Point3d(4), Point3d(1) };
  order = { 3, 1, 0 };
  comp  = { Point3d(1), Point3d(2), Point3d(3), Point3d(4) };
  EXPECT_EQ(tracking.reassignment(past, input, order), comp);


  past = { Point3d(1), Point3d(2) };
  input = { Point3d(3), Point3d(2), Point3d(4), Point3d(1) };
  order = { 3, 1 };
  EXPECT_EQ(tracking.reassignment(past, input, order), comp);

  past = { Point3d(1), Point3d(2) };
  input = { Point3d(3), Point3d(2), Point3d(4), Point3d(1) };
  order = { 3, 1, -1, -1 };
  comp  = { Point3d(1), Point3d(2), Point3d(3), Point3d(4) };
  EXPECT_EQ(tracking.reassignment(past, input, order), comp);
}


TEST_F(TrackingTest, Occlusion) {
  Tracking tracking("test", "");
  
  vector<Point3d> past, input, comp;
  vector<int> order;
  vector<int> occlusion;
  
  past = { Point3d(1), Point3d(2), Point3d(3) };
  input = { Point3d(3), Point3d(2), Point3d(1) };
  order = { 2, 1, 0 };
  occlusion = { };
  EXPECT_EQ(tracking.reassignment(past, input, order), past);
  EXPECT_EQ(tracking.findOcclusion(order), occlusion);

  past = { Point3d(1), Point3d(2), Point3d(3) };
  input = { Point3d(3), Point3d(2) };
  order = { -1, 1, 0 };
  occlusion = {0};
  EXPECT_EQ(tracking.reassignment(past, input, order), past);
  EXPECT_EQ(tracking.findOcclusion(order), occlusion);

  past = { Point3d(1), Point3d(2), Point3d(3) };
  input = { Point3d(3) };
  order = { -1, -1, 0 };
  occlusion = {0, 1};
  EXPECT_EQ(tracking.reassignment(past, input, order), past);
  EXPECT_EQ(tracking.findOcclusion(order), occlusion);

  past = { Point3d(1), Point3d(2), Point3d(3) };
  input = {  };
  order = { -1, -1, -1 };
  occlusion = {0, 1, 2};
  EXPECT_EQ(tracking.reassignment(past, input, order), past);
  EXPECT_EQ(tracking.findOcclusion(order), occlusion);
  
  past = { Point3d(1), Point3d(2), Point3d(3) };
  input = { Point3d(3), Point3d(2), Point3d(4), Point3d(1) };
  comp = { Point3d(1), Point3d(2), Point3d(3), Point3d(4) };
  order = { 3, 1, 0 };
  occlusion = { };
  EXPECT_EQ(tracking.reassignment(past, input, order), comp);
  EXPECT_EQ(tracking.findOcclusion(order), occlusion);
  
}


TEST_F(TrackingTest, Cleaning) {
  
  Tracking tracking("test", "");
  
  vector<vector<Point3d>> input, comp;
  vector<int> occlusion, lost, id, lostComp, idComp;
  
  input = { { Point3d(3) }, { Point3d(2), }, { Point3d(1), } };
  comp = { { Point3d(3), }, { Point3d(2), }, { Point3d(1), } };
  id = {0, 1, 2};
  idComp = {0, 1, 2};
  lost = {0, 0, 0};
  lostComp = {0, 0, 0};
  occlusion = { };
  tracking.cleaning(occlusion, lost, id, input, 10);
  EXPECT_EQ(input, comp);
  EXPECT_EQ(id, idComp);
  EXPECT_EQ(lost, lostComp);


  input = { { Point3d(3) }, { Point3d(2), }, { Point3d(1), } };
  comp = { { Point3d(3) }, { Point3d(2), }, { Point3d(1), } };
  id = {0, 1, 2};
  idComp = {0, 1, 2};
  lost = {0, 0, 0};
  lostComp = {0, 1, 0};
  occlusion = { 1 };
  tracking.cleaning(occlusion, lost, id, input, 10);
  EXPECT_EQ(input, comp);
  EXPECT_EQ(id, idComp);
  EXPECT_EQ(lost, lostComp);


  input = { { Point3d(3) }, { Point3d(2), }, { Point3d(1), } };
  comp = { { Point3d(3) }, { Point3d(2), }, { Point3d(1), } };
  id = {0, 1, 2};
  idComp = {0, 1, 2};
  lost = {0, 0, 0};
  lostComp = {1, 1, 0};
  occlusion = {0, 1 };
  tracking.cleaning(occlusion, lost, id, input, 10);
  EXPECT_EQ(input, comp);
  EXPECT_EQ(id, idComp);
  EXPECT_EQ(lost, lostComp);


  input = { { Point3d(0), Point3d(1) }, { Point3d(0), Point3d(1) }, { Point3d(0), Point3d(1) } };
  comp = { { Point3d(1) }, { Point3d(1) }, { Point3d(1) } };
  id = {0, 1, 2};
  idComp = {1, 2};
  lost = {10, 0, 0};
  lostComp = {0, 0};
  occlusion = { 0 };
  tracking.cleaning(occlusion, lost, id, input, 10);
  EXPECT_EQ(input, comp);
  EXPECT_EQ(id, idComp);
  EXPECT_EQ(lost, lostComp);


  input = { { Point3d(0), Point3d(1) }, { Point3d(0), Point3d(1) }, { Point3d(0), Point3d(1) } };
  comp = { {  }, {  }, {  } };
  id = {0, 1};
  idComp = {};
  lost = {10, 10};
  lostComp = {};
  occlusion = { 0, 1 };
  tracking.cleaning(occlusion, lost, id, input, 10);
  EXPECT_EQ(input, comp);
  EXPECT_EQ(id, idComp);
  EXPECT_EQ(lost, lostComp);
}


TEST_F(TrackingTest, Information) {
  Tracking tracking("test", "");
  vector<double> info, test;
  Mat H;
  UMat imageReference, translate;
  
  imread("../dataSet/rectangle_centered.png", IMREAD_GRAYSCALE).copyTo(imageReference);
  test = tracking.objectInformation(imageReference);
  EXPECT_EQ(round(test.at(0)), 4166 );
  EXPECT_EQ(round(test.at(1)), 4166 );

  for (int i = -60; i < 60; i += 2) {
    H = (Mat_<float>(2, 3) << 1.0, 0.0, i, 0.0, 1.0, -i);
    warpAffine(imageReference, translate, H, Size(imageReference.cols, imageReference.rows));
    test = tracking.objectInformation(translate);
    EXPECT_EQ(round(test.at(0)), 4166 + i);
    EXPECT_EQ(round(test.at(1)), 4166 - i);
  }
}

TEST_F(TrackingTest, InformationOrientation) {
  Tracking tracking("test", "");
  vector<double> info, test;
  UMat imageReference, rotate;
  
  imread("../dataSet/assymetric_left.png", IMREAD_GRAYSCALE).copyTo(imageReference);
  Point2d center(double(imageReference.cols)*.5, double(imageReference.rows)*.5);
  test = tracking.objectInformation(imageReference);
  tracking.objectDirection(imageReference, Point(test.at(0), test.at(1)), test);
  EXPECT_EQ(round(test.at(2)*1000), round(1000*(M_PI)));
}


TEST_F(TrackingTest, costFunction) {
  Tracking tracking("test", "");
  
  vector<Point3d> past, current, pTest;
  vector<int> order, test;


  // Empty current vector
  past = { Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0) };
  current = { };
  order = tracking.costFunc(past, current, 10, 90, 1, 20);
  test = {-1, -1, -1};
  EXPECT_EQ(order, test);
  EXPECT_EQ(tracking.reassignment(past, current, order), past);
  
  
  past = { Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0) };
  current = { Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0) };

  order = tracking.costFunc(past, current, 10, 90, 1, 0);
  test = {-1, -1, -1};
  EXPECT_EQ(order, test);
  pTest = { Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0), Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0) };
  EXPECT_EQ(tracking.reassignment(past, current, order), pTest);
 
  
  past = { Point3d(10, 10, 0), Point3d(20, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0) };
  current = { Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0) };

  order = tracking.costFunc(past, current, 10, 90, 1, 0);
  test = {-1, -1, -1 ,-1};
  EXPECT_EQ(order, test);
  pTest = { Point3d(10, 10, 0), Point3d(20, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0), Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0) };
  EXPECT_EQ(tracking.reassignment(past, current, order), pTest);
 
  
  // Empty past vector
  current = { Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0) };
  past = { };
  order = tracking.costFunc(past, current, 10, 90, 1, 20);
  test = {};
  EXPECT_EQ(order, test);
  EXPECT_EQ(tracking.reassignment(past, current, order), current);


  current = { };
  past = { };
  order = tracking.costFunc(past, current, 10, 90, 1, 20);
  test = {};
  EXPECT_EQ(order, test);
  EXPECT_EQ(tracking.reassignment(past, current, order), current);

  
  // Only translation
  past = { Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0) };
  current = { Point3d(50, 60, 0), Point3d(10, 10, 0), Point3d(30, 40, 0) };
  order = tracking.costFunc(past, current, 10, 90, 1, 20);
  test = {1, 2, 0};
  EXPECT_EQ(order, test);
  EXPECT_EQ(tracking.reassignment(past, current, order), past);

  // Only translation
  past = { Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0) };
  current = { Point3d(50, 50, 0), Point3d(10, 20, 0), Point3d(30, 30, 0) };
  order = tracking.costFunc(past, current, 10, 90, 1, 1);
  test = {1, 2, 0};
  EXPECT_NE(order, test);

  // Only angle no movement
  past = { Point3d(10, 10, 90), Point3d(30, 40, 180), Point3d(50, 60, 0) };
  current = { Point3d(50, 60, 0), Point3d(10, 10, 90), Point3d(30, 40, 180) };
  order = tracking.costFunc(past, current, 10, 90, 0, 20);
  test = {1, 2, 0};
  EXPECT_EQ(order, test);
  EXPECT_EQ(tracking.reassignment(past, current, order), past);
  
  // Only translation
  past = { Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0) };
  current = { Point3d(60, 60, 0), Point3d(10, 0, 0), Point3d(30, 50, 0) };
  order = tracking.costFunc(past, current, 15, 90, 0.5, 20);
  test = {1, 2, 0};
  EXPECT_EQ(order, test);

  // Rotation and translation
  past = { Point3d(10, 10, 0), Point3d(30, 40, 3*0.5*M_PI), Point3d(50, 60, 0.5*M_PI) };
  current = { Point3d(60, 60, 0), Point3d(10, 0, 0.5*M_PI), Point3d(30, 50, 0) };
  order = tracking.costFunc(past, current, 15, 90, 0.5, 20);
  test = {1, 2, 0};
  EXPECT_EQ(order, test);

  // Only translation
  past = { Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0) };
  current = { Point3d(60, 60, 0), Point3d(10, 0, 0), Point3d(30, 50, 0), Point3d(30, 150, 0)};
  order = tracking.costFunc(past, current, 15, 90, 0.5, 20);
  test = {1, 2, 0};
  EXPECT_EQ(order, test);

  // Rotation and translation
  past = { Point3d(10, 10, 0), Point3d(30, 40, 3*0.5*M_PI), Point3d(50, 60, 0.5*M_PI) };
  current = { Point3d(60, 60, 0), Point3d(10, 0, 0.5*M_PI) };
  order = tracking.costFunc(past, current, 15, 90, 0.5, 20);
  test = {1, -1, 0};
  EXPECT_EQ(order, test);

  // Only rotation
  past = { Point3d(10, 10, 0), Point3d(30, 40, 3*0.5*M_PI), Point3d(50, 60, 0.5*M_PI) };
  current = { Point3d(50, 60, 0), Point3d(10, 10, 0.5*M_PI), Point3d(30, 40, 0) };
  order = tracking.costFunc(past, current, 15, 90, 0.5, 20);
  test = {1, 2, 0};
  EXPECT_EQ(order, test);


  past = { Point3d(10, 10, 0), Point3d(30, 40, 3*0.5*M_PI), Point3d(50, 60, 0.5*M_PI) };
  current = { Point3d(50, 60, 0), Point3d(100, 100, 0.5*M_PI), Point3d(30, 40, 0) };
  order = tracking.costFunc(past, current, 15, 90, 0.5, 0);
  test = {-1, -1, -1};
  EXPECT_EQ(order, test);

  past = { Point3d(10, 10, 0), Point3d(30, 40, 3*0.5*M_PI), Point3d(50, 60, 0.5*M_PI) };
  current = { Point3d(50, 60, 0), Point3d(100, 100, 0.5*M_PI), Point3d(30, 40, 0) };
  order = tracking.costFunc(past, current, 15, 90, 0.5, 10);
  test = {-1, 2, 0};
  EXPECT_EQ(order, test);
}
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
