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

int cli(int, char **) {
  QCommandLineParser parser;
  const QStringList parameterNames = {
      QStringLiteral("maxArea"),
      QStringLiteral("minArea"),
      QStringLiteral("spot"),
      QStringLiteral("normDist"),
      QStringLiteral("normAngle"),
      QStringLiteral("normArea"),
      QStringLiteral("normPerim"),
      QStringLiteral("maxDist"),
      QStringLiteral("maxTime"),
      QStringLiteral("thresh"),
      QStringLiteral("nBack"),
      QStringLiteral("methBack"),
      QStringLiteral("regBack"),
      QStringLiteral("xTop"),
      QStringLiteral("yTop"),
      QStringLiteral("xBottom"),
      QStringLiteral("yBottom"),
      QStringLiteral("reg"),
      QStringLiteral("lightBack"),
      QStringLiteral("morph"),
      QStringLiteral("morphSize"),
      QStringLiteral("morphType"),
      QStringLiteral("path"),
      QStringLiteral("backPath")};

  for (const QString &name : parameterNames) {
    parser.addOption(QCommandLineOption(name, QString(), QStringLiteral("value")));
  }
  parser.addOption(QCommandLineOption(QStringLiteral("cfg"), QString(), QStringLiteral("path")));
  parser.addOption(QCommandLineOption(QStringLiteral("help")));
  parser.addOption(QCommandLineOption(QStringLiteral("version")));

  if (!parser.parse(QCoreApplication::arguments())) {
    fprintf(stderr, "%s\n", parser.errorText().toLocal8Bit().constData());
    return 1;
  }
  if (parser.isSet(QStringLiteral("help"))) {
    help();
    return 0;
  }
  if (parser.isSet(QStringLiteral("version"))) {
    printf("FastTrack ");
    printf(APP_VERSION);
    printf("\nLicense GPLv3+: GNU GPL version 3\nThis is free software: you are free to change and redistribute it.\nThere is NO WARRANTY, to the extent permitted by law.\nWritten by Benjamin Gallois\n");
    return 0;
  }

  QHash<QString, QString> parameters;
  for (const QString &name : parameterNames) {
    if (parser.isSet(name)) {
      parameters.insert(name, parser.value(name));
    }
  }
  if (parser.isSet(QStringLiteral("cfg"))) {
    loadConfig(parser.value(QStringLiteral("cfg")), parameters);
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
