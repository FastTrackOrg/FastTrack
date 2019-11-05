#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <tracking.h>
#include <QMap>
#include <QMapIterator>
#include <QString>
#include <QObject>

using namespace std;

void help() {
      printf (("Usage: %s [OPTION]... [FILE]...\n"));
      fputs (("\
Use FastTrack from the command line.\n\
\n\
"), stdout);
      fputs (("\
All argument are mandatory except --backPath.\n\
"), stdout);
      fputs (("\
  --maxArea                  maximal area of objects\n\
  --minArea                  maximal area of objects\n\
\n\
  --lightBack                is the background light? 0: Yes, 1: No\n\
  --thresh                   binary threshold\n\
   --reg                     registration method, 0: None, 1: Simple, 2: ECC, 3: Features\n\
\n\
  --spot                     part of the object that features is used for the matching, 0: head, 1: tail, 2: body\n\
  --normDist                 normalization distance\n\
  --normAngle                normalization angle\n\
  --weight                   weight between distance and angle for computing the cost function\n\
  --maxDist                  maximal distance of matching, if an object travels more than this distance, it is considered as a new object\n\
  --maxTime                  maximal time, if an object disappears more than this time, it is considered as a new object\n\
\n\
  --nBack                    number of images to compute the background\n\
  --methBack                 method to compute the background. 0: min, 1: max, 2: average\n\
  --regBack                  registration method to compute the background. 0: None, 1: Simple, 2: ECC, 3: Features\n\
\n\
  --xTop                     roi x top corner\n\
  --yTop                     roi y top corner\n\
  --xBottom                  roi x top bottom, set to 0 to have the full image\n\
  --yBottom                  roi y top bottom, set to 0 to have the full image\n\
\n\ 
  --morph                    morphological operation, 0: None, 1: Erode, 2: Dilate, 3: Open, 4: Close, 5: Gradient, 6: TopHat, 7: BlackHat, 8: HitMiss\n\
  --morphSize                size of the kernel used in the morphological operation, can be omited if no operation are performed\n\
  --morphType                type of the kernel used in the morphological operation, can be omited if no operation are performed, 0: Rect, 1: Cross, 2: Ellipse\n\
\n\
  --path                     path to the folder where the images are stored\n\
  --backPath                 optional, path to a background image\n\
      "), stdout);
}

int main (int argc, char **argv){
static struct option long_options[] =
  {
    {"maxArea",     required_argument,       0, 'a'},
    {"minArea",  required_argument,       0, 'b'},
    {"spot",  required_argument, 0, 'c'},
    {"normDist",  required_argument, 0, 'd'},
    {"normAngle",    required_argument, 0, 'e'},
    {"weight",    required_argument, 0, 'f'},
    {"maxDist",    required_argument, 0, 'g'},
    {"maxTime",    required_argument, 0, 'h'},
    {"thresh",    required_argument, 0, 'i'},
    {"nBack",    required_argument, 0, 'j'},
    {"methBack",    required_argument, 0, 'k'},
    {"regBack",    required_argument, 0, 'l'},
    {"xTop",    required_argument, 0, 'm'},
    {"yTop",    required_argument, 0, 'n'},
    {"xBottom",    required_argument, 0, 'o'},
    {"yBottom",    required_argument, 0, 'p'},
    {"reg",    required_argument, 0, 'q'},
    {"lightBack",    required_argument, 0, 'r'},
    {"morph",    required_argument, 0, 's'},
    {"morphSize",    required_argument, 0, 't'},
    {"morphType",    required_argument, 0, 'u'},
    {"path",    required_argument, 0, 'v'},
    {"backPath",    required_argument, 0, 'w'},
    {"help",    no_argument, 0, 'x'},
    {0, 0, 0, 0}
  };

  int option_index = 0;
  int c;
  QMap<QString, QString> parameters;
  while (1) {

    c = getopt_long (argc, argv, "a:b:c:d:e:f:g:h:i:j:k:l:m:n:o:p:q:r:s:t:u:v:wx", long_options, &option_index);

    if (c == -1) {
      help();
      break;
    }

    switch (c) {
      case 'a':
        parameters.insert("Maximal size", QString::QString::fromStdString(optarg));
        break;
      case 'b':
        parameters.insert("Minimal size", QString::fromStdString(optarg));
        break;
      case 'c':
        parameters.insert("Spot to track", QString::fromStdString(optarg));
        break;
      case 'd':
        parameters.insert("Maximal length", QString::fromStdString(optarg));
        break;
      case 'e':
        parameters.insert("Maximal angle", QString::fromStdString(optarg));
        break;
      case 'f':
        parameters.insert("Weight", QString::fromStdString(optarg));
        break;
      case 'g':
        parameters.insert("Maximal occlusion", QString::fromStdString(optarg));
        break;
      case 'h':
        parameters.insert("Maximal time", QString::fromStdString(optarg));
        break;
      case 'i':
        parameters.insert("Binary threshold", QString::fromStdString(optarg));
        break;
      case 'j':
        parameters.insert("Number of images background", QString::fromStdString(optarg));
        break;
      case 'k':
        parameters.insert("Background method", QString::fromStdString(optarg));
        break;
      case 'l':
        parameters.insert("Background registration method", QString::fromStdString(optarg));
        break;
      case 'm':
        parameters.insert("ROI top x", QString::fromStdString(optarg));
        break;
      case 'n':
        parameters.insert("ROI top y", QString::fromStdString(optarg));
        break;
      case 'o':
        parameters.insert("ROI bottom x", QString::fromStdString(optarg));
        break;
      case 'p':
        parameters.insert("ROI bottom y", QString::fromStdString(optarg));
        break;
      case 'q':
        parameters.insert("Registration", QString::fromStdString(optarg));
        break;
      case 'r':
        parameters.insert("Light background", QString::fromStdString(optarg));
        break;
      case 's':
        parameters.insert("Morphological operation", QString::fromStdString(optarg));
        break;
      case 't':
        parameters.insert("Kernel size", QString::fromStdString(optarg));
        break;
      case 'u':
        parameters.insert("Kernel type", QString::fromStdString(optarg));
        break;
      case 'v':
        parameters.insert("path", QString::fromStdString(optarg) + "/");
        break;
      case 'w':
        parameters.insert("backPath", QString::fromStdString(optarg));
        break;
      case 'x':
        help();
        break;
    }
  }

  QMapIterator<QString, QString> i(parameters);
  while (i.hasNext()) {
    i.next();
    cout << i.key().toStdString() << " set to: " << i.value().toStdString() << endl;
  }
  Tracking *tracking;
  if(parameters.contains("backPath")){
    tracking = new Tracking(parameters.value("path").toStdString(), parameters.value("backPath").toStdString());
  }
  else{
    tracking = new Tracking(parameters.value("path").toStdString(), "");
  }
  QObject::connect(tracking, &Tracking::finished, []() {
    cout << "Tracking ended normally" << endl;
  });
  QObject::connect(tracking, &Tracking::forceFinished, []() {
    cout << "An error occurs during the tracking" << endl;
  });

  tracking->updatingParameters(parameters);
  tracking->startProcess();
  return 0;
}
