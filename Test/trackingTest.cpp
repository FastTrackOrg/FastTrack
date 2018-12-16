#include "gtest/gtest.h"
#include "../functions.cpp"
#include "../mainwindow.cpp"
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
  Tracking tracking("test");
  EXPECT_EQ(tracking.curvatureCenter( Point3f(1, 1, 0.25*M_PI + 0.5*M_PI), Point3f(2, 2, 0.5*M_PI + 0.5*M_PI)), Point2f(0, 0));
  EXPECT_EQ(tracking.curvatureCenter( Point3f(1, 1, 0.25*5*M_PI + 0.5*M_PI), Point3f(2, 2, 0.5*M_PI + 0.5*M_PI)), Point2f(0, 0));
  EXPECT_EQ(tracking.curvatureCenter( Point3f(1, 1, -3*0.25*M_PI + 0.5*M_PI), Point3f(2, 2, 0.5*M_PI + 0.5*M_PI)), Point2f(0, 0));
}


//Registration test
TEST_F(TrackingTest, Registration) {
  Tracking tracking("test");
  
  UMat imageReference, registered, diff;
  Mat H;
  imread("../dataSet/lena.jpg", IMREAD_GRAYSCALE).copyTo(imageReference);

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
  Tracking tracking("test");
  EXPECT_EQ(tracking.modul(0), 0);
  EXPECT_EQ(tracking.modul(-M_PI), M_PI);
  EXPECT_EQ(tracking.modul(4*M_PI), 0);
  EXPECT_EQ(tracking.modul(3*M_PI), M_PI);
  EXPECT_EQ(tracking.modul(-0.5*M_PI), 0.5*3*M_PI);
  EXPECT_EQ(tracking.modul(-0.5*M_PI), 0.5*3*M_PI);
}

TEST_F(TrackingTest, Reassignement) {
  Tracking tracking("test");
  
  vector<Point3f> past, input;
  vector<int> order;
  
  past = { Point3f(1), Point3f(2), Point3f(3) };
  input = { Point3f(3), Point3f(2), Point3f(1) };
  order = { 2, 1, 0 };
  EXPECT_EQ(tracking.reassignment(past, input, order), past);

  past = { Point3f(1), Point3f(2), Point3f(3) };
  input = { Point3f(3), Point3f(2) };
  order = { -1, 1, 0 };
  EXPECT_EQ(tracking.reassignment(past, input, order), past);

  past = { Point3f(1), Point3f(2), Point3f(3) };
  input = { Point3f(3), Point3f(2), Point3f(4), Point3f(1) };
  order = { 3, 1, 0 };
  EXPECT_EQ(tracking.reassignment(past, input, order), past);
}



TEST_F(TrackingTest, Information) {
  Tracking tracking("test");
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
  Tracking tracking("test");
  vector<double> info, test;
  UMat imageReference, rotate;
  
  imread("../dataSet/assymetric_left.png", IMREAD_GRAYSCALE).copyTo(imageReference);
  Point2f center(double(imageReference.cols)*.5, double(imageReference.rows)*.5);
  test = tracking.objectInformation(imageReference);
  tracking.objectDirection(imageReference, Point(test.at(0), test.at(1)), test);
  EXPECT_EQ(round(test.at(2)*1000), round(1000*(M_PI)));
}
}

namespace{
class MainWindowTest : public ::testing::Test {
 protected:
	  virtual void SetUp() {
	  }
	 
	  virtual void TearDown() {
	  }
};

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
