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

#ifndef CLI_H
#define CLI_H
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <tracking.h>
#include <QCoreApplication>
#include <QHash>
#include <QHashIterator>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTextStream>

using namespace std;

void loadConfig(const QString &path, QHash<QString, QString> &parameters);
void help();
int cli(int argc, char **argv);

#endif
