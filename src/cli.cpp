/*
This file is part of Fast Track.

    FastTrack is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FastTrack is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FastTrack.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "cli.h"

void loadConfig(const QString &path, QHash<QString, QString> &parameters) {
  QFile parameterFile(path);
  if (parameterFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&parameterFile);
    QString line;
    QStringList params;
    while (in.readLineInto(&line)) {
      if (line.contains(QLatin1String("="))) {
        params = line.split(QStringLiteral("="), Qt::SkipEmptyParts);
        parameters.insert(params[0].trimmed(), params[1].trimmed());
      }
    }
    parameters.remove(QStringLiteral("title"));
    parameterFile.close();
  }
}

void help() {
  printf("Usage:  [OPTION]... [FILE]...\n");
  fputs(("\
Use FastTrack version \
"),
        stdout);
  fputs((APP_VERSION),
        stdout);
  fputs(("\
 from the command line.\n\
\n\
"),
        stdout);
  fputs(("\
All arguments are mandatory except --backPath and --cfg. Loading a configuration file with --cfg overwrite any selected parameters.\n\
"),
        stdout);
  fputs(("\
  --maxArea                  maximal area of objects\n\
  --minArea                  minimal area of objects\n\
\n\
  --lightBack                is the background light? 0: Yes, 1: No\n\
  --thresh                   binary threshold, if lightBack is set to 0 (resp. 1), pixels with values less (resp. more) than thresh are considered to belong to an object\n\
   --reg                     registration method, 0: None, 1: Simple, 2: ECC, 3: Features\n\
\n\
  --spot                     part of the object that features is used for the matching, 0: head, 1: tail, 2: body\n\
  --normDist                 normalization distance pixels\n\
  --normAngle                normalization angle degres\n\
  --normArea                 normalization area pixels\n\
  --normPerim            normalization perimeter pixels\n\
  --maxDist                  maximal distance of matching in pixels, if an object travels more than this distance, it is considered as a new object\n\
  --maxTime                  maximal time, if an object disappears more than this time, it is considered as a new object\n\
\n\
  --nBack                    number of images to compute the background\n\
  --methBack                 method to compute the background. 0: min, 1: max, 2: average\n\
  --regBack                  registration method to compute the background. 0: None, 1: Simple, 2: ECC, 3: Features\n\
\n\
  --xTop                     roi x top corner (0:width-1)\n\
  --yTop                     roi y top corner (0:height-1)\n\
  --xBottom                  roi x top corner (0:xTop-1), set to 0 to avoid cropping\n\
  --yBottom                  roi y top corner (0:yTop-1), set to 0 to avoid cropping\n\
\n\
  --morph                    morphological operation, 0: None, 1: Erode, 2: Dilate, 3: Open, 4: Close, 5: Gradient, 6: TopHat, 7: BlackHat, 8: HitMiss\n\
  --morphSize                size of the kernel used in the morphological operation, can be omited if no operation are performed\n\
  --morphType                type of the kernel used in the morphological operation, can be omited if no operation are performed, 0: Rect, 1: Cross, 2: Ellipse\n\
\n\
  --path                     path to the movie or one image of a sequence\n\
  --backPath                 optional, path to a background image\n\
\n\
  --cfg                      optional, path to a configuration file, if path is not included in the configuration file, --path option need to be put before --cfg option\n\
"),
        stdout);
}

int cli(int argc, char **argv) {
  static struct option long_options[] =
      {
          {"maxArea", required_argument, 0, 'a'},
          {"minArea", required_argument, 0, 'b'},
          {"spot", required_argument, 0, 'c'},
          {"normDist", required_argument, 0, 'd'},
          {"normAngle", required_argument, 0, 'e'},
          {"normArea", required_argument, 0, 'y'},
          {"normPerim", required_argument, 0, 'z'},
          {"maxDist", required_argument, 0, 'g'},
          {"maxTime", required_argument, 0, 'h'},
          {"thresh", required_argument, 0, 'i'},
          {"nBack", required_argument, 0, 'j'},
          {"methBack", required_argument, 0, 'k'},
          {"regBack", required_argument, 0, 'l'},
          {"xTop", required_argument, 0, 'm'},
          {"yTop", required_argument, 0, 'n'},
          {"xBottom", required_argument, 0, 'o'},
          {"yBottom", required_argument, 0, 'p'},
          {"reg", required_argument, 0, 'q'},
          {"lightBack", required_argument, 0, 'r'},
          {"morph", required_argument, 0, 's'},
          {"morphSize", required_argument, 0, 't'},
          {"morphType", required_argument, 0, 'u'},
          {"path", required_argument, 0, 'v'},
          {"backPath", required_argument, 0, 'w'},
          {"cfg", required_argument, 0, 'A'},
          {"help", no_argument, 0, 'x'},
          {"version", no_argument, 0, 'V'},
          {0, 0, 0, 0}};

  int option_index = 0;
  int c;
  QHash<QString, QString> parameters;
  while (1) {
    c = getopt_long(argc, argv, "a:b:c:d:e:f:g:h:i:j:k:l:m:n:o:p:q:r:s:t:u:v:w:x:y:z:A", long_options, &option_index);

    if (c == -1) {
      break;
    }
    else if (c == 'A') {
      loadConfig(QString::fromStdString(optarg), parameters);
      break;
    }
    else if (c == 'V') {
      printf("FastTrack ");
      printf(APP_VERSION);
      printf("\nLicense GPLv3+: GNU GPL version 3\nThis is free software: you are free to change and redistribute it.\nThere is NO WARRANTY, to the extent permitted by law.\nWritten by Benjamin Gallois\n");
      return 0;
    }
    else if (c == 'x') {
      help();
      return 0;
    }

    switch (c) {
      case 'a':
        parameters.insert(QStringLiteral("maxArea"), QString::QString::fromStdString(optarg));
        break;
      case 'b':
        parameters.insert(QStringLiteral("minArea"), QString::fromStdString(optarg));
        break;
      case 'c':
        parameters.insert(QStringLiteral("spot"), QString::fromStdString(optarg));
        break;
      case 'd':
        parameters.insert(QStringLiteral("normDist"), QString::fromStdString(optarg));
        break;
      case 'e':
        parameters.insert(QStringLiteral("normAngle"), QString::fromStdString(optarg));
        break;
      case 'g':
        parameters.insert(QStringLiteral("maxDist"), QString::fromStdString(optarg));
        break;
      case 'h':
        parameters.insert(QStringLiteral("maxTime"), QString::fromStdString(optarg));
        break;
      case 'i':
        parameters.insert(QStringLiteral("thresh"), QString::fromStdString(optarg));
        break;
      case 'j':
        parameters.insert(QStringLiteral("nBack"), QString::fromStdString(optarg));
        break;
      case 'k':
        parameters.insert(QStringLiteral("methBack"), QString::fromStdString(optarg));
        break;
      case 'l':
        parameters.insert(QStringLiteral("regBack"), QString::fromStdString(optarg));
        break;
      case 'm':
        parameters.insert(QStringLiteral("xTop"), QString::fromStdString(optarg));
        break;
      case 'n':
        parameters.insert(QStringLiteral("yTop"), QString::fromStdString(optarg));
        break;
      case 'o':
        parameters.insert(QStringLiteral("xBottom"), QString::fromStdString(optarg));
        break;
      case 'p':
        parameters.insert(QStringLiteral("yBottom"), QString::fromStdString(optarg));
        break;
      case 'q':
        parameters.insert(QStringLiteral("reg"), QString::fromStdString(optarg));
        break;
      case 'r':
        parameters.insert(QStringLiteral("lightBack"), QString::fromStdString(optarg));
        break;
      case 's':
        parameters.insert(QStringLiteral("morph"), QString::fromStdString(optarg));
        break;
      case 't':
        parameters.insert(QStringLiteral("morphSize"), QString::fromStdString(optarg));
        break;
      case 'u':
        parameters.insert(QStringLiteral("morphType"), QString::fromStdString(optarg));
        break;
      case 'v':
        parameters.insert(QStringLiteral("path"), QString::fromStdString(optarg));
        break;
      case 'w':
        parameters.insert(QStringLiteral("backPath"), QString::fromStdString(optarg));
        break;
      case 'y':
        parameters.insert(QStringLiteral("normArea"), QString::fromStdString(optarg));
        break;
      case 'z':
        parameters.insert(QStringLiteral("normPerim"), QString::fromStdString(optarg));
        break;
    }
  }

  QHashIterator<QString, QString> i(parameters);
  while (i.hasNext()) {
    i.next();
    cout << i.key().toStdString() << " set to: " << i.value().toStdString() << endl;
  }
  Tracking *tracking;
  if (parameters.contains(QStringLiteral("backPath"))) {
    tracking = new Tracking(parameters.value(QStringLiteral("path")).toStdString(), parameters.value(QStringLiteral("backPath")).toStdString());
  }
  else {
    tracking = new Tracking(parameters.value(QStringLiteral("path")).toStdString(), "");
  }
  QObject::connect(tracking, &Tracking::finished, []() {
    cout << "Tracking ended normally" << endl;
  });
  QObject::connect(tracking, &Tracking::forceFinished, [](const QString &message) {
    cout << message.toStdString() << endl;
  });

  tracking->updatingParameters(parameters);
  tracking->startProcess();
  return 0;
}
