#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
 int currentExitCode = 0;

 do {
  QApplication a(argc, argv);
  MainWindow w;
  w.show();
  currentExitCode = a.exec();
 } while( currentExitCode == MainWindow::EXIT_CODE_REBOOT );

 return currentExitCode;
}
