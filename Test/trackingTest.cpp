#include <QMap>
#include <string>
#include "../src/tracking.cpp"
#include "gtest/gtest.h"

using namespace std;
using namespace cv;

namespace {
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
  Point2i test = tracking.curvatureCenter(Point3d(1, 1, 0.25 * M_PI + 0.5 * M_PI), Point3d(2, 2, 0.5 * M_PI + 0.5 * M_PI));
  EXPECT_DOUBLE_EQ(test.x, 2);
  EXPECT_DOUBLE_EQ(test.y, 0);
  test = tracking.curvatureCenter(Point3d(1, 1, 0.5 * M_PI), Point3d(2, 2, 0.5 * M_PI + 0.5 * M_PI));
  EXPECT_DOUBLE_EQ(test.x, 2);
  EXPECT_DOUBLE_EQ(test.y, 1);
  EXPECT_EQ(tracking.curvatureCenter(Point3d(1, 1, M_PI), Point3d(1, 1, M_PI)), Point2d(0, 0));
  EXPECT_EQ(tracking.curvatureCenter(Point3d(1, 1, M_PI), Point3d(1, 20, M_PI)), Point2d(0, 0));
  EXPECT_EQ(tracking.curvatureCenter(Point3d(1, 1, M_PI), Point3d(2, 2, M_PI)), Point2d(0, 0));
}

//Registration test
TEST_F(TrackingTest, RegistrationMethod0Lena) {
  Tracking tracking("test", "");

  UMat imageReference, registered, diff;
  Mat H;
  imread("../dataSet/len_full.jpg", IMREAD_GRAYSCALE).copyTo(imageReference);
  // Adds padding to avoid cropping
  UMat padded(imageReference.rows + 100, imageReference.cols + 100, imageReference.depth());
  copyMakeBorder(imageReference, padded, 50, 50, 50, 50, BORDER_CONSTANT);

  H = (Mat_<float>(2, 3) << 1.0, 0.0, -20, 0.0, 1.0, -20);
  warpAffine(padded, registered, H, padded.size());
  tracking.registration(padded, registered, 0);
  compare(registered, padded, diff, cv::CMP_NE);
  EXPECT_EQ(countNonZero(diff), 0);

  H = (Mat_<float>(2, 3) << 1.0, 0.0, 50, 0.0, 1.0, 10);
  warpAffine(padded, registered, H, padded.size());
  tracking.registration(padded, registered, 0);
  compare(registered, padded, diff, cv::CMP_NE);
  EXPECT_EQ(countNonZero(diff), 0);

  H = (Mat_<float>(2, 3) << 1.0, 0.0, 0, 0.0, 1.0, 0);
  warpAffine(padded, registered, H, padded.size());
  tracking.registration(padded, registered, 0);
  compare(registered, padded, diff, cv::CMP_NE);
  EXPECT_EQ(countNonZero(diff), 0);
}

TEST_F(TrackingTest, RegistrationMethod1Lena) {
  Tracking tracking("test", "");

  UMat imageReference, registered, diff;
  Mat H;
  imread("../dataSet/len_full.jpg", IMREAD_GRAYSCALE).copyTo(imageReference);
  // Adds padding to avoid cropping
  UMat padded(imageReference.rows + 100, imageReference.cols + 100, imageReference.depth());
  copyMakeBorder(imageReference, padded, 50, 50, 50, 50, BORDER_CONSTANT);

  H = (Mat_<float>(2, 3) << 1.0, 0.0, -20, 0.0, 1.0, -20);
  warpAffine(padded, registered, H, padded.size());
  tracking.registration(padded, registered, 1);
  compare(registered, padded, diff, cv::CMP_NE);
  EXPECT_EQ(countNonZero(diff), 0);

  H = (Mat_<float>(2, 3) << 1.0, 0.0, 50, 0.0, 1.0, 10);
  warpAffine(padded, registered, H, padded.size());
  tracking.registration(padded, registered, 1);
  compare(registered, padded, diff, cv::CMP_NE);
  EXPECT_EQ(countNonZero(diff), 0);

  H = (Mat_<float>(2, 3) << 1.0, 0.0, 0, 0.0, 1.0, 0);
  tracking.registration(padded, registered, 1);
  compare(registered, padded, diff, cv::CMP_NE);
  EXPECT_EQ(countNonZero(diff), 0);
}

// Not working pixel resolution
TEST_F(TrackingTest, RegistrationMethod2Lena) {
  Tracking tracking("test", "");

  UMat imageReference, registered, diff;
  Mat H;
  imread("../dataSet/len_full.jpg", IMREAD_GRAYSCALE).copyTo(imageReference);
  // Adds padding to avoid cropping
  UMat padded(imageReference.rows + 100, imageReference.cols + 100, imageReference.depth());
  copyMakeBorder(imageReference, padded, 50, 50, 50, 50, BORDER_CONSTANT);

  /*H = (Mat_<float>(2, 3) << 1.0, 0.0, -2, 0.0, 1.0, -2);
	warpAffine(padded, registered, H, padded.size());
  tracking.registration(padded, registered, 2);
  subtract(registered, padded, registered);
  imwrite("/home/benjamin/reg.pgm", registered);
  compare(registered, padded, diff, cv::CMP_NE);
  EXPECT_EQ(countNonZero(diff), 0);
  
  H = (Mat_<float>(2, 3) << 1.0, 0.0, 50, 0.0, 1.0, 10);
	warpAffine(padded, registered, H, padded.size());
  tracking.registration(padded, registered, 2);
  compare(registered, padded, diff, cv::CMP_NE);
  EXPECT_EQ(countNonZero(diff), 0);

  H = (Mat_<float>(2, 3) << 1.0, 0.0, 0, 0.0, 1.0, 0);
	warpAffine(padded, registered, H, padded.size());
  tracking.registration(padded, registered, 2);
  compare(registered, padded, diff, cv::CMP_NE);
  EXPECT_EQ(countNonZero(diff), 0);*/
}

// Method not working for this type of image
TEST_F(TrackingTest, RegistrationMethod0Dual) {
  Tracking tracking("test", "");

  UMat imageReference, registered, diff;
  Mat H;
  imread("../dataSet/dual.pgm", IMREAD_GRAYSCALE).copyTo(imageReference);
  // Adds padding to avoid cropping
  UMat padded(imageReference.rows + 100, imageReference.cols + 100, imageReference.depth());
  copyMakeBorder(imageReference, padded, 50, 50, 50, 50, BORDER_CONSTANT);

  H = (Mat_<float>(2, 3) << 1.0, 0.0, -20, 0.0, 1.0, -20);
  warpAffine(padded, registered, H, padded.size());
  tracking.registration(padded, registered, 0);
  compare(registered, padded, diff, cv::CMP_NE);
  EXPECT_NE(countNonZero(diff), 0);

  H = (Mat_<float>(2, 3) << 1.0, 0.0, 50, 0.0, 1.0, 10);
  warpAffine(padded, registered, H, padded.size());
  tracking.registration(padded, registered, 0);
  compare(registered, padded, diff, cv::CMP_NE);
  EXPECT_NE(countNonZero(diff), 0);

  H = (Mat_<float>(2, 3) << 1.0, 0.0, 0, 0.0, 1.0, 0);
  warpAffine(padded, registered, H, padded.size());
  tracking.registration(padded, registered, 0);
  compare(registered, padded, diff, cv::CMP_NE);
  EXPECT_NE(countNonZero(diff), 0);
}

TEST_F(TrackingTest, RegistrationMethod1Dual) {
  Tracking tracking("test", "");

  UMat imageReference, registered, diff;
  Mat H;
  imread("../dataSet/dual.pgm", IMREAD_GRAYSCALE).copyTo(imageReference);
  // Adds padding to avoid cropping
  UMat padded(imageReference.rows + 100, imageReference.cols + 100, imageReference.depth());
  copyMakeBorder(imageReference, padded, 50, 50, 50, 50, BORDER_CONSTANT);

  H = (Mat_<float>(2, 3) << 1.0, 0.0, -20, 0.0, 1.0, -20);
  warpAffine(padded, registered, H, padded.size());
  tracking.registration(padded, registered, 1);
  compare(registered, padded, diff, cv::CMP_NE);
  EXPECT_EQ(countNonZero(diff), 0);

  H = (Mat_<float>(2, 3) << 1.0, 0.0, 50, 0.0, 1.0, 10);
  warpAffine(padded, registered, H, padded.size());
  tracking.registration(padded, registered, 1);
  compare(registered, padded, diff, cv::CMP_NE);
  EXPECT_EQ(countNonZero(diff), 0);

  H = (Mat_<float>(2, 3) << 1.0, 0.0, 0, 0.0, 1.0, 0);
  tracking.registration(padded, registered, 1);
  compare(registered, padded, diff, cv::CMP_NE);
  EXPECT_EQ(countNonZero(diff), 0);
}

TEST_F(TrackingTest, RegistrationMethod2Dual) {
  Tracking tracking("test", "");

  /*UMat imageReference, registered, diff;
  Mat H;
  imread("../dataSet/dual.pgm", IMREAD_GRAYSCALE).copyTo(imageReference);
  // Adds padding to avoid cropping
  UMat padded(imageReference.rows + 100, imageReference.cols + 100, imageReference.depth());
  copyMakeBorder(imageReference, padded, 50, 50, 50, 50, BORDER_CONSTANT);

  H = (Mat_<float>(2, 3) << 1.0, 0.0, -20, 0.0, 1.0, -20);
	warpAffine(padded, registered, H, padded.size());
  tracking.registration(padded, registered, 2);
  compare(registered, padded, diff, cv::CMP_NE);
  EXPECT_EQ(countNonZero(diff), 0);
  
  H = (Mat_<float>(2, 3) << 1.0, 0.0, 50, 0.0, 1.0, 10);
	warpAffine(padded, registered, H, padded.size());
  tracking.registration(padded, registered, 2);
  compare(registered, padded, diff, cv::CMP_NE);
  EXPECT_EQ(countNonZero(diff), 0);

  H = (Mat_<float>(2, 3) << 1.0, 0.0, 0, 0.0, 1.0, 0);
  tracking.registration(padded, registered, 2);
  compare(registered, padded, diff, cv::CMP_NE);
  EXPECT_EQ(countNonZero(diff), 0);*/
}

TEST_F(TrackingTest, Module) {
  Tracking tracking("test", "");
  EXPECT_EQ(tracking.modul(0), 0);
  EXPECT_EQ(tracking.modul(M_PI), M_PI);
  EXPECT_EQ(tracking.modul(M_PI * 0.5 + 2 * M_PI), M_PI * 0.5);
  EXPECT_EQ(tracking.modul(M_PI * 0.5 - 2 * M_PI), M_PI * 0.5);
  EXPECT_EQ(tracking.modul(-M_PI * 0.5 + 2 * M_PI), 3 * M_PI * 0.5);
  EXPECT_EQ(tracking.modul(-M_PI * 0.5 - 2 * M_PI), 3 * M_PI * 0.5);
  EXPECT_EQ(tracking.modul(-M_PI), M_PI);
  EXPECT_EQ(tracking.modul(4 * M_PI), 0);
  EXPECT_EQ(tracking.modul(3 * M_PI), M_PI);
  EXPECT_EQ(tracking.modul(-0.5 * M_PI), 0.5 * 3 * M_PI);
  EXPECT_EQ(tracking.modul(-0.5 * M_PI), 0.5 * 3 * M_PI);
}

TEST_F(TrackingTest, AngleDifferrence) {
  Tracking tracking("test", "");
  EXPECT_EQ(tracking.angleDifference(M_PI, 0.5 * M_PI), -0.5 * M_PI);
  EXPECT_EQ(tracking.angleDifference(0.5 * M_PI, M_PI), 0.5 * M_PI);
  EXPECT_EQ(tracking.angleDifference(-0.5 * M_PI, -M_PI), -0.5 * M_PI);
  EXPECT_EQ(tracking.angleDifference(-M_PI, -0.5 * M_PI), +0.5 * M_PI);
  EXPECT_EQ(tracking.angleDifference(4 * M_PI + M_PI, 4 * M_PI + 0.5 * M_PI), -0.5 * M_PI);
  EXPECT_EQ(tracking.angleDifference(4 * M_PI + 0.5 * M_PI, 4 * M_PI + M_PI), 0.5 * M_PI);
  EXPECT_EQ(tracking.angleDifference(-4 * M_PI + 0.5 * M_PI, -4 * M_PI + M_PI), +0.5 * M_PI);
  EXPECT_EQ(tracking.angleDifference(-4 * M_PI + M_PI, 4 * M_PI + -0.5 * M_PI), +0.5 * M_PI);
  EXPECT_EQ(tracking.angleDifference(4 * M_PI + M_PI, 0.5 * M_PI), -0.5 * M_PI);
  EXPECT_EQ(tracking.angleDifference(4 * M_PI + 0.5 * M_PI, M_PI), 0.5 * M_PI);
  EXPECT_EQ(tracking.angleDifference(-4 * M_PI + 0.5 * M_PI, -M_PI), 0.5 * M_PI);
  EXPECT_EQ(tracking.angleDifference(-4 * M_PI + M_PI, -0.5 * M_PI), +0.5 * M_PI);
  EXPECT_EQ(tracking.angleDifference(M_PI, 4 * M_PI + 0.5 * M_PI), -0.5 * M_PI);
  EXPECT_EQ(tracking.angleDifference(0.5 * M_PI, 4 * M_PI + M_PI), 0.5 * M_PI);
  EXPECT_EQ(tracking.angleDifference(-0.5 * M_PI, -4 * M_PI + M_PI), -0.5 * M_PI);
  EXPECT_EQ(tracking.angleDifference(-M_PI, 4 * M_PI + -0.5 * M_PI), +0.5 * M_PI);
}

TEST_F(TrackingTest, Divide) {
  Tracking tracking("test", "");
  EXPECT_EQ(tracking.divide(1, 2), 0.5);
  EXPECT_EQ(tracking.divide(-1, 2), -0.5);
  EXPECT_EQ(tracking.divide(1, -2), -0.5);
  EXPECT_EQ(tracking.divide(1, 0), 0);
}

TEST_F(TrackingTest, Reassignement) {
  Tracking tracking("test", "");

  vector<Point3d> past, input, comp;
  vector<int> order, id, lost, idComp, lostComp;

  past = {Point3d(1), Point3d(2), Point3d(3)};
  input = {};
  order = {-1, -1, -1};
  EXPECT_EQ(tracking.reassignment(past, input, order), past);

  past = {Point3d(1), Point3d(2), Point3d(3)};
  input = {Point3d(3), Point3d(2), Point3d(1)};
  order = {2, 1, 0};
  EXPECT_EQ(tracking.reassignment(past, input, order), past);

  past = {Point3d(1), Point3d(2), Point3d(3)};
  input = {Point3d(3), Point3d(2)};
  order = {-1, 1, 0};
  EXPECT_EQ(tracking.reassignment(past, input, order), past);

  past = {Point3d(1), Point3d(2), Point3d(3)};
  input = {Point3d(3), Point3d(2), Point3d(4), Point3d(1)};
  order = {3, 1, 0};
  comp = {Point3d(1), Point3d(2), Point3d(3), Point3d(4)};
  EXPECT_EQ(tracking.reassignment(past, input, order), comp);

  past = {Point3d(1), Point3d(2)};
  input = {Point3d(3), Point3d(2), Point3d(4), Point3d(1)};
  order = {3, 1};
  EXPECT_EQ(tracking.reassignment(past, input, order), comp);

  past = {Point3d(1), Point3d(2)};
  input = {Point3d(3), Point3d(2), Point3d(4), Point3d(1)};
  order = {3, 1, -1, -1};
  comp = {Point3d(1), Point3d(2), Point3d(3), Point3d(4)};
  EXPECT_EQ(tracking.reassignment(past, input, order), comp);
}

TEST_F(TrackingTest, Occlusion) {
  Tracking tracking("test", "");

  vector<Point3d> past, input, comp;
  vector<int> order;
  vector<int> occlusion;

  past = {Point3d(1), Point3d(2), Point3d(3)};
  input = {Point3d(3), Point3d(2), Point3d(1)};
  order = {2, 1, 0};
  occlusion = {};
  EXPECT_EQ(tracking.reassignment(past, input, order), past);
  EXPECT_EQ(tracking.findOcclusion(order), occlusion);

  past = {Point3d(1), Point3d(2), Point3d(3)};
  input = {Point3d(3), Point3d(2)};
  order = {-1, 1, 0};
  occlusion = {0};
  EXPECT_EQ(tracking.reassignment(past, input, order), past);
  EXPECT_EQ(tracking.findOcclusion(order), occlusion);

  past = {Point3d(1), Point3d(2), Point3d(3)};
  input = {Point3d(3)};
  order = {-1, -1, 0};
  occlusion = {0, 1};
  EXPECT_EQ(tracking.reassignment(past, input, order), past);
  EXPECT_EQ(tracking.findOcclusion(order), occlusion);

  past = {Point3d(1), Point3d(2), Point3d(3)};
  input = {};
  order = {-1, -1, -1};
  occlusion = {0, 1, 2};
  EXPECT_EQ(tracking.reassignment(past, input, order), past);
  EXPECT_EQ(tracking.findOcclusion(order), occlusion);

  past = {Point3d(1), Point3d(2), Point3d(3)};
  input = {Point3d(3), Point3d(2), Point3d(4), Point3d(1)};
  comp = {Point3d(1), Point3d(2), Point3d(3), Point3d(4)};
  order = {3, 1, 0};
  occlusion = {};
  EXPECT_EQ(tracking.reassignment(past, input, order), comp);
  EXPECT_EQ(tracking.findOcclusion(order), occlusion);
}

TEST_F(TrackingTest, Cleaning) {
  Tracking tracking("test", "");

  vector<vector<Point3d>> input, comp;
  vector<int> occlusion, lost, id, lostComp, idComp;

  input = {{Point3d(3)}, {
                             Point3d(2),
                         },
           {
               Point3d(1),
           }};
  comp = {{
              Point3d(3),
          },
          {
              Point3d(2),
          },
          {
              Point3d(1),
          }};
  id = {0, 1, 2};
  idComp = {0, 1, 2};
  lost = {0, 0, 0};
  lostComp = {0, 0, 0};
  occlusion = {};
  tracking.cleaning(occlusion, lost, id, input, 10);
  EXPECT_EQ(input, comp);
  EXPECT_EQ(id, idComp);
  EXPECT_EQ(lost, lostComp);

  input = {{Point3d(3)}, {
                             Point3d(2),
                         },
           {
               Point3d(1),
           }};
  comp = {{Point3d(3)}, {
                            Point3d(2),
                        },
          {
              Point3d(1),
          }};
  id = {0, 1, 2};
  idComp = {0, 1, 2};
  lost = {0, 0, 0};
  lostComp = {0, 1, 0};
  occlusion = {1};
  tracking.cleaning(occlusion, lost, id, input, 10);
  EXPECT_EQ(input, comp);
  EXPECT_EQ(id, idComp);
  EXPECT_EQ(lost, lostComp);

  input = {{Point3d(3)}, {
                             Point3d(2),
                         },
           {
               Point3d(1),
           }};
  comp = {{Point3d(3)}, {
                            Point3d(2),
                        },
          {
              Point3d(1),
          }};
  id = {0, 1, 2};
  idComp = {0, 1, 2};
  lost = {0, 0, 0};
  lostComp = {1, 1, 0};
  occlusion = {0, 1};
  tracking.cleaning(occlusion, lost, id, input, 10);
  EXPECT_EQ(input, comp);
  EXPECT_EQ(id, idComp);
  EXPECT_EQ(lost, lostComp);

  input = {{Point3d(0), Point3d(1)}, {Point3d(0), Point3d(1)}, {Point3d(0), Point3d(1)}};
  comp = {{Point3d(1)}, {Point3d(1)}, {Point3d(1)}};
  id = {0, 1, 2};
  idComp = {1, 2};
  lost = {10, 0, 0};
  lostComp = {0, 0};
  occlusion = {0};
  tracking.cleaning(occlusion, lost, id, input, 10);
  EXPECT_EQ(input, comp);
  EXPECT_EQ(id, idComp);
  EXPECT_EQ(lost, lostComp);

  input = {{Point3d(0), Point3d(1)}, {Point3d(0), Point3d(1)}, {Point3d(0), Point3d(1)}};
  comp = {{}, {}, {}};
  id = {0, 1};
  idComp = {};
  lost = {10, 10};
  lostComp = {};
  occlusion = {0, 1};
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
  EXPECT_EQ(round(test.at(0)), 4166);
  EXPECT_EQ(round(test.at(1)), 4166);

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
  Point2d center(double(imageReference.cols) * .5, double(imageReference.rows) * .5);
  test = tracking.objectInformation(imageReference);
  tracking.objectDirection(imageReference, test);
  EXPECT_EQ(round(test.at(2) * 1000), round(1000 * (M_PI)));
}

TEST_F(TrackingTest, costFunction) {
  Tracking tracking("test", "");
  QMap<QString, QString> params;
  params["Spot to track"] = "0";
  tracking.updatingParameters(params);

  vector<vector<Point3d>> past, current;
  vector<Point3d> pTest;
  vector<int> order, test;

  // Maximal distance test
  past = {{Point3d(1, 1, 45), Point3d(3, 4, 0), Point3d(6, 6, 180)}, {Point3d(0, 0, 0), Point3d(0, 0, 0), Point3d(0, 0, 0)}, {Point3d(0, 0, 0), Point3d(0, 0, 0), Point3d(0, 0, 0)}, {Point3d(0, 0, 0), Point3d(0, 0, 0), Point3d(0, 0, 0)}};
  current = {{Point3d(2, 2, 40), Point3d(2, 3.5, 50), Point3d(7.5, 5, 90)}, {Point3d(0, 0, 0), Point3d(0, 0, 0), Point3d(0, 0, 0)}, {Point3d(0, 0, 0), Point3d(0, 0, 0), Point3d(0, 0, 0)}, {Point3d(0, 0, 0), Point3d(0, 0, 0), Point3d(0, 0, 0)}};
  order = tracking.costFunc(past, current, 0, 0.5 * M_PI, 5, 0, 0);
  test = {0, 1, 2};
  EXPECT_EQ(order, test);
  EXPECT_EQ(tracking.reassignment(past[0], current[0], order), current[0]);

  // Empty current vector
  past = {{Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0)}, {Point3d(0, 0, 0), Point3d(0, 0, 0), Point3d(0, 0, 0)}, {Point3d(0, 0, 0), Point3d(0, 0, 0), Point3d(0, 0, 0)}, {Point3d(0, 0, 0), Point3d(0, 0, 0), Point3d(0, 0, 0)}};
  pTest = {Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0)};
  current = {{}, {}, {}, {}};
  order = tracking.costFunc(past, current, 0, 0.5 * M_PI, 20, 0, 0);
  test = {-1, -1, -1};
  EXPECT_EQ(order, test);
  EXPECT_EQ(tracking.reassignment(past[0], current[0], order), past[0]);

  // Maximal distance equal top 0 ==> new objects at each step
  past = {{Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0)}, {Point3d(0, 0, 0), Point3d(0, 0, 0), Point3d(0, 0, 0)}, {Point3d(0, 0, 0), Point3d(0, 0, 0), Point3d(0, 0, 0)}, {Point3d(0, 0, 0), Point3d(0, 0, 0), Point3d(0, 0, 0)}};
  current = {{Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0)}, {Point3d(0, 0, 0), Point3d(0, 0, 0), Point3d(0, 0, 0)}, {Point3d(0, 0, 0), Point3d(0, 0, 0), Point3d(0, 0, 0)}, {Point3d(0, 0, 0), Point3d(0, 0, 0), Point3d(0, 0, 0)}};
  order = tracking.costFunc(past, current, 0, 0.5 * M_PI, 0, 0, 0);
  test = {-1, -1, -1};
  EXPECT_EQ(order, test);
  pTest = {Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0), Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0)};
  EXPECT_EQ(tracking.reassignment(past[0], current[0], order), pTest);

  // One object loss
  past = {{Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0)}, {Point3d(0, 0, 0), Point3d(0, 0, 0), Point3d(0, 0, 0)}, {Point3d(0, 0, 0), Point3d(0, 0, 0), Point3d(0, 0, 0)}, {Point3d(0, 0, 0), Point3d(0, 0, 0), Point3d(0, 0, 0)}};
  current = {{Point3d(10, 15, 0), Point3d(35, 40, 0), Point3d(60, 60, 0)}, {Point3d(0, 0, 0), Point3d(0, 0, 0), Point3d(0, 0, 0)}, {Point3d(0, 0, 0), Point3d(0, 0, 0), Point3d(0, 0, 0)}, {Point3d(0, 0, 0), Point3d(0, 0, 0), Point3d(0, 0, 0)}};

  order = tracking.costFunc(past, current, 0, 0.5 * M_PI, 10, 0, 0);
  test = {0, 1, -1};
  EXPECT_EQ(order, test);
  pTest = {Point3d(10, 15, 0), Point3d(35, 40, 0), Point3d(50, 60, 0), Point3d(60, 60, 0)};
  EXPECT_EQ(tracking.reassignment(past[0], current[0], order), pTest);

  past = {{Point3d(10, 10, 0), Point3d(20, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0)}, {}, {}, {Point3d(10, 10, 0), Point3d(20, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0)}};
  current = {{Point3d(15, 10, 0), Point3d(35, 40, 0), Point3d(55, 60, 0)}, {}, {}, {Point3d(15, 10, 0), Point3d(35, 40, 0), Point3d(55, 60, 0)}};

  order = tracking.costFunc(past, current, 0, 0.5 * M_PI, 0, 0, 0);
  test = {-1, -1, -1, -1};
  EXPECT_EQ(order, test);
  pTest = {Point3d(10, 10, 0), Point3d(20, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0), Point3d(15, 10, 0), Point3d(35, 40, 0), Point3d(55, 60, 0)};
  EXPECT_EQ(tracking.reassignment(past[0], current[0], order), pTest);

  past = {{Point3d(15, 10, 0), Point3d(25, 10, 0), Point3d(35, 40, 0)}, {}, {}, {Point3d(15, 10, 0), Point3d(25, 10, 0), Point3d(35, 40, 0)}};
  current = {{Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0), Point3d(70, 80, 0)}, {}, {}, {Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0), Point3d(70, 80, 0)}};

  order = tracking.costFunc(past, current, 0, 0.5 * M_PI, 0, 0, 0);
  test = {-1, -1, -1};
  EXPECT_EQ(order, test);
  pTest = {Point3d(15, 10, 0), Point3d(25, 10, 0), Point3d(35, 40, 0), Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0), Point3d(70, 80, 0)};
  EXPECT_EQ(tracking.reassignment(past[0], current[0], order), pTest);

  past = {{Point3d(5, 5, 0), Point3d(0, 20, 0), Point3d(30, 40, 0)}, {}, {}, {Point3d(5, 5, 0), Point3d(0, 20, 0), Point3d(30, 40, 0)}};
  current = {{Point3d(50, 60, 0), Point3d(10, 20, 0)}, {}, {}, {Point3d(50, 60, 0), Point3d(10, 20, 0)}};

  order = tracking.costFunc(past, current, 0, 0.5 * M_PI, 11, 0, 0);
  test = {-1, 1, -1};
  EXPECT_EQ(order, test);
  pTest = {Point3d(5, 5, 0), Point3d(10, 20, 0), Point3d(30, 40, 0), Point3d(50, 60, 0)};
  EXPECT_EQ(tracking.reassignment(past[0], current[0], order), pTest);

  // Empty past vector
  current = {{Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0)}, {}, {}, {Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0)}};
  past = {{}, {}, {}, {}};
  order = tracking.costFunc(past, current, 0, 0.5 * M_PI, 20, 0, 0);
  test = {};
  EXPECT_EQ(order, test);
  EXPECT_EQ(tracking.reassignment(past[0], current[0], order), current[0]);

  current = {{Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0)}, {}, {}, {Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0)}};
  pTest = {Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0)};
  past = {{Point3d(15, 10, 0)}, {}, {}, {Point3d(15, 10, 0)}};
  order = tracking.costFunc(past, current, 0, 0.5 * M_PI, 20, 0, 0);
  test = {0};
  EXPECT_EQ(order, test);
  EXPECT_EQ(tracking.reassignment(past[0], current[0], order), pTest);

  current = {{}, {}, {}, {}};
  past = {{}, {}, {}, {}};
  order = tracking.costFunc(past, current, 0, 0.5 * M_PI, 20, 0, 0);
  test = {};
  EXPECT_EQ(order, test);
  EXPECT_EQ(tracking.reassignment(past[0], current[0], order), current[0]);

  // Only translation
  past = {{Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0)}, {}, {}, {Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0)}};
  current = {{Point3d(50, 60, 0), Point3d(10, 10, 0), Point3d(30, 40, 0)}, {}, {}, {Point3d(50, 60, 0), Point3d(10, 10, 0), Point3d(30, 40, 0)}};
  order = tracking.costFunc(past, current, 0, 0.5 * M_PI, 20, 0, 0);
  test = {1, 2, 0};
  EXPECT_EQ(order, test);
  EXPECT_EQ(tracking.reassignment(past[0], current[0], order), past[0]);

  // Only translation
  past = {{Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0)}, {}, {}, {Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0)}};
  current = {{Point3d(50, 50, 0), Point3d(10, 20, 0), Point3d(30, 30, 0)}, {}, {}, {Point3d(50, 50, 0), Point3d(10, 20, 0), Point3d(30, 30, 0)}};
  order = tracking.costFunc(past, current, 0, 0.5 * M_PI, 1, 0, 0);
  test = {1, 2, 0};
  EXPECT_NE(order, test);

  // Only angle no movement
  past = {{Point3d(10, 10, 0.5 * M_PI), Point3d(30, 40, M_PI), Point3d(50, 60, 0)}, {}, {}, {Point3d(10, 10, 0.5 * M_PI), Point3d(30, 40, M_PI), Point3d(50, 60, 0)}};
  current = {{Point3d(50, 60, 0), Point3d(10, 10, 0.5 * M_PI), Point3d(30, 40, M_PI)}, {}, {}, {Point3d(50, 60, 0), Point3d(10, 10, 0.5 * M_PI), Point3d(30, 40, M_PI)}};
  order = tracking.costFunc(past, current, 10, 0, 20, 0, 0);
  test = {1, 2, 0};
  EXPECT_EQ(order, test);
  EXPECT_EQ(tracking.reassignment(past[0], current[0], order), past[0]);

  // Only translation
  past = {{Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0)}, {}, {}, {Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0)}};
  current = {{Point3d(60, 60, 0), Point3d(10, 0, 0), Point3d(30, 50, 0)}, {}, {}, {Point3d(60, 60, 0), Point3d(10, 0, 0), Point3d(30, 50, 0)}};
  order = tracking.costFunc(past, current, 15, 0.5 * M_PI, 20, 0, 0);
  test = {1, 2, 0};
  EXPECT_EQ(order, test);

  // Rotation and translation
  past = {{Point3d(10, 10, 0), Point3d(30, 40, 3 * 0.5 * M_PI), Point3d(50, 60, 0.5 * M_PI)}, {}, {}, {Point3d(10, 10, 0), Point3d(30, 40, 3 * 0.5 * M_PI), Point3d(50, 60, 0.5 * M_PI)}};
  current = {{Point3d(60, 60, 0), Point3d(10, 0, 0.5 * M_PI), Point3d(30, 50, 0)}, {}, {}, {Point3d(60, 60, 0), Point3d(10, 0, 0.5 * M_PI), Point3d(30, 50, 0)}};
  order = tracking.costFunc(past, current, 15, 0.5 * M_PI, 20, 0, 0);
  test = {1, 2, 0};
  EXPECT_EQ(order, test);

  // Only translation
  past = {{Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0)}, {}, {}, {Point3d(10, 10, 0), Point3d(30, 40, 0), Point3d(50, 60, 0)}};
  current = {{Point3d(60, 60, 0), Point3d(10, 0, 0), Point3d(30, 50, 0), Point3d(30, 150, 0)}, {}, {}, {Point3d(60, 60, 0), Point3d(10, 0, 0), Point3d(30, 50, 0), Point3d(30, 150, 0)}};
  order = tracking.costFunc(past, current, 15, 0.5 * M_PI, 20, 0, 0);
  test = {1, 2, 0};
  EXPECT_EQ(order, test);

  // Rotation and translation
  past = {{Point3d(10, 10, 0), Point3d(30, 40, 3 * 0.5 * M_PI), Point3d(50, 60, 0.5 * M_PI)}, {}, {}, {Point3d(10, 10, 0), Point3d(30, 40, 3 * 0.5 * M_PI), Point3d(50, 60, 0.5 * M_PI)}};
  current = {{Point3d(60, 60, 0), Point3d(10, 0, 0.5 * M_PI)}, {}, {}, {Point3d(60, 60, 0), Point3d(10, 0, 0.5 * M_PI)}};
  order = tracking.costFunc(past, current, 15, 0.5 * M_PI, 20, 0, 0);
  test = {1, -1, 0};
  EXPECT_EQ(order, test);

  // Only rotation
  past = {{Point3d(10, 10, 0), Point3d(30, 40, 3 * 0.5 * M_PI), Point3d(50, 60, 0.5 * M_PI)}, {}, {}, {Point3d(10, 10, 0), Point3d(30, 40, 3 * 0.5 * M_PI), Point3d(50, 60, 0.5 * M_PI)}};
  current = {{Point3d(50, 60, 0), Point3d(10, 10, 0.5 * M_PI), Point3d(30, 40, 0)}, {}, {}, {Point3d(50, 60, 0), Point3d(10, 10, 0.5 * M_PI), Point3d(30, 40, 0)}};
  order = tracking.costFunc(past, current, 15, 0.5 * M_PI, 20, 0, 0);
  test = {1, 2, 0};
  EXPECT_EQ(order, test);

  past = {{Point3d(10, 10, 0), Point3d(30, 40, 3 * 0.5 * M_PI), Point3d(50, 60, 0.5 * M_PI)}, {}, {}, {Point3d(10, 10, 0), Point3d(30, 40, 3 * 0.5 * M_PI), Point3d(50, 60, 0.5 * M_PI)}};
  current = {{Point3d(50, 60, 0), Point3d(100, 100, 0.5 * M_PI), Point3d(30, 40, 0)}, {}, {}, {Point3d(50, 60, 0), Point3d(100, 100, 0.5 * M_PI), Point3d(30, 40, 0)}};
  order = tracking.costFunc(past, current, 15, 0.5 * M_PI, 0, 0, 0);
  test = {-1, -1, -1};
  EXPECT_EQ(order, test);

  past = {{Point3d(10, 10, 0), Point3d(30, 40, 3 * 0.5 * M_PI), Point3d(50, 60, 0.5 * M_PI)}, {}, {}, {Point3d(10, 10, 0), Point3d(30, 40, 3 * 0.5 * M_PI), Point3d(50, 60, 0.5 * M_PI)}};
  current = {{Point3d(50, 60, 0), Point3d(100, 100, 0.5 * M_PI), Point3d(30, 40, 0)}, {}, {}, {Point3d(50, 60, 0), Point3d(100, 100, 0.5 * M_PI), Point3d(30, 40, 0)}};
  order = tracking.costFunc(past, current, 15, 0.5 * M_PI, 10, 0, 0);
  test = {-1, 2, 0};
  EXPECT_EQ(order, test);

  // Area test
  past = {{Point3d(10, 10, 10), Point3d(20, 20, 20), Point3d(30, 30, 30)}, {}, {}, {Point3d(10, 10, 10), Point3d(20, 20, 20), Point3d(30, 30, 30)}};
  current = {{Point3d(20, 20, 20), Point3d(30, 30, 30), Point3d(10, 10, 10)}, {}, {}, {Point3d(20, 20, 20), Point3d(30, 30, 30), Point3d(10, 10, 10)}};
  order = tracking.costFunc(past, current, 0, 0, 200, 1, 0);
  test = {2, 0, 1};
  EXPECT_EQ(order, test);

  // Perimeter test
  past = {{Point3d(10, 10, 10), Point3d(20, 20, 20), Point3d(30, 30, 30)}, {}, {}, {Point3d(10, 10, 10), Point3d(20, 20, 20), Point3d(30, 30, 30)}};
  current = {{Point3d(20, 20, 20), Point3d(30, 30, 30), Point3d(10, 10, 10)}, {}, {}, {Point3d(20, 20, 20), Point3d(30, 30, 30), Point3d(10, 10, 10)}};
  order = tracking.costFunc(past, current, 0, 0, 200, 0, 1);
  test = {2, 0, 1};
  EXPECT_EQ(order, test);
}
}  // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
