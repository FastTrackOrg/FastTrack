#include <QMap>
#include <string>
#include "../src/autolevel.cpp"
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

class AutoLevelTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }

  virtual void TearDown() {
  }
};

//Curvature method test
TEST_F(TrackingTest, CurvatureCenter) {
  Tracking tracking("", "");
  Point2d test = tracking.curvatureCenter(Point3d(1, 1, M_PI * 0.25), Point3d(2, 2, M_PI));
  EXPECT_NEAR(test.x, 2, 0.005);
  EXPECT_NEAR(test.y, 0, 0.005);
  test = tracking.curvatureCenter(Point3d(2, 2, M_PI), Point3d(1, 1, M_PI * 0.25));
  EXPECT_NEAR(test.x, 2, 0.005);
  EXPECT_NEAR(test.y, 0, 0.005);
  test = tracking.curvatureCenter(Point3d(1, 1, 0.5 * M_PI), Point3d(2, 2, 0.5 * M_PI + 0.5 * M_PI));
  EXPECT_NEAR(test.x, 2, 0.005);
  EXPECT_NEAR(test.y, 1, 0.005);
  EXPECT_EQ(tracking.curvatureCenter(Point3d(1, 1, M_PI), Point3d(1, 1, M_PI)), Point2d(0, 0));
  EXPECT_EQ(tracking.curvatureCenter(Point3d(1, 1, M_PI), Point3d(1, 20, M_PI)), Point2d(0, 0));
  EXPECT_EQ(tracking.curvatureCenter(Point3d(1, 1, M_PI), Point3d(200, 200, M_PI)), Point2d(0, 0));
  EXPECT_EQ(tracking.curvatureCenter(Point3d(1, 1, M_PI * 0.5), Point3d(2, 2, M_PI * 0.5)), Point2d(0, 0));
  EXPECT_EQ(tracking.curvatureCenter(Point3d(1, 1, M_PI * 0.5), Point3d(1000, 1000, M_PI * 0.5)), Point2d(0, 0));
  test = tracking.curvatureCenter(Point3d(1, 1, M_PI), Point3d(2, 2, 0.5 * M_PI));
  EXPECT_NEAR(test.x, 1, 0.005);
  EXPECT_NEAR(test.y, 2, 0.005);
  test = tracking.curvatureCenter(Point3d(1, 1, 0.25 * M_PI), Point3d(3, 1, 3 * 0.25 * M_PI));
  EXPECT_NEAR(test.x, 2, 0.005);
  EXPECT_NEAR(test.y, 2, 0.005);
}

//Registration test
TEST_F(TrackingTest, RegistrationMethod0Lena) {
  Tracking tracking("", "");

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
  Tracking tracking("", "");

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
  Tracking tracking("", "");

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
  Tracking tracking("", "");

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
  Tracking tracking("", "");

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
  Tracking tracking("", "");

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
  Tracking tracking("", "");
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
  Tracking tracking("", "");
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
  Tracking tracking("", "");
  EXPECT_EQ(tracking.divide(1, 2), 0.5);
  EXPECT_EQ(tracking.divide(-1, 2), -0.5);
  EXPECT_EQ(tracking.divide(1, -2), -0.5);
  EXPECT_EQ(tracking.divide(1, 0), 0);
}

TEST_F(TrackingTest, Reassignement) {
  Tracking tracking("", "");

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
  Tracking tracking("", "");

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
  Tracking tracking("", "");

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
  Tracking tracking("", "");
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
  Tracking tracking("", "");
  vector<double> info, test;
  UMat imageReference, rotate;

  imread("../dataSet/assymetric_left.png", IMREAD_GRAYSCALE).copyTo(imageReference);
  Point2d center(double(imageReference.cols) * .5, double(imageReference.rows) * .5);
  test = tracking.objectInformation(imageReference);
  tracking.objectDirection(imageReference, test);
  EXPECT_EQ(round(test.at(2) * 1000), round(1000 * (M_PI)));
}

TEST_F(TrackingTest, costFunction) {
  Tracking tracking("", "");
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

// AutoSet test
TEST_F(AutoLevelTest, AutoLevel) {
  UMat background;
  imread("../dataSet/images/Groundtruth/Tracking_Result/background.pgm", IMREAD_GRAYSCALE).copyTo(background);
  QMap<QString, QString> parameters;
  parameters.insert("Background method", "1");
  parameters.insert("Background registration method", "0");
  parameters.insert("Binary threshold", "60");
  parameters.insert("Kernel size", "0");
  parameters.insert("Kernel type", "2");
  parameters.insert("Light background", "0");
  parameters.insert("Maximal angle", "90");
  parameters.insert("Maximal length", "100");
  parameters.insert("Maximal occlusion", "200");
  parameters.insert("Maximal size", "170");
  parameters.insert("Maximal time", "100");
  parameters.insert("Minimal size", "50");
  parameters.insert("Morphological operation", "8");
  parameters.insert("Number of images background", "20");
  parameters.insert("ROI bottom x", "0");
  parameters.insert("ROI bottom y", "0");
  parameters.insert("ROI top x", "0");
  parameters.insert("ROI top y", "0");
  parameters.insert("Registration", "0");
  parameters.insert("Spot to track", "0");
  parameters.insert("Normalization area", "0");
  parameters.insert("Normalization perimeter", "0");

  AutoLevel autolevel("../dataSet/images/frame_000001.pgm", background, parameters);
  QMap<QString, double> test = autolevel.level();
  EXPECT_EQ(std::lround(test.value("Normalization area")), 11);
  EXPECT_EQ(std::lround(test.value("Normalization perimeter")), 8);
  EXPECT_EQ(std::lround(test.value("Maximal angle")), 19);
  EXPECT_EQ(std::lround(test.value("Maximal length")), 2);
}
TEST_F(AutoLevelTest, AutoLevelStd) {
  QVector<double> test{8.504, 23.595, 81.471, 17.786, 97.959, 50.48, 96.469, 93.478, 79.251, 85.518, 69.064, 84.745, 53.369, 92.446, 86.935, 79.377, 67.804, 16.93, 49.407, 31.369, 16.355, 65.729, 67.652, 92.545, 56.909, 41.386, 6.141, 46.579, 29.573, 92.854, 35.241, 95.463, 42.066, 78.556, 92.12, 67.968, 20.008, 66.805, 63.092, 38.044, 74.936, 63.188, 25.586, 52.291, 41.38, 50.23, 1.072, 71.48, 66.107, 44.628, 42.135, 4.703, 87.119, 80.56, 43.915, 73.213, 86.908, 41.986, 80.081, 75.481, 92.773, 44.169, 29.05, 39.353, 59.543, 30.848, 97.805, 65.71, 90.388, 38.274, 98.07, 1.811, 45.201, 7.668, 91.251, 98.431, 58.871, 36.679, 7.002, 40.137, 37.95, 78.639, 77.687, 73.302, 66.698, 75.623, 61.33, 54.43, 17.075, 56.37, 54.612, 61.175, 53.911, 55.865, 56.265, 96.036, 87.017, 43.591, 60.39, 29.949};
  double sd = AutoLevel::stdev(test);
  EXPECT_EQ(sd, 26.517632067020994);
}
}  // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
