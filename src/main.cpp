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

#include <QApplication>
#include <QDir>
#include <QFont>
#include <QFontDatabase>
#include <QPixmap>
#include <QScopedPointer>
#include <QSplashScreen>
#include <QString>
#include "mainwindow.h"

QScopedPointer<QFile> logFile;

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
  QTextStream out(logFile.data());
  out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");
  out << context.category << ": " << msg << Qt::endl;
}

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  QPixmap pixmap(":/assets/icon.png");
  QSplashScreen splash(pixmap);
  splash.show();
  a.setApplicationName("FastTrack");
  a.setApplicationVersion(APP_VERSION);
  a.setOrganizationName("FastTrackOrg");
  a.setOrganizationDomain("www.fasttrack.sh");
  logFile.reset(new QFile(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/fasttrack.log"));
  QDir().mkpath(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
  logFile.data()->open(QFile::Append | QFile::Text);
  qInstallMessageHandler(messageHandler);
  MainWindow w;
  w.setWindowIcon(QIcon(":/assets/icon.png"));
  QFontDatabase::addApplicationFont(":/assets/Font.ttf");
  w.setStyleSheet("QWidget {font-family: 'Lato', sans-serif;}");
  w.show();
  splash.finish(&w);
  a.exec();
}
