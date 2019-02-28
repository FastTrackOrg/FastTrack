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

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

using namespace std;

/**
 * @class MainWindow
 * 
 * @brief The MainWindow class is derived from a QMainWindow widget. It displays the main window of the program.
 *
 * @author Benjamin Gallois
 *
 * @version $Revision: 4.0 $
 *
 * Contact: gallois.benjamin08@gmail.com
 *
*/




/**
 * @brief Constructs the MainWindow QObject and initializes the UI. 
*/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QDir::setCurrent(QCoreApplication::applicationDirPath());
    ui->setupUi(this);
    setWindowState(Qt::WindowMaximized);
    setWindowTitle("Fast Track");
    
    // Setup style
    QFile stylesheet(":/theme.qss");

    if(stylesheet.open(QIODevice::ReadOnly | QIODevice::Text)) { // Read the theme file
        qApp->setStyleSheet(stylesheet.readAll());
        stylesheet.close();
    }

    interactive = new Interactive(this);
    ui->tabWidget->addTab(interactive, tr("Interactive tracking"));

    batch = new Batch(this);
    ui->tabWidget->addTab(batch, tr("Batch tracking"));
    
    replay = new Replay(this);
    ui->tabWidget->addTab(replay, tr("Tracking inspector"));
    

} // Constructor




/**
  * @brief Destructs the MainWindow object and saves the previous set of parameters.  
*/
MainWindow::~MainWindow()
{
    delete ui;
}

