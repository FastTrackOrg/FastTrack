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
#include <QFont>
#include <QFontDatabase>
#include <QString>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  MainWindow w;
  a.setApplicationName("FastTrack");
  a.setApplicationVersion(APP_VERSION);
  a.setOrganizationName("FastTrackOrg");
  a.setOrganizationDomain("www.fasttrack.sh");
  w.setWindowIcon(QIcon(":/assets/icon.png"));
  QFontDatabase::addApplicationFont(":/assets/Font.ttf");
  w.setStyleSheet("QWidget { font-family: 'Lato'; }");
  w.show();
  a.exec();
}
