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


#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{

  QApplication a(argc, argv);
  MainWindow w;
  a.setApplicationName("Fast Track");
  a.setOrganizationName("Benjamin Gallois");
  a.setOrganizationDomain("fasttrack.benjamin-gallois.fr");
  w.setWindowIcon(QIcon(":/assets/icon.png")); 
  w.show();
  a.exec();
}
